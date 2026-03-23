#include "app/runtime_paths.hpp"
#include "app/server_runtime.hpp"
#include "common/path_utils.hpp"
#include "search/search_service.hpp"
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

#ifndef MCDK_SERVER
    // 检测 --stdio 参数：使用 stdio 传输模式（兼容 VSCode / Copilot 原生 stdio MCP 接入）
    // 必须在任何 stdout 输出之前完成，避免污染协议流。
    bool use_stdio = false;
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

    bool has_dicts     = fs::exists(dicts_dir);
    bool has_knowledge = fs::exists(knowledge_dir);
    bool has_cache     = fs::is_regular_file(cache_path);

    MCDK_LOG << "[MCDK] dicts: "     << mcdk::path::to_utf8(dicts_dir)     << (has_dicts     ? "" : " (NOT FOUND)") << std::endl;
    MCDK_LOG << "[MCDK] knowledge: " << mcdk::path::to_utf8(knowledge_dir) << (has_knowledge ? "" : " (NOT FOUND)") << std::endl;
    MCDK_LOG << "[MCDK] cache: "     << mcdk::path::to_utf8(cache_path)    << (has_cache     ? "" : " (NOT FOUND)") << std::endl;

    if (!has_dicts) {
        std::cerr << "[MCDK] 错误：词库目录不存在: " << mcdk::path::to_utf8(dicts_dir) << std::endl;
        return 1;
    }

    bool cache_only_mode = !has_knowledge && has_cache;

    if (cache_only_mode) {
        MCDK_LOG << "[MCDK] 缓存解析：无外部资料，正在从索引文件解析..." << std::endl;
    } else if (!has_knowledge && !has_cache) {
#ifndef MCDK_SERVER
        std::cerr << "[MCDK] 错误：缺少知识库目录和缓存文件，无法启动。" << std::endl;
        return 1;
#endif
    } else {
        MCDK_LOG << "[MCDK] 正在初始化知识库索引，请稍候..." << std::endl;
    }

    // 提供仅缓存模式和完整模式两套构造函数，以便工程使用/分发版优化启动速度。
    std::unique_ptr<mcdk::SearchService> search_svc;
    if (cache_only_mode) {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir, cache_path, true);
    } else {
        search_svc = std::make_unique<mcdk::SearchService>(dicts_dir, knowledge_dir, cache_path);
    }

    mcp::server::configuration conf = mcdk::app::make_server_config();

    mcp::server srv(conf);

    mcdk::app::register_server_endpoints(srv, exe_dir, conf.port);
    mcdk::app::register_tools(srv, *search_svc, knowledge_dir, cache_only_mode);

#ifndef MCDK_SERVER
    if (use_stdio) {
        // stdio 模式：不启动 HTTP 监听，直接在当前线程跑 stdio 循环。
        // 这里统一使用 std::endl，确保宿主按“逐行日志”处理 stderr。
        std::cerr << "[MCDK] stdio transport mode" << std::endl;
        std::cerr << "[MCDK] docs indexed: " << search_svc->doc_count() << std::endl;
        std::cerr << "[MCDK] game assets indexed: " << search_svc->game_assets_count() << std::endl;
        if (cache_only_mode) {
            std::cerr << "[MCDK] 已解析缓存索引库（缓存模式）" << std::endl;
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
