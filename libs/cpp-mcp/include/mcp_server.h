/**
 * @file mcp_server.h
 * @brief MCP Server implementation
 *
 * This file implements the server-side functionality for the Model Context Protocol.
 * Supports 2025-03-26 Streamable HTTP transport with backward compatibility for 2024-11-05 SSE.
 */

#ifndef MCP_SERVER_H
#define MCP_SERVER_H

#include "mcp_message.h"
#include "mcp_resource.h"
#include "mcp_tool.h"
#include "mcp_thread_pool.h"
#include "mcp_logger.h"

// Include the HTTP library
#include "httplib.h"

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <chrono>
#include <condition_variable>
#include <future>
#include <atomic>
#include <optional>


namespace mcp {

    using method_handler          = std::function<json(const json&, const std::string&)>;
    using tool_handler            = method_handler;
    using notification_handler    = std::function<void(const json&, const std::string&)>;
    using auth_handler            = std::function<bool(const std::string&, const std::string&)>;
    using session_cleanup_handler = std::function<void(const std::string&)>;
    /**
     * @brief Tool call event handler — fired every time a tool is successfully dispatched.
     * @param tool_name  The name of the tool being invoked.
     *
     * The handler is called from within the request-processing thread, before the
     * actual tool handler runs.  It must be non-blocking and thread-safe.
     */
    using tool_call_handler = std::function<void(const std::string& tool_name)>;

    /**
     * @brief Completion-signal event dispatcher.
     *
     * Design:
     *   - IO thread calls attach_and_wait(&sink): registers the DataSink and parks
     *     in closed_cv_.wait() consuming zero CPU until close() is called.
     *   - Worker thread calls send_event(msg): acquires sink_mutex_, writes directly
     *     to the DataSink (zero-copy for the IO thread).
     *   - close() signals closed_cv_ to wake the parked IO thread.
     *
     * Thread safety: send_event and attach_and_wait are mutually exclusive via sink_mutex_.
     */
    class event_dispatcher {
    public:
        event_dispatcher()
            : last_activity_(std::chrono::steady_clock::now()) {}

        ~event_dispatcher() { close(); }

        /**
         * @brief Called by the IO thread inside the chunked content provider.
         *
         * Registers the sink, then blocks until close() is called.
         * Returns false (tells httplib to end the chunked response).
         */
        bool attach_and_wait(httplib::DataSink* sink) {
            if (!sink) return false;

            {
                std::lock_guard<std::mutex> lk(sink_mutex_);
                sink_ = sink;
                sink_ready_.store(true, std::memory_order_release);
            }
            sink_cv_.notify_all();

            // Park IO thread here (zero CPU) until close() is called
            {
                std::unique_lock<std::mutex> lk(closed_mutex_);
                closed_cv_.wait(lk, [this] { return closed_.load(std::memory_order_acquire); });
            }

            {
                std::lock_guard<std::mutex> lk(sink_mutex_);
                sink_ = nullptr;
                sink_ready_.store(false, std::memory_order_release);
            }
            return false; // signal httplib to finish chunked response
        }

        /**
         * @brief Called by worker/heartbeat threads to push an SSE event.
         *
         * Waits up to sink_wait for the IO thread to attach, then writes
         * directly to the DataSink under sink_mutex_.
         */
        bool send_event(
            const std::string&              message,
            std::chrono::milliseconds sink_wait = std::chrono::milliseconds(5000)
        ) {
            if (closed_.load(std::memory_order_acquire) || message.empty()) return false;

            std::unique_lock<std::mutex> lk(sink_mutex_);

            // Wait for IO thread to attach sink (with timeout)
            if (!sink_ready_.load(std::memory_order_acquire)) {
                sink_cv_.wait_for(lk, sink_wait, [this] {
                    return sink_ready_.load(std::memory_order_acquire)
                        || closed_.load(std::memory_order_acquire);
                });
            }

            if (!sink_ || closed_.load(std::memory_order_acquire)) return false;

            bool ok = sink_->write(message.data(), message.size());
            if (!ok) {
                lk.unlock();
                close();
                return false;
            }

            // Update activity under sink_mutex_ (saves an extra lock acquisition)
            {
                std::lock_guard<std::mutex> alk(activity_mutex_);
                last_activity_ = std::chrono::steady_clock::now();
            }
            return true;
        }

        void close() {
            bool was_closed = closed_.exchange(true, std::memory_order_release);
            if (was_closed) return;

            closed_cv_.notify_all();
            sink_cv_.notify_all();
        }

        bool is_closed() const { return closed_.load(std::memory_order_acquire); }

        std::chrono::steady_clock::time_point last_activity() const {
            std::lock_guard<std::mutex> lk(activity_mutex_);
            return last_activity_;
        }

        void update_activity() {
            std::lock_guard<std::mutex> lk(activity_mutex_);
            last_activity_ = std::chrono::steady_clock::now();
        }

    private:
        // Sink access (worker ↔ IO thread)
        mutable std::mutex      sink_mutex_;
        std::condition_variable sink_cv_;
        httplib::DataSink*      sink_{nullptr};
        std::atomic<bool>       sink_ready_{false};

        // Close signal (IO thread parks here)
        std::mutex              closed_mutex_;
        std::condition_variable closed_cv_;
        std::atomic<bool>       closed_{false};

        // Activity tracking
        mutable std::mutex                     activity_mutex_;
        std::chrono::steady_clock::time_point  last_activity_;
    };

    /**
     * @brief Elastic task queue — one detached thread per connection.
     *
     * Replaces httplib's fixed-size ThreadPool.  There is no upper bound on
     * concurrent connections; each enqueued task gets its own thread.
     * shutdown() waits for all active tasks to finish before returning.
     */
    class elastic_task_queue : public httplib::TaskQueue {
    public:
        bool enqueue(std::function<void()> fn) override {
            {
                std::lock_guard<std::mutex> lk(m_);
                if (shutdown_) return false;
                ++active_;
            }
            std::thread([this, fn = std::move(fn)]() mutable {
                try { fn(); } catch (...) {}
                std::lock_guard<std::mutex> lk(m_);
                if (--active_ == 0 && shutdown_) cv_.notify_all();
            }).detach();
            return true;
        }

        void shutdown() override {
            {
                std::lock_guard<std::mutex> lk(m_);
                shutdown_ = true;
            }
            std::unique_lock<std::mutex> lk(m_);
            cv_.wait(lk, [this] { return active_ == 0; });
        }

    private:
        std::mutex              m_;
        std::condition_variable cv_;
        int                     active_{0};
        bool                    shutdown_{false};
    };

    /**
     * @class server
     * @brief Main MCP server class
     *
     * The server class implements an HTTP server that handles JSON-RPC requests
     * according to the Model Context Protocol specification.
     */
    class server {
    public:
        /**
         * @struct configuration
         * @brief Configuration settings for the server.
         *
         * This struct holds all configurable parameters for the server, including
         * network bindings, identification, and endpoint paths. If SSL is enabled,
         * it also includes paths to the server certificate and private key.
         */
        struct configuration {
            /** Host to bind to (e.g., "localhost", "0.0.0.0") */
            std::string host{"localhost"};

            /** Port to listen on */
            int port{8080};

            /** Server name */
            std::string name{"MCP Server"};

            /** Server version */
            std::string version{"0.0.1"};

            /** SSE endpoint path */
            std::string sse_endpoint{"/sse"};

            /** Message endpoint path */
            std::string msg_endpoint{"/message"};

            /** Streamable HTTP endpoint path (2025-03-26) */
            std::string streamable_endpoint{"/mcp"};

            /** Enable legacy SSE transport for 2024-11-05 backward compatibility */
            bool enable_legacy_sse{true};

            unsigned int threadpool_size{std::thread::hardware_concurrency()};

#ifdef MCP_SSL
            /**
             * @brief SSL configuration settings.
             *
             * Contains optional paths to the server certificate and private key.
             * These are used when SSL support is enabled.
             */
            struct {
                /** Path to the server certificate */
                std::optional<std::string> server_cert_path{std::nullopt};

                /** Path to the server private key */
                std::optional<std::string> server_private_key_path{std::nullopt};
            } ssl;
#endif
        };

        /**
         * @brief Constructor
         * @param conf The server configuration
         */
        server(const server::configuration& conf);

        /**
         * @brief Destructor
         */
        ~server();

        /**
         * @brief Start the server
         * @param blocking If true, this call blocks until the server stops
         * @return True if the server started successfully
         */
        bool start(bool blocking = true);

        /**
         * @brief Stop the server
         */
        void stop();

        /**
         * @brief Check if the server is running
         * @return True if the server is running
         */
        bool is_running() const;

        /**
         * @brief Set server information
         * @param name The name of the server
         * @param version The version of the server
         */
        void set_server_info(const std::string& name, const std::string& version);

        /**
         * @brief Set server capabilities
         * @param capabilities The capabilities of the server
         */
        void set_capabilities(const json& capabilities);

        /**
         * @brief Register a method handler
         * @param method The method name
         * @param handler The function to call when the method is invoked
         */
        void register_method(const std::string& method, method_handler handler);

        /**
         * @brief Register a notification handler
         * @param method The notification method name
         * @param handler The function to call when the notification is received
         */
        void register_notification(const std::string& method, notification_handler handler);

        /**
         * @brief Register a resource
         * @param path The path to mount the resource at
         * @param resource The resource to register
         */
        void register_resource(const std::string& path, std::shared_ptr<resource> resource);

        /**
         * @brief Register a tool
         * @param tool The tool to register
         * @param handler The function to call when the tool is invoked
         */
        void register_tool(const tool& tool, tool_handler handler);

        /**
         * @brief Register a session cleanup handler
         * @param key Tool or resource name to be cleaned up
         * @param handler The function to call when the session is closed
         */
        void register_session_cleanup(const std::string& key, session_cleanup_handler handler);

        /**
         * @brief Get the list of available tools
         * @return JSON array of available tools
         */
        std::vector<tool> get_tools() const;

        /**
         * @brief Set authentication handler
         * @param handler Function that takes a token and returns true if valid
         * @note The handler should return true if the token is valid, otherwise false
         * @note Not used in the current implementation
         */
        void set_auth_handler(auth_handler handler);

        /**
         * @brief Register a callback invoked whenever a tool is called via tools/call.
         *
         * Thread-safe.  Pass nullptr to clear the handler.
         * The callback receives the tool name and is called before the tool handler runs.
         */
        void set_tool_call_handler(tool_call_handler handler);

        /**
         * @brief Send a request (or notification) to a client
         * @param session_id The session ID of the client
         * @param req The request to send
         */
        void send_request(const std::string& session_id, const request& req);

        /**
         * @brief Set mount point for server
         * @param mount_point The mount point to set
         * @param dir The directory to serve from the mount point
         * @param headers Optional headers to include in the response
         * @return True if the mount point was set successfully
         */
        bool set_mount_point(
            const std::string& mount_point,
            const std::string& dir,
            httplib::Headers   headers = httplib::Headers()
        );

        /**
         * @brief Register a custom HTTP GET route on the MCP server's own HTTP port.
         *
         * Allows callers to expose extra REST endpoints (e.g. /api/tool-stats)
         * on the same port as the MCP protocol without running a second server.
         * Must be called before start().
         *
         * @param path     URL path (e.g. "/api/tool-stats")
         * @param handler  httplib GET handler
         */
        void register_http_get(const std::string& path, httplib::Server::Handler handler);

        /**
         * @brief Register a custom HTTP POST route on the MCP server's own HTTP port.
         * Must be called before start().
         */
        void register_http_post(const std::string& path, httplib::Server::Handler handler);

    private:
        std::string host_;
        int         port_;
        std::string name_;
        std::string version_;
        json        capabilities_ = json::object();

        // The HTTP server
        std::unique_ptr<httplib::Server> http_server_;

        // Server thread (for non-blocking mode)
        std::unique_ptr<std::thread> server_thread_;

        // Session-specific event dispatchers (session_id -> dispatcher)
        std::map<std::string, std::shared_ptr<event_dispatcher>> session_dispatchers_;

        // Global heartbeat manager — one shared thread for all SSE sessions.
        // Sends a heartbeat to every live dispatcher every heartbeat_interval_ seconds.
        std::unique_ptr<std::thread> heartbeat_thread_;
        std::mutex                   heartbeat_mutex_;
        std::condition_variable      heartbeat_cv_;
        bool                         heartbeat_run_{false};
        static constexpr int         heartbeat_interval_s_{25}; // seconds between heartbeats

        // Server-sent events endpoint
        std::string sse_endpoint_;
        std::string msg_endpoint_;

        // Streamable HTTP endpoint (2025-03-26)
        std::string streamable_endpoint_;
        bool        enable_legacy_sse_;

        // Method handlers
        std::map<std::string, method_handler> method_handlers_;

        // Notification handlers
        std::map<std::string, notification_handler> notification_handlers_;

        // Resources map (path -> resource)
        std::map<std::string, std::shared_ptr<resource>> resources_;

        // Tools map (name -> handler)
        std::map<std::string, std::pair<tool, tool_handler>> tools_;

        // Authentication handler
        auth_handler auth_handler_;

        // Tool call event handler (optional, e.g. for statistics)
        tool_call_handler tool_call_handler_;

        // Mutex for thread safety
        mutable std::mutex mutex_;

        // Running flag
        bool running_ = false;

        // Thread pool for async method handlers
        thread_pool thread_pool_;

        // Map to track session initialization status (session_id -> initialized)
        std::map<std::string, bool> session_initialized_;

        // Negotiated protocol version per session (2025-03-26)
        std::map<std::string, std::string> session_protocol_version_;

        // Handle SSE requests
        void handle_sse(const httplib::Request& req, httplib::Response& res);

        // Handle incoming JSON-RPC requests
        void handle_jsonrpc(const httplib::Request& req, httplib::Response& res);

        // Send a JSON-RPC message to a client
        void send_jsonrpc(const std::string& session_id, const json& message);

        // Process a JSON-RPC request
        json process_request(const request& req, const std::string& session_id);

        // Handle initialization request
        json handle_initialize(const request& req, const std::string& session_id);

        // Handle Streamable HTTP transport (2025-03-26)
        void handle_streamable_post(const httplib::Request& req, httplib::Response& res);
        void handle_streamable_get(const httplib::Request& req, httplib::Response& res);
        void handle_streamable_delete(const httplib::Request& req, httplib::Response& res);

        // Process JSON-RPC batch request (2025-03-26)
        json process_batch_request(const json& batch, const std::string& session_id);

        // Check if a session is initialized
        bool is_session_initialized(const std::string& session_id) const;

        // Set session initialization status
        void set_session_initialized(const std::string& session_id, bool initialized);

        // Generate a random session ID
        std::string generate_session_id() const;

        // Auxiliary function to create an async handler from a regular handler
        template <typename F>
        std::function<std::future<json>(const json&, const std::string&)> make_async_handler(F&& handler) {
            return
                [handler =
                     std::forward<F>(handler)](const json& params, const std::string& session_id) -> std::future<json> {
                    return std::async(std::launch::async, [handler, params, session_id]() -> json {
                        return handler(params, session_id);
                    });
                };
        }

        // Helper class to simplify lock management
        class auto_lock {
        public:
            explicit auto_lock(std::mutex& mutex) : lock_(mutex) {}

        private:
            std::lock_guard<std::mutex> lock_;
        };

        // Get auto lock
        auto_lock get_lock() const { return auto_lock(mutex_); }

        // Session management and maintenance
        void check_inactive_sessions();

        std::mutex                   maintenance_mutex_;
        std::condition_variable      maintenance_cond_;
        std::unique_ptr<std::thread> maintenance_thread_;
        bool                         maintenance_thread_run_ = false;

        // Session cleanup handler
        std::map<std::string, session_cleanup_handler> session_cleanup_handler_;

        // Close session
        void close_session(const std::string& session_id);
    };

} // namespace mcp

#endif // MCP_SERVER_H
