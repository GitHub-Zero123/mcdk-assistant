#include "plugins/pocketpy_bridge.h"

#include <pocketpy.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mcdk_py_config g_cfg;
static char* g_current_plugin_id = NULL;
static char* g_current_plugin_dir = NULL;
static int g_next_handler_id = 1;
static bool g_initialized = false;
static bool g_finalized = false;

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

static void set_current_plugin(const char* plugin_id, const char* plugin_dir) {
    free(g_current_plugin_id);
    free(g_current_plugin_dir);
    g_current_plugin_id = mcdk_strdup(plugin_id ? plugin_id : "");
    g_current_plugin_dir = mcdk_strdup(plugin_dir ? plugin_dir : "");
}

static const char* current_plugin_id(void) {
    return g_current_plugin_id ? g_current_plugin_id : "";
}

static const char* current_plugin_dir(void) {
    return g_current_plugin_dir ? g_current_plugin_dir : "";
}

static bool is_absolute_path_text(const char* path) {
    if(!path || !*path) return false;
    if(path[0] == '/' || path[0] == '\\') return true;
    return strlen(path) >= 3 && path[1] == ':' && (path[2] == '/' || path[2] == '\\');
}

static char* join_plugin_path(const char* path) {
    if(!path) path = "";
    if(is_absolute_path_text(path)) return mcdk_strdup(path);

    const char* root = current_plugin_dir();
    if(!root || !*root) return mcdk_strdup(path);

    size_t root_len = strlen(root);
    size_t path_len = strlen(path);
    bool need_sep = root_len > 0 && root[root_len - 1] != '/' && root[root_len - 1] != '\\';
    char* out = (char*)malloc(root_len + (need_sep ? 1 : 0) + path_len + 1);
    if(!out) return NULL;
    memcpy(out, root, root_len);
    size_t pos = root_len;
    if(need_sep) out[pos++] = '/';
    memcpy(out + pos, path, path_len + 1);
    return out;
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
    snprintf(key, key_size, "%s_%d", current_plugin_id(), g_next_handler_id++);
    py_Ref handlers = handlers_dict();
    py_newstr(py_r0(), key);
    return py_dict_setitem(handlers, py_r0(), func);
}

static bool native_register_tool(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(5);
    if(!py_checkstr(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checkstr(py_arg(2))) return false;
    if(!py_checkstr(py_arg(3))) return false;
    if(!py_callable(py_arg(4))) {
        return TypeError("tool handler must be callable");
    }

    char key[192];
    if(!store_handler(py_arg(4), key, sizeof(key))) return false;

    if(g_cfg.register_tool) {
        g_cfg.register_tool(current_plugin_id(),
                            py_tostr(py_arg(0)),
                            py_tostr(py_arg(1)),
                            py_tostr(py_arg(2)),
                            py_tostr(py_arg(3)),
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
        g_cfg.register_hook(current_plugin_id(),
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

static bool native_fs_scandir(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    char* json = NULL;
    if(g_cfg.fs_scandir) {
        json = g_cfg.fs_scandir(py_tostr(py_arg(0)), g_cfg.userdata);
    }
    if(!json) json = mcdk_strdup("[]");
    py_newstr(py_retval(), json);
    free(json);
    return true;
}

static bool native_fs_exists(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    py_newbool(py_retval(), g_cfg.fs_exists ? g_cfg.fs_exists(py_tostr(py_arg(0)), g_cfg.userdata) : false);
    return true;
}

static bool native_fs_isfile(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    py_newbool(py_retval(), g_cfg.fs_is_file ? g_cfg.fs_is_file(py_tostr(py_arg(0)), g_cfg.userdata) : false);
    return true;
}

static bool native_fs_isdir(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    py_newbool(py_retval(), g_cfg.fs_is_dir ? g_cfg.fs_is_dir(py_tostr(py_arg(0)), g_cfg.userdata) : false);
    return true;
}

static bool native_read_text_file(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(1);
    if(!py_checkstr(py_arg(0))) return false;
    if(!g_cfg.read_text_file) return OSError("host read_text_file callback is not configured");
    char* text = g_cfg.read_text_file(py_tostr(py_arg(0)), g_cfg.userdata);
    if(!text) return OSError("cannot read file: '%s'", py_tostr(py_arg(0)));
    py_newstr(py_retval(), text);
    free(text);
    return true;
}

static bool native_write_text_file(int argc, py_StackRef argv) {
    PY_CHECK_ARGC(3);
    if(!py_checkstr(py_arg(0))) return false;
    if(!py_checkstr(py_arg(1))) return false;
    if(!py_checktype(py_arg(2), tp_bool)) return false;
    if(!g_cfg.write_text_file) return OSError("host write_text_file callback is not configured");
    bool ok = g_cfg.write_text_file(py_tostr(py_arg(0)),
                                    py_tostr(py_arg(1)),
                                    py_tobool(py_arg(2)),
                                    g_cfg.userdata);
    if(!ok) return OSError("cannot write file: '%s'", py_tostr(py_arg(0)));
    py_newnone(py_retval());
    return true;
}

static char* bridge_importfile(const char* path, int* data_size) {
    if(!g_cfg.read_text_file) return NULL;
    char* full_path = join_plugin_path(path);
    if(!full_path) return NULL;
    char* text = g_cfg.read_text_file(full_path, g_cfg.userdata);
    free(full_path);
    if(!text) return NULL;

    size_t n = strlen(text);
    char* out = (char*)PK_MALLOC(n + 1);
    if(!out) {
        free(text);
        return NULL;
    }
    memcpy(out, text, n + 1);
    free(text);
    if(data_size) *data_size = (int)n;
    return out;
}

static const char* BOOTSTRAP_SOURCE =
    "import json\n"
    "import sys\n"
    "try:\n"
    "    import builtins as _builtins\n"
    "except Exception:\n"
    "    _builtins = None\n"
    "try:\n"
    "    import os as _os\n"
    "except Exception:\n"
    "    _os = None\n"
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
    "        self.fs = fs\n"
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
    "class ToolOptions:\n"
    "    def __init__(self, read_only=None, idempotent=None, open_world=None, destructive=None):\n"
    "        self.read_only = read_only\n"
    "        self.idempotent = idempotent\n"
    "        self.open_world = open_world\n"
    "        self.destructive = destructive\n"
    "    def to_annotations(self):\n"
    "        out = {}\n"
    "        if self.read_only is not None:\n"
    "            out['readOnlyHint'] = bool(self.read_only)\n"
    "        if self.idempotent is not None:\n"
    "            out['idempotentHint'] = bool(self.idempotent)\n"
    "        if self.open_world is not None:\n"
    "            out['openWorldHint'] = bool(self.open_world)\n"
    "        if self.destructive is not None:\n"
    "            out['destructiveHint'] = bool(self.destructive)\n"
    "        return out\n"
    "def _tool_options_json(options):\n"
    "    if options is None:\n"
    "        return '{}'\n"
    "    if hasattr(options, 'to_annotations'):\n"
    "        return json.dumps(options.to_annotations())\n"
    "    if isinstance(options, dict):\n"
    "        out = {}\n"
    "        pairs = [\n"
    "            ('read_only', 'readOnlyHint'), ('readOnlyHint', 'readOnlyHint'),\n"
    "            ('idempotent', 'idempotentHint'), ('idempotentHint', 'idempotentHint'),\n"
    "            ('open_world', 'openWorldHint'), ('openWorldHint', 'openWorldHint'),\n"
    "            ('destructive', 'destructiveHint'), ('destructiveHint', 'destructiveHint'),\n"
    "        ]\n"
    "        for src, dst in pairs:\n"
    "            if src in options and options[src] is not None:\n"
    "                out[dst] = bool(options[src])\n"
    "        return json.dumps(out)\n"
    "    return '{}'\n"
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
    "def register_tool(name, description='', schema=None, handler=None, options=None):\n"
    "    if handler is None:\n"
    "        def deco(func):\n"
    "            _native_register_tool(name, description, _schema_json(schema), _tool_options_json(options), _wrap_handler(func))\n"
    "            return func\n"
    "        return deco\n"
    "    _native_register_tool(name, description, _schema_json(schema), _tool_options_json(options), _wrap_handler(handler))\n"
    "    return handler\n"
    "def tool(name, description='', schema=None, options=None):\n"
    "    return register_tool(name, description, schema, None, options)\n"
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
    "class _Utf8TextFile:\n"
    "    def __init__(self, path, mode='r', encoding='utf-8'):\n"
    "        self.path = str(path)\n"
    "        self.mode = str(mode or 'r')\n"
    "        self.encoding = encoding\n"
    "        self.closed = False\n"
    "        if 'b' in self.mode:\n"
    "            raise ValueError('binary mode is not supported by mcdk plugin open')\n"
    "        if '+' in self.mode:\n"
    "            raise ValueError('update mode is not supported by mcdk plugin open')\n"
    "        self._readable = 'r' in self.mode or not ('w' in self.mode or 'a' in self.mode)\n"
    "        self._writable = 'w' in self.mode or 'a' in self.mode\n"
    "        self._append = 'a' in self.mode\n"
    "        self._pos = 0\n"
    "        self._buffer = ''\n"
    "        self._flushed = 0\n"
    "        if self._readable:\n"
    "            self._data = _native_read_text_file(self.path)\n"
    "        else:\n"
    "            self._data = ''\n"
    "    def _check_open(self):\n"
    "        if self.closed:\n"
    "            raise ValueError('I/O operation on closed file')\n"
    "    def read(self, size=-1):\n"
    "        self._check_open()\n"
    "        if not self._readable:\n"
    "            raise ValueError('file is not readable')\n"
    "        size = int(size)\n"
    "        if size < 0:\n"
    "            out = self._data[self._pos:]\n"
    "            self._pos = len(self._data)\n"
    "            return out\n"
    "        out = self._data[self._pos:self._pos + size]\n"
    "        self._pos += len(out)\n"
    "        return out\n"
    "    def readline(self, size=-1):\n"
    "        self._check_open()\n"
    "        if not self._readable:\n"
    "            raise ValueError('file is not readable')\n"
    "        end = self._data.find('\\n', self._pos)\n"
    "        if end < 0:\n"
    "            end = len(self._data)\n"
    "        else:\n"
    "            end += 1\n"
    "        size = int(size)\n"
    "        if size >= 0 and self._pos + size < end:\n"
    "            end = self._pos + size\n"
    "        out = self._data[self._pos:end]\n"
    "        self._pos = end\n"
    "        return out\n"
    "    def write(self, text):\n"
    "        self._check_open()\n"
    "        if not self._writable:\n"
    "            raise ValueError('file is not writable')\n"
    "        s = str(text)\n"
    "        self._buffer += s\n"
    "        return len(s)\n"
    "    def flush(self):\n"
    "        self._check_open()\n"
    "        if self._writable:\n"
    "            chunk = self._buffer[self._flushed:]\n"
    "            append = self._append or self._flushed > 0\n"
    "            _native_write_text_file(self.path, chunk, append)\n"
    "            self._flushed = len(self._buffer)\n"
    "        return None\n"
    "    def close(self):\n"
    "        if not self.closed:\n"
    "            self.flush()\n"
    "            self.closed = True\n"
    "        return None\n"
    "    def __enter__(self):\n"
    "        self._check_open()\n"
    "        return self\n"
    "    def __exit__(self):\n"
    "        self.close()\n"
    "        return False\n"
    "def _utf8_open(file, mode='r', buffering=-1, encoding='utf-8', errors=None, newline=None):\n"
    "    return _Utf8TextFile(file, mode, encoding)\n"
    "if _builtins is not None:\n"
    "    _builtins.open = _utf8_open\n"
    "def _join_path(root, name):\n"
    "    root = str(root)\n"
    "    name = str(name)\n"
    "    if not root:\n"
    "        return name\n"
    "    if root.endswith('/') or root.endswith('\\\\'):\n"
    "        return root + name\n"
    "    return root + '/' + name\n"
    "class _Fs:\n"
    "    def read_text(self, path):\n"
    "        return _native_read_text_file(str(path))\n"
    "    def write_text(self, path, text, append=False):\n"
    "        _native_write_text_file(str(path), str(text), bool(append))\n"
    "        return None\n"
    "    def scandir(self, path):\n"
    "        return json.loads(_native_fs_scandir(str(path)))\n"
    "    def listdir(self, path):\n"
    "        return [item.get('name', '') for item in self.scandir(path)]\n"
    "    def exists(self, path):\n"
    "        return bool(_native_fs_exists(str(path)))\n"
    "    def isfile(self, path):\n"
    "        return bool(_native_fs_isfile(str(path)))\n"
    "    def isdir(self, path):\n"
    "        return bool(_native_fs_isdir(str(path)))\n"
    "    def walk(self, top, topdown=True, onerror=None, followlinks=False):\n"
    "        stack = [str(top)]\n"
    "        while stack:\n"
    "            root = stack.pop()\n"
    "            try:\n"
    "                entries = self.scandir(root)\n"
    "            except Exception as e:\n"
    "                if onerror is not None:\n"
    "                    onerror(e)\n"
    "                continue\n"
    "            dirs = []\n"
    "            files = []\n"
    "            for item in entries:\n"
    "                name = item.get('name', '')\n"
    "                if item.get('is_dir', False):\n"
    "                    dirs.append(name)\n"
    "                elif item.get('is_file', False):\n"
    "                    files.append(name)\n"
    "            if topdown:\n"
    "                yield (root, dirs, files)\n"
    "            for name in reversed(dirs):\n"
    "                stack.append(_join_path(root, name))\n"
    "            if not topdown:\n"
    "                yield (root, dirs, files)\n"
    "fs = _Fs()\n"
    "if _os is not None:\n"
    "    _os.walk = fs.walk\n"
    "    _os.listdir = fs.listdir\n"
    "    _os.scandir = fs.scandir\n"
    "    if hasattr(_os, 'path'):\n"
    "        _os.path.exists = fs.exists\n"
    "        _os.path.isfile = fs.isfile\n"
    "        _os.path.isdir = fs.isdir\n"
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
    if(g_finalized) return set_error(error, "pocketPy VM was already finalized");
    if(g_initialized) return true;
    if(config) g_cfg = *config;
    py_initialize();
    g_initialized = true;
    py_callbacks()->print = bridge_print;
    py_callbacks()->flush = bridge_flush;
    py_callbacks()->importfile = bridge_importfile;

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
    py_bindfunc(mod, "_native_fs_scandir", native_fs_scandir);
    py_bindfunc(mod, "_native_fs_exists", native_fs_exists);
    py_bindfunc(mod, "_native_fs_isfile", native_fs_isfile);
    py_bindfunc(mod, "_native_fs_isdir", native_fs_isdir);
    py_bindfunc(mod, "_native_read_text_file", native_read_text_file);
    py_bindfunc(mod, "_native_write_text_file", native_write_text_file);

    if(!py_exec(BOOTSTRAP_SOURCE, "<mcdk_assistant>", EXEC_MODE, mod)) {
        bool ret = set_py_error(error);
        mcdk_py_finalize();
        return ret;
    }
    return true;
}

void mcdk_py_finalize(void) {
    if(!g_initialized || g_finalized) return;
    py_finalize();
    g_initialized = false;
    g_finalized = true;
    memset(&g_cfg, 0, sizeof(g_cfg));
    free(g_current_plugin_id);
    free(g_current_plugin_dir);
    g_current_plugin_id = NULL;
    g_current_plugin_dir = NULL;
}

bool mcdk_py_load_plugin(const char* plugin_id,
                         const char* plugin_dir,
                         const char* entry_path,
                         char** error) {
    set_current_plugin(plugin_id, plugin_dir);
    if(!g_cfg.read_text_file) return set_error(error, "host read_text_file callback is not configured");

    char* source = g_cfg.read_text_file(entry_path ? entry_path : "", g_cfg.userdata);
    if(!source) return set_error(error, "cannot read plugin entry");

    py_Ref main_mod = py_getmodule("__main__");
    if(!main_mod) main_mod = py_newmodule("__main__");
    py_newstr(py_r0(), entry_path ? entry_path : "");
    py_setdict(main_mod, py_name("__file__"), py_r0());
    py_newstr(py_r0(), current_plugin_dir());
    py_setdict(main_mod, py_name("__plugin_dir__"), py_r0());
    py_newstr(py_r0(), current_plugin_id());
    py_setdict(main_mod, py_name("__plugin_id__"), py_r0());
    py_Ref sys_mod = py_getmodule("sys");
    if(sys_mod) {
        py_ItemRef sys_path = py_getdict(sys_mod, py_name("path"));
        if(sys_path && py_islist(sys_path)) {
            py_newstr(py_r0(), current_plugin_dir());
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
