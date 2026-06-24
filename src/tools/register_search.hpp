#pragma once
// register_search.hpp — 搜索类 & 知识库 处理逻辑（handler）
//
// 【已合并】原先这里直接注册的 8 个 search_* 工具 + search_game_assets +
// read_knowledge + list_knowledge，现已统一并入 minecraft_docs 单工具
// （见 register_minecraft_docs.hpp）。本文件保留各自的核心 handler 自由函数，
// 由 minecraft_docs 路由器复用；下方 register_search_tools() 的注册代码整段注释保留，
// 便于随时审视 / 回滚。
//
// 暴露给路由器复用的 handler：
//   handle_search()              —— 文档检索（配合 SearchFn 指定分区）
//   handle_search_game_assets()  —— 原版游戏资产模糊搜索
//   handle_read_knowledge()      —— 读取 knowledge 目录文件
//   handle_list_knowledge()      —— 列出 knowledge 目录内容
#include "search/search_service.hpp"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <fstream>
#include <climits>
#include <filesystem>
#include <string>
#include <vector>

#include "common/path_utils.hpp"

namespace mcdk {

using SearchFn = std::vector<SearchResult>(SearchService::*)(const std::string&, int) const;

// ── 文档检索 handler（按 SearchFn 选择分区/聚合搜索）──
inline mcp::json handle_search(SearchService& svc, SearchFn fn, const mcp::json& params) {
    std::string keyword = params.value("keyword", "");
    if (keyword.empty())
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "keyword is required");
    int top_k = params.contains("top_k") && !params["top_k"].is_null()
        ? params["top_k"].get<int>() : 6;

    auto results = (svc.*fn)(keyword, top_k);

    mcp::json arr = mcp::json::array();
    for (const auto& r : results)
        arr.push_back({{"type","text"},{"text",r.fragment->content},
            {"file",r.fragment->file},{"line_start",r.fragment->line_start},
            {"line_end",r.fragment->line_end},{"score",r.score}});
    return {{"content", arr}};
}

// ── 原版游戏资产模糊搜索 handler ──
// params: keyword(必填), scope(0=全部/1=行为包/2=资源包, 默认0), top_k(默认6)
inline mcp::json handle_search_game_assets(SearchService& search_svc, const mcp::json& params) {
    std::string keyword = params.value("keyword", "");
    if (keyword.empty())
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "keyword is required");
    int scope = params.contains("scope") && !params["scope"].is_null()
        ? params["scope"].get<int>() : 0;
    int top_k = params.contains("top_k") && !params["top_k"].is_null()
        ? params["top_k"].get<int>() : 6;

    auto results = search_svc.search_game_assets(keyword, scope, top_k);
    mcp::json arr = mcp::json::array();
    for (const auto& r : results)
        arr.push_back({{"type","text"},{"text",r.snippet},
            {"file",r.rel_path},{"score",r.score}});
    return {{"content", arr}};
}

// ── 读取 knowledge 文件 handler ──
// knowledge_root 为空（缓存模式）时自动回退到索引缓存读取。
inline mcp::json handle_read_knowledge(const std::filesystem::path& knowledge_root,
                                       SearchService& search_svc, const mcp::json& params) {
    std::string rel = params.value("path", "");
    if (rel.empty())
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "path is required");
    if (rel.find("..") != std::string::npos)
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "path must not contain '..'");

    int ls = params.contains("line_start") && !params["line_start"].is_null()
        ? params["line_start"].get<int>() : 1;
    int le = params.contains("line_end") && !params["line_end"].is_null()
        ? params["line_end"].get<int>() : INT_MAX;
    if (ls < 1) ls = 1;
    if (le < ls) le = ls;

    if (!knowledge_root.empty()) {
        auto full = knowledge_root / mcdk::path::from_utf8(rel);
        std::ifstream ifs(full);
        if (ifs.is_open()) {
            std::string result, line; int cur = 0;
            while (std::getline(ifs, line)) {
                ++cur;
                if (cur < ls) continue;
                if (cur > le) break;
                result += line; result += '\n';
            }
            return {{"content", mcp::json::array({
                {{"type","text"},{"text",result},{"file",rel},
                 {"line_start",ls},{"line_end",std::min(cur,le)},{"total_lines",cur}}
            })}};
        }
    }

    auto cached_result = search_svc.read_cached_file(rel, ls, le);
    if (!cached_result.found)
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "file not found: " + rel);

    return {{"content", mcp::json::array({
        {{"type","text"},{"text",cached_result.content},{"file",rel},
         {"line_start",ls},{"line_end",std::min(cached_result.total_lines,le)},
         {"total_lines",cached_result.total_lines},{"source","cache"}}
    })}};
}

// ── 列出 knowledge 目录 handler ──
inline mcp::json handle_list_knowledge(const std::filesystem::path& knowledge_root,
                                       SearchService& search_svc, const mcp::json& params) {
    namespace fs = std::filesystem;
    std::string rel = params.value("path", "");
    if (rel.find("..") != std::string::npos)
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "path must not contain '..'");

    if (!knowledge_root.empty()) {
        fs::path dir = knowledge_root / (rel.empty() ? fs::path() : mcdk::path::from_utf8(rel));
        if (fs::exists(dir) && fs::is_directory(dir)) {
            mcp::json dirs = mcp::json::array(), files = mcp::json::array();
            for (const auto& entry : fs::directory_iterator(dir)) {
                std::string s = mcdk::path::filename_to_utf8(entry.path());
                if (entry.is_directory()) dirs.push_back(s);
                else files.push_back(s);
            }
            std::string text = "目录: " + (rel.empty() ? "/" : rel) + "\n";
            for (const auto& d : dirs)  text += "[DIR]  " + d.get<std::string>() + "\n";
            for (const auto& f : files) text += "       " + f.get<std::string>() + "\n";
            return {{"content", mcp::json::array({{{"type","text"},{"text",text}}})}};
        }
    }

    auto cached_list = search_svc.list_cached_files(rel);
    if (!cached_list.found)
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "directory not found: " + rel);

    std::string text = "目录: " + (rel.empty() ? "/" : rel) + " (from cache)\n";
    for (const auto& d : cached_list.dirs)  text += "[DIR]  " + d + "\n";
    for (const auto& f : cached_list.files) text += "       " + f + "\n";
    return {{"content", mcp::json::array({{{"type","text"},{"text",text}}})}};
}

// ──────────────────────────────────────────────────────────────────────────
// 【已合并到 minecraft_docs — 注册代码整段注释保留，便于随时审视/回滚】
//
// 原 register_search_tools() 会注册以下独立工具，现统一由 minecraft_docs 的
// search / read / list 子命令承担（复用上方同一批 handler）：
//   search_api / search_event / search_enum / search_all / search_wiki /
//   search_bedrock_dev / search_qumod / search_netease_guide /
//   search_game_assets / read_knowledge / list_knowledge
//
// inline void register_search_tools(mcp::server& srv, SearchService& search_svc,
//                                    const std::filesystem::path& knowledge_dir = {}) {
//     const std::filesystem::path knowledge_root = knowledge_dir;
//
//     struct ToolDef { const char* name; const char* desc; SearchFn fn; };
//     static const ToolDef tools[] = {
//         {"search_api",   "搜索 ModAPI 接口文档", &SearchService::search_api},
//         {"search_event", "搜索 ModAPI 事件文档", &SearchService::search_event},
//         {"search_enum",  "搜索 ModAPI 枚举值文档", &SearchService::search_enum},
//         {"search_all",   "搜索全部 ModAPI 文档（接口+事件+枚举值）", &SearchService::search_all},
//         {"search_wiki",  "Search Bedrock Wiki documentation (English keywords)", &SearchService::search_wiki},
//         {"search_bedrock_dev",
//             "Search Bedrock Edition official format documentation from bedrock.dev 1.21.90 "
//             "(Addons/Animations/Biomes/Blocks/Entities/Features/Fogs/Items/Molang/Particles/Recipes/Schemas/TextureSets/Volumes). "
//             "Use this for schema definitions, component properties, and official format specs (English keywords).",
//             &SearchService::search_bedrock_dev},
//         {"search_qumod",
//             "搜索 QuModLibs 框架库文档（QuMod是网易流行的热门框架库，当用户使用QuMod开发时应优先查找此处功能/设计规范），"
//             "注意：文档本身并不全面，当用户项目中存在QuModLibs包时还应直接分析其源代码",
//             &SearchService::search_qumod},
//         {"search_netease_guide", "搜索网易MC独占的教学资料内容（不包含国际版通用内容）",
//             &SearchService::search_netease_guide},
//     };
//
//     for (const auto& td : tools) {
//         auto tool = mcp::tool_builder(td.name)
//             .with_description(td.desc)
//             .with_string_param("keyword", "搜索关键词", true)
//             .with_number_param("top_k", "返回结果数量上限，默认返回 6 个", false)
//             .with_read_only_hint(true).with_idempotent_hint(true).build();
//         auto fn = td.fn;
//         srv.register_tool(tool,
//             [&search_svc, fn](const mcp::json& params, const std::string&) -> mcp::json {
//                 return handle_search(search_svc, fn, params);
//             });
//     }
//
//     {
//         auto tool = mcp::tool_builder("search_game_assets")
//             .with_description(
//                 "模糊搜索原版 Minecraft 游戏资产文件（行为包/资源包），同时匹配文件路径名与文件内容。"
//                 "scope: 0=搜索全部资产, 1=仅搜索行为包(behavior_packs), 2=仅搜索资源包(resource_packs)")
//             .with_string_param("keyword", "搜索关键词（支持文件名片段或文件内容关键词）", true)
//             .with_number_param("scope",   "搜索范围：0=全部（默认），1=仅行为包，2=仅资源包", false)
//             .with_number_param("top_k",   "返回结果数量上限，默认返回 6 个", false)
//             .with_read_only_hint(true).with_idempotent_hint(true).build();
//         srv.register_tool(tool,
//             [&search_svc](const mcp::json& params, const std::string&) -> mcp::json {
//                 return handle_search_game_assets(search_svc, params);
//             });
//     }
//
//     {
//         auto tool = mcp::tool_builder("read_knowledge")
//             .with_description("读取 knowledge 目录下的指定文件内容，搜索结果中的 file 字段可直接作为 path 参数传入")
//             .with_string_param("path",       "文件相对路径（相对于 knowledge 目录），如 BedrockWiki/items/items-intro.md", true)
//             .with_number_param("line_start", "起始行号（1-based），不传则从第1行开始", false)
//             .with_number_param("line_end",   "结束行号（1-based，含），不传则读到文件末尾", false)
//             .with_read_only_hint(true).with_idempotent_hint(true).build();
//         srv.register_tool(tool,
//             [knowledge_root, &search_svc](const mcp::json& params, const std::string&) -> mcp::json {
//                 return handle_read_knowledge(knowledge_root, search_svc, params);
//             });
//     }
//
//     {
//         auto tool = mcp::tool_builder("list_knowledge")
//             .with_description("列出 knowledge 目录下指定路径的文件和文件夹列表")
//             .with_string_param("path", "相对路径（相对于 knowledge 目录），默认为根目录", false)
//             .with_read_only_hint(true).with_idempotent_hint(true).build();
//         srv.register_tool(tool,
//             [knowledge_root, &search_svc](const mcp::json& params, const std::string&) -> mcp::json {
//                 return handle_list_knowledge(knowledge_root, search_svc, params);
//             });
//     }
// }
// ──────────────────────────────────────────────────────────────────────────

} // namespace mcdk
