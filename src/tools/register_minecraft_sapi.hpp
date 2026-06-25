#pragma once

#include "sapi/sapi_index.hpp"
#include "tools/command_parser.hpp"

#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace mcdk {
namespace minecraft_sapi_detail {

constexpr const char* kToolName = "minecraft_sapi";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

inline std::string help_text() {
    return R"(minecraft_sapi - Minecraft Bedrock Script API symbol database
Usage: minecraft_sapi(command="<subcommand> [args...] [--option value]")

This tool searches the compiled SAPI symbol index from mcdk_sapi_index_cache.bin.
It is registered only when that cache file is available.

重要：网易版 MC MOD 通常使用 Py ModSDK；相关资料在 minecraft_docs。
仅当项目明确使用 SAPI / Script API / 国际版脚本时再调用本工具。

Commands:
  help
      Show this help.

  search <query...> [--offset <n>] [--limit <n>|--top <n>] [--refs <0-3>]
      Fuzzy English search for API symbols and members.
      Designed for dirty queries such as "spawn entity", "secret string",
      "camera preset", or "scoreboard objective".
      Search one target per command. Do not combine independent API names
      such as "Player Dimension ItemStack"; run separate searches instead.
      Continue a previous search by reusing the same query with --offset.

  symbol <name> [--refs <0-3>] [--member-offset <n>] [--members <n>]
      Exact or near-exact symbol/member lookup. Class/interface results include
      a compact member list by default.
      Examples: symbol Player --refs 1
                symbol Player --member-offset 32 --members 32
                symbol Dimension.spawnEntity --refs 1
                symbol @minecraft/server.Player --refs 1

  refs <symbol> [--offset <n>] [--limit <n>]
      Reverse lookup: list symbols/members whose signatures reference a type.
      Example: refs SecretString --limit 20

  module <module> [--offset <n>] [--limit <n>|--top <n>] [--detail true] [--refs <0-3>]
      List symbols from a module/package. Default output is compact and paged,
      so large modules do not flood the context.
      Examples: module @minecraft/server --limit 40
                module @minecraft/server --offset 40 --limit 40
                module @minecraft/server --detail true --limit 8 --refs 1

Reference depth:
  --refs 0   Return only matched symbol/member.
  --refs 1   Include directly referenced types. Default.
  --refs 2   Include one more hop, with an internal safety limit.
  For module list, refs default to 0 unless explicitly requested.

Guide placeholder:
  Later versions may add a short SAPI project setup guide here, including
  manifest/module configuration and script entry conventions.)";
}

inline mcp::json help_result() {
    return text_result(help_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + help_text());
}

inline int clamped_refs(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "refs", 1), 0, 3);
}

inline int clamped_refs(const ParsedCommand& pc, int def) {
    return std::clamp(flag_int(pc, "refs", def), 0, 3);
}

inline int clamped_top(const ParsedCommand& pc, int def = 8) {
    return std::clamp(flag_int(pc, "top", def), 1, 50);
}

inline int clamped_limit(const ParsedCommand& pc, int def = 30) {
    int value = has_flag(pc, "limit") ? flag_int(pc, "limit", def) : flag_int(pc, "top", def);
    return std::clamp(value, 1, 200);
}

inline int clamped_offset(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "offset", 0), 0, 1000000);
}

inline int clamped_members(const ParsedCommand& pc, int def = 24) {
    return std::clamp(flag_int(pc, "members", def), 0, 200);
}

inline int clamped_member_offset(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "member-offset", flag_int(pc, "member_offset", 0)), 0, 1000000);
}

inline bool flag_bool(const ParsedCommand& pc, const std::string& key, bool def = false) {
    if (!has_flag(pc, key)) return def;
    std::string value = command_parser_detail::to_lower_ascii(flag_str(pc, key));
    if (value.empty() || value == "1" || value == "true" || value == "yes" || value == "on")
        return true;
    if (value == "0" || value == "false" || value == "no" || value == "off")
        return false;
    return def;
}

inline std::string symbol_header(const sapi::SapiSymbol& symbol) {
    std::ostringstream out;
    out << symbol.fqname << "\n"
        << "kind: " << symbol.kind << "\n"
        << "module: " << symbol.module << "\n"
        << "source: " << symbol.source_file << ":" << symbol.line_start << "\n";
    if (!symbol.signature.empty())
        out << "signature: " << symbol.signature << "\n";
    if (!symbol.doc.empty())
        out << "doc:\n" << symbol.doc << "\n";
    return out.str();
}

inline void append_refs(std::ostringstream& out,
                        const sapi::SapiIndex& index,
                        const sapi::SapiSearchResult& result) {
    if (result.referenced_symbols.empty()) return;
    out << "referenced types:\n";
    for (size_t i = 0; i < result.referenced_symbols.size(); ++i) {
        int id = result.referenced_symbols[i];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& ref = index.symbols[id];
        int depth = i < result.referenced_depths.size() ? result.referenced_depths[i] : 1;
        out << "- ";
        if (depth <= 1) out << "direct ";
        else out << "depth " << depth << " ";
        out << ref.fqname << " (" << ref.kind << ")";
        if (!ref.signature.empty()) out << " - " << ref.signature;
        out << "\n";
    }
}

inline void append_member_summary(std::ostringstream& out,
                                  const sapi::SapiSymbol& symbol,
                                  int max_members,
                                  int member_offset = 0) {
    if (max_members <= 0 || symbol.members.empty()) return;
    int total = static_cast<int>(symbol.members.size());
    if (member_offset >= total) {
        out << "members: " << total << ". member-offset " << member_offset
            << " is out of range.\n";
        return;
    }
    int end = std::min(total, member_offset + max_members);
    out << "members (" << member_offset << "-" << end << "/" << total << "):\n";
    for (int i = member_offset; i < end; ++i) {
        const auto& member = symbol.members[i];
        out << "- " << member.kind << " " << member.name;
        if (!member.signature.empty()) out << ": " << member.signature;
        out << "\n";
    }
    if (end < total)
        out << "Use symbol " << symbol.fqname << " --members " << total
            << " or --member-offset " << end << " --members " << max_members
            << " to list more members.\n";
}

inline std::string format_result(const sapi::SapiIndex& index,
                                 const sapi::SapiSearchResult& result,
                                 int ordinal,
                                 int max_members = 24,
                                 int member_offset = 0) {
    std::ostringstream out;
    if (result.symbol_id < 0 || result.symbol_id >= static_cast<int>(index.symbols.size()))
        return {};

    const auto& symbol = index.symbols[result.symbol_id];
    out << ordinal << ". ";
    if (result.member_index >= 0 && result.member_index < static_cast<int>(symbol.members.size())) {
        const auto& member = symbol.members[result.member_index];
        out << member.fqname << "\n"
            << "kind: " << member.kind << "\n"
            << "owner: " << symbol.fqname << "\n"
            << "module: " << symbol.module << "\n"
            << "source: " << symbol.source_file << ":" << member.line_start << "\n"
            << "score: " << result.score << " (" << result.match_kind << ")\n";
        if (!member.signature.empty())
            out << "signature: " << member.signature << "\n";
        if (!member.doc.empty())
            out << "doc:\n" << member.doc << "\n";
    } else {
        out << symbol.fqname << "\n"
            << "kind: " << symbol.kind << "\n"
            << "module: " << symbol.module << "\n"
            << "source: " << symbol.source_file << ":" << symbol.line_start << "\n"
            << "score: " << result.score << " (" << result.match_kind << ")\n";
        if (!symbol.signature.empty())
            out << "signature: " << symbol.signature << "\n";
        if (!symbol.doc.empty())
            out << "doc:\n" << symbol.doc << "\n";
        append_member_summary(out, symbol, max_members, member_offset);
    }
    append_refs(out, index, result);
    return out.str();
}

inline mcp::json format_results(const sapi::SapiIndex& index,
                                const std::vector<sapi::SapiSearchResult>& results,
                                const std::string& empty_message,
                                int max_members = 24,
                                int member_offset = 0,
                                int ordinal_base = 0) {
    if (results.empty()) return text_result(empty_message);
    std::ostringstream out;
    for (size_t i = 0; i < results.size(); ++i) {
        if (i > 0) out << "\n---\n";
        out << format_result(index, results[i], ordinal_base + static_cast<int>(i + 1),
                             max_members, member_offset);
    }
    return text_result(out.str());
}

inline const sapi::SapiModule* find_module_exact(const sapi::SapiIndex& index,
                                                 const std::string& module_name) {
    std::string query = command_parser_detail::to_lower_ascii(module_name);
    for (const auto& module : index.modules) {
        if (command_parser_detail::to_lower_ascii(module.name) == query) return &module;
    }
    return nullptr;
}

inline std::string module_suggestions(const sapi::SapiIndex& index,
                                      const std::string& module_name,
                                      int limit = 8) {
    std::string needle = command_parser_detail::to_lower_ascii(module_name);
    std::ostringstream out;
    int count = 0;
    for (const auto& module : index.modules) {
        std::string hay = command_parser_detail::to_lower_ascii(module.name);
        if (hay.find(needle) == std::string::npos && needle.find(hay) == std::string::npos)
            continue;
        if (count == 0) out << "Did you mean:\n";
        out << "- " << module.name << " (" << module.symbol_ids.size() << " symbols)\n";
        if (++count >= limit) break;
    }
    return out.str();
}

inline std::string first_doc_line(const std::string& doc) {
    size_t pos = doc.find('\n');
    std::string line = pos == std::string::npos ? doc : doc.substr(0, pos);
    if (line.size() > 140) {
        line.resize(137);
        line += "...";
    }
    return line;
}

inline mcp::json format_module_compact(const sapi::SapiIndex& index,
                                       const sapi::SapiModule& module,
                                       int offset,
                                       int limit) {
    int total = static_cast<int>(module.symbol_ids.size());
    if (offset >= total) {
        std::ostringstream empty;
        empty << "Module " << module.name << " has " << total
              << " symbols. Offset " << offset << " is out of range.";
        return text_result(empty.str());
    }

    int end = std::min(total, offset + limit);
    std::ostringstream out;
    out << "module: " << module.name << "\n"
        << "source: " << module.source_file << "\n"
        << "symbols: " << total << "\n"
        << "range: [" << offset << ", " << end << ")\n\n";

    for (int ordinal = offset; ordinal < end; ++ordinal) {
        int id = module.symbol_ids[ordinal];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& symbol = index.symbols[id];
        out << (ordinal + 1) << ". " << symbol.kind << " " << symbol.name;
        if (!symbol.members.empty()) out << " members=" << symbol.members.size();
        out << " line=" << symbol.line_start << "\n";
        if (!symbol.signature.empty()) out << "   " << symbol.signature << "\n";
        if (!symbol.doc.empty()) out << "   doc: " << first_doc_line(symbol.doc) << "\n";
    }

    if (end < total) {
        out << "\nNext page: module " << module.name
            << " --offset " << end << " --limit " << limit << "\n";
    }
    out << "Use --detail true with a small --limit when full signatures/docs/refs are needed.";
    return text_result(out.str());
}

inline mcp::json dispatch_search(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("search requires a query.");
    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 8);
    int requested = std::clamp(offset + limit, 1, 300);
    auto results = sapi::search_sapi_symbols(index, pc.positional, requested, clamped_refs(pc));
    if (offset >= static_cast<int>(results.size()))
        return text_result("No more SAPI symbols matched the query at offset " + std::to_string(offset) + ".");
    int end = std::min(static_cast<int>(results.size()), offset + limit);
    std::vector<sapi::SapiSearchResult> page(results.begin() + offset, results.begin() + end);
    return format_results(index, page, "No SAPI symbol matched the query.",
                          clamped_members(pc, 12), clamped_member_offset(pc), offset);
}

inline mcp::json dispatch_symbol(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("symbol requires a symbol name.");
    auto results = sapi::lookup_sapi_symbol(index, pc.positional, clamped_refs(pc));
    if (results.empty())
        results = sapi::search_sapi_symbols(index, pc.positional, clamped_top(pc, 8), clamped_refs(pc));
    return format_results(index, results, "No SAPI symbol matched the name.",
                          clamped_members(pc, 32), clamped_member_offset(pc));
}

inline std::vector<std::string> reverse_ref_names(const sapi::SapiIndex& index,
                                                  const ParsedCommand& pc,
                                                  std::unordered_set<int>& target_ids) {
    std::vector<std::string> names;
    auto targets = sapi::lookup_sapi_symbol(index, pc.positional, 0);
    for (const auto& target : targets) {
        if (target.symbol_id < 0 || target.symbol_id >= static_cast<int>(index.symbols.size()))
            continue;
        target_ids.insert(target.symbol_id);
        const auto& symbol = index.symbols[target.symbol_id];
        names.push_back(symbol.name);
        names.push_back(symbol.fqname);
        size_t dot = symbol.fqname.rfind('.');
        if (dot != std::string::npos) names.push_back(symbol.fqname.substr(dot + 1));
    }
    if (names.empty()) names.push_back(pc.positional);

    std::vector<std::string> out;
    std::unordered_set<std::string> seen;
    for (auto& name : names) {
        name = command_parser_detail::to_lower_ascii(name);
        if (!name.empty() && seen.insert(name).second) out.push_back(std::move(name));
    }
    return out;
}

inline bool text_references_any(const std::string& text, const std::vector<std::string>& names) {
    std::unordered_set<std::string> wanted;
    for (const auto& name : names) {
        size_t dot = name.rfind('.');
        std::string simple = dot == std::string::npos ? name : name.substr(dot + 1);
        if (!simple.empty()) wanted.insert(command_parser_detail::to_lower_ascii(simple));
    }
    if (wanted.empty()) return false;

    for (size_t pos = 0; pos < text.size();) {
        unsigned char ch = static_cast<unsigned char>(text[pos]);
        if (!(std::isalpha(ch) || text[pos] == '_' || text[pos] == '$')) {
            ++pos;
            continue;
        }
        size_t begin = pos++;
        while (pos < text.size()) {
            unsigned char next = static_cast<unsigned char>(text[pos]);
            if (!(std::isalnum(next) || text[pos] == '_' || text[pos] == '$')) break;
            ++pos;
        }
        std::string token = text.substr(begin, pos - begin);
        if (!std::isupper(static_cast<unsigned char>(token[0]))) continue;
        if (wanted.count(command_parser_detail::to_lower_ascii(token))) return true;
    }
    return false;
}

inline mcp::json dispatch_reverse_refs(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("refs requires a symbol name, for example SecretString.");

    std::unordered_set<int> target_ids;
    auto names = reverse_ref_names(index, pc, target_ids);
    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 30);
    std::vector<sapi::SapiSearchResult> matches;

    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        if (target_ids.count(i)) continue;
        const auto& symbol = index.symbols[i];
        if (text_references_any(symbol.signature, names)) {
            matches.push_back({i, -1, 100.0, "used-by", {}});
        }
        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            if (text_references_any(symbol.members[m].signature, names)) {
                matches.push_back({i, m, 100.0, "used-by", {}});
            }
        }
    }

    if (matches.empty())
        return text_result("No reverse references found for: " + pc.positional);

    int total = static_cast<int>(matches.size());
    if (offset >= total) {
        std::ostringstream out;
        out << "Reverse refs for " << pc.positional << ": " << total
            << " matches. Offset " << offset << " is out of range.";
        return text_result(out.str());
    }

    int end = std::min(total, offset + limit);
    std::ostringstream out;
    out << "reverse refs: " << pc.positional << "\n"
        << "matches: " << total << "\n"
        << "range: [" << offset << ", " << end << ")\n\n";
    for (int i = offset; i < end; ++i) {
        const auto& match = matches[i];
        const auto& symbol = index.symbols[match.symbol_id];
        out << (i + 1) << ". ";
        if (match.member_index >= 0 && match.member_index < static_cast<int>(symbol.members.size())) {
            const auto& member = symbol.members[match.member_index];
            out << member.fqname << "\n"
                << "   kind: " << member.kind << "\n"
                << "   signature: " << member.signature << "\n";
        } else {
            out << symbol.fqname << "\n"
                << "   kind: " << symbol.kind << "\n"
                << "   signature: " << symbol.signature << "\n";
        }
    }
    if (end < total)
        out << "\nNext page: refs " << pc.positional << " --offset " << end
            << " --limit " << limit;
    return text_result(out.str());
}

inline mcp::json dispatch_module(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("module requires a module name, for example @minecraft/server.");

    std::string module_name = pc.positional;
    const sapi::SapiModule* module = find_module_exact(index, module_name);
    if (!module) {
        std::string hint = module_suggestions(index, module_name);
        return text_result("No SAPI module matched the name: " + module_name +
                           (hint.empty() ? "" : "\n\n" + hint));
    }

    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 30);
    bool detail = flag_bool(pc, "detail", false) || flag_bool(pc, "full", false);
    if (!detail)
        return format_module_compact(index, *module, offset, limit);

    int refs = clamped_refs(pc, 0);
    std::vector<sapi::SapiSearchResult> results;
    int total = static_cast<int>(module->symbol_ids.size());
    int end = std::min(total, offset + limit);
    for (int ordinal = offset; ordinal < end; ++ordinal) {
        int id = module->symbol_ids[ordinal];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& symbol = index.symbols[id];
        sapi::SapiSearchResult result;
        result.symbol_id = id;
        result.member_index = -1;
        result.score = 100.0;
        result.match_kind = "module";
        if (refs > 0) {
            auto expanded = sapi::lookup_sapi_symbol(index, symbol.fqname, refs);
            if (!expanded.empty()) result.referenced_symbols = std::move(expanded.front().referenced_symbols);
        }
        results.push_back(std::move(result));
    }
    return format_results(index, results, "No SAPI module matched the name.", clamped_members(pc, 12));
}

inline mcp::json dispatch_minecraft_sapi(const sapi::SapiIndex& index, const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = command_parser_detail::to_lower_ascii(pc.sub);

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "search" || sub == "find")
        return dispatch_search(index, pc);
    if (sub == "symbol" || sub == "sym" || sub == "member")
        return dispatch_symbol(index, pc);
    if (sub == "refs" || sub == "used-by" || sub == "usages")
        return dispatch_reverse_refs(index, pc);
    if (sub == "module" || sub == "mod")
        return dispatch_module(index, pc);

    return error_with_help("Unknown minecraft_sapi subcommand: '" + pc.sub + "'.");
}

} // namespace minecraft_sapi_detail

inline void register_minecraft_sapi_tools(mcp::server& srv,
                                          std::shared_ptr<sapi::SapiIndex> sapi_index) {
    auto tool = mcp::tool_builder(minecraft_sapi_detail::kToolName)
        .with_description(
            "Minecraft Bedrock SAPI/Script API symbol database. "
            "网易版MC通常用 Py ModSDK，请优先查 minecraft_docs；"
            "仅项目明确使用 SAPI/Script API/国际版脚本时调用。")
        .with_string_param("command",
            "命令，如 'search spawn entity --refs 1', 'symbol Player --refs 1', "
            "或 'module @minecraft/server --limit 30'；首次用 'help'。", true)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(
        tool,
        [sapi_index](const mcp::json& params, const std::string&) -> mcp::json {
            return minecraft_sapi_detail::dispatch_minecraft_sapi(
                *sapi_index,
                params.value("command", ""));
        });
}

} // namespace mcdk
