#include "app/runtime_paths.hpp"
#include "common/path_utils.hpp"
#include "search/search_service.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {

namespace fs = std::filesystem;
using json = nlohmann::json;

constexpr int kDefaultPort = 18767;
constexpr int kDefaultPageSize = 20;
constexpr int kMaxPageSize = 50;
constexpr int kMaxResultWindow = 500;
constexpr size_t kMaxQueryBytes = 256;
constexpr size_t kMaxPathBytes = 1024;
constexpr size_t kMaxDocumentBytes = 2 * 1024 * 1024;

struct ServerOptions {
    std::string host = "127.0.0.1";
    int port = kDefaultPort;
    fs::path web_root;
};

std::string trim(std::string value) {
    const auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
    value.erase(value.begin(), std::find_if_not(value.begin(), value.end(), is_space));
    value.erase(std::find_if_not(value.rbegin(), value.rend(), is_space).base(), value.end());
    return value;
}

std::optional<int> parse_int(std::string_view text) {
    int value = 0;
    const auto* begin = text.data();
    const auto* end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    if (result.ec != std::errc() || result.ptr != end) return std::nullopt;
    return value;
}

void print_usage() {
    std::cout
        << "Usage: mcdk-web-server [--host <address>] [--port <port>] [--web-root <directory>]\n"
        << "Defaults: --host 127.0.0.1 --port " << kDefaultPort << '\n';
}

std::optional<ServerOptions> parse_options(int argc, char* argv[]) {
    ServerOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_usage();
            return std::nullopt;
        }
        if (arg != "--host" && arg != "--port" && arg != "--web-root") {
            std::cerr << "[MCDK-Web] unknown argument: " << arg << '\n';
            print_usage();
            return std::nullopt;
        }
        if (++i >= argc) {
            std::cerr << "[MCDK-Web] missing value for " << arg << '\n';
            return std::nullopt;
        }
        const std::string value = argv[i];
        if (arg == "--host") {
            options.host = value;
        } else if (arg == "--web-root") {
            options.web_root = mcdk::path::from_utf8(value);
        } else {
            const auto port = parse_int(value);
            if (!port || *port < 1 || *port > 65535) {
                std::cerr << "[MCDK-Web] invalid port: " << value << '\n';
                return std::nullopt;
            }
            options.port = *port;
        }
    }
    if (options.host.empty()) {
        std::cerr << "[MCDK-Web] host cannot be empty\n";
        return std::nullopt;
    }
    return options;
}

void set_json(httplib::Response& response, const json& body, int status = 200, const char* cache_control = "no-store") {
    response.status = status;
    response.set_header("Cache-Control", cache_control);
    response.set_header("X-Content-Type-Options", "nosniff");
    response.set_content(body.dump(), "application/json; charset=utf-8");
}

void set_error(httplib::Response& response, int status, std::string code, std::string message) {
    set_json(response, {
        {"error", {
            {"code", std::move(code)},
            {"message", std::move(message)}
        }}
    }, status);
}

int query_int(const httplib::Request& request, const char* name, int fallback) {
    if (!request.has_param(name)) return fallback;
    const auto parsed = parse_int(request.get_param_value(name));
    return parsed.value_or(fallback);
}

std::string utf8_prefix(const std::string& text, size_t max_bytes) {
    if (text.size() <= max_bytes) return text;
    size_t end = max_bytes;
    while (end > 0 && (static_cast<unsigned char>(text[end]) & 0xC0U) == 0x80U) --end;
    return text.substr(0, end);
}

std::string normalize_snippet(const std::string& content) {
    std::string output;
    output.reserve(content.size());
    bool previous_space = false;
    for (unsigned char c : content) {
        const bool space = std::isspace(c) != 0;
        if (space) {
            if (!previous_space && !output.empty()) output.push_back(' ');
        } else {
            output.push_back(static_cast<char>(c));
        }
        previous_space = space;
    }
    const bool truncated = output.size() > 420;
    output = trim(utf8_prefix(output, 420));
    return truncated ? output + "..." : output;
}

std::string filename_title(const std::string& path) {
    const auto slash = path.find_last_of('/');
    std::string title = slash == std::string::npos ? path : path.substr(slash + 1);
    const auto dot = title.find_last_of('.');
    if (dot != std::string::npos) title.erase(dot);
    std::replace(title.begin(), title.end(), '-', ' ');
    std::replace(title.begin(), title.end(), '_', ' ');
    return title.empty() ? path : title;
}

std::string extract_title(const mcdk::DocFragment& fragment) {
    std::string_view content = fragment.content;
    size_t cursor = 0;
    while (cursor < content.size()) {
        const size_t end = content.find('\n', cursor);
        std::string line(content.substr(cursor, end == std::string_view::npos
            ? content.size() - cursor
            : end - cursor));
        line = trim(std::move(line));
        if (!line.empty() && line.front() == '#') {
            const auto first_text = line.find_first_not_of("# \t");
            if (first_text != std::string::npos) return utf8_prefix(line.substr(first_text), 160);
        }
        if (end == std::string_view::npos) break;
        cursor = end + 1;
    }
    return filename_title(fragment.file);
}

std::string source_from_path(const std::string& path) {
    if (path.rfind("NeteaseGuide/", 0) == 0 || path.find("/NeteaseGuide/") != std::string::npos) return "netease";
    if (path.rfind("QuModDocs/", 0) == 0 || path.find("/QuModDocs/") != std::string::npos) return "qumod";
    if (path.rfind("BedrockDev/", 0) == 0 || path.find("/BedrockDev/") != std::string::npos) return "dev";
    if (path.rfind("BedrockWiki/", 0) == 0 || path.find("/BedrockWiki/") != std::string::npos) return "wiki";
    if (path.rfind("接口/", 0) == 0 || path.find("/接口/") != std::string::npos) return "api";
    if (path.rfind("事件/", 0) == 0 || path.find("/事件/") != std::string::npos) return "event";
    if (path.rfind("枚举值/", 0) == 0 || path.find("/枚举值/") != std::string::npos) return "enum";
    return "other";
}

using SearchMethod = std::vector<mcdk::SearchResult> (mcdk::SearchService::*)(const std::string&, int) const;

std::optional<SearchMethod> search_method(const std::string& scope) {
    if (scope == "all") return &mcdk::SearchService::search_all;
    if (scope == "api") return &mcdk::SearchService::search_api;
    if (scope == "event") return &mcdk::SearchService::search_event;
    if (scope == "enum") return &mcdk::SearchService::search_enum;
    if (scope == "wiki") return &mcdk::SearchService::search_wiki;
    if (scope == "dev") return &mcdk::SearchService::search_bedrock_dev;
    if (scope == "qumod") return &mcdk::SearchService::search_qumod;
    if (scope == "netease") return &mcdk::SearchService::search_netease_guide;
    return std::nullopt;
}

bool valid_document_path(const std::string& path) {
    if (path.empty() || path.size() > kMaxPathBytes || path.front() == '/' || path.front() == '\\') return false;
    if (path.find('\0') != std::string::npos || path.find('\\') != std::string::npos) return false;
    if (path == ".." || path.rfind("../", 0) == 0 || path.find("/../") != std::string::npos) return false;
    return true;
}

fs::path default_web_root(const fs::path& executable_dir) {
    return executable_dir / "web";
}

void register_api(httplib::Server& server, mcdk::SearchService& search) {
    server.Get("/favicon.ico", [](const httplib::Request&, httplib::Response& response) {
        response.status = 204;
        response.set_header("Cache-Control", "public, max-age=86400");
    });

    server.Get("/healthz", [&search](const httplib::Request&, httplib::Response& response) {
        set_json(response, {{"status", "ok"}, {"documents", search.doc_count()}});
    });

    server.Get("/api/v1/meta", [&search](const httplib::Request&, httplib::Response& response) {
        set_json(response, {
            {"name", "MCDK 资料库"},
            {"version", 1},
            {"documents", search.doc_count()},
            {"scopes", json::array({"all", "api", "event", "enum", "wiki", "dev", "qumod", "netease"})}
        });
    });

    server.Get("/api/v1/search", [&search](const httplib::Request& request, httplib::Response& response) {
        const std::string query = trim(request.has_param("q") ? request.get_param_value("q") : "");
        const std::string scope = request.has_param("scope") ? request.get_param_value("scope") : "all";
        const int page = query_int(request, "page", 1);
        const int page_size = query_int(request, "page_size", kDefaultPageSize);
        const auto method = search_method(scope);

        if (query.empty()) return set_error(response, 400, "empty_query", "查询内容不能为空");
        if (query.size() > kMaxQueryBytes) return set_error(response, 400, "query_too_long", "查询内容过长");
        if (!method) return set_error(response, 400, "invalid_scope", "未知的资料分类");
        if (page < 1 || page_size < 1 || page_size > kMaxPageSize) {
            return set_error(response, 400, "invalid_pagination", "分页参数超出允许范围");
        }

        const long long offset64 = static_cast<long long>(page - 1) * page_size;
        if (offset64 >= kMaxResultWindow) {
            return set_error(response, 400, "result_window_exceeded", "搜索结果最多浏览前 500 条");
        }
        const size_t offset = static_cast<size_t>(offset64);
        const size_t wanted = std::min<size_t>(offset + page_size + 1, kMaxResultWindow);
        const int candidate_limit = static_cast<int>(std::min<size_t>(wanted * 3, kMaxResultWindow));

        auto candidates = (search.*(*method))(query, candidate_limit);
        std::vector<mcdk::SearchResult> unique_results;
        unique_results.reserve(std::min(candidates.size(), wanted));
        std::unordered_set<std::string> seen_paths;
        for (const auto& result : candidates) {
            if (!result.fragment || !seen_paths.insert(result.fragment->file).second) continue;
            unique_results.push_back(result);
            if (unique_results.size() >= wanted) break;
        }

        json items = json::array();
        const size_t end = std::min(unique_results.size(), offset + static_cast<size_t>(page_size));
        for (size_t index = offset; index < end; ++index) {
            const auto& result = unique_results[index];
            const auto& fragment = *result.fragment;
            items.push_back({
                {"source", source_from_path(fragment.file)},
                {"path", fragment.file},
                {"line_start", fragment.line_start},
                {"line_end", fragment.line_end},
                {"title", extract_title(fragment)},
                {"snippet", normalize_snippet(fragment.content)},
                {"score", std::round(result.score * 1000.0) / 1000.0}
            });
        }

        set_json(response, {
            {"query", query},
            {"scope", scope},
            {"page", page},
            {"page_size", page_size},
            {"has_more", unique_results.size() > end},
            {"items", std::move(items)}
        });
    });

    server.Get("/api/v1/document", [&search](const httplib::Request& request, httplib::Response& response) {
        const std::string path = request.has_param("path") ? request.get_param_value("path") : "";
        if (!valid_document_path(path)) {
            return set_error(response, 400, "invalid_path", "文档路径无效");
        }

        auto document = search.read_cached_file(path);
        if (!document.found) return set_error(response, 404, "not_found", "未找到文档");
        if (document.content.size() > kMaxDocumentBytes) {
            return set_error(response, 413, "document_too_large", "文档超过在线阅读大小限制");
        }

        set_json(response, {
            {"path", path},
            {"source", source_from_path(path)},
            {"title", filename_title(path)},
            {"total_lines", document.total_lines},
            {"content", std::move(document.content)}
        }, 200, "public, max-age=300");
    });
}

} // namespace

int main(int argc, char* argv[]) {
    const auto options = parse_options(argc, argv);
    if (!options) return argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") ? 0 : 2;

    const auto runtime = mcdk::app::detect_runtime_paths();
    const fs::path web_root = options->web_root.empty() ? default_web_root(runtime.exe_dir) : options->web_root;

    if (!fs::is_directory(runtime.dicts_dir)) {
        std::cerr << "[MCDK-Web] dicts directory not found: " << mcdk::path::to_utf8(runtime.dicts_dir) << '\n';
        return 1;
    }
    if (!fs::is_regular_file(runtime.cache_path)) {
        std::cerr << "[MCDK-Web] index cache not found: " << mcdk::path::to_utf8(runtime.cache_path) << '\n';
        return 1;
    }
    if (!fs::is_regular_file(web_root / "index.html")) {
        std::cerr << "[MCDK-Web] frontend assets not found: " << mcdk::path::to_utf8(web_root) << '\n';
        return 1;
    }

    try {
        mcdk::SearchService search(runtime.dicts_dir, runtime.cache_path, true);
        httplib::Server server;
        server.set_payload_max_length(16 * 1024);
        server.set_read_timeout(10, 0);
        server.set_write_timeout(30, 0);

        register_api(server, search);
        if (!server.set_mount_point("/", mcdk::path::to_utf8(web_root), {
                {"Cache-Control", "public, max-age=300"},
                {"X-Content-Type-Options", "nosniff"},
                {"X-Frame-Options", "DENY"},
                {"Referrer-Policy", "same-origin"},
                {"Content-Security-Policy", "default-src 'self'; script-src 'self'; style-src 'self'; img-src 'self' data: https:; connect-src 'self'; object-src 'none'; frame-ancestors 'none'; base-uri 'none'; form-action 'none'"}
            })) {
            std::cerr << "[MCDK-Web] failed to mount frontend directory\n";
            return 1;
        }

        server.set_exception_handler([](const auto&, auto& response, std::exception_ptr error) {
            try {
                if (error) std::rethrow_exception(error);
            } catch (const std::exception& exception) {
                std::cerr << "[MCDK-Web] request failed: " << exception.what() << '\n';
            } catch (...) {
                std::cerr << "[MCDK-Web] request failed with an unknown exception\n";
            }
            set_error(response, 500, "internal_error", "服务器处理请求失败");
        });

        std::cout << "[MCDK-Web] documents: " << search.doc_count() << '\n';
        std::cout << "[MCDK-Web] frontend: " << mcdk::path::to_utf8(web_root) << '\n';
        std::cout << "[MCDK-Web] listening on http://" << options->host << ':' << options->port << '\n';
        if (!server.listen(options->host, options->port)) {
            std::cerr << "[MCDK-Web] failed to listen on " << options->host << ':' << options->port << '\n';
            return 1;
        }
    } catch (const std::exception& exception) {
        std::cerr << "[MCDK-Web] startup failed: " << exception.what() << '\n';
        return 1;
    }
    return 0;
}
