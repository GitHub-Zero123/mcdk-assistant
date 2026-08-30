// SAPI 索引的运行时侧：读取预编译缓存并提供查询。
// 构建侧（解析 .d.ts）在 sapi_build.cpp，只编进 mcdk-index-compiler —— 原因见该文件顶部。
// 本文件不得引入 tree-sitter 依赖。

#include "sapi/sapi_index.hpp"
#include "sapi/sapi_internal.hpp"

#include "common/path_utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <system_error>
#include <unordered_map>
#include <unordered_set>

namespace mcdk::sapi {
namespace fs = std::filesystem;

using namespace detail;

bool load_sapi_cache(const std::filesystem::path& cache_path,
                     SapiIndex& index,
                     std::string* error) {
    std::string path = mcdk::path::to_utf8(cache_path);
    FILE* fp = std::fopen(path.c_str(), "rb");
    if (!fp) {
        if (error) *error = "cannot open for reading: " + path;
        return false;
    }

    char magic[8];
    if (std::fread(magic, 1, 8, fp) != 8 || std::memcmp(magic, kMagic, 8) != 0) {
        std::fclose(fp);
        if (error) *error = "bad SAPI cache magic";
        return false;
    }
    uint32_t version = read_u32(fp);
    if (version != kVersion) {
        std::fclose(fp);
        if (error) *error = "unsupported SAPI cache version: " + std::to_string(version);
        return false;
    }

    SapiIndex out;
    out.files_scanned = read_i32(fp);
    out.parse_error_files = read_i32(fp);
    out.error_nodes = read_i32(fp);
    out.missing_nodes = read_i32(fp);

    uint32_t module_count = read_u32(fp);
    out.modules.resize(module_count);
    for (auto& module : out.modules) {
        if (!read_string(fp, module.name) || !read_string(fp, module.source_file)) {
            std::fclose(fp); return false;
        }
        uint32_t import_count = read_u32(fp);
        module.imports.resize(import_count);
        for (auto& imp : module.imports) {
            if (!read_string(fp, imp.module) || !read_string_vec(fp, imp.symbols)) {
                std::fclose(fp); return false;
            }
            imp.line = read_i32(fp);
        }
        uint32_t ids = read_u32(fp);
        module.symbol_ids.resize(ids);
        for (auto& id : module.symbol_ids) id = read_i32(fp);
    }

    uint32_t symbol_count = read_u32(fp);
    out.symbols.resize(symbol_count);
    for (auto& sym : out.symbols) {
        if (!read_string(fp, sym.module) ||
            !read_string(fp, sym.kind) ||
            !read_string(fp, sym.name) ||
            !read_string(fp, sym.fqname) ||
            !read_string(fp, sym.signature) ||
            !read_string(fp, sym.doc) ||
            !read_string(fp, sym.source_file)) {
            std::fclose(fp); return false;
        }
        sym.line_start = read_i32(fp);
        sym.line_end = read_i32(fp);
        if (!read_string_vec(fp, sym.type_refs)) {
            std::fclose(fp); return false;
        }
        uint32_t members = read_u32(fp);
        sym.members.resize(members);
        for (auto& member : sym.members) {
            if (!read_string(fp, member.kind) ||
                !read_string(fp, member.name) ||
                !read_string(fp, member.fqname) ||
                !read_string(fp, member.signature) ||
                !read_string(fp, member.doc) ||
                !read_string(fp, member.value)) {
                std::fclose(fp); return false;
            }
            member.line_start = read_i32(fp);
            member.line_end = read_i32(fp);
        }
    }

    if (!read_string_vec(fp, out.diagnostics)) {
        std::fclose(fp); return false;
    }

    bool ok = std::ferror(fp) == 0;
    std::fclose(fp);
    if (!ok) {
        if (error) *error = "read error: " + path;
        return false;
    }
    index = std::move(out);
    return true;
}

std::filesystem::path default_sapi_cache_path(const std::filesystem::path& exe_dir) {
    return exe_dir / "mcdk_sapi_index_cache.bin";
}

namespace {

std::unordered_map<std::string, std::vector<int>> build_name_map(const SapiIndex& index) {
    std::unordered_map<std::string, std::vector<int>> map;
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        map[to_lower_ascii(symbol.name)].push_back(i);
        map[to_lower_ascii(symbol.fqname)].push_back(i);
        map[to_lower_ascii(symbol.module + "." + symbol.name)].push_back(i);
    }
    return map;
}

std::optional<std::string> base_type_from_signature(const std::string& signature) {
    static const std::regex re(R"(\bextends\s+([A-Za-z_$][A-Za-z0-9_.$]*))");
    std::smatch match;
    if (!std::regex_search(signature, match, re) || match.size() < 2) return std::nullopt;
    std::string base = match[1].str();
    size_t dot = base.rfind('.');
    if (dot != std::string::npos) base = base.substr(dot + 1);
    return base;
}

std::vector<int> find_symbols_by_name(const SapiIndex& index, const std::string& name) {
    std::vector<int> ids;
    std::string needle = to_lower_ascii(name);
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        if (to_lower_ascii(symbol.name) == needle || to_lower_ascii(symbol.fqname) == needle)
            ids.push_back(i);
    }
    return ids;
}

std::vector<SapiSearchResult> lookup_inherited_member(const SapiIndex& index,
                                                      const std::string& owner_name,
                                                      const std::string& member_name) {
    std::vector<SapiSearchResult> results;
    auto owners = find_symbols_by_name(index, owner_name);
    if (owners.empty()) return results;

    std::string member_lower = to_lower_ascii(member_name);
    std::set<int> visited;

    struct QueueItem {
        int symbol_id = -1;
        std::vector<std::string> path;
    };
    std::vector<QueueItem> queue;
    for (int id : owners) {
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        queue.push_back({id, {index.symbols[id].name}});
        visited.insert(id);
    }

    for (size_t head = 0; head < queue.size() && results.size() < 16; ++head) {
        const auto item = queue[head];
        const auto& symbol = index.symbols[item.symbol_id];
        auto base = base_type_from_signature(symbol.signature);
        if (!base) continue;

        auto base_ids = find_symbols_by_name(index, *base);
        for (int base_id : base_ids) {
            if (base_id < 0 || base_id >= static_cast<int>(index.symbols.size())) continue;
            const auto& base_symbol = index.symbols[base_id];
            std::vector<std::string> path = item.path;
            path.push_back(base_symbol.name);

            for (int m = 0; m < static_cast<int>(base_symbol.members.size()); ++m) {
                if (to_lower_ascii(base_symbol.members[m].name) != member_lower) continue;
                SapiSearchResult result;
                result.symbol_id = base_id;
                result.member_index = m;
                result.score = 940.0;
                result.match_kind = "inherited-member";
                result.inherited_from = item.path.front() + "." + member_name;
                result.inheritance_path = std::move(path);
                results.push_back(std::move(result));
            }

            if (visited.insert(base_id).second) queue.push_back({base_id, std::move(path)});
        }
    }

    return results;
}

std::vector<int> resolve_type_refs(const SapiIndex& index, const std::vector<std::string>& refs) {
    auto name_map = build_name_map(index);
    std::vector<int> ids;
    std::unordered_set<int> seen;
    for (const auto& ref : refs) {
        auto it = name_map.find(to_lower_ascii(ref));
        if (it == name_map.end()) continue;
        for (int id : it->second) {
            if (seen.insert(id).second) ids.push_back(id);
        }
    }
    return ids;
}

std::vector<int> expand_refs(const SapiIndex& index,
                             int symbol_id,
                             int member_index,
                             int depth,
                             std::vector<int>* out_depths = nullptr,
                             size_t limit = 24) {
    std::vector<int> result;
    if (out_depths) out_depths->clear();
    if (depth <= 0 || symbol_id < 0 || symbol_id >= static_cast<int>(index.symbols.size())) return result;

    std::set<int> seen;
    struct QueueItem {
        int id = -1;
        int remaining = 0;
        int depth = 1;
    };
    std::vector<QueueItem> queue;

    auto enqueue_refs = [&](const std::vector<std::string>& refs, int next_depth, int ref_depth) {
        for (int id : resolve_type_refs(index, refs)) {
            if (id == symbol_id) continue;
            if (seen.insert(id).second) {
                result.push_back(id);
                if (out_depths) out_depths->push_back(ref_depth);
                queue.push_back({id, next_depth, ref_depth});
                if (result.size() >= limit) return;
            }
        }
    };

    const auto& symbol = index.symbols[symbol_id];
    if (member_index >= 0 && member_index < static_cast<int>(symbol.members.size())) {
        std::vector<std::string> refs;
        collect_type_refs_from_text(symbol.members[member_index].signature, refs);
        enqueue_refs(refs, depth - 1, 1);
    } else {
        enqueue_refs(symbol.type_refs, depth - 1, 1);
    }

    for (size_t head = 0; head < queue.size() && result.size() < limit; ++head) {
        const auto item = queue[head];
        if (item.remaining <= 0) continue;
        enqueue_refs(index.symbols[item.id].type_refs, item.remaining - 1, item.depth + 1);
    }

    return result;
}

double score_symbol_candidate(const SapiSymbol& symbol,
                              const std::string& raw_query,
                              const std::vector<std::string>& q_tokens,
                              std::string& match_kind) {
    std::string q_lower = to_lower_ascii(raw_query);
    std::string name_lower = to_lower_ascii(symbol.name);
    std::string fq_lower = to_lower_ascii(symbol.fqname);
    std::string module_lower = to_lower_ascii(symbol.module);

    double score = 0.0;
    if (q_lower == fq_lower) { match_kind = "exact-fqname"; return 1000.0; }
    if (q_lower == name_lower) { match_kind = "exact-symbol"; return 900.0; }
    if (fq_lower.find(q_lower) != std::string::npos && !q_lower.empty()) {
        score = std::max(score, 720.0);
        match_kind = "fqname-contains";
    }
    if (name_lower.find(q_lower) == 0 && !q_lower.empty()) {
        score = std::max(score, 680.0);
        match_kind = "symbol-prefix";
    }

    auto name_tokens = split_identifier_tokens(symbol.name);
    double name_score = token_coverage_score(q_tokens, name_tokens, 620.0, 360.0);
    if (name_score > score) {
        score = name_score;
        match_kind = "symbol-name-tokens";
    }

    auto module_tokens = split_identifier_tokens(symbol.module);
    double module_score = token_coverage_score(q_tokens, module_tokens, 260.0, 120.0);
    if (module_score > score) {
        score = module_score;
        match_kind = "module";
    }

    auto sig_tokens = split_identifier_tokens(symbol.signature);
    double sig_score = token_coverage_score(q_tokens, sig_tokens, 220.0, 90.0, false);
    if (sig_score > score) {
        score = sig_score;
        match_kind = "signature";
    }

    if (!symbol.doc.empty()) {
        int hits = 0;
        std::string doc_lower = to_lower_ascii(symbol.doc);
        for (const auto& token : q_tokens)
            if (doc_lower.find(token) != std::string::npos) ++hits;
        if (hits > 0) {
            double per = static_cast<double>(hits) / std::max<size_t>(1, q_tokens.size());
            // 模块符号的文档即其主要内容（Manifest Details/版本/示例），权重高于普通符号 doc，
            // 但仍低于精确/前缀名匹配，避免淹没真实 API。
            bool is_module = symbol.kind == "module";
            double doc_score = is_module ? 200.0 + 140.0 * per : 60.0 + 40.0 * per;
            if (doc_score > score) {
                score = doc_score;
                match_kind = is_module ? "module-doc" : "doc";
            }
        }
    }

    return score;
}

double score_member_candidate(const SapiMember& member,
                              const std::string& raw_query,
                              const std::vector<std::string>& q_tokens,
                              std::string& match_kind) {
    std::string q_lower = to_lower_ascii(raw_query);
    std::string name_lower = to_lower_ascii(member.name);
    std::string fq_lower = to_lower_ascii(member.fqname);

    double score = 0.0;
    if (q_lower == fq_lower) { match_kind = "exact-member-fqname"; return 980.0; }
    if (q_lower == name_lower) { match_kind = "exact-member"; return 860.0; }
    if (fq_lower.find(q_lower) != std::string::npos && !q_lower.empty()) {
        score = std::max(score, 700.0);
        match_kind = "member-fqname-contains";
    }
    if (name_lower.find(q_lower) == 0 && !q_lower.empty()) {
        score = std::max(score, 660.0);
        match_kind = "member-prefix";
    }

    auto name_tokens = split_identifier_tokens(member.name);
    double name_score = token_coverage_score(q_tokens, name_tokens, 640.0, 340.0);
    if (name_score > score) {
        score = name_score;
        match_kind = "member-name-tokens";
    }

    auto fq_tokens = split_identifier_tokens(member.fqname);
    double fq_score = token_coverage_score(q_tokens, fq_tokens, 760.0, 360.0, false);
    if (fq_score > score) {
        score = fq_score;
        match_kind = "member-fqname-tokens";
    }

    auto sig_tokens = split_identifier_tokens(member.signature);
    double sig_score = token_coverage_score(q_tokens, sig_tokens, 300.0, 120.0, false);
    if (sig_score > score) {
        score = sig_score;
        match_kind = "member-signature";
    }

    if (!member.doc.empty()) {
        int hits = 0;
        std::string doc_lower = to_lower_ascii(member.doc);
        for (const auto& token : q_tokens)
            if (doc_lower.find(token) != std::string::npos) ++hits;
        if (hits > 0) {
            double doc_score = 70.0 + 60.0 * hits / std::max<size_t>(1, q_tokens.size());
            if (doc_score > score) {
                score = doc_score;
                match_kind = "member-doc";
            }
        }
    }

    return score;
}

void attach_refs(const SapiIndex& index, std::vector<SapiSearchResult>& results, int ref_depth) {
    for (auto& result : results)
        result.referenced_symbols = expand_refs(index, result.symbol_id, result.member_index,
                                                ref_depth, &result.referenced_depths);
}

} // namespace

std::vector<SapiSearchResult> search_sapi_symbols(const SapiIndex& index,
                                                  const std::string& query,
                                                  int top_k,
                                                  int ref_depth) {
    std::vector<SapiSearchResult> results;
    std::string q = trim(query);
    if (q.empty()) return results;
    if (top_k <= 0) top_k = 8;
    ref_depth = std::clamp(ref_depth, 0, 3);

    auto q_tokens = query_tokens(q);
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        std::string match;
        double score = score_symbol_candidate(symbol, q, q_tokens, match);
        if (score > 0.0) results.push_back({i, -1, score, match, {}});

        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            std::string member_match;
            double member_score = score_member_candidate(symbol.members[m], q, q_tokens, member_match);
            if (member_score > 0.0) {
                results.push_back({i, m, member_score, member_match, {}});
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.symbol_id != b.symbol_id) return a.symbol_id < b.symbol_id;
        return a.member_index < b.member_index;
    });

    std::set<std::pair<int, int>> seen;
    std::vector<SapiSearchResult> deduped;
    deduped.reserve(std::min<int>(top_k, static_cast<int>(results.size())));
    for (auto& result : results) {
        std::pair<int, int> key{result.symbol_id, result.member_index};
        if (!seen.insert(key).second) continue;
        deduped.push_back(std::move(result));
        if (static_cast<int>(deduped.size()) >= top_k) break;
    }
    attach_refs(index, deduped, ref_depth);
    return deduped;
}

std::vector<SapiSearchResult> lookup_sapi_symbol(const SapiIndex& index,
                                                 const std::string& name,
                                                 int ref_depth) {
    std::vector<SapiSearchResult> exact_symbols;
    std::vector<SapiSearchResult> exact_members;
    std::vector<SapiSearchResult> member_name_matches;
    std::string q = trim(name);
    if (q.empty()) return {};
    std::string q_lower = to_lower_ascii(q);
    ref_depth = std::clamp(ref_depth, 0, 3);

    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        if (to_lower_ascii(symbol.fqname) == q_lower || to_lower_ascii(symbol.name) == q_lower) {
            exact_symbols.push_back({i, -1, 1000.0, "symbol", {}});
        }
        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            const auto& member = symbol.members[m];
            if (to_lower_ascii(member.fqname) == q_lower ||
                to_lower_ascii(symbol.name + "." + member.name) == q_lower) {
                exact_members.push_back({i, m, 980.0, "member", {}});
            } else if (to_lower_ascii(member.name) == q_lower) {
                member_name_matches.push_back({i, m, 860.0, "member-name", {}});
            }
        }
    }

    std::vector<SapiSearchResult> results;
    if (!exact_symbols.empty()) {
        results = std::move(exact_symbols);
    } else if (!exact_members.empty()) {
        results = std::move(exact_members);
    } else {
        size_t dot = q.rfind('.');
        if (dot != std::string::npos && dot + 1 < q.size()) {
            auto inherited = lookup_inherited_member(index, q.substr(0, dot), q.substr(dot + 1));
            if (!inherited.empty()) results = std::move(inherited);
        }
        if (results.empty()) results = std::move(member_name_matches);
    }

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.symbol_id != b.symbol_id) return a.symbol_id < b.symbol_id;
        return a.member_index < b.member_index;
    });
    if (results.size() > 16) results.resize(16);
    attach_refs(index, results, ref_depth);
    return results;
}

} // namespace mcdk::sapi