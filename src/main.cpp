#include "search/search_service.h"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <iostream>
#include <string>
#include <filesystem>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static std::string get_exe_dir() {
    namespace fs = std::filesystem;
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return fs::path(buf).parent_path().string();
#else
    return fs::canonical("/proc/self/exe").parent_path().string();
#endif
}

// 通用搜索工具处理函数
using SearchFn = std::vector<mcdk::SearchResult>(mcdk::SearchService::*)(const std::string&, int) const;

static mcp::json handle_search(mcdk::SearchService& svc, SearchFn fn, const mcp::json& params) {
    std::string keyword = params.value("keyword", "");
    if (keyword.empty()) {
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "keyword is required");
    }
    int top_k = params.contains("top_k") && !params["top_k"].is_null() ? params["top_k"].get<int>() : -1;

    auto results = (svc.*fn)(keyword, top_k);

    mcp::json content_arr = mcp::json::array();
    for (const auto& r : results) {
        content_arr.push_back({
            {"type", "text"}, {"text", r.fragment->content},
            {"file", r.fragment->file}, {"line_start", r.fragment->line_start},
            {"line_end", r.fragment->line_end}, {"score", r.score}
        });
    }
    return {{"content", content_arr}};
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::string exe_dir = get_exe_dir();
    std::string dicts_dir     = exe_dir + "/dicts";
    std::string knowledge_dir = exe_dir + "/knowledge";

    std::cout << "[MCDK] dicts: " << dicts_dir << std::endl;
    std::cout << "[MCDK] knowledge: " << knowledge_dir << std::endl;

    mcdk::SearchService search_svc(dicts_dir, knowledge_dir);

    mcp::server::configuration conf;
    conf.host    = "127.0.0.1";
    conf.port    = 18766;
    conf.name    = "mcdk-assistant";
    conf.version = "0.1.0";

    mcp::server srv(conf);

    // 注册3个分类搜索工具
    struct ToolDef {
        const char* name;
        const char* desc;
        SearchFn    fn;
    };
    ToolDef tools[] = {
        {"search_api",   "搜索 ModAPI 接口文档", &mcdk::SearchService::search_api},
        {"search_event", "搜索 ModAPI 事件文档", &mcdk::SearchService::search_event},
        {"search_enum",  "搜索 ModAPI 枚举值文档", &mcdk::SearchService::search_enum},
        {"search_all",   "搜索全部 ModAPI 文档（接口+事件+枚举值）", &mcdk::SearchService::search_all},
        {"search_wiki",  "Search Bedrock Wiki documentation (English keywords)", &mcdk::SearchService::search_wiki},
    };

    for (auto& td : tools) {
        auto tool = mcp::tool_builder(td.name)
            .with_description(td.desc)
            .with_string_param("keyword", "搜索关键词", true)
            .with_number_param("top_k", "返回结果数量上限，默认返回全部", false)
            .with_read_only_hint(true)
            .with_idempotent_hint(true)
            .build();

        auto fn = td.fn;
        srv.register_tool(tool,
            [&search_svc, fn](const mcp::json& params, const std::string&) -> mcp::json {
                return handle_search(search_svc, fn, params);
            }
        );
    }

    // 网易版与国际版差异说明工具
    static const char* NETEASE_DIFF_TEXT =
        "网易中国版基岩版与国际版基岩版的关键差异（当Wiki文档提到以下国际版路径时，请自动替换为网易版路径）：\n"
        "\n"
        "[行为包目录映射]\n"
        "国际版 BP/items/        -> 网易版 BP/netease_items_beh/\n"
        "国际版 BP/blocks/       -> 网易版 BP/netease_blocks/\n"
        "国际版 BP/biomes/       -> 网易版 BP/netease_biomes/\n"
        "国际版 BP/features/     -> 网易版 BP/netease_features/\n"
        "国际版 BP/feature_rules/-> 网易版 BP/netease_feature_rules/\n"
        "国际版 BP/recipes/      -> 网易版 BP/netease_recipes/\n"
        "\n"
        "[资源包目录映射]\n"
        "国际版 RP/items/        -> 网易版 RP/netease_items_res/\n"
        "\n"
        "[脚本系统]\n"
        "国际版使用 Script API（JavaScript/TypeScript），网易版使用 Mod SDK（Python2）。\n"
        "Wiki中涉及 Script API / @minecraft/server 的内容不适用于网易版，网易版应使用 mod.server.extraServerApi / mod.client.extraClientApi。\n"
        "\n"
        "[开发框架]\n"
        "如果项目中存在 QuModLibs 库，优先使用 QuModLibs 的框架模块来简化网易Mod开发。";

    auto diff_tool = mcp::tool_builder("get_netease_diff")
        .with_description("获取网易版与国际版基岩版之间的差异说明")
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(diff_tool, [](const mcp::json&, const std::string&) -> mcp::json {
        return {{"content", mcp::json::array({{{"type","text"},{"text", NETEASE_DIFF_TEXT}}})}};
    });

    std::cout << "[MCDK] MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cout << "[MCDK] docs indexed: " << search_svc.doc_count() << std::endl;

    srv.start(true);

    return 0;
}
