#include "app/server_runtime.hpp"

#include "common/path_utils.hpp"
#include "search/search_service.hpp"
#include "tools/register_netease.hpp"
#include "tools/register_search.hpp"

#ifndef MCDK_LITE
#include "tools/register_animation.hpp"
#include "tools/register_jsonui.hpp"
#include "tools/register_model.hpp"
#include "tools/register_pixel_art.hpp"
#endif

#ifdef MCDK_SERVER
#include "server/tool_stats.hpp"
#endif

#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace mcdk::app {

bool init_console_encoding() {
#ifdef _WIN32
    // 这里只负责控制台 UTF-8，文件路径编码由 path_utils 统一处理。
    return SetConsoleOutputCP(65001) != 0 && SetConsoleCP(65001) != 0;
#else
    return true;
#endif
}

mcp::server::configuration make_server_config() {
    mcp::server::configuration conf;
    conf.port = 18766;
#ifdef MCDK_SERVER
    conf.host = "0.0.0.0";
    conf.name = "mcdk-assistant-server";
    conf.version = "0.5.0-server";
#elif defined(MCDK_LITE)
    conf.host = "127.0.0.1";
    conf.name = "mcdk-asst-lite";
    conf.version = "0.5.0-lite";
#else
    conf.host = "127.0.0.1";
    conf.name = "mcdk-assistant";
    conf.version = "0.5.0";
#endif
    conf.threadpool_size = std::max(std::thread::hardware_concurrency(), 6u);
    return conf;
}

void register_tools(mcp::server& srv,
                    mcdk::SearchService& search_svc,
                    const std::filesystem::path& knowledge_dir,
                    bool cache_only_mode) {
    // 缓存模式下禁用磁盘 knowledge 根目录，让工具自动回退到缓存读取。
    const std::filesystem::path effective_knowledge_dir = cache_only_mode
        ? std::filesystem::path()
        : knowledge_dir;

    mcdk::register_search_tools(srv, search_svc, effective_knowledge_dir);
    mcdk::register_netease_tools(srv);
#ifndef MCDK_LITE
    mcdk::register_jsonui_tools(srv);
    mcdk::register_pixel_art_tools(srv);
    mcdk::register_model_tools(srv);
    mcdk::register_animation_tools(srv);
#endif
}

void log_startup_banner(const mcp::server::configuration& conf,
                        const mcdk::SearchService& search_svc,
                        bool cache_only_mode) {
#ifdef MCDK_SERVER
    std::cerr << "[MCDK-Server] ";
#elif defined(MCDK_LITE)
    std::cerr << "[MCDK-Lite] ";
#else
    std::cerr << "[MCDK] ";
#endif
    std::cerr << "MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cerr << "[MCDK] docs indexed: " << search_svc.doc_count() << std::endl;
    std::cerr << "[MCDK] game assets indexed: " << search_svc.game_assets_count() << std::endl;
    if (cache_only_mode) {
        std::cerr << "[MCDK] 已解析缓存索引库（缓存模式）" << std::endl;
    }
}

void register_server_endpoints(mcp::server& srv,
                               const std::filesystem::path& exe_dir,
                               int port) {
#ifdef MCDK_SERVER
    // 统计模块只在 server 版启用，lite/full 保持最小启动面。
    auto datas_dir = exe_dir / mcdk::server::DATAS_DIR;
    auto stats = std::make_shared<mcdk::server::ToolStats>(datas_dir);
    std::cerr << "[MCDK-Server] 统计模块已启动，数据目录: "
              << mcdk::path::to_utf8(datas_dir) << std::endl;

    srv.set_tool_call_handler([stats](const std::string& tool_name) {
        stats->increment(tool_name);
    });

    srv.register_http_get("/api/tool-stats",
        [stats](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Cache-Control", "no-cache");

            auto snap = stats->get_snapshot();
            std::sort(snap.begin(), snap.end(),
                [](const mcdk::server::ToolStats::Snapshot& a,
                   const mcdk::server::ToolStats::Snapshot& b) {
                    return a.total > b.total;
                });

            uint64_t grand_total = 0;
            nlohmann::json tools_arr = nlohmann::json::array();
            for (const auto& s : snap) {
                grand_total += s.total;
                tools_arr.push_back({{"tool", s.tool}, {"count", s.total}});
            }
            nlohmann::json resp = {
                {"total_requests", grand_total},
                {"tools", tools_arr}
            };
            res.set_content(resp.dump(2), "application/json");
            res.status = 200;
        });

    std::cerr << "[MCDK-Server] 统计查询端点已注册: GET http://127.0.0.1:"
              << port << "/api/tool-stats" << std::endl;
#else
    (void)srv;
    (void)exe_dir;
    (void)port;
#endif
}

} // namespace mcdk::app

