#include "app/runtime_paths.hpp"
#include "app/server_runtime.hpp"
#include "common/console_encoding.hpp"
#include "common/path_utils.hpp"
#include "sapi/sapi_index.hpp"
#include "search/search_service.hpp"
#ifdef MCDK_WITH_PLUGINS
#include "plugins/plugin_manager.h"
#endif
#ifdef MCDK_WITH_SOLUTIONS
#include <mcdk_solutions/solution_index.hpp>
#endif
#include <mcp_server.h>
#ifndef MCDK_SERVER
#include <mcp_stdio_server.h>
#endif
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>

int main(int argc, char* argv[]) {
    mcdk::app::init_console_encoding();

    bool use_stdio = false;
#ifndef MCDK_SERVER
    // 检测 --stdio 参数：使用 stdio 传输模式（兼容 VSCode / Copilot 原生 stdio MCP 接入）
    // 必须在任何 stdout 输出之前完成，避免污染协议流。
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--stdio") {
            use_stdio = true;
            break;
        }
    }
    // stdio 模式下，诊断输出全部走 stderr，stdout 只允许写 JSON-RPC 协议数据。
#define MCDK_LOG (use_stdio ? std::cerr : std::cout)
#else
#define MCDK_LOG std::cout
#endif

    namespace fs = std::filesystem;

    const auto runtime_paths  = mcdk::app::detect_runtime_paths();
    const auto& exe_dir       = runtime_paths.exe_dir;
    const auto& dicts_dir     = runtime_paths.dicts_dir;
    const auto& knowledge_dir = runtime_paths.knowledge_dir;
    const auto& cache_path    = runtime_paths.cache_path;
    const auto sapi_cache_path = mcdk::sapi::default_sapi_cache_path(exe_dir);

    bool has_dicts     = fs::exists(dicts_dir);
    bool has_knowledge = fs::exists(knowledge_dir);
    bool has_cache     = fs::is_regular_file(cache_path);
    bool has_sapi_cache = fs::is_regular_file(sapi_cache_path);

    MCDK_LOG << "[MCDK] dicts: "     << mcdk::path::to_utf8(dicts_dir)     << (has_dicts     ? "" : " (NOT FOUND)") << std::endl;
    MCDK_LOG << "[MCDK] knowledge: " << mcdk::path::to_utf8(knowledge_dir) << (has_knowledge ? "" : " (NOT FOUND)") << std::endl;
    MCDK_LOG << "[MCDK] cache: "     << mcdk::path::to_utf8(cache_path)    << (has_cache     ? "" : " (NOT FOUND)") << std::endl;
    MCDK_LOG << "[MCDK] sapi cache: " << mcdk::path::to_utf8(sapi_cache_path) << (has_sapi_cache ? "" : " (NOT FOUND)") << std::endl;

    if (!has_dicts) {
        std::cerr << "[MCDK] 错误：词库目录不存在: " << mcdk::path::to_utf8(dicts_dir) << std::endl;
        return 1;
    }

    bool cache_only_mode = !has_knowledge && has_cache;

    if (cache_only_mode) {
        MCDK_LOG << "[MCDK] 缓存模式：正在读取索引目录，数据将按需加载..." << std::endl;
    } else if (!has_knowledge && !has_cache) {
#ifndef MCDK_SERVER
        std::cerr << "[MCDK] 错误：缺少知识库目录和缓存文件，无法启动。" << std::endl;
        return 1;
#endif
    } else {
        MCDK_LOG << "[MCDK] 正在读取知识库索引目录..." << std::endl;
    }

    // 提供仅缓存模式和完整模式两套构造函数，以便工程使用/分发版优化启动速度。
    std::unique_ptr<mcdk::SearchService> search_svc;
    if (cache_only_mode) {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir, cache_path, true);
    } else {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir, knowledge_dir, cache_path);
    }

    std::shared_ptr<mcdk::sapi::SapiIndex> sapi_index;
    if (has_sapi_cache) {
        auto loaded = std::make_shared<mcdk::sapi::SapiIndex>();
        std::string error;
        if (mcdk::sapi::load_sapi_cache(sapi_cache_path, *loaded, &error)) {
            sapi_index = std::move(loaded);
            MCDK_LOG << "[MCDK] SAPI symbols indexed: " << sapi_index->symbols.size() << std::endl;
        } else {
            MCDK_LOG << "[MCDK] SAPI cache load failed: " << error << std::endl;
        }
    }

#ifdef MCDK_WITH_SOLUTIONS
    // 运行时闸门：exe 相邻存在解决方案缓存时才加载；缺失则整套 --solution 功能自动不提供。
    std::shared_ptr<mcdk::solutions::SolutionIndex> solutions_index;
    const auto solutions_cache_path = mcdk::solutions::default_solution_cache_path(exe_dir);
    const bool has_solutions_cache = fs::is_regular_file(solutions_cache_path);
    MCDK_LOG << "[MCDK] solutions cache: " << mcdk::path::to_utf8(solutions_cache_path)
             << (has_solutions_cache ? "" : " (NOT FOUND)") << std::endl;
    if (has_solutions_cache) {
        auto loaded = std::make_shared<mcdk::solutions::SolutionIndex>();
        std::string error;
        if (mcdk::solutions::load_solution_cache(solutions_cache_path, *loaded, &error)) {
            solutions_index = std::move(loaded);
            MCDK_LOG << "[MCDK] solutions indexed: " << solutions_index->solutions.size() << std::endl;
        } else {
            MCDK_LOG << "[MCDK] solutions cache load failed: " << error << std::endl;
        }
    }
#endif

#ifdef MCDK_WITH_PLUGINS
    auto plugin_manager = std::make_shared<mcdk::plugins::PluginManager>(exe_dir / "plugins", dicts_dir, use_stdio);
    if (!plugin_manager->load_all()) {
        plugin_manager.reset();
    }
#endif

    mcp::server::configuration conf = mcdk::app::make_server_config();

    mcp::server srv(conf);

    mcdk::app::register_server_endpoints(srv, exe_dir, conf.port);
    mcdk::app::register_tools(srv, *search_svc, knowledge_dir, cache_only_mode, sapi_index
#ifdef MCDK_WITH_PLUGINS
        , plugin_manager
#endif
#ifdef MCDK_WITH_SOLUTIONS
        , solutions_index
#endif
    );

#ifndef MCDK_SERVER
    if (use_stdio) {
        // stdio 模式：不启动 HTTP 监听，直接在当前线程跑 stdio 循环。
        // 这里统一使用 std::endl，确保宿主按“逐行日志”处理 stderr。
        std::cerr << "[MCDK] stdio transport mode" << std::endl;
        std::cerr << "[MCDK] docs available: " << search_svc->doc_count() << std::endl;
        std::cerr << "[MCDK] game assets available: " << search_svc->game_assets_count() << std::endl;
        if (cache_only_mode) {
            std::cerr << "[MCDK] 缓存索引目录已就绪，数据按需加载" << std::endl;
        }

        mcp::stdio_server stdio_srv(srv);
        stdio_srv.run();
        return 0;
    }
#endif

    mcdk::app::log_startup_banner(conf, *search_svc, cache_only_mode);
    srv.start(true);

    return 0;
}
