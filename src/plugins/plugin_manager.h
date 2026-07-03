#pragma once

#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace mcdk { class MemorySearchIndex; }

namespace mcdk::plugins {

struct PluginManifest {
    std::string id;
    std::string name;
    std::string version;
    std::string entry;
    std::string description;
    std::filesystem::path dir;
};

struct PluginTool {
    std::string plugin_id;
    std::string public_name;
    std::string local_name;
    std::string description;
    mcp::json schema;
    mcp::tool_annotations annotations;
    std::string handler_key;
    size_t runtime_index = 0;
};

struct PluginHook {
    std::string plugin_id;
    std::string event;
    int priority = 10;
    size_t order = 0;
    std::string handler_key;
    size_t runtime_index = 0;
};

class PluginManager {
public:
    struct Runtime;

    PluginManager(std::filesystem::path plugins_dir,
                  std::filesystem::path dicts_dir,
                  bool stdio_mode);
    ~PluginManager();

    bool load_all();
    void register_tools(mcp::server& srv);
    mcp::json run_hook(const std::string& event, const mcp::json& payload);

    const std::vector<PluginTool>& tools() const { return tools_; }
    const std::vector<PluginHook>& hooks() const { return hooks_; }

    Runtime* current_runtime();
    void add_tool(std::string plugin_id,
                  std::string name,
                  std::string description,
                  mcp::json schema,
                  mcp::tool_annotations annotations,
                  std::string handler_key);
    void add_hook(std::string plugin_id,
                  std::string event,
                  int priority,
                  std::string handler_key);

    int create_memory_index(const std::string& mode);
    void memory_index_add_doc(int handle,
                              const std::string& id,
                              const std::string& text,
                              const std::string& file,
                              int line_start);
    void memory_index_set_dir(int handle,
                              const std::string& root,
                              const std::vector<std::string>& globs,
                              const std::string& chunk);
    void memory_index_build(int handle);
    void memory_index_invalidate(int handle);
    mcp::json memory_index_search(int handle, const std::string& keyword, int top_k);
    void log_output(const std::string& text);

private:
    std::filesystem::path plugins_dir_;
    std::filesystem::path dicts_dir_;
    bool stdio_mode_ = false;
    bool initialized_ = false;
    std::mutex vm_mutex_;
    std::mutex output_mutex_;
    std::mutex memory_indices_mutex_;
    std::vector<std::unique_ptr<Runtime>> runtimes_;
    std::vector<PluginTool> tools_;
    std::vector<PluginHook> hooks_;
    size_t next_hook_order_ = 0;
    std::vector<std::unique_ptr<MemorySearchIndex>> memory_indices_;
    std::unordered_map<std::string, size_t> runtime_by_plugin_id_;

    bool ensure_vm();
    void load_plugin_dir(const std::filesystem::path& dir);
    bool load_manifest(const std::filesystem::path& dir, PluginManifest& out, std::string& error) const;
    bool execute_plugin(Runtime& runtime, std::string& error);
    void sort_hooks();
    void log_line(const std::string& text);

    mcp::json invoke_tool(size_t tool_index, const mcp::json& params);
    mcp::json call_hook(const PluginHook& hook, const mcp::json& payload, bool& modified);
};

} // namespace mcdk::plugins
