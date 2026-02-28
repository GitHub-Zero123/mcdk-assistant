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

    std::cout << "[MCDK] MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cout << "[MCDK] docs indexed: " << search_svc.doc_count() << std::endl;

    srv.start(true);

    return 0;
}
