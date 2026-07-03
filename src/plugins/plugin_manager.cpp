#include "plugins/plugin_manager.h"

#include "common/path_utils.hpp"
#include "plugins/pocketpy_bridge.h"
#include "search/memory_search_index.hpp"

#include <mcp_tool.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace mcdk::plugins {

namespace {

char* dup_for_c(const std::string& text) {
    char* out = static_cast<char*>(std::malloc(text.size() + 1));
    if (!out) return nullptr;
    std::memcpy(out, text.c_str(), text.size() + 1);
    return out;
}

std::string safe_string(const char* value) {
    return value ? std::string(value) : std::string();
}

int clamp_priority(int priority) {
    if (priority < 0) return 0;
    return priority;
}

bool is_mcp_tool_result(const mcp::json& value) {
    return value.is_object() && value.contains("content") && value["content"].is_array();
}

mcp::json text_tool_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

mcp::json adapt_tool_return(const mcp::json& value) {
    if (is_mcp_tool_result(value)) return value;
    if (value.is_null()) return text_tool_result("");
    if (value.is_string()) return text_tool_result(value.get<std::string>());
    return text_tool_result(value.dump(2));
}

std::vector<std::string> parse_string_array(const char* json_text) {
    std::vector<std::string> values;
    try {
        auto parsed = mcp::json::parse(json_text ? json_text : "[]");
        if (!parsed.is_array()) return values;
        for (const auto& item : parsed) {
            if (item.is_string()) values.push_back(item.get<std::string>());
        }
    } catch (...) {
    }
    return values;
}

PluginManager* from_userdata(void* userdata) {
    return static_cast<PluginManager*>(userdata);
}

void bridge_output(const char* text, void* userdata) {
    if (auto* self = from_userdata(userdata)) self->log_output(safe_string(text));
}

void bridge_register_tool(const char* plugin_id,
                          const char* name,
                          const char* description,
                          const char* schema_json,
                          const char* handler_key,
                          void* userdata) {
    auto* self = from_userdata(userdata);
    if (!self) return;
    mcp::json schema = mcp::json::object({{"type", "object"}});
    try {
        if (schema_json && *schema_json) schema = mcp::json::parse(schema_json);
    } catch (const std::exception& e) {
        self->log_output("[plugin] invalid tool schema for " + safe_string(name) + ": " + e.what() + "\n");
    }
    self->add_tool(safe_string(plugin_id),
                   safe_string(name),
                   safe_string(description),
                   std::move(schema),
                   safe_string(handler_key));
}

void bridge_register_hook(const char* plugin_id,
                          const char* event,
                          int priority,
                          const char* handler_key,
                          void* userdata) {
    auto* self = from_userdata(userdata);
    if (!self) return;
    self->add_hook(safe_string(plugin_id),
                   safe_string(event),
                   priority,
                   safe_string(handler_key));
}

int bridge_memory_index_create(const char* mode, void* userdata) {
    auto* self = from_userdata(userdata);
    return self ? self->create_memory_index(safe_string(mode)) : -1;
}

void bridge_memory_index_add_doc(int handle,
                                 const char* id,
                                 const char* text,
                                 const char* file,
                                 int line_start,
                                 void* userdata) {
    if (auto* self = from_userdata(userdata)) {
        self->memory_index_add_doc(handle, safe_string(id), safe_string(text), safe_string(file), line_start);
    }
}

void bridge_memory_index_set_dir(int handle,
                                 const char* root,
                                 const char* globs_json,
                                 const char* chunk,
                                 void* userdata) {
    if (auto* self = from_userdata(userdata)) {
        self->memory_index_set_dir(handle, safe_string(root), parse_string_array(globs_json), safe_string(chunk));
    }
}

void bridge_memory_index_build(int handle, void* userdata) {
    if (auto* self = from_userdata(userdata)) self->memory_index_build(handle);
}

void bridge_memory_index_invalidate(int handle, void* userdata) {
    if (auto* self = from_userdata(userdata)) self->memory_index_invalidate(handle);
}

char* bridge_memory_index_search(int handle, const char* keyword, int top_k, void* userdata) {
    auto* self = from_userdata(userdata);
    if (!self) return dup_for_c("[]");
    return dup_for_c(self->memory_index_search(handle, safe_string(keyword), top_k).dump());
}

} // namespace

struct PluginManager::Runtime {
    PluginManifest manifest;
    size_t load_order = 0;
};

PluginManager::PluginManager(std::filesystem::path plugins_dir,
                             std::filesystem::path dicts_dir,
                             bool stdio_mode)
    : plugins_dir_(std::move(plugins_dir))
    , dicts_dir_(std::move(dicts_dir))
    , stdio_mode_(stdio_mode) {}

PluginManager::~PluginManager() {
    std::lock_guard<std::mutex> lock(vm_mutex_);
    if (initialized_) {
        mcdk_py_finalize();
        initialized_ = false;
    }
}

bool PluginManager::ensure_vm() {
    if (initialized_) return true;
    mcdk_py_config cfg{};
    cfg.stdio_mode = stdio_mode_;
    cfg.userdata = this;
    cfg.output = bridge_output;
    cfg.register_tool = bridge_register_tool;
    cfg.register_hook = bridge_register_hook;
    cfg.memory_index_create = bridge_memory_index_create;
    cfg.memory_index_add_doc = bridge_memory_index_add_doc;
    cfg.memory_index_set_dir = bridge_memory_index_set_dir;
    cfg.memory_index_build = bridge_memory_index_build;
    cfg.memory_index_invalidate = bridge_memory_index_invalidate;
    cfg.memory_index_search = bridge_memory_index_search;

    char* error = nullptr;
    if (!mcdk_py_initialize(&cfg, &error)) {
        log_output(std::string("[plugin] pocketPy init failed: ") + (error ? error : "unknown") + "\n");
        mcdk_py_free(error);
        return false;
    }
    initialized_ = true;
    return true;
}

bool PluginManager::load_all() {
    namespace fs = std::filesystem;
    std::lock_guard<std::mutex> lock(vm_mutex_);
    if (!fs::exists(plugins_dir_) || !fs::is_directory(plugins_dir_)) return false;
    if (!ensure_vm()) return false;

    std::vector<fs::path> dirs;
    for (const auto& entry : fs::directory_iterator(plugins_dir_)) {
        if (entry.is_directory()) dirs.push_back(entry.path());
    }
    std::sort(dirs.begin(), dirs.end());

    for (const auto& dir : dirs) {
        load_plugin_dir(dir);
    }
    sort_hooks();
    return !runtimes_.empty();
}

void PluginManager::load_plugin_dir(const std::filesystem::path& dir) {
    PluginManifest manifest;
    std::string error;
    if (!load_manifest(dir, manifest, error)) {
        log_output("[plugin] skip " + mcdk::path::to_utf8(dir) + ": " + error + "\n");
        return;
    }

    auto runtime = std::make_unique<Runtime>();
    runtime->manifest = std::move(manifest);
    runtime->load_order = runtimes_.size();
    const auto plugin_id = runtime->manifest.id;

    if (!execute_plugin(*runtime, error)) {
        log_output("[plugin] failed loading " + plugin_id + ": " + error + "\n");
        return;
    }

    runtime_by_plugin_id_[plugin_id] = runtimes_.size();
    runtimes_.push_back(std::move(runtime));
    log_output("[plugin] loaded " + plugin_id + "\n");
}

bool PluginManager::load_manifest(const std::filesystem::path& dir,
                                  PluginManifest& out,
                                  std::string& error) const {
    const auto path = dir / "manifest.json";
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
        error = "manifest.json not found";
        return false;
    }

    mcp::json manifest;
    try {
        ifs >> manifest;
    } catch (const std::exception& e) {
        error = e.what();
        return false;
    }

    auto required_string = [&](const char* key, std::string& target) -> bool {
        if (!manifest.contains(key) || !manifest[key].is_string() || manifest[key].get<std::string>().empty()) {
            error = std::string("manifest field is required: ") + key;
            return false;
        }
        target = manifest[key].get<std::string>();
        return true;
    };

    if (!required_string("id", out.id)) return false;
    if (!required_string("name", out.name)) return false;
    if (!required_string("version", out.version)) return false;
    if (!required_string("entry", out.entry)) return false;
    if (manifest.contains("description") && manifest["description"].is_string()) {
        out.description = manifest["description"].get<std::string>();
    }
    out.dir = dir;

    if (out.id.find_first_of(" \t\r\n/\\:") != std::string::npos) {
        error = "manifest id contains invalid characters";
        return false;
    }
    const auto entry_path = dir / mcdk::path::from_utf8(out.entry);
    if (!std::filesystem::is_regular_file(entry_path)) {
        error = "entry file not found: " + out.entry;
        return false;
    }
    return true;
}

bool PluginManager::execute_plugin(Runtime& runtime, std::string& error) {
    const auto entry_path = runtime.manifest.dir / mcdk::path::from_utf8(runtime.manifest.entry);
    char* bridge_error = nullptr;
    const bool ok = mcdk_py_load_plugin(runtime.manifest.id.c_str(),
                                        mcdk::path::to_utf8(runtime.manifest.dir).c_str(),
                                        mcdk::path::to_utf8(entry_path).c_str(),
                                        &bridge_error);
    if (!ok) {
        error = bridge_error ? bridge_error : "unknown error";
        mcdk_py_free(bridge_error);
        return false;
    }
    return true;
}

void PluginManager::register_tools(mcp::server& srv) {
    std::unordered_set<std::string> names;
    for (size_t i = 0; i < tools_.size(); ++i) {
        auto& plugin_tool = tools_[i];
        std::string public_name = plugin_tool.local_name;
        if (!names.insert(public_name).second) {
            public_name = plugin_tool.plugin_id + "." + plugin_tool.local_name;
            names.insert(public_name);
        }
        plugin_tool.public_name = public_name;

        mcp::tool tool;
        tool.name = public_name;
        tool.description = plugin_tool.description.empty()
            ? ("Plugin tool from " + plugin_tool.plugin_id)
            : plugin_tool.description;
        tool.parameters_schema = plugin_tool.schema.is_object()
            ? plugin_tool.schema
            : mcp::json::object({{"type", "object"}});
        tool.annotations.read_only_hint = false;
        tool.annotations.open_world_hint = true;

        srv.register_tool(tool, [this, i](const mcp::json& params, const std::string&) -> mcp::json {
            return invoke_tool(i, params);
        });
    }
}

mcp::json PluginManager::invoke_tool(size_t tool_index, const mcp::json& params) {
    if (tool_index >= tools_.size()) {
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "plugin tool not found");
    }
    const auto& tool = tools_[tool_index];
    mcp::json ctx = {
        {"plugin_id", tool.plugin_id},
        {"tool", tool.local_name},
    };

    std::lock_guard<std::mutex> lock(vm_mutex_);
    char* out_json = nullptr;
    char* error = nullptr;
    const bool ok = mcdk_py_call_handler_json(tool.handler_key.c_str(),
                                              params.dump().c_str(),
                                              ctx.dump().c_str(),
                                              &out_json,
                                              &error);
    if (!ok) {
        std::string msg = error ? error : "plugin tool failed";
        mcdk_py_free(error);
        throw mcp::mcp_exception(mcp::error_code::internal_error, msg);
    }

    try {
        auto value = mcp::json::parse(out_json ? out_json : "null");
        mcdk_py_free(out_json);
        return adapt_tool_return(value);
    } catch (const std::exception& e) {
        mcdk_py_free(out_json);
        throw mcp::mcp_exception(mcp::error_code::internal_error,
                                 std::string("invalid plugin return JSON: ") + e.what());
    }
}

mcp::json PluginManager::run_hook(const std::string& event, const mcp::json& payload) {
    mcp::json current = payload;
    for (const auto& hook : hooks_) {
        if (hook.event != event) continue;
        bool modified = false;
        mcp::json next = call_hook(hook, current, modified);
        if (modified) current = std::move(next);
    }
    return current;
}

mcp::json PluginManager::call_hook(const PluginHook& hook, const mcp::json& payload, bool& modified) {
    modified = false;
    mcp::json ctx = {
        {"plugin_id", hook.plugin_id},
        {"event", hook.event},
        {"priority", hook.priority},
    };

    std::lock_guard<std::mutex> lock(vm_mutex_);
    char* out_json = nullptr;
    char* error = nullptr;
    const bool ok = mcdk_py_call_handler_json(hook.handler_key.c_str(),
                                              payload.dump().c_str(),
                                              ctx.dump().c_str(),
                                              &out_json,
                                              &error);
    if (!ok) {
        log_output("[plugin] hook " + hook.event + " failed in " + hook.plugin_id + ": " +
                   (error ? error : "unknown") + "\n");
        mcdk_py_free(error);
        return payload;
    }

    try {
        auto value = mcp::json::parse(out_json ? out_json : "null");
        mcdk_py_free(out_json);
        if (value.is_null()) return payload;
        modified = true;
        return value;
    } catch (const std::exception& e) {
        mcdk_py_free(out_json);
        log_output("[plugin] hook " + hook.event + " returned invalid JSON: " + e.what() + "\n");
        return payload;
    }
}

PluginManager::Runtime* PluginManager::current_runtime() {
    return runtimes_.empty() ? nullptr : runtimes_.back().get();
}

void PluginManager::add_tool(std::string plugin_id,
                             std::string name,
                             std::string description,
                             mcp::json schema,
                             std::string handler_key) {
    if (plugin_id.empty() || name.empty() || handler_key.empty()) return;
    PluginTool tool;
    tool.plugin_id = std::move(plugin_id);
    tool.local_name = std::move(name);
    tool.public_name = tool.local_name;
    tool.description = std::move(description);
    tool.schema = std::move(schema);
    tool.handler_key = std::move(handler_key);
    tools_.push_back(std::move(tool));
}

void PluginManager::add_hook(std::string plugin_id,
                             std::string event,
                             int priority,
                             std::string handler_key) {
    if (plugin_id.empty() || event.empty() || handler_key.empty()) return;
    PluginHook hook;
    hook.plugin_id = std::move(plugin_id);
    hook.event = std::move(event);
    hook.priority = clamp_priority(priority);
    hook.order = next_hook_order_++;
    hook.handler_key = std::move(handler_key);
    hooks_.push_back(std::move(hook));
}

void PluginManager::sort_hooks() {
    std::stable_sort(hooks_.begin(), hooks_.end(), [](const PluginHook& a, const PluginHook& b) {
        if (a.event != b.event) return a.event < b.event;
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.order < b.order;
    });
}

int PluginManager::create_memory_index(const std::string& mode) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    auto index = std::make_unique<MemorySearchIndex>(dicts_dir_, mode.empty() ? "zh" : mode);
    memory_indices_.push_back(std::move(index));
    return static_cast<int>(memory_indices_.size() - 1);
}

void PluginManager::memory_index_add_doc(int handle,
                                         const std::string& id,
                                         const std::string& text,
                                         const std::string& file,
                                         int line_start) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    if (handle < 0 || static_cast<size_t>(handle) >= memory_indices_.size()) return;
    memory_indices_[static_cast<size_t>(handle)]->add_doc(id, text, file, line_start);
}

void PluginManager::memory_index_set_dir(int handle,
                                         const std::string& root,
                                         const std::vector<std::string>& globs,
                                         const std::string& chunk) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    if (handle < 0 || static_cast<size_t>(handle) >= memory_indices_.size()) return;
    MemorySearchIndex::DirSource source;
    source.root = mcdk::path::from_utf8(root);
    source.globs = globs;
    source.chunk = chunk.empty() ? "markdown_heading" : chunk;
    memory_indices_[static_cast<size_t>(handle)]->set_dir_source(std::move(source));
}

void PluginManager::memory_index_build(int handle) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    if (handle < 0 || static_cast<size_t>(handle) >= memory_indices_.size()) return;
    memory_indices_[static_cast<size_t>(handle)]->build();
}

void PluginManager::memory_index_invalidate(int handle) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    if (handle < 0 || static_cast<size_t>(handle) >= memory_indices_.size()) return;
    memory_indices_[static_cast<size_t>(handle)]->invalidate();
}

mcp::json PluginManager::memory_index_search(int handle, const std::string& keyword, int top_k) {
    std::lock_guard<std::mutex> lock(memory_indices_mutex_);
    if (handle < 0 || static_cast<size_t>(handle) >= memory_indices_.size()) return mcp::json::array();
    auto results = memory_indices_[static_cast<size_t>(handle)]->search(keyword, top_k > 0 ? top_k : 6);
    mcp::json arr = mcp::json::array();
    for (const auto& r : results) {
        arr.push_back({
            {"text", r.fragment->content},
            {"file", r.fragment->file},
            {"line_start", r.fragment->line_start},
            {"line_end", r.fragment->line_end},
            {"score", r.score},
        });
    }
    return arr;
}

void PluginManager::log_output(const std::string& text) {
    std::lock_guard<std::mutex> lock(output_mutex_);
    std::cerr << text;
    if (!text.empty() && text.back() != '\n') std::cerr << std::endl;
}

void PluginManager::log_line(const std::string& text) {
    log_output(text + "\n");
}

} // namespace mcdk::plugins
