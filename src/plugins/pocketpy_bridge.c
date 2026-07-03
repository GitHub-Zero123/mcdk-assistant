#include "plugins/pocketpy_bridge.h"

#include <pocketpy.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mcdk_py_config g_cfg;
static char g_current_plugin_id[128];
static char g_current_plugin_dir[1024];
static int g_next_handler_id = 1;

static char* mcdk_strdup(const char* s) {
    if(!s) s = "";
    size_t n = strlen(s);
    char* out = (char*)malloc(n + 1);
    if(!out) return NULL;
    memcpy(out, s, n + 1);
    return out;
}

void mcdk_py_free(char* ptr) {
    free(ptr);
}

static void emit_output(const char* text) {
    if(g_cfg.output) {
        g_cfg.output(text ? text : "", g_cfg.userdata);
    }
}

static void bridge_print(const char* text) {
    emit_output(text);
}

static void bridge_flush(void) {}

static char* format_error(void) {
    char* msg = py_formatexc();
    if(!msg) return mcdk_strdup("unknown pocketPy error");
    char* out = mcdk_strdup(msg);
    py_free(msg);
    return out;
}

static bool set_error(char** error, const char* msg) {
    if(error) *error = mcdk_strdup(msg);
    return false;
}

static bool set_py_error(char** error) {
    if(error) *error = format_error();
    py_clearexc(NULL);
    return false;
}

static py_Ref assistant_module(void) {
    py_Ref mod = py_getmodule("mcdk_assistant");
    if(!mod) mod = py_newmodule("mcdk_assistant");
    return mod;
}

static py_Ref handlers_dict(void) {
    py_Ref mod = assistant_module();
    py_ItemRef handlers = py_getdict(mod, py_name("_handlers"));
    if(!handlers) {
        handlers = py_emplacedict(mod, py_name("_handlers"));
        py_newdict(handlers);
    }
    return handlers;
}

static bool native_write_output(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    emit_output(py_tostr(py_arg(0)));
    py_newnone(py_retval());
    return true;
}

static bool store_handler(py_Ref func, char* key, size_t key_size) {
    snprintf(key, key_size, "%s_%d", g_current_plugin_id, g_next_handler_id++);
    py_Ref handlers = handlers_dict();
    py_newstr(py_r0(), key);
    return py_dict_setitem(handlers, py_r0(), func);
}

static bool native_register_tool(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    if(!py_checkstr(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checkstr(py_arg(2))) return false;
    if(!py_callable(py_arg(3))) {
        return TypeError("tool handler must be callable");
    }

    char key[192];
    if(!store_handler(py_arg(3), key, sizeof(key))) return false;

    if(g_cfg.register_tool) {
        g_cfg.register_tool(g_current_plugin_id,
                            py_tostr(py_arg(0)),
                            py_tostr(py_arg(1)),
                            py_tostr(py_arg(2)),
                            key,
                            g_cfg.userdata);
    }
    py_newstr(py_retval(), key);
    return true;
}

static bool native_register_hook(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    if(!py_checkstr(py_arg(0))) return false;
    if(!py_checkint(py_arg(1))) return false;
    if(!py_callable(py_arg(2))) {
        return TypeError("hook handler must be callable");
    }

    char key[192];
    if(!store_handler(py_arg(2), key, sizeof(key))) return false;

    if(g_cfg.register_hook) {
        g_cfg.register_hook(g_current_plugin_id,
                            py_tostr(py_arg(0)),
                            (int)py_toint(py_arg(1)),
                            key,
                            g_cfg.userdata);
    }
    py_newstr(py_retval(), key);
    return true;
}

static bool native_memory_index_create(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    int handle = -1;
    if(g_cfg.memory_index_create) {
        handle = g_cfg.memory_index_create(py_tostr(py_arg(0)), g_cfg.userdata);
    }
    py_newint(py_retval(), handle);
    return true;
}

static bool native_memory_index_add_doc(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    if(!py_checkint(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checkstr(py_arg(2))) return false;
    if(!py_checkstr(py_arg(3))) return false;
    if(!py_checkint(py_arg(4))) return false;
    if(g_cfg.memory_index_add_doc) {
        g_cfg.memory_index_add_doc((int)py_toint(py_arg(0)),
                                   py_tostr(py_arg(1)),
                                   py_tostr(py_arg(2)),
                                   py_tostr(py_arg(3)),
                                   (int)py_toint(py_arg(4)),
                                   g_cfg.userdata);
    }
    py_newnone(py_retval());
    return true;
}

static bool native_memory_index_set_dir(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(4);
    if(!py_checkint(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checkstr(py_arg(2))) return false;
    if(!py_checkstr(py_arg(3))) return false;
    if(g_cfg.memory_index_set_dir) {
        g_cfg.memory_index_set_dir((int)py_toint(py_arg(0)),
                                   py_tostr(py_arg(1)),
                                   py_tostr(py_arg(2)),
                                   py_tostr(py_arg(3)),
                                   g_cfg.userdata);
    }
    py_newnone(py_retval());
    return true;
}

static bool native_memory_index_build(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkint(py_arg(0))) return false;
    if(g_cfg.memory_index_build) {
        g_cfg.memory_index_build((int)py_toint(py_arg(0)), g_cfg.userdata);
    }
    py_newnone(py_retval());
    return true;
}

static bool native_memory_index_invalidate(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkint(py_arg(0))) return false;
    if(g_cfg.memory_index_invalidate) {
        g_cfg.memory_index_invalidate((int)py_toint(py_arg(0)), g_cfg.userdata);
    }
    py_newnone(py_retval());
    return true;
}

static bool native_memory_index_search(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    if(!py_checkint(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checkint(py_arg(2))) return false;
    char* json = NULL;
    if(g_cfg.memory_index_search) {
        json = g_cfg.memory_index_search((int)py_toint(py_arg(0)),
                                         py_tostr(py_arg(1)),
                                         (int)py_toint(py_arg(2)),
                                         g_cfg.userdata);
    }
    if(!json) json = mcdk_strdup("[]");
    py_newstr(py_retval(), json);
    free(json);
    return true;
}

static const char* BOOTSTRAP_SOURCE =
    "import json\n"
    "import sys\n"
    "_handlers = {}\n"
    "def _schema_json(schema):\n"
    "    return json.dumps(_schema_to_jsonable(schema))\n"
    "class _Stream:\n"
    "    def write(self, text):\n"
    "        s = str(text)\n"
    "        _native_write_output(s)\n"
    "        return len(s)\n"
    "    def flush(self):\n"
    "        return None\n"
    "sys.stdout = _Stream()\n"
    "sys.stderr = _Stream()\n"
    "class _Hooks:\n"
    "    MINECRAFT_DOCS_SEARCH_BEFORE = 'minecraft_docs.search.before'\n"
    "    MINECRAFT_DOCS_SEARCH_AFTER_RENDER = 'minecraft_docs.search.after_render'\n"
    "hooks = _Hooks()\n"
    "class _Context:\n"
    "    def __init__(self, data=None):\n"
    "        self._data = data or {}\n"
    "        for k, v in self._data.items():\n"
    "            setattr(self, str(k), v)\n"
    "        self.plugin_id = str(self._data.get('plugin_id', ''))\n"
    "        self.plugin_dir = str(self._data.get('plugin_dir', ''))\n"
    "        self.tool = str(self._data.get('tool', ''))\n"
    "        self.event = str(self._data.get('event', ''))\n"
    "        self.priority = int(self._data.get('priority', 10))\n"
    "        self.log = log\n"
    "        self.search = search\n"
    "    def get(self, key, default=None):\n"
    "        return self._data.get(key, default)\n"
    "    def __getitem__(self, key):\n"
    "        return self._data[key]\n"
    "    def __contains__(self, key):\n"
    "        return key in self._data\n"
    "    def to_json(self):\n"
    "        out = {}\n"
    "        for k, v in self._data.items():\n"
    "            out[k] = v\n"
    "        return out\n"
    "Context = _Context\n"
    "ToolContext = _Context\n"
    "HookContext = _Context\n"
    "def _wrap_handler(func):\n"
    "    def wrapped(args, ctx):\n"
    "        return func(args, _Context(ctx))\n"
    "    return wrapped\n"
    "def _schema_to_jsonable(value):\n"
    "    if value is None:\n"
    "        return {'type': 'object'}\n"
    "    if hasattr(value, 'to_json'):\n"
    "        return value.to_json()\n"
    "    if isinstance(value, dict):\n"
    "        out = {}\n"
    "        for k, v in value.items():\n"
    "            out[k] = _schema_to_jsonable(v)\n"
    "        return out\n"
    "    if isinstance(value, list):\n"
    "        return [_schema_to_jsonable(v) for v in value]\n"
    "    return value\n"
    "class _SchemaField:\n"
    "    def __init__(self, type_name, description='', required=False, default=None, enum=None, minimum=None, maximum=None):\n"
    "        self.required = bool(required)\n"
    "        self._data = {'type': str(type_name)}\n"
    "        if description:\n"
    "            self._data['description'] = str(description)\n"
    "        if default is not None:\n"
    "            self._data['default'] = default\n"
    "        if enum is not None:\n"
    "            self._data['enum'] = enum\n"
    "        if minimum is not None:\n"
    "            self._data['minimum'] = minimum\n"
    "        if maximum is not None:\n"
    "            self._data['maximum'] = maximum\n"
    "    def to_json(self):\n"
    "        out = {}\n"
    "        for k, v in self._data.items():\n"
    "            out[k] = v\n"
    "        return out\n"
    "class _ArraySchema:\n"
    "    def __init__(self, items=None, description='', required=False):\n"
    "        self.items = items or _SchemaField('string')\n"
    "        self.description = description\n"
    "        self.required = bool(required)\n"
    "    def to_json(self):\n"
    "        out = {'type': 'array', 'items': _schema_to_jsonable(self.items)}\n"
    "        if self.description:\n"
    "            out['description'] = str(self.description)\n"
    "        return out\n"
    "class _ObjectSchema:\n"
    "    def __init__(self, properties=None, description=''):\n"
    "        self._properties = {}\n"
    "        self._required = []\n"
    "        self._description = description\n"
    "        if properties:\n"
    "            for k, v in properties.items():\n"
    "                self.field(k, v)\n"
    "    def field(self, name, schema, required=None):\n"
    "        key = str(name)\n"
    "        self._properties[key] = schema\n"
    "        is_required = schema.required if required is None and hasattr(schema, 'required') else required\n"
    "        if is_required and key not in self._required:\n"
    "            self._required.append(key)\n"
    "        return self\n"
    "    def require(self, *names):\n"
    "        for name in names:\n"
    "            key = str(name)\n"
    "            if key not in self._required:\n"
    "                self._required.append(key)\n"
    "        return self\n"
    "    def to_json(self):\n"
    "        props = {}\n"
    "        for k, v in self._properties.items():\n"
    "            props[k] = _schema_to_jsonable(v)\n"
    "        out = {'type': 'object', 'properties': props}\n"
    "        if self._description:\n"
    "            out['description'] = str(self._description)\n"
    "        if self._required:\n"
    "            out['required'] = list(self._required)\n"
    "        return out\n"
    "class _Schema:\n"
    "    def String(self, description='', required=False, default=None, enum=None):\n"
    "        return _SchemaField('string', description, required, default, enum)\n"
    "    def Integer(self, description='', required=False, default=None, enum=None, minimum=None, maximum=None):\n"
    "        return _SchemaField('integer', description, required, default, enum, minimum, maximum)\n"
    "    def Number(self, description='', required=False, default=None, enum=None, minimum=None, maximum=None):\n"
    "        return _SchemaField('number', description, required, default, enum, minimum, maximum)\n"
    "    def Boolean(self, description='', required=False, default=None):\n"
    "        return _SchemaField('boolean', description, required, default)\n"
    "    def Array(self, items=None, description='', required=False):\n"
    "        return _ArraySchema(items, description, required)\n"
    "    def Object(self, properties=None, description=''):\n"
    "        return _ObjectSchema(properties, description)\n"
    "schema = _Schema()\n"
    "def register_tool(name, description='', schema=None, handler=None):\n"
    "    if handler is None:\n"
    "        def deco(func):\n"
    "            _native_register_tool(name, description, _schema_json(schema), _wrap_handler(func))\n"
    "            return func\n"
    "        return deco\n"
    "    _native_register_tool(name, description, _schema_json(schema), _wrap_handler(handler))\n"
    "    return handler\n"
    "def tool(name, description='', schema=None):\n"
    "    return register_tool(name, description, schema)\n"
    "def register_hook(event, handler=None, priority=10):\n"
    "    if handler is None:\n"
    "        def deco(func):\n"
    "            _native_register_hook(event, int(priority), _wrap_handler(func))\n"
    "            return func\n"
    "        return deco\n"
    "    _native_register_hook(event, int(priority), _wrap_handler(handler))\n"
    "    return handler\n"
    "def hook(event, priority=10):\n"
    "    return register_hook(event, None, priority)\n"
    "class _Log:\n"
    "    def info(self, text): _native_write_output('[plugin][info] ' + str(text) + '\\n')\n"
    "    def warn(self, text): _native_write_output('[plugin][warn] ' + str(text) + '\\n')\n"
    "    def error(self, text): _native_write_output('[plugin][error] ' + str(text) + '\\n')\n"
    "log = _Log()\n"
    "class MemoryIndex:\n"
    "    def __init__(self, mode='zh'):\n"
    "        self._handle = _native_memory_index_create(str(mode))\n"
    "    def add_doc(self, id, text, metadata=None, file='', line_start=1):\n"
    "        if metadata and not file:\n"
    "            file = metadata.get('file', '')\n"
    "            line_start = int(metadata.get('line_start', line_start))\n"
    "        _native_memory_index_add_doc(self._handle, str(id), str(text), str(file), int(line_start))\n"
    "    def set_dir(self, root, glob=None, chunk='markdown_heading'):\n"
    "        _native_memory_index_set_dir(self._handle, str(root), json.dumps(glob or ['**/*.md', '**/*.txt']), str(chunk))\n"
    "    def build(self): _native_memory_index_build(self._handle)\n"
    "    def invalidate(self): _native_memory_index_invalidate(self._handle)\n"
    "    def rebuild(self): self.invalidate(); self.build()\n"
    "    def search(self, keyword, top_k=6):\n"
    "        return json.loads(_native_memory_index_search(self._handle, str(keyword), int(top_k)))\n"
    "class _Search:\n"
    "    MemoryIndex = MemoryIndex\n"
    "    def build_index_from_dir(self, root, glob=None, mode='zh', chunk='markdown_heading', lazy=True):\n"
    "        idx = MemoryIndex(mode)\n"
    "        idx.set_dir(root, glob, chunk)\n"
    "        if not lazy:\n"
    "            idx.build()\n"
    "        return idx\n"
    "search = _Search()\n";

bool mcdk_py_initialize(const mcdk_py_config* config, char** error) {
    if(config) g_cfg = *config;
    py_initialize();
    py_callbacks()->print = bridge_print;
    py_callbacks()->flush = bridge_flush;

    py_Ref mod = assistant_module();
    py_bindfunc(mod, "_native_write_output", native_write_output);
    py_bindfunc(mod, "_native_register_tool", native_register_tool);
    py_bindfunc(mod, "_native_register_hook", native_register_hook);
    py_bindfunc(mod, "_native_memory_index_create", native_memory_index_create);
    py_bindfunc(mod, "_native_memory_index_add_doc", native_memory_index_add_doc);
    py_bindfunc(mod, "_native_memory_index_set_dir", native_memory_index_set_dir);
    py_bindfunc(mod, "_native_memory_index_build", native_memory_index_build);
    py_bindfunc(mod, "_native_memory_index_invalidate", native_memory_index_invalidate);
    py_bindfunc(mod, "_native_memory_index_search", native_memory_index_search);

    if(!py_exec(BOOTSTRAP_SOURCE, "<mcdk_assistant>", EXEC_MODE, mod)) {
        return set_py_error(error);
    }
    return true;
}

void mcdk_py_finalize(void) {
    py_finalize();
    memset(&g_cfg, 0, sizeof(g_cfg));
}

bool mcdk_py_load_plugin(const char* plugin_id,
                         const char* plugin_dir,
                         const char* entry_path,
                         char** error) {
    snprintf(g_current_plugin_id, sizeof(g_current_plugin_id), "%s", plugin_id ? plugin_id : "");
    snprintf(g_current_plugin_dir, sizeof(g_current_plugin_dir), "%s", plugin_dir ? plugin_dir : "");

    FILE* fp = fopen(entry_path, "rb");
    if(!fp) return set_error(error, "cannot open plugin entry");
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(size < 0) {
        fclose(fp);
        return set_error(error, "cannot read plugin entry");
    }
    char* source = (char*)malloc((size_t)size + 1);
    if(!source) {
        fclose(fp);
        return set_error(error, "out of memory");
    }
    size_t read = fread(source, 1, (size_t)size, fp);
    fclose(fp);
    source[read] = '\0';

    py_Ref main_mod = py_getmodule("__main__");
    if(!main_mod) main_mod = py_newmodule("__main__");
    py_newstr(py_r0(), entry_path ? entry_path : "");
    py_setdict(main_mod, py_name("__file__"), py_r0());
    py_newstr(py_r0(), g_current_plugin_dir);
    py_setdict(main_mod, py_name("__plugin_dir__"), py_r0());
    py_newstr(py_r0(), g_current_plugin_id);
    py_setdict(main_mod, py_name("__plugin_id__"), py_r0());
    py_Ref sys_mod = py_getmodule("sys");
    if(sys_mod) {
        py_ItemRef sys_path = py_getdict(sys_mod, py_name("path"));
        if(sys_path && py_islist(sys_path)) {
            py_newstr(py_r0(), g_current_plugin_dir);
            py_list_insert(sys_path, 0, py_r0());
        }
    }

    bool ok = py_exec(source, entry_path ? entry_path : "<plugin>", EXEC_MODE, main_mod);
    free(source);
    if(!ok) return set_py_error(error);
    return true;
}

bool mcdk_py_call_handler_json(const char* handler_key,
                               const char* params_json,
                               const char* ctx_json,
                               char** out_json,
                               char** error) {
    py_Ref handlers = handlers_dict();
    int found = py_dict_getitem_by_str(handlers, handler_key);
    if(found < 0) return set_py_error(error);
    if(found == 0) return set_error(error, "plugin handler not found");
    py_assign(py_r2(), py_retval());

    if(!py_json_loads(params_json ? params_json : "{}")) return set_py_error(error);
    py_assign(py_r0(), py_retval());
    if(!py_json_loads(ctx_json ? ctx_json : "{}")) return set_py_error(error);
    py_assign(py_r1(), py_retval());

    if(!py_call(py_r2(), 2, py_r0())) return set_py_error(error);
    if(!py_json_dumps(py_retval(), -1)) return set_py_error(error);

    const char* text = py_tostr(py_retval());
    if(out_json) *out_json = mcdk_strdup(text);
    return true;
}
