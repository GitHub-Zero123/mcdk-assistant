#pragma once

#include "common/path_utils.hpp"
#include "search/bm25.hpp"
#include "search/index_cache.hpp"
#include "search/search_text.hpp"
#include <cppjieba/QuerySegment.hpp>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <climits>
#include <mutex>
#include <array>
#include <atomic>
#include <stdexcept>
#include <thread>
#include <future>

namespace mcdk {

inline std::string fs_to_ansi(const std::filesystem::path& path) {
    return path.string();
}

enum class DocCategory { Unknown, API, Event, Enum, Beta, Wiki, BedrockDev, QuMod, NeteaseGuide };

class SearchService {
public:
    // 正常模式：词典、知识库、缓存都来自磁盘目录。
    SearchService(const std::filesystem::path& dicts_dir,
                  const std::filesystem::path& knowledge_dir,
                  const std::filesystem::path& cache_path = {},
                  bool force_rebuild_cache = false)
        : dicts_dir_(dicts_dir)
        , knowledge_dir_(knowledge_dir)
        , cache_path_(cache_path)
        , force_rebuild_cache_(force_rebuild_cache)
        , cache_only_mode_(false)
    {
        stop_words_ = search_text::load_stop_words(dicts_dir / "stop_words.utf8");
        init_indices();
    }

    // cache-only 模式保留搜索能力，但不再要求 knowledge 目录存在。
    SearchService(const std::filesystem::path& dicts_dir,
                  const std::filesystem::path& cache_path,
                  bool cache_only)
        : dicts_dir_(dicts_dir)
        , cache_path_(cache_path)
        , force_rebuild_cache_(false)
        , cache_only_mode_(cache_only)
    {
        stop_words_ = search_text::load_stop_words(dicts_dir / "stop_words.utf8");
        init_indices();
    }

    bool is_cache_only_mode() const { return cache_only_mode_; }

    std::vector<SearchResult> search_api(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_API);
        return search_category_flexible(api_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_event(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_EVENT);
        if (looks_english_query(keyword) && !is_precise_identifier_query(keyword)) {
            const int limited_top_k = top_k > 0 ? std::min(top_k, 8) : 8;
            return search_event_keyword_index(keyword, limited_top_k);
        }
        return search_category_flexible(event_index_, keyword, top_k, SearchMode::PreferEnglishFallback);
    }
    std::vector<SearchResult> search_enum(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_ENUM);
        return search_category_flexible(enum_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_netease_guide(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_NETEASE_GUIDE);
        return search_category_flexible(netease_guide_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_all(const std::string& keyword, int top_k = -1) const {
        // 聚合搜索必须限制每个分区的候选数，否则高频词会把所有分区全量结果拼接，
        // 导致响应体过大甚至表现为无响应。
        const int per_bucket_k = top_k > 0 ? std::max(top_k * 3, 30) : 60;

        auto a = search_api(keyword, per_bucket_k);
        auto b = search_event(keyword, per_bucket_k);
        auto c = search_enum(keyword, per_bucket_k);
        auto d = search_wiki(keyword, per_bucket_k);
        auto e = search_qumod(keyword, per_bucket_k);
        auto f = search_netease_guide(keyword, per_bucket_k);
        auto g = search_bedrock_dev(keyword, per_bucket_k);

        std::vector<SearchResult> merged;
        merged.reserve(a.size() + b.size() + c.size() + d.size() + e.size() + f.size() + g.size());
        merged.insert(merged.end(), a.begin(), a.end());
        merged.insert(merged.end(), b.begin(), b.end());
        merged.insert(merged.end(), c.begin(), c.end());
        merged.insert(merged.end(), d.begin(), d.end());
        merged.insert(merged.end(), e.begin(), e.end());
        merged.insert(merged.end(), f.begin(), f.end());
        merged.insert(merged.end(), g.begin(), g.end());

        std::sort(merged.begin(), merged.end(), [](const SearchResult& x, const SearchResult& y) {
            return x.score != y.score ? x.score > y.score : x.fragment->file < y.fragment->file;
        });

        if (top_k > 0 && static_cast<size_t>(top_k) < merged.size()) merged.resize(static_cast<size_t>(top_k));
        return merged;
    }
    std::vector<SearchResult> search_wiki(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_WIKI);
        return search_category_en(wiki_index_, keyword, top_k);
    }
    std::vector<SearchResult> search_bedrock_dev(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_BEDROCK_DEV);
        return search_category_en(bedrockdev_index_, keyword, top_k);
    }
    std::vector<SearchResult> search_qumod(const std::string& keyword, int top_k = -1) const {
        ensure_category_loaded(IndexCache::SECTION_QUMOD);
        return search_category_flexible(qumod_index_, keyword, top_k, SearchMode::Auto);
    }

    size_t doc_count() const {
        if (lazy_cache_mode_) {
            size_t count = 0;
            for (uint32_t kind = IndexCache::SECTION_API;
                 kind <= IndexCache::SECTION_BEDROCK_DEV;
                 ++kind) {
                count += manifest_item_count(kind);
            }
            return count;
        }
        return api_index_.engine.doc_count() + event_index_.engine.doc_count()
             + enum_index_.engine.doc_count() + wiki_index_.engine.doc_count()
             + bedrockdev_index_.engine.doc_count()
             + qumod_index_.engine.doc_count() + netease_guide_index_.engine.doc_count();
    }

    struct FileReadResult {
        std::string content;
        int         total_lines = 0;
        bool        found       = false;
    };

    FileReadResult read_cached_file(const std::string& rel_path, int line_start = 1, int line_end = INT_MAX) const {
        ensure_sections_for_path(rel_path, true);
        FileReadResult result;
        std::string full_content;
        bool found = false;

        auto collect = [&](const CategoryIndex& idx) {
            for (const auto& frag : idx.fragments) {
                if (frag.file == rel_path) {
                    full_content += frag.content;
                    found = true;
                }
            }
        };

        if (category_section_loaded(IndexCache::SECTION_API)) collect(api_index_);
        if (category_section_loaded(IndexCache::SECTION_EVENT)) collect(event_index_);
        if (category_section_loaded(IndexCache::SECTION_ENUM)) collect(enum_index_);
        if (category_section_loaded(IndexCache::SECTION_WIKI)) collect(wiki_index_);
        if (category_section_loaded(IndexCache::SECTION_BEDROCK_DEV)) collect(bedrockdev_index_);
        if (category_section_loaded(IndexCache::SECTION_QUMOD)) collect(qumod_index_);
        if (category_section_loaded(IndexCache::SECTION_NETEASE_GUIDE)) collect(netease_guide_index_);

        auto collect_ga = [&](const GameAssetIndex& idx) {
            for (const auto& frag : idx.fragments) {
                if (frag.file == rel_path) {
                    full_content += frag.content;
                    found = true;
                }
            }
        };
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_BP)) collect_ga(game_assets_bp_);
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_RP)) collect_ga(game_assets_rp_);

        if (!found) return result;

        result.found = true;

        std::istringstream iss(full_content);
        std::string line;
        int cur = 0;
        if (line_start < 1) line_start = 1;
        if (line_end < line_start) line_end = line_start;

        while (std::getline(iss, line)) {
            ++cur;
            if (cur < line_start) continue;
            if (cur > line_end) break;
            result.content += line;
            result.content += '\n';
        }
        result.total_lines = cur;
        return result;
    }

    struct ListResult {
        std::vector<std::string> dirs;
        std::vector<std::string> files;
        bool                     found = false;
    };

    ListResult list_cached_files(const std::string& rel_path = "") const {
        ensure_sections_for_path(rel_path, false);
        ListResult result;
        std::set<std::string> dir_set, file_set;

        std::string prefix = rel_path;
        if (!prefix.empty()) {
            for (auto& c : prefix) if (c == '\\') c = '/';
            if (prefix.back() != '/') prefix += '/';
        }

        auto collect = [&](const std::vector<DocFragment>& frags) {
            for (const auto& frag : frags) {
                const std::string& file = frag.file;
                if (!prefix.empty() && file.find(prefix) != 0) continue;
                std::string remainder = file.substr(prefix.size());
                auto slash_pos = remainder.find('/');
                if (slash_pos != std::string::npos) {
                    dir_set.insert(remainder.substr(0, slash_pos));
                } else if (!remainder.empty()) {
                    file_set.insert(remainder);
                }
            }
        };

        if (category_section_loaded(IndexCache::SECTION_API)) collect(api_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_EVENT)) collect(event_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_ENUM)) collect(enum_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_WIKI)) collect(wiki_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_BEDROCK_DEV)) collect(bedrockdev_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_QUMOD)) collect(qumod_index_.fragments);
        if (category_section_loaded(IndexCache::SECTION_NETEASE_GUIDE)) collect(netease_guide_index_.fragments);
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_BP)) collect(game_assets_bp_.fragments);
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_RP)) collect(game_assets_rp_.fragments);

        auto collect_ga_paths = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.path_entries) {
                const std::string& file = entry.first;
                if (!prefix.empty() && file.find(prefix) != 0) continue;
                std::string remainder = file.substr(prefix.size());
                auto slash_pos = remainder.find('/');
                if (slash_pos != std::string::npos) {
                    dir_set.insert(remainder.substr(0, slash_pos));
                } else if (!remainder.empty()) {
                    file_set.insert(remainder);
                }
            }
        };
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_BP)) collect_ga_paths(game_assets_bp_);
        if (game_asset_section_loaded(IndexCache::SECTION_GAME_ASSETS_RP)) collect_ga_paths(game_assets_rp_);

        result.dirs.assign(dir_set.begin(), dir_set.end());
        result.files.assign(file_set.begin(), file_set.end());
        result.found = !result.dirs.empty() || !result.files.empty() || prefix.empty();
        return result;
    }

    struct AssetResult {
        std::string rel_path;
        std::string snippet;
        double      score;
    };

    static constexpr int DEFAULT_GAME_ASSET_TOP_K = 200;
    static constexpr size_t MAX_GAME_ASSET_QUERY_TOKENS = 32;

    std::vector<AssetResult> search_game_assets(const std::string& keyword, int scope, int top_k = -1) const {
        if (scope == 0 || scope == 1) ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_BP);
        if (scope == 0 || scope == 2) ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_RP);
        const int effective_top_k = (top_k > 0) ? top_k : DEFAULT_GAME_ASSET_TOP_K;

        std::vector<std::string> tokens;
        tokenize_en(keyword, tokens);
        if (tokens.empty()) {
            tokenize(keyword, tokens);
        }
        normalize_tokens(tokens);
        if (tokens.empty()) return {};

        const GameAssetIndex* idx_bp = (scope == 0 || scope == 1) ? &game_assets_bp_ : nullptr;
        const GameAssetIndex* idx_rp = (scope == 0 || scope == 2) ? &game_assets_rp_ : nullptr;

        if (tokens.size() > MAX_GAME_ASSET_QUERY_TOKENS) {
            struct RankedToken {
                size_t index;
                double idf;
                size_t identifier_signal;
            };

            auto identifier_signal = [](const std::string& token) {
                size_t signal = std::min<size_t>(token.size(), 64);
                for (char c : token) {
                    if (c == '_' || c == ':') signal += 16;
                }
                return signal;
            };
            auto corpus_idf = [&](const std::string& token, double& best_idf) {
                bool found = false;
                auto collect = [&](const GameAssetIndex* idx) {
                    if (!idx) return;
                    auto it = idx->engine.idf().find(token);
                    if (it == idx->engine.idf().end()) return;
                    best_idf = found ? std::max(best_idf, it->second) : it->second;
                    found = true;
                };
                collect(idx_bp);
                collect(idx_rp);
                return found;
            };

            std::vector<RankedToken> indexed;
            std::vector<RankedToken> path_fallback;
            indexed.reserve(tokens.size());
            path_fallback.reserve(tokens.size());
            for (size_t i = 0; i < tokens.size(); ++i) {
                double idf = 0.0;
                RankedToken candidate{i, idf, identifier_signal(tokens[i])};
                if (corpus_idf(tokens[i], candidate.idf)) indexed.push_back(candidate);
                else path_fallback.push_back(candidate);
            }

            auto by_corpus_signal = [](const RankedToken& a, const RankedToken& b) {
                if (a.idf != b.idf) return a.idf > b.idf;
                if (a.identifier_signal != b.identifier_signal)
                    return a.identifier_signal > b.identifier_signal;
                return a.index < b.index;
            };
            auto by_identifier_signal = [](const RankedToken& a, const RankedToken& b) {
                if (a.identifier_signal != b.identifier_signal)
                    return a.identifier_signal > b.identifier_signal;
                return a.index < b.index;
            };
            std::sort(indexed.begin(), indexed.end(), by_corpus_signal);
            std::sort(path_fallback.begin(), path_fallback.end(), by_identifier_signal);

            constexpr size_t PATH_FALLBACK_TOKEN_SLOTS = MAX_GAME_ASSET_QUERY_TOKENS / 4;
            std::vector<size_t> selected;
            selected.reserve(MAX_GAME_ASSET_QUERY_TOKENS);
            const size_t fallback_reserve = std::min(PATH_FALLBACK_TOKEN_SLOTS, path_fallback.size());
            for (size_t i = 0; i < fallback_reserve; ++i)
                selected.push_back(path_fallback[i].index);
            for (const auto& candidate : indexed) {
                if (selected.size() == MAX_GAME_ASSET_QUERY_TOKENS) break;
                selected.push_back(candidate.index);
            }
            for (size_t i = fallback_reserve;
                 i < path_fallback.size() && selected.size() < MAX_GAME_ASSET_QUERY_TOKENS;
                 ++i) {
                selected.push_back(path_fallback[i].index);
            }

            std::sort(selected.begin(), selected.end());
            std::vector<std::string> limited;
            limited.reserve(selected.size());
            for (size_t index : selected) limited.push_back(std::move(tokens[index]));
            tokens.swap(limited);
        }

        std::string kw_lower = keyword;
        for (auto& c : kw_lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        std::unordered_map<std::string, double>      score_map;
        std::unordered_map<std::string, std::string> snippet_map;
        score_map.reserve(512);
        snippet_map.reserve(512);

        auto collect_bm25 = [&](const GameAssetIndex& idx) {
            auto bm25_results = idx.engine.search(tokens, effective_top_k * 4);
            for (const auto& r : bm25_results) {
                const std::string& path = r.fragment->file;
                score_map[path] += r.score;
                if (snippet_map.find(path) == snippet_map.end()) {
                    const std::string& content = r.fragment->content;
                    snippet_map[path] = content.size() > 300 ? content.substr(0, 300) + "..." : content;
                }
            }
        };

        if (idx_bp) collect_bm25(*idx_bp);
        if (idx_rp) collect_bm25(*idx_rp);

        auto add_path_score = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.path_entries) {
                const std::string& rel       = entry.first;
                const std::string& rel_lower = entry.second;

                double path_score = 0.0;
                if (!kw_lower.empty() && rel_lower.find(kw_lower) != std::string::npos)
                    path_score += 10.0;
                for (const auto& tok : tokens) {
                    if (!tok.empty() && rel_lower.find(tok) != std::string::npos)
                        path_score += 3.0;
                }
                if (path_score > 0.0) {
                    score_map[rel] += path_score;
                    if (snippet_map.find(rel) == snippet_map.end()) {
                        snippet_map.emplace(rel, make_asset_path_snippet(rel));
                    }
                }
            }
        };

        if (idx_bp) add_path_score(*idx_bp);
        if (idx_rp) add_path_score(*idx_rp);

        std::vector<AssetResult> results;
        results.reserve(score_map.size());
        for (auto& [path, score] : score_map) {
            std::string snip;
            auto it = snippet_map.find(path);
            if (it != snippet_map.end()) snip = std::move(it->second);
            results.push_back({path, std::move(snip), score});
        }

        if (static_cast<size_t>(effective_top_k) < results.size()) {
            std::partial_sort(results.begin(),
                              results.begin() + effective_top_k,
                              results.end(),
                              [](const AssetResult& a, const AssetResult& b) {
                                  return a.score != b.score ? a.score > b.score : a.rel_path < b.rel_path;
                              });
            results.resize(static_cast<size_t>(effective_top_k));
        } else {
            std::sort(results.begin(), results.end(), [](const AssetResult& a, const AssetResult& b) {
                return a.score != b.score ? a.score > b.score : a.rel_path < b.rel_path;
            });
        }
        return results;
    }

    size_t game_assets_count() const {
        if (lazy_cache_mode_) {
            return manifest_item_count(IndexCache::SECTION_GAME_ASSETS_BP)
                 + manifest_item_count(IndexCache::SECTION_GAME_ASSETS_RP);
        }
        return game_assets_bp_.path_entries.size() + game_assets_rp_.path_entries.size();
    }

private:
    enum class SearchMode {
        ChineseOnly,
        EnglishOnly,
        Auto,
        PreferEnglishFallback,
    };

    struct CategoryIndex {
        struct IdentifierEntry {
            size_t      fragment_index;
            std::string identifier_compact;
        };

        std::vector<DocFragment>              fragments;
        std::vector<std::vector<std::string>> tokenized_docs;
        BM25Engine                            engine;
        std::vector<IdentifierEntry>          identifier_entries;
        std::unordered_map<std::string, std::vector<size_t>> identifier_exact_index;
    };

    struct GameAssetIndex {
        std::vector<std::pair<std::string,std::string>> path_entries;
        std::vector<DocFragment>                        fragments;
        std::vector<std::vector<std::string>>           tokenized_docs;
        BM25Engine                                      engine;
    };

    struct EventKeywordEntry {
        size_t      fragment_index;
        std::string identifier_lower;
    };

    std::filesystem::path           dicts_dir_;
    mutable std::unique_ptr<cppjieba::QuerySegment> jieba_;
    mutable std::once_flag          jieba_load_once_;
    std::filesystem::path           knowledge_dir_;
    std::filesystem::path           cache_path_;
    bool                            force_rebuild_cache_ = false;
    bool                            cache_only_mode_ = false;
    bool                            lazy_cache_mode_ = false;
    IndexCache::Manifest            cache_manifest_;
    std::unordered_set<std::string> stop_words_;
    mutable CategoryIndex           api_index_;
    mutable CategoryIndex           event_index_;
    mutable CategoryIndex           enum_index_;
    mutable CategoryIndex           wiki_index_;
    mutable CategoryIndex           bedrockdev_index_;
    mutable CategoryIndex           qumod_index_;
    mutable CategoryIndex           netease_guide_index_;
    mutable GameAssetIndex          game_assets_bp_;
    mutable GameAssetIndex          game_assets_rp_;
    mutable std::vector<EventKeywordEntry> event_keyword_entries_;
    mutable std::array<std::once_flag, IndexCache::CATEGORY_SECTION_COUNT> category_load_once_;
    mutable std::array<std::once_flag, IndexCache::GAME_ASSET_SECTION_COUNT> asset_load_once_;
    mutable std::array<std::atomic_bool, IndexCache::CATEGORY_SECTION_COUNT> category_loaded_{};
    mutable std::array<std::atomic_bool, IndexCache::GAME_ASSET_SECTION_COUNT> asset_loaded_{};

    void init_indices() {
        if (cache_only_mode_) {
            if (cache_path_.empty()) {
                std::cerr << "[MCDK] cache-only mode: no cache path provided" << std::endl;
                return;
            }
            IndexCache::Manifest manifest;
            if (IndexCache::load_manifest(cache_path_, "", manifest, /*skip_fingerprint_check=*/true)) {
                configure_lazy_cache(std::move(manifest));
                std::cerr << "[MCDK] 缓存模式：索引目录已加载，数据按需解析" << std::endl;
            } else {
                std::cerr << "[MCDK] 缓存模式：缓存文件加载失败: " << mcdk::path::to_utf8(cache_path_) << std::endl;
            }
            return;
        }

        if (!cache_path_.empty() && !force_rebuild_cache_) {
            // 缓存命中时只读取 section table，具体索引在首次查询时恢复。
            std::string fp = IndexCache::compute_fingerprint(knowledge_dir_);
            std::cerr << "[MCDK] knowledge fingerprint: " << fp << std::endl;

            IndexCache::Manifest manifest;
            if (IndexCache::load_manifest(cache_path_, fp, manifest)) {
                configure_lazy_cache(std::move(manifest));
                std::cerr << "[MCDK] 命中缓存，索引数据将在首次查询时解析" << std::endl;
                return;
            }
        }

        if (!cache_path_.empty() && force_rebuild_cache_) {
            // std::cerr << "[MCDK] 已请求强制重建索引，跳过缓存恢复..." << std::endl;
            // std::cerr << "[MCDK] 强制重建模式：开始完整构建索引..." << std::endl;
        } else {
            std::cerr << "[MCDK] 缓存未命中，开始完整构建索引..." << std::endl;
        }
        load_knowledge_parallel();
        build_indices();

        if (!cache_path_.empty()) {
            save_cache();  // save_cache() 内部已释放 tokenized_docs
        } else {
            auto free_td = [](CategoryIndex& idx) {
                std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
            };
            free_td(api_index_);
            free_td(event_index_);
            free_td(enum_index_);
            free_td(wiki_index_);
            free_td(qumod_index_);
            free_td(netease_guide_index_);
            free_td(bedrockdev_index_);
            auto free_td_ga = [](GameAssetIndex& idx) {
                std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
            };
            free_td_ga(game_assets_bp_);
            free_td_ga(game_assets_rp_);
            std::cerr << "[MCDK] tokenized_docs released (no-cache mode)" << std::endl;
        }
    }

    void configure_lazy_cache(IndexCache::Manifest&& manifest) {
        cache_manifest_ = std::move(manifest);
        lazy_cache_mode_ = true;
    }

    size_t manifest_item_count(uint32_t kind) const {
        const auto* section = cache_manifest_.find(kind);
        return section ? static_cast<size_t>(section->item_count) : 0;
    }

    static size_t category_slot(uint32_t kind) {
        return static_cast<size_t>(kind - IndexCache::SECTION_API);
    }

    static size_t asset_slot(uint32_t kind) {
        return static_cast<size_t>(kind - IndexCache::SECTION_GAME_ASSETS_BP);
    }

    bool category_section_loaded(uint32_t kind) const {
        return !lazy_cache_mode_ ||
               category_loaded_[category_slot(kind)].load(std::memory_order_acquire);
    }

    bool game_asset_section_loaded(uint32_t kind) const {
        return !lazy_cache_mode_ ||
               asset_loaded_[asset_slot(kind)].load(std::memory_order_acquire);
    }

    static uint32_t section_for_category(DocCategory category) {
        switch (category) {
        case DocCategory::API:          return IndexCache::SECTION_API;
        case DocCategory::Event:        return IndexCache::SECTION_EVENT;
        case DocCategory::Enum:         return IndexCache::SECTION_ENUM;
        case DocCategory::Wiki:         return IndexCache::SECTION_WIKI;
        case DocCategory::BedrockDev:   return IndexCache::SECTION_BEDROCK_DEV;
        case DocCategory::QuMod:        return IndexCache::SECTION_QUMOD;
        case DocCategory::NeteaseGuide: return IndexCache::SECTION_NETEASE_GUIDE;
        default:                        return 0;
        }
    }

    CategoryIndex& category_index_for_section(uint32_t kind) const {
        switch (kind) {
        case IndexCache::SECTION_API:           return api_index_;
        case IndexCache::SECTION_EVENT:         return event_index_;
        case IndexCache::SECTION_ENUM:          return enum_index_;
        case IndexCache::SECTION_WIKI:          return wiki_index_;
        case IndexCache::SECTION_QUMOD:         return qumod_index_;
        case IndexCache::SECTION_NETEASE_GUIDE: return netease_guide_index_;
        case IndexCache::SECTION_BEDROCK_DEV:   return bedrockdev_index_;
        default: throw std::logic_error("invalid category section");
        }
    }

    GameAssetIndex& game_asset_index_for_section(uint32_t kind) const {
        switch (kind) {
        case IndexCache::SECTION_GAME_ASSETS_BP: return game_assets_bp_;
        case IndexCache::SECTION_GAME_ASSETS_RP: return game_assets_rp_;
        default: throw std::logic_error("invalid game asset section");
        }
    }

    void ensure_category_loaded(uint32_t kind) const {
        if (!lazy_cache_mode_) return;
        const size_t slot = category_slot(kind);
        std::call_once(category_load_once_[slot], [this, kind, slot]() {
            IndexCache::CatData data;
            if (!IndexCache::load_category(cache_path_, cache_manifest_, kind, data)) {
                throw std::runtime_error("failed to load index cache section " + std::to_string(kind));
            }

            auto& index = category_index_for_section(kind);
            index.fragments = std::move(data.fragments);
            index.engine.restore_index(
                index.fragments,
                std::move(data.bm25.doc_lengths), data.bm25.avg_dl,
                std::move(data.bm25.idf), std::move(data.bm25.inverted_index));

            if (kind != IndexCache::SECTION_WIKI && kind != IndexCache::SECTION_BEDROCK_DEV) {
                rebuild_identifier_entries(index);
            }
            if (kind == IndexCache::SECTION_EVENT) rebuild_event_keyword_entries();
            category_loaded_[slot].store(true, std::memory_order_release);
            std::cerr << "[MCDK] lazy-loaded index section " << kind
                      << ": " << index.fragments.size() << " items" << std::endl;
        });
    }

    void ensure_game_assets_loaded(uint32_t kind) const {
        if (!lazy_cache_mode_) return;
        const size_t slot = asset_slot(kind);
        std::call_once(asset_load_once_[slot], [this, kind, slot]() {
            IndexCache::GameAssetData data;
            if (!IndexCache::load_game_assets(cache_path_, cache_manifest_, kind, data)) {
                throw std::runtime_error("failed to load game asset cache section " + std::to_string(kind));
            }

            auto& index = game_asset_index_for_section(kind);
            index.fragments = std::move(data.fragments);
            index.path_entries.resize(data.rel_paths.size());
            for (size_t i = 0; i < data.rel_paths.size(); ++i) {
                std::string lower = data.rel_paths[i];
                for (auto& c : lower) {
                    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                }
                index.path_entries[i].first = std::move(data.rel_paths[i]);
                index.path_entries[i].second = std::move(lower);
            }
            index.engine.restore_index(
                index.fragments,
                std::move(data.bm25.doc_lengths), data.bm25.avg_dl,
                std::move(data.bm25.idf), std::move(data.bm25.inverted_index));

            asset_loaded_[slot].store(true, std::memory_order_release);
            std::cerr << "[MCDK] lazy-loaded game asset section " << kind
                      << ": " << index.fragments.size() << " items" << std::endl;
        });
    }

    void ensure_sections_for_path(const std::string& rel_path, bool exact_file) const {
        if (!lazy_cache_mode_) return;

        std::string normalized = rel_path;
        for (auto& c : normalized) if (c == '\\') c = '/';

        if (normalized.rfind("GameAssets/behavior_packs/", 0) == 0) {
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_BP);
            return;
        }
        if (normalized.rfind("GameAssets/resource_packs/", 0) == 0) {
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_RP);
            return;
        }
        if (normalized == "GameAssets" || normalized.rfind("GameAssets/", 0) == 0) {
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_BP);
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_RP);
            return;
        }

        const uint32_t category_section = section_for_category(classify_path(normalized));
        if (category_section != 0) {
            ensure_category_loaded(category_section);
            return;
        }

        for (uint32_t kind = IndexCache::SECTION_API;
             kind <= IndexCache::SECTION_BEDROCK_DEV;
             ++kind) {
            ensure_category_loaded(kind);
        }
        if (normalized.empty() || exact_file) {
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_BP);
            ensure_game_assets_loaded(IndexCache::SECTION_GAME_ASSETS_RP);
        }
    }

    // 保存缓存：GameAssets 只存 rel_path，content 通过 fragments 序列化（去掉重复）
    void save_cache() {
        std::string fp = IndexCache::compute_fingerprint(knowledge_dir_);

        // 提取 GA 的 rel_path 列表（从 path_entries 取原始路径）
        std::vector<std::string> bp_rel_paths, rp_rel_paths;
        bp_rel_paths.reserve(game_assets_bp_.path_entries.size());
        rp_rel_paths.reserve(game_assets_rp_.path_entries.size());
        for (const auto& e : game_assets_bp_.path_entries) bp_rel_paths.push_back(e.first);
        for (const auto& e : game_assets_rp_.path_entries) rp_rel_paths.push_back(e.first);

        std::vector<IndexCache::CatIndexRef> cat_refs = {
            {&api_index_.fragments,           &api_index_.engine},
            {&event_index_.fragments,         &event_index_.engine},
            {&enum_index_.fragments,          &enum_index_.engine},
            {&wiki_index_.fragments,          &wiki_index_.engine},
            {&qumod_index_.fragments,         &qumod_index_.engine},
            {&netease_guide_index_.fragments, &netease_guide_index_.engine},
            {&bedrockdev_index_.fragments,    &bedrockdev_index_.engine},
        };

        std::vector<IndexCache::GameIndexRef> ga_refs = {
            {&bp_rel_paths, &game_assets_bp_.fragments, &game_assets_bp_.engine},
            {&rp_rel_paths, &game_assets_rp_.fragments, &game_assets_rp_.engine},
        };

        IndexCache::save(cache_path_, fp, cat_refs, ga_refs);

        // ── 修复①：缓存写完后立即释放 tokenized_docs ──
        // 运行时搜索完全不读 tokenized_docs，只有构建索引时用到。
        // 200MB 原文 → tokenized_docs 约 300MB，3GB 原文对应约 4.5GB，全部可释放。
        auto free_tokenized = [](CategoryIndex& idx) {
            std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
        };
        free_tokenized(api_index_);
        free_tokenized(event_index_);
        free_tokenized(enum_index_);
        free_tokenized(wiki_index_);
        free_tokenized(qumod_index_);
        free_tokenized(netease_guide_index_);
        free_tokenized(bedrockdev_index_);

        auto free_tokenized_ga = [](GameAssetIndex& idx) {
            std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
        };
        free_tokenized_ga(game_assets_bp_);
        free_tokenized_ga(game_assets_rp_);

        std::cerr << "[MCDK] tokenized_docs released after cache save" << std::endl;
    }

    // ── 搜索辅助 ──
    std::vector<SearchResult> search_category(const CategoryIndex& idx, const std::string& keyword, int top_k) const {
        std::vector<std::string> query_tokens;
        tokenize(keyword, query_tokens);
        return idx.engine.search(query_tokens, top_k);
    }
    std::vector<SearchResult> search_category_en(const CategoryIndex& idx, const std::string& keyword, int top_k) const {
        std::vector<std::string> query_tokens;
        tokenize_en(keyword, query_tokens);
        return idx.engine.search(query_tokens, top_k);
    }

    std::vector<SearchResult> search_category_flexible(const CategoryIndex& idx,
                                                       const std::string& keyword,
                                                       int top_k,
                                                       SearchMode mode) const {
        if (looks_english_query(keyword)) {
            auto identifier_hits = search_identifier_index(idx, keyword, top_k > 0 ? top_k : 8);
            if (!identifier_hits.empty()) {
                if (top_k <= 0 || identifier_hits.size() >= static_cast<size_t>(top_k)) return identifier_hits;

                auto bm25_hits = idx.engine.search(make_query_tokens(keyword, mode, false), top_k);
                append_unique_results(identifier_hits, bm25_hits, top_k);
                return identifier_hits;
            }
        }

        std::vector<std::string> primary_tokens = make_query_tokens(keyword, mode, false);
        auto results = idx.engine.search(primary_tokens, top_k);
        if (!results.empty()) return results;

        if (mode == SearchMode::ChineseOnly || mode == SearchMode::EnglishOnly) {
            return results;
        }

        std::vector<std::string> fallback_tokens = make_query_tokens(keyword, mode, true);
        if (fallback_tokens == primary_tokens) return results;
        return idx.engine.search(fallback_tokens, top_k);
    }

    static void append_unique_results(std::vector<SearchResult>& dst,
                                      const std::vector<SearchResult>& src,
                                      int top_k) {
        std::unordered_set<const DocFragment*> seen;
        seen.reserve(dst.size() + src.size());
        for (const auto& item : dst) seen.insert(item.fragment);
        for (const auto& item : src) {
            if (top_k > 0 && dst.size() >= static_cast<size_t>(top_k)) break;
            if (!item.fragment || !seen.insert(item.fragment).second) continue;
            dst.push_back(item);
        }
    }

    std::vector<SearchResult> search_event_keyword_index(const std::string& keyword, int top_k) const {
        if (top_k <= 0) top_k = 8;

        std::string keyword_lower = to_ascii_lower(keyword);
        std::vector<SearchResult> exact_prefix_matches;
        std::vector<SearchResult> loose_matches;
        exact_prefix_matches.reserve(static_cast<size_t>(top_k));
        loose_matches.reserve(static_cast<size_t>(top_k));

        auto try_push = [&](std::vector<SearchResult>& out, const DocFragment& frag, double score) {
            for (const auto& item : out) {
                if (item.fragment == &frag) return;
            }
            out.push_back({&frag, score});
        };

        for (const auto& entry : event_keyword_entries_) {
            const auto& frag = event_index_.fragments[entry.fragment_index];
            const std::string& best_lower = entry.identifier_lower;

            if (best_lower.find(keyword_lower) == 0) {
                double score = 1000.0 - static_cast<double>(best_lower.size() - keyword_lower.size());
                try_push(exact_prefix_matches, frag, score);
                continue;
            }
            if (best_lower.find(keyword_lower) != std::string::npos) {
                double score = 600.0 - static_cast<double>(best_lower.find(keyword_lower));
                try_push(loose_matches, frag, score);
            }
        }

        auto sorter = [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.fragment->file < b.fragment->file;
        };
        std::sort(exact_prefix_matches.begin(), exact_prefix_matches.end(), sorter);
        std::sort(loose_matches.begin(), loose_matches.end(), sorter);

        std::vector<SearchResult> merged;
        merged.reserve(static_cast<size_t>(top_k));
        for (const auto& item : exact_prefix_matches) {
            if (merged.size() >= static_cast<size_t>(top_k)) break;
            merged.push_back(item);
        }
        for (const auto& item : loose_matches) {
            if (merged.size() >= static_cast<size_t>(top_k)) break;
            merged.push_back(item);
        }

        if (!merged.empty()) return merged;
        return search_category_flexible(event_index_, keyword, top_k, SearchMode::PreferEnglishFallback);
    }

    std::vector<SearchResult> search_identifier_index(const CategoryIndex& idx,
                                                      const std::string& keyword,
                                                      int top_k) const {
        if (top_k <= 0) top_k = 8;

        std::vector<std::string> raw_terms;
        tokenize_en(keyword, raw_terms);
        if (raw_terms.empty()) return {};

        std::vector<std::string> query_terms;
        query_terms.reserve(raw_terms.size());
        std::unordered_set<std::string> seen_terms;
        for (const auto& term : raw_terms) {
            std::string compact = compact_ascii_identifier(term);
            if (compact.empty()) continue;
            if (seen_terms.insert(compact).second) query_terms.push_back(std::move(compact));
        }
        if (query_terms.empty()) return {};

        std::unordered_map<size_t, double> score_map;
        score_map.reserve(static_cast<size_t>(top_k) * 4);

        for (const auto& term : query_terms) {
            auto exact_it = idx.identifier_exact_index.find(term);
            if (exact_it != idx.identifier_exact_index.end()) {
                for (size_t entry_index : exact_it->second) {
                    if (entry_index >= idx.identifier_entries.size()) continue;
                    const auto& entry = idx.identifier_entries[entry_index];
                    score_map[entry.fragment_index] += 3000.0;
                }
            }

            static constexpr size_t kMaxIdentifierScan = 20000;
            const size_t scan_count = std::min(idx.identifier_entries.size(), kMaxIdentifierScan);
            for (size_t i = 0; i < scan_count; ++i) {
                const auto& entry = idx.identifier_entries[i];
                const auto& ident = entry.identifier_compact;
                if (ident.empty() || ident == term) continue;
                if (ident.rfind(term, 0) == 0) {
                    score_map[entry.fragment_index] += 2000.0 - static_cast<double>(ident.size() - term.size());
                    continue;
                }
                auto pos = ident.find(term);
                if (pos != std::string::npos) {
                    score_map[entry.fragment_index] += 1200.0 - static_cast<double>(pos * 4);
                }
            }
        }

        if (score_map.empty()) return {};

        std::vector<SearchResult> results;
        results.reserve(score_map.size());
        for (const auto& [fragment_index, score] : score_map) {
            if (fragment_index >= idx.fragments.size()) continue;
            results.push_back({&idx.fragments[fragment_index], score});
        }

        auto sorter = [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            if (a.fragment->file != b.fragment->file) return a.fragment->file < b.fragment->file;
            return a.fragment->line_start < b.fragment->line_start;
        };
        if (top_k > 0 && results.size() > static_cast<size_t>(top_k)) {
            std::partial_sort(results.begin(), results.begin() + static_cast<ptrdiff_t>(top_k), results.end(), sorter);
            results.resize(static_cast<size_t>(top_k));
        } else {
            std::sort(results.begin(), results.end(), sorter);
        }
        return results;
    }

    std::vector<std::string> make_query_tokens(const std::string& text,
                                               SearchMode mode,
                                               bool fallback_phase) const {
        std::vector<std::string> tokens;

        const bool prefer_english = mode == SearchMode::EnglishOnly
            || (mode == SearchMode::PreferEnglishFallback && fallback_phase)
            || (mode == SearchMode::Auto && looks_english_query(text));

        const bool prefer_chinese = mode == SearchMode::ChineseOnly
            || (mode == SearchMode::PreferEnglishFallback && !fallback_phase)
            || (mode == SearchMode::Auto && !prefer_english);

        if (prefer_chinese) {
            tokenize(text, tokens);
            if (!tokens.empty()) return tokens;
            if (mode == SearchMode::ChineseOnly) return tokens;
        }

        if (prefer_english) {
            tokenize_en(text, tokens);
            return tokens;
        }

        tokenize_en(text, tokens);
        return tokens;
    }

    static bool looks_english_query(const std::string& text) {
        return search_text::looks_english_query(text);
    }

    static bool is_precise_identifier_query(const std::string& text) {
        if (text.empty()) return false;
        bool has_upper = false;
        bool has_lower = false;
        size_t alpha_count = 0;
        for (unsigned char c : text) {
            if (!std::isalnum(c) && c != '_') return false;
            if (std::isalpha(c)) {
                ++alpha_count;
                if (std::isupper(c)) has_upper = true;
                if (std::islower(c)) has_lower = true;
            }
        }
        return alpha_count >= 12 && has_upper && has_lower;
    }

    static std::string to_ascii_lower(const std::string& text) {
        std::string out = text;
        for (auto& c : out) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return out;
    }

    static std::string compact_ascii_identifier(const std::string& text) {
        std::string out;
        out.reserve(text.size());
        for (unsigned char c : text) {
            if (std::isalnum(c)) out.push_back(static_cast<char>(std::tolower(c)));
        }
        return out;
    }

    static std::string extract_heading_identifier_limited(const std::string& content) {
        size_t pos = 0;
        const size_t limit = std::min<size_t>(content.size(), 2048);
        for (int line_index = 0; line_index < 8 && pos < limit; ++line_index) {
            size_t line_end = content.find('\n', pos);
            if (line_end == std::string::npos || line_end > limit) line_end = limit;
            size_t line_size = line_end - pos;
            if (line_size > 0 && content[pos + line_size - 1] == '\r') --line_size;

            size_t level = 0;
            while (level < line_size && content[pos + level] == '#') ++level;
            if (level > 0 && level < line_size && std::isspace(static_cast<unsigned char>(content[pos + level]))) {
                size_t begin = pos + level + 1;
                size_t end = pos + line_size;
                while (begin < end && std::isspace(static_cast<unsigned char>(content[begin]))) ++begin;
                while (end > begin && std::isspace(static_cast<unsigned char>(content[end - 1]))) --end;

                std::string word;
                for (size_t i = begin; i < end; ++i) {
                    unsigned char c = static_cast<unsigned char>(content[i]);
                    if (std::isalnum(c) || c == '_') word.push_back(static_cast<char>(c));
                    else if (!word.empty()) break;
                }
                if (!word.empty() && std::isalpha(static_cast<unsigned char>(word.front()))) return word;
            }
            pos = line_end + 1;
        }
        return {};
    }

    static std::string extract_best_event_identifier(const std::string& content) {
        std::string best;
        std::string word;
        auto flush = [&]() {
            if (word.empty()) return;
            if (std::isupper(static_cast<unsigned char>(word[0])) && word.find("Event") != std::string::npos) {
                if (word.size() > best.size()) best = word;
            }
            word.clear();
        };

        for (unsigned char c : content) {
            if (std::isalnum(c) || c == '_') {
                word.push_back(static_cast<char>(c));
            } else {
                flush();
            }
        }
        flush();
        return best;
    }

    void rebuild_event_keyword_entries() const {
        event_keyword_entries_.clear();
        event_keyword_entries_.reserve(event_index_.fragments.size());
        for (size_t i = 0; i < event_index_.fragments.size(); ++i) {
            std::string best = extract_best_event_identifier(event_index_.fragments[i].content);
            if (best.empty()) continue;
            event_keyword_entries_.push_back({i, to_ascii_lower(best)});
        }
    }

    static void rebuild_identifier_entries(CategoryIndex& idx) {
        idx.identifier_entries.clear();
        idx.identifier_exact_index.clear();
        idx.identifier_entries.reserve(idx.fragments.size());

        for (size_t i = 0; i < idx.fragments.size(); ++i) {
            std::string identifier = extract_heading_identifier_limited(idx.fragments[i].content);
            std::string compact = compact_ascii_identifier(identifier);
            if (compact.empty()) continue;

            const size_t entry_index = idx.identifier_entries.size();
            idx.identifier_entries.push_back({i, compact});
            idx.identifier_exact_index[compact].push_back(entry_index);
        }
    }

    void rebuild_cn_identifier_entries() {
        rebuild_identifier_entries(api_index_);
        rebuild_identifier_entries(event_index_);
        rebuild_identifier_entries(enum_index_);
        rebuild_identifier_entries(qumod_index_);
        rebuild_identifier_entries(netease_guide_index_);
    }

    static void normalize_tokens(std::vector<std::string>& tokens) {
        search_text::normalize_tokens(tokens);
    }

    static std::string make_asset_path_snippet(const std::string& rel_path) {
        return std::string("[PATH MATCH] ") + rel_path;
    }

    // ── 文件分类 ──
    static DocCategory classify_path(const std::string& rel_path) {
        if (rel_path.find("NeteaseGuide/") == 0 || rel_path.find("/NeteaseGuide/") != std::string::npos)
            return DocCategory::NeteaseGuide;
        if (rel_path.find("QuModDocs/") == 0 || rel_path.find("/QuModDocs/") != std::string::npos)
            return DocCategory::QuMod;
        if (rel_path.find("BedrockDev/") == 0 || rel_path.find("/BedrockDev/") != std::string::npos)
            return DocCategory::BedrockDev;
        if (rel_path.find("BedrockWiki/") == 0 || rel_path.find("/BedrockWiki/") != std::string::npos)
            return DocCategory::Wiki;
        if (rel_path.find("/接口/") != std::string::npos || rel_path.find("接口/") == 0)
            return DocCategory::API;
        if (rel_path.find("/事件/") != std::string::npos || rel_path.find("事件/") == 0)
            return DocCategory::Event;
        if (rel_path.find("/枚举值/") != std::string::npos || rel_path.find("枚举值/") == 0)
            return DocCategory::Enum;
        if (rel_path.find("/beta/") != std::string::npos || rel_path.find("beta/") == 0)
            return DocCategory::Beta;
        return DocCategory::Unknown;
    }

    CategoryIndex* index_for(DocCategory cat) {
        switch (cat) {
        case DocCategory::API:          return &api_index_;
        case DocCategory::Event:        return &event_index_;
        case DocCategory::Enum:         return &enum_index_;
        case DocCategory::Wiki:         return &wiki_index_;
        case DocCategory::BedrockDev:      return &bedrockdev_index_;
        case DocCategory::QuMod:        return &qumod_index_;
        case DocCategory::NeteaseGuide: return &netease_guide_index_;
        default:                        return nullptr;
        }
    }

    // ── 分词 ──
    void load_stop_words(const std::filesystem::path& path) {
        stop_words_ = search_text::load_stop_words(path);
    }

    void tokenize(const std::string& text, std::vector<std::string>& tokens) const {
        std::call_once(jieba_load_once_, [this]() {
            jieba_ = search_text::make_query_segment(dicts_dir_);
            std::cerr << "[MCDK] lazy-loaded Jieba query segment" << std::endl;
        });
        search_text::tokenize_zh(*jieba_, stop_words_, text, tokens);
    }

    static void tokenize_en(const std::string& text, std::vector<std::string>& tokens) {
        search_text::tokenize_en(text, tokens);
    }

    void load_knowledge_parallel() {
        namespace fs = std::filesystem;
        if (!fs::exists(knowledge_dir_)) {
            std::cerr << "[MCDK] knowledge dir not found: " << mcdk::path::to_utf8(knowledge_dir_) << std::endl;
            return;
        }

        std::vector<std::pair<fs::path, std::string>> md_files;
        std::vector<std::pair<fs::path, std::string>> ga_files;

        // 先完成路径扫描，再按 Markdown / GameAssets 分开处理，便于后续控制并发策略。
        for (const auto& entry : fs::recursive_directory_iterator(knowledge_dir_)) {
            if (!entry.is_regular_file()) continue;
            std::string rel_path = mcdk::path::to_utf8(fs::relative(entry.path(), knowledge_dir_));

            if (rel_path.find("GameAssets/") == 0 || rel_path.find("GameAssets\\") == 0) {
                std::string assets_rel = rel_path.substr(std::string("GameAssets/").size());
                for (auto& c : assets_rel) if (c == '\\') c = '/';
                if (assets_rel.find("behavior_packs/") == 0 || assets_rel.find("resource_packs/") == 0)
                    ga_files.push_back({entry.path(), assets_rel});
                continue;
            }
            auto ext = mcdk::path::to_utf8(entry.path().extension());
            if (ext == ".md" || ext == ".MD")
                md_files.push_back({entry.path(), rel_path});
        }

        std::cerr << "[MCDK] 扫描完成: " << md_files.size() << " md, "
                  << ga_files.size() << " game asset 文件" << std::endl;

        unsigned int nthreads = std::max(1u, std::thread::hardware_concurrency());
        std::cerr << "[MCDK] 使用 " << nthreads << " 线程读取 GameAssets..." << std::endl;

        size_t ga_total = ga_files.size();
        std::mutex bp_mutex, rp_mutex;

        size_t batch = std::max<size_t>(1, (ga_total + nthreads - 1) / nthreads);
        std::vector<std::future<void>> futures;

        for (size_t start = 0; start < ga_total; start += batch) {
            size_t end = std::min(start + batch, ga_total);
            futures.push_back(std::async(std::launch::async, [&, start, end]() {
                for (size_t i = start; i < end; ++i) {
                    const auto& [abs_path, assets_rel] = ga_files[i];
                    std::ifstream ifs(abs_path, std::ios::binary);
                    std::string content;
                    if (ifs.is_open()) {
                        std::ostringstream ss;
                        ss << ifs.rdbuf();
                        content = ss.str();
                    }
                    std::string ga_path = "GameAssets/" + assets_rel;
                    std::string ga_path_lower = ga_path;
                    for (auto& c : ga_path_lower)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

                    DocFragment frag{std::move(content), ga_path, 1, 0};
                    std::pair<std::string,std::string> pe{ga_path, std::move(ga_path_lower)};

                    if (assets_rel.find("behavior_packs/") == 0) {
                        std::lock_guard<std::mutex> lk(bp_mutex);
                        game_assets_bp_.path_entries.push_back(std::move(pe));
                        game_assets_bp_.fragments.push_back(std::move(frag));
                    } else {
                        std::lock_guard<std::mutex> lk(rp_mutex);
                        game_assets_rp_.path_entries.push_back(std::move(pe));
                        game_assets_rp_.fragments.push_back(std::move(frag));
                    }
                }
            }));
        }
        for (auto& f : futures) f.get();

        // jieba 相关分词阶段仍保持单线程，减少额外同步复杂度。
        for (const auto& [abs_path, rel_path] : md_files)
            load_markdown_file(abs_path, rel_path);

        std::cerr << "[MCDK] loaded " << doc_count() << " md-fragments, "
                  << game_assets_count() << " game assets" << std::endl;
    }

    void load_markdown_file(const std::filesystem::path& abs_path, const std::string& rel_path) {
        std::ifstream ifs(abs_path);
        if (!ifs.is_open()) return;

        DocCategory cat = classify_path(rel_path);
        CategoryIndex* idx = index_for(cat);
        if (!idx) return;

        std::string line;
        std::ostringstream current_content;
        int fragment_start = 1, line_num = 0;
        bool has_content = false;

        auto flush_fragment = [&]() {
            std::string content = current_content.str();
            if (!content.empty() && has_content)
                idx->fragments.push_back({std::move(content), rel_path, fragment_start, line_num});
            current_content.str("");
            current_content.clear();
            has_content = false;
            fragment_start = line_num + 1;
        };

        while (std::getline(ifs, line)) {
            ++line_num;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.size() >= 2 && line[0] == '#') {
                size_t level = 0;
                while (level < line.size() && line[level] == '#') ++level;
                if (level >= 2 && level <= 4) { flush_fragment(); fragment_start = line_num; }
            }
            current_content << line << "\n";
            if (!line.empty()) has_content = true;
        }
        flush_fragment();
    }

    // ── 构建 BM25 索引 ──
    void build_indices() {
        auto build_cn = [this](CategoryIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i)
                tokenize(idx.fragments[i].content, idx.tokenized_docs[i]);
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cerr << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        auto build_en_parallel = [](CategoryIndex& idx, const char* name) {
            size_t n = idx.fragments.size();
            idx.tokenized_docs.resize(n);
            unsigned int nt = std::max(1u, std::thread::hardware_concurrency());
            size_t batch = std::max<size_t>(1, (n + nt - 1) / nt);
            std::vector<std::future<void>> futs;
            for (size_t s = 0; s < n; s += batch) {
                size_t e = std::min(s + batch, n);
                futs.push_back(std::async(std::launch::async, [&idx, s, e]() {
                    for (size_t i = s; i < e; ++i)
                        tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
                }));
            }
            for (auto& f : futs) f.get();
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cerr << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        build_cn(api_index_,            "API");
        build_cn(event_index_,          "Event");
        rebuild_event_keyword_entries();
        build_cn(enum_index_,           "Enum");
        build_en_parallel(wiki_index_,   "Wiki");
        build_en_parallel(bedrockdev_index_, "BedrockDev");
        build_cn(qumod_index_,            "QuMod");
        build_cn(netease_guide_index_,  "NeteaseGuide");
        rebuild_cn_identifier_entries();

        auto build_ga_parallel = [](GameAssetIndex& idx, const char* name) {
            size_t n = idx.fragments.size();
            idx.tokenized_docs.resize(n);
            unsigned int nt = std::max(1u, std::thread::hardware_concurrency());
            size_t batch = std::max<size_t>(1, (n + nt - 1) / nt);
            std::vector<std::future<void>> futs;
            for (size_t s = 0; s < n; s += batch) {
                size_t e = std::min(s + batch, n);
                futs.push_back(std::async(std::launch::async, [&idx, s, e]() {
                    for (size_t i = s; i < e; ++i)
                        tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
                }));
            }
            for (auto& f : futs) f.get();
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cerr << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        build_ga_parallel(game_assets_bp_, "GameAssets/BP");
        build_ga_parallel(game_assets_rp_, "GameAssets/RP");
    }
};

} // namespace mcdk
