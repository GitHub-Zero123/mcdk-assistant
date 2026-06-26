#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include <mcp_server.h>

namespace mcdk { class SearchService; }
namespace mcdk::sapi { struct SapiIndex; }

namespace mcdk::app {

bool init_console_encoding();
mcp::server::configuration make_server_config();
void register_tools(mcp::server& srv,
                    SearchService& search_svc,
                    const std::filesystem::path& knowledge_dir,
                    bool cache_only_mode,
                    std::shared_ptr<sapi::SapiIndex> sapi_index = {});
void log_startup_banner(const mcp::server::configuration& conf,
                        const SearchService& search_svc,
                        bool cache_only_mode);
void register_server_endpoints(mcp::server& srv,
                               const std::filesystem::path& exe_dir,
                               int port);

} // namespace mcdk::app
