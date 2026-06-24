#pragma once
// register_minecraft_docs.hpp — 资料/文档统一入口工具 `minecraft_docs`
//
// 把原先 13 个资料类工具合并为单一命令式 MCP tool：
// search_* / search_game_assets / read_knowledge / list_knowledge /
// get_netease_diff / get_netease_jsonui。
//
// 本文件只负责命令分发和 JSON 参数适配；检索、读取、速查文本仍复用原 handler。
#include "tools/command_parser.hpp"
#include "tools/register_search.hpp"
#include "tools/register_netease.hpp"
#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <climits>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

namespace mcdk {
namespace minecraft_docs_detail {

constexpr const char* kToolName = "minecraft_docs";

inline SearchFn search_fn_for_scope(const std::string& scope) {
    static const std::unordered_map<std::string, SearchFn> kMap = {
        {"all",     &SearchService::search_all},
        {"api",     &SearchService::search_api},
        {"event",   &SearchService::search_event},
        {"enum",    &SearchService::search_enum},
        {"wiki",    &SearchService::search_wiki},
        {"dev",     &SearchService::search_bedrock_dev},
        {"qumod",   &SearchService::search_qumod},
        {"netease", &SearchService::search_netease_guide},
    };
    auto it = kMap.find(scope);
    return it != kMap.end() ? it->second : nullptr;
}

inline std::string lower_ascii(std::string value) {
    return command_parser_detail::to_lower_ascii(std::move(value));
}

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type","text"},{"text", text}}})}};
}

inline std::string minecraft_docs_help_text() {
    return R"(minecraft_docs — Minecraft 基岩版资料/文档统一入口（命令式用法）
用法: minecraft_docs(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/search minecraft:food --scope wiki，与 help/search 等价。
重要: 如果某个参数本身包含空格，必须用 "..." 或 '...' 包裹，整体才会算一个参数。
示例: /search "custom food item" --scope wiki；/read "BedrockWiki/items/items intro.md" --start 1 --end 20

【search】 文档检索
  search <关键词...> [--scope <分区>] [--top <n>] [--assets <0|1|2>]
  --scope 分区（默认 all）:
    all      全部（ModAPI 接口+事件+枚举 / Wiki / QuMod / 网易教程 / BedrockDev）
    api      ModAPI 接口文档
    event    ModAPI 事件文档
    enum     ModAPI 枚举值文档
    wiki     Bedrock Wiki（英文关键词）
    dev      bedrock.dev 官方格式文档 1.21.90（英文关键词，schema/组件属性）
    qumod    QuModLibs 框架库文档（网易热门框架，用 QuMod 开发时优先查这里）
    netease  网易MC独占教学资料（不含国际版通用内容）
    assets   原版游戏资产（行为包/资源包），配 --assets: 0=全部 1=仅行为包 2=仅资源包
  --top    返回结果数量上限（默认 6；assets 默认更大）
  示例:
    search minecraft:food --scope wiki --top 8
    search "自定义 方块"
    search stair --scope assets --assets 1

【read】 读取 knowledge 文件
  read <path> [--start <n>] [--end <n>]    （path 可直接用搜索结果里的 file 字段）
  示例: read BedrockWiki/items/items-intro.md --start 1 --end 40

【list】 列出 knowledge 目录
  list [path]                              （省略 path 列根目录）
  示例: list BedrockWiki

【netease】 网易版参考速查
  netease diff      网易版 ↔ 国际版关键差异（目录映射/脚本系统/框架/版本兼容）
  netease jsonui    网易版 JSON UI 内置组件库（netease_editor_template_namespace）定义

【help】 显示本帮助
)";
}

inline mcp::json help_result() {
    return text_result(minecraft_docs_help_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + minecraft_docs_help_text());
}

inline bool is_assets_scope(const std::string& scope) {
    return scope == "assets" || scope == "asset" || scope == "ga";
}

inline mcp::json dispatch_search(SearchService& svc, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("缺少搜索关键词。");

    std::string scope = lower_ascii(flag_str(pc, "scope", "all"));

    if (is_assets_scope(scope)) {
        mcp::json params = {
            {"keyword", pc.positional},
            {"scope",   flag_int(pc, "assets", 0)},
        };
        if (has_flag(pc, "top")) params["top_k"] = flag_int(pc, "top", 6);
        return handle_search_game_assets(svc, params);
    }

    SearchFn fn = search_fn_for_scope(scope);
    if (!fn)
        return error_with_help("未知的 --scope: '" + scope + "'。");

    mcp::json params = {
        {"keyword", pc.positional},
        {"top_k",   flag_int(pc, "top", 6)},
    };
    return handle_search(svc, fn, params);
}

inline mcp::json dispatch_read(const std::filesystem::path& knowledge_root,
                               SearchService& svc,
                               const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("缺少文件路径。");

    mcp::json params = {{"path", pc.positional}};
    if (has_flag(pc, "start")) params["line_start"] = flag_int(pc, "start", 1);
    if (has_flag(pc, "end"))   params["line_end"]   = flag_int(pc, "end", INT_MAX);
    return handle_read_knowledge(knowledge_root, svc, params);
}

inline mcp::json dispatch_list(const std::filesystem::path& knowledge_root,
                               SearchService& svc,
                               const ParsedCommand& pc) {
    return handle_list_knowledge(knowledge_root, svc, {{"path", pc.positional}});
}

inline mcp::json dispatch_netease(const ParsedCommand& pc) {
    std::string topic = lower_ascii(pc.positional);
    if (topic.empty()) topic = lower_ascii(flag_str(pc, "type", ""));

    if (topic == "diff")   return text_result(netease_diff_text());
    if (topic == "jsonui") return text_result(netease_jsonui_text());
    return error_with_help("netease 子项应为 'diff' 或 'jsonui'。");
}

inline mcp::json dispatch_minecraft_docs(const std::filesystem::path& knowledge_root,
                                         SearchService& svc,
                                         const std::string& command) {
    ParsedCommand pc = parse_command(command);

    if (pc.sub.empty() || pc.sub == "help" || pc.sub == "?")
        return help_result();
    if (pc.sub == "search")  return dispatch_search(svc, pc);
    if (pc.sub == "read")    return dispatch_read(knowledge_root, svc, pc);
    if (pc.sub == "list")    return dispatch_list(knowledge_root, svc, pc);
    if (pc.sub == "netease") return dispatch_netease(pc);

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_docs_detail

inline void register_minecraft_docs_tools(mcp::server& srv, SearchService& search_svc,
                                          const std::filesystem::path& knowledge_dir = {}) {
    const std::filesystem::path knowledge_root = knowledge_dir;

    auto tool = mcp::tool_builder(minecraft_docs_detail::kToolName)
        .with_description(
            "Minecraft 基岩版 Addon/Mod 资料和文档统一入口（网易版/国际版通用）："
            "文档检索（ModAPI/Wiki/QuMod/BedrockDev/网易教程）、原版资源搜索、"
            "网易版差异速查、知识库文件读取。采用命令式用法，"
            "开发 Minecraft addon/mod 时请先调用 command=\"help\" 查看完整命令与子命令用法。")
        .with_string_param("command",
            "命令语句，如 'search minecraft:food --scope wiki' 或 'read <path>'；"
            "首次使用请传 'help' 查看全部命令。", true)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(tool,
        [knowledge_root, &search_svc](const mcp::json& params, const std::string&) -> mcp::json {
            std::string command = params.value("command", "");
            return minecraft_docs_detail::dispatch_minecraft_docs(knowledge_root, search_svc, command);
        });
}

} // namespace mcdk
