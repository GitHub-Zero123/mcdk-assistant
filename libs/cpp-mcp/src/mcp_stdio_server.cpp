/**
 * @file mcp_stdio_server.cpp
 * @brief Implementation of the MCP stdio server transport wrapper.
 *
 * Protocol: newline-delimited JSON-RPC 2.0 over stdin/stdout.
 *   - One JSON object per line (no embedded newlines inside the object).
 *   - Responses are written as a single line followed by '\n'.
 *   - Notifications (no "id" field) receive no response.
 *   - stderr is left untouched for diagnostic output.
 *
 * Windows note: We switch stdin/stdout to binary mode so that the CR+LF
 * translation layer does not corrupt the JSON stream.
 */

#include "mcp_stdio_server.h"
#include "mcp_logger.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#  include <io.h>
#  include <fcntl.h>
#endif

namespace mcp {

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

        json msg;
        try {
            msg = json::parse(line);
        } catch (const json::exception& e) {
            // Malformed JSON — emit a parse-error response with null id.
            json err = response::create_error(
                json(nullptr),
                error_code::parse_error,
                std::string("Parse error: ") + e.what()
            ).to_json();
            std::cout << err.dump() << '\n';
            std::cout.flush();
            continue;
        }

        std::string reply = dispatch_one(msg);
        if (!reply.empty()) {
            std::cout << reply << '\n';
            std::cout.flush();
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

    return result.dump(-1, ' ', false, json::error_handler_t::replace);
}

} // namespace mcp
