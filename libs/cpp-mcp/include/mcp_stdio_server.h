/**
 * @file mcp_stdio_server.h
 * @brief MCP Stdio Server — thin stdio transport wrapper over mcp::server
 *
 * Wraps an existing mcp::server instance and drives it via stdin/stdout
 * using newline-delimited JSON-RPC 2.0 (the standard MCP stdio transport).
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
#include "mcp_thread_pool.h"

#include <atomic>
#include <mutex>
#include <string>

namespace mcp {

/**
 * @class stdio_server
 * @brief Drives an mcp::server instance over stdin/stdout transport.
 *
 * Reads newline-delimited JSON-RPC messages from stdin, dispatches them
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
     * Reads lines from stdin until EOF or until stop() is called.
     * Each non-empty line is parsed as JSON-RPC 2.0 and dispatched to the
     * wrapped server.  Responses (if any) are written to stdout followed by
     * a newline.
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
    static constexpr unsigned int kRequestThreads = 4;

    mcp::server&      srv_;
    std::mutex        output_mutex_;
    std::atomic<bool> stop_{ false };
    // Declared last so it is destroyed first and joins workers while the
    // output mutex and server reference are still valid.
    thread_pool       request_pool_;

    // Dispatch one parsed JSON-RPC object and return the serialised response
    // string (empty string for pure notifications that need no reply).
    std::string dispatch_one(const json& msg);
    void dispatch_and_write(json msg);
    void write_line(const std::string& line);
};

} // namespace mcp

#endif // MCP_STDIO_SERVER_H
