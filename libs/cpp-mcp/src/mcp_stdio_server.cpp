/**
 * @file mcp_stdio_server.cpp
 * @brief Implementation of the MCP stdio server transport wrapper.
 *
 * Protocol: MCP Content-Length framed JSON-RPC 2.0 over stdin/stdout.
 *   - Requests are read as "Content-Length: <n>\r\n\r\n<body>" frames.
 *   - Responses are written with the same framing.
 *   - Legacy newline-delimited JSON-RPC input is accepted for local scripts.
 *   - Notifications (no "id" field) receive no response.
 *   - stderr is left untouched for diagnostic output.
 *
 * Windows note: We switch stdin/stdout to binary mode so that the CR+LF
 * translation layer does not corrupt the JSON stream.
 */

#include "mcp_stdio_server.h"
#include "mcp_logger.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#  include <io.h>
#  include <fcntl.h>
#endif

namespace mcp {

namespace {

std::string trim_ascii(std::string s) {
    auto is_space = [](unsigned char ch) { return std::isspace(ch) != 0; };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](char ch) {
        return !is_space(static_cast<unsigned char>(ch));
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [&](char ch) {
        return !is_space(static_cast<unsigned char>(ch));
    }).base(), s.end());
    return s;
}

std::string to_lower_ascii(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return s;
}

bool is_header_line(const std::string& line) {
    auto colon = line.find(':');
    if (colon == std::string::npos || colon == 0) {
        return false;
    }
    for (std::size_t i = 0; i < colon; ++i) {
        unsigned char ch = static_cast<unsigned char>(line[i]);
        if (!std::isalpha(ch) && ch != '-') {
            return false;
        }
    }
    return true;
}

bool is_frame_header_line(const std::string& line) {
    auto colon = line.find(':');
    if (colon == std::string::npos) {
        return false;
    }

    // Only known MCP/LSP transport headers should switch the parser into
    // framed mode; arbitrary "key:value" legacy JSON lines must stay line mode.
    std::string name = to_lower_ascii(trim_ascii(line.substr(0, colon)));
    return name == "content-length" || name == "content-type";
}

bool read_content_length_body(const std::string& first_header, std::string& body) {
    std::size_t content_length = std::string::npos;

    auto parse_header = [&](const std::string& header_line) {
        auto colon = header_line.find(':');
        if (colon == std::string::npos) {
            return;
        }
        std::string name = to_lower_ascii(trim_ascii(header_line.substr(0, colon)));
        if (name != "content-length") {
            return;
        }
        std::string value = trim_ascii(header_line.substr(colon + 1));
        content_length = static_cast<std::size_t>(std::stoull(value));
    };

    parse_header(first_header);

    std::string header_line;
    while (std::getline(std::cin, header_line)) {
        if (!header_line.empty() && header_line.back() == '\r') {
            header_line.pop_back();
        }
        if (header_line.empty()) {
            break;
        }
        if (is_header_line(header_line)) {
            parse_header(header_line);
        }
    }

    if (content_length == std::string::npos) {
        throw std::runtime_error("Missing Content-Length header");
    }

    body.assign(content_length, '\0');
    std::cin.read(body.data(), static_cast<std::streamsize>(content_length));
    return static_cast<std::size_t>(std::cin.gcount()) == content_length;
}

void write_framed_response(const std::string& reply) {
    std::cout << "Content-Length: " << reply.size() << "\r\n\r\n" << reply;
    std::cout.flush();
}

void write_line_response(const std::string& reply) {
    std::cout << reply << '\n';
    std::cout.flush();
}

} // namespace

stdio_server::stdio_server(mcp::server& srv)
    : srv_(srv)
{}

void stdio_server::stop() {
    stop_.store(true, std::memory_order_release);
}

void stdio_server::run() {
#ifdef _WIN32
    // Switch stdin/stdout to binary mode on Windows to prevent CR/LF mangling.
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    // Disable sync with C stdio for faster I/O.
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    LOG_INFO("MCP stdio server started — reading from stdin");

    std::string line;
    while (!stop_.load(std::memory_order_acquire) && std::getline(std::cin, line)) {
        // Strip trailing '\r' (Windows line endings arriving on non-Windows or
        // after binary-mode switch with text-mode leftovers).
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue; // skip blank lines
        }

        bool framed = is_frame_header_line(line);
        std::string payload;
        if (framed) {
            try {
                if (!read_content_length_body(line, payload)) {
                    break;
                }
            } catch (const std::exception& e) {
                json err = response::create_error(
                    json(nullptr),
                    error_code::parse_error,
                    std::string("Parse error: ") + e.what()
                ).to_json();
                write_framed_response(err.dump());
                continue;
            }
        } else {
            payload = line;
        }

        json msg;
        try {
            msg = json::parse(payload);
        } catch (const json::exception& e) {
            // Malformed JSON — emit a parse-error response with null id.
            json err = response::create_error(
                json(nullptr),
                error_code::parse_error,
                std::string("Parse error: ") + e.what()
            ).to_json();
            if (framed) {
                write_framed_response(err.dump());
            } else {
                write_line_response(err.dump());
            }
            continue;
        }

        std::string reply = dispatch_one(msg);
        if (!reply.empty()) {
            if (framed) {
                write_framed_response(reply);
            } else {
                write_line_response(reply);
            }
        }
    }

    LOG_INFO("MCP stdio server stopped (stdin closed or stop() called)");
}

std::string stdio_server::dispatch_one(const json& msg) {
    json result = srv_.dispatch(msg, "stdio");

    // dispatch() returns an empty json() for notifications — no wire output.
    if (result.is_null() || result.empty()) {
        return {};
    }

    return result.dump();
}

} // namespace mcp
