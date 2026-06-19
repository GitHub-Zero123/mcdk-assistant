/**
 * @file mcp_stdio_server.h
 * @brief MCP Stdio Server — thin stdio transport wrapper over mcp::server
 *
 * Wraps an existing mcp::server instance and drives it via stdin/stdout
 * using MCP Content-Length framed JSON-RPC 2.0, with a legacy newline
 * JSON-RPC fallback for local scripts.
 *
 * Usage:
 *   mcp::server srv(conf);
 *   // register tools / resources ...
 *   mcp::stdio_server stdio(srv);
 *   stdio.run();   // blocks until stdin is closed
 */

#ifndef MCP_STDIO_SERVER_H
#define MCP_STDIO_SERVER_H

#include "mcp_message.h"
#include "mcp_server.h"

#include <atomic>
#include <string>

namespace mcp {

/**
 * @class stdio_server
 * @brief Drives an mcp::server instance over stdin/stdout transport.
 *
 * Reads JSON-RPC messages from stdin, dispatches them
 * through the wrapped server's handler pipeline, and writes responses to
 * stdout.  stderr is intentionally left untouched so callers can still
 * emit diagnostic output without polluting the protocol stream.
 *
 * Thread safety: run() is meant to be called once from the main thread.
 */
class stdio_server {
public:
    /**
     * @brief Construct a stdio_server backed by the given mcp::server.
     * @param srv  A fully configured mcp::server (tools/resources registered).
     *             The server must outlive this object.
     */
    explicit stdio_server(mcp::server& srv);

    ~stdio_server() = default;

    // Non-copyable, non-movable
    stdio_server(const stdio_server&)            = delete;
    stdio_server& operator=(const stdio_server&) = delete;

    /**
     * @brief Enter the read-dispatch-write loop.
     *
     * Reads MCP Content-Length frames from stdin until EOF or until stop()
     * is called.  A legacy newline-delimited JSON input is also accepted for
     * local scripts.  Responses use the same framing style as the request.
     *
     * @note This call blocks the calling thread.
     */
    void run();

    /**
     * @brief Request a graceful stop of the run() loop.
     *
     * Safe to call from any thread.  The loop will exit after the current
     * line has been processed.
     */
    void stop();

private:
    mcp::server&      srv_;
    std::atomic<bool> stop_{ false };

    // Dispatch one parsed JSON-RPC object and return the serialised response
    // string (empty string for pure notifications that need no reply).
    std::string dispatch_one(const json& msg);
};

} // namespace mcp

#endif // MCP_STDIO_SERVER_H
