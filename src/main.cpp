#include "search/search_service.hpp"
#include "search/index_cache.hpp"
#include "tools/register_search.hpp"
#include "tools/register_netease.hpp"
#include "tools/register_jsonui.hpp"
#include "tools/register_pixel_art.hpp"
#include <mcp_server.h>
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

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::string exe_dir       = get_exe_dir();
    std::string dicts_dir     = exe_dir + "/dicts";
    std::string knowledge_dir = exe_dir + "/knowledge";
    std::string cache_path    = exe_dir + "/mcdk_index_cache.bin";

    std::cout << "[MCDK] dicts: "     << dicts_dir     << std::endl;
    std::cout << "[MCDK] knowledge: " << knowledge_dir << std::endl;
    std::cout << "[MCDK] cache: "     << cache_path    << std::endl;
    std::cout << "[MCDK] 正在初始化知识库索引，请稍候..." << std::endl;

    mcdk::SearchService search_svc(dicts_dir, knowledge_dir, cache_path);

    mcp::server::configuration conf;
    conf.host    = "127.0.0.1";
    conf.port    = 18766;
    conf.name    = "mcdk-assistant";
    conf.version = "0.4.3";

    mcp::server srv(conf);

    mcdk::register_search_tools(srv, search_svc, knowledge_dir);
    mcdk::register_netease_tools(srv);
    mcdk::register_jsonui_tools(srv);
    mcdk::register_pixel_art_tools(srv);

    std::cout << "[MCDK] MCP server starting on " << conf.host << ":" << conf.port << std::endl;
    std::cout << "[MCDK] docs indexed: "         << search_svc.doc_count()         << std::endl;
    std::cout << "[MCDK] game assets indexed: "  << search_svc.game_assets_count() << std::endl;

    srv.start(true);
    return 0;
}
