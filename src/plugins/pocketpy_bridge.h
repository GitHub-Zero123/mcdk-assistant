#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mcdk_py_output_fn)(const char* text, void* userdata);
typedef void (*mcdk_py_register_tool_fn)(const char* plugin_id,
                                         const char* name,
                                         const char* description,
                                         const char* schema_json,
                                         const char* handler_key,
                                         void* userdata);
typedef void (*mcdk_py_register_hook_fn)(const char* plugin_id,
                                         const char* event,
                                         int priority,
                                         const char* handler_key,
                                         void* userdata);

typedef int (*mcdk_py_memory_index_create_fn)(const char* mode, void* userdata);
typedef void (*mcdk_py_memory_index_add_doc_fn)(int handle,
                                                const char* id,
                                                const char* text,
                                                const char* file,
                                                int line_start,
                                                void* userdata);
typedef void (*mcdk_py_memory_index_set_dir_fn)(int handle,
                                                const char* root,
                                                const char* globs_json,
                                                const char* chunk,
                                                void* userdata);
typedef void (*mcdk_py_memory_index_build_fn)(int handle, void* userdata);
typedef void (*mcdk_py_memory_index_invalidate_fn)(int handle, void* userdata);
typedef char* (*mcdk_py_memory_index_search_fn)(int handle,
                                                const char* keyword,
                                                int top_k,
                                                void* userdata);

typedef struct mcdk_py_config {
    bool stdio_mode;
    void* userdata;
    mcdk_py_output_fn output;
    mcdk_py_register_tool_fn register_tool;
    mcdk_py_register_hook_fn register_hook;
    mcdk_py_memory_index_create_fn memory_index_create;
    mcdk_py_memory_index_add_doc_fn memory_index_add_doc;
    mcdk_py_memory_index_set_dir_fn memory_index_set_dir;
    mcdk_py_memory_index_build_fn memory_index_build;
    mcdk_py_memory_index_invalidate_fn memory_index_invalidate;
    mcdk_py_memory_index_search_fn memory_index_search;
} mcdk_py_config;

bool mcdk_py_initialize(const mcdk_py_config* config, char** error);
void mcdk_py_finalize(void);

bool mcdk_py_load_plugin(const char* plugin_id,
                         const char* plugin_dir,
                         const char* entry_path,
                         char** error);

bool mcdk_py_call_handler_json(const char* handler_key,
                               const char* params_json,
                               const char* ctx_json,
                               char** out_json,
                               char** error);

void mcdk_py_free(char* ptr);

#ifdef __cplusplus
}
#endif
