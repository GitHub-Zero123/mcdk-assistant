#include "search/search_service.hpp"
#include "search/index_cache.hpp"
#include "tools/register_search.hpp"
#include "tools/register_netease.hpp"
#ifndef MCDK_LITE
#include "tools/register_jsonui.hpp"
#include "tools/register_pixel_art.hpp"
#include "tools/register_model.hpp"
#include "tools/register_animation.hpp"
#endif
#ifdef MCDK_SERVER
#include "server/tool_stats.hpp"
#endif
#include <mcp_server.h>
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include <thread>
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif


static std::filesystem::path get_exe_dir() {
    namespace fs = std::filesystem;
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return fs::path(buf).parent_path();
#else
    return fs::canonical("/proc/self/exe").parent_path();
#endif
}

static std::string fsToUtf8(const std::filesystem::path& p) {
    auto u8 = p.generic_u8string();
    return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    namespace fs = std::filesystem;

    auto exe_dir       = get_exe_dir();
    auto dicts_dir     = exe_dir / "dicts";
    auto knowledge_dir = exe_dir / "knowledge";
    auto cache_path    = exe_dir / "mcdk_index_cache.bin";
    std::string knowledge_dir_ansi = knowledge_dir.string();
    std::string cache_path_ansi    = cache_path.string();
    std::string dicts_dir_ansi     = dicts_dir.string();

    bool has_dicts     = fs::exists(dicts_dir);
    bool has_knowledge = fs::exists(knowledge_dir);
    bool has_cache     = fs::is_regular_file(cache_path);

    std::cout << "[MCDK] dicts: "     << fsToUtf8(dicts_dir)     << (has_dicts     ? "" : " (NOT FOUND)") << std::endl;
    std::cout << "[MCDK] knowledge: " << fsToUtf8(knowledge_dir) << (has_knowledge ? "" : " (NOT FOUND)") << std::endl;
    std::cout << "[MCDK] cache: "     << fsToUtf8(cache_path)    << (has_cache     ? "" : " (NOT FOUND)") << std::endl;

    if (!has_dicts) {
        std::cerr << "[MCDK] 错误：词库目录不存在: " << fsToUtf8(dicts_dir) << std::endl;
        return 1;
    }

    bool cache_only_mode = !has_knowledge && has_cache;

    if (cache_only_mode) {
        std::cout << "[MCDK] 缓存解析：无外部资料，正在从索引文件解析..." << std::endl;
    } else if (!has_knowledge && !has_cache) {
        std::cerr << "[MCDK] 错误：缺少知识库目录和缓存文件，无法启动。" << std::endl;
        return 1;
    } else {
        std::cout << "[MCDK] 正在初始化知识库索引，请稍候..." << std::endl;
    }

    std::unique_ptr<mcdk::SearchService> search_svc;
    if (cache_only_mode) {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir_ansi, cache_path_ansi, /*cache_only=*/true);
    } else {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir_ansi, knowledge_dir_ansi, cache_path_ansi);
    }

    // ── 工具调用统计（仅 MCDK_SERVER 模式）───────────────────────────────────
#ifdef MCDK_SERVER
    auto datas_dir = exe_dir / mcdk::server::DATAS_DIR;
    auto stats     = std::make_shared<mcdk::server::ToolStats>(datas_dir);
    std::cout << "[MCDK-Server] 统计模块已启动，数据目录: " << fsToUtf8(datas_dir) << std::endl;
#endif

    mcp::server::configuration conf;
    conf.port = 18766;
#ifdef MCDK_SERVER
    // 服务器模式（同时隐含 MCDK_LITE，屏蔽高风险本地文件工具）
    // 监听所有接口，供外部 AI 工具直接调用
    conf.host    = "0.0.0.0";
    conf.name    = "mcdk-assistant-server";
    conf.version = "0.5.0-server";
#elif defined(MCDK_LITE)
    conf.host    = "127.0.0.1";
    conf.name    = "mcdk-asst-lite";
    conf.version = "0.5.0-lite";
#else
    conf.host    = "127.0.0.1";
    conf.name    = "mcdk-assistant";
    conf.version = "0.5.0";
#endif
    // 线程池最少 6 线程，避免 2 核 ECS 上只有 2 线程导致耗时工具阻塞搜索请求
    conf.threadpool_size = std::max(std::thread::hardware_concurrency(), 6u);

    mcp::server srv(conf);

    // ── 注册工具调用统计钩子（在所有工具注册前，server 启动前）────────────────
#ifdef MCDK_SERVER
    srv.set_tool_call_handler([stats](const std::string& tool_name) {
        stats->increment(tool_name);
    });
#endif

    std::string effective_knowledge_dir = cache_only_mode ? "" : knowledge_dir.string();
    mcdk::register_search_tools(srv, *search_svc, effective_knowledge_dir);
    mcdk::register_netease_tools(srv);
#ifndef MCDK_LITE
    mcdk::register_jsonui_tools(srv);
    mcdk::register_pixel_art_tools(srv);
    mcdk::register_model_tools(srv);
    mcdk::register_animation_tools(srv);
#endif

#ifdef MCDK_SERVER
    // MCDK_SERVER 隐含 MCDK_LITE，必须先检查 SERVER
    std::cout << "[MCDK-Server] ";
#elif defined(MCDK_LITE)
    std::cout << "[MCDK-Lite] ";
#else
    std::cout << "[MCDK] ";
#endif
    std::cout << "MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cout << "[MCDK] docs indexed: "        << search_svc->doc_count()         << std::endl;
    std::cout << "[MCDK] game assets indexed: " << search_svc->game_assets_count() << std::endl;
    if (cache_only_mode) {
        std::cout << "[MCDK] 已解析缓存索引库（缓存模式）" << std::endl;
    }

    // ── 统计查询端点（仅 MCDK_SERVER，挂载到 MCP 同一端口）──────────────────
    // GET /api/tool-stats 与 MCP 协议共用 18766 端口，无需开放额外防火墙规则
#ifdef MCDK_SERVER
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
    std::cout << "[MCDK-Server] 统计查询端点已注册: GET http://"
              << "127.0.0.1:" << conf.port << "/api/tool-stats" << std::endl;
#endif

    srv.start(true); // blocking
    // stats 析构时自动做最后一次 flush

    return 0;
}
