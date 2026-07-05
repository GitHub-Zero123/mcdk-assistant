"""Development-time helpers for MCDK Assistant plugins.

The real module is injected by the MCDK Assistant pocketPy runtime. At runtime,
the engine directly redirects pocketPy ``print(...)`` plus ``sys.stdout`` and
``sys.stderr`` writes to the host stderr so stdio MCP stdout stays protocol-only.

This file is only for IDE completion, local imports, and simple CPython-side
smoke checks.
"""

from __future__ import annotations

import os as _os
from typing import Any, Callable


class _Log:
    def info(self, text: object) -> None:
        print(f"[plugin][info] {text}")

    def warn(self, text: object) -> None:
        print(f"[plugin][warn] {text}")

    def error(self, text: object) -> None:
        print(f"[plugin][error] {text}")


class _Hooks:
    MINECRAFT_DOCS_SEARCH_BEFORE = "minecraft_docs.search.before"
    MINECRAFT_DOCS_SEARCH_AFTER_RENDER = "minecraft_docs.search.after_render"


class Context:
    def __init__(self, data: dict[str, Any] | None = None) -> None:
        self._data = data or {}
        for key, value in self._data.items():
            setattr(self, str(key), value)
        self.plugin_id = str(self._data.get("plugin_id", ""))
        self.plugin_dir = str(self._data.get("plugin_dir", ""))
        self.tool = str(self._data.get("tool", ""))
        self.event = str(self._data.get("event", ""))
        self.priority = int(self._data.get("priority", 10))
        self.log = log
        self.search = search
        self.fs = fs

    def get(self, key: str, default: Any = None) -> Any:
        return self._data.get(key, default)

    def __getitem__(self, key: str) -> Any:
        return self._data[key]

    def __contains__(self, key: str) -> bool:
        return key in self._data

    def to_json(self) -> dict[str, Any]:
        return self._data.copy()


class ToolContext(Context):
    tool: str


class HookContext(Context):
    event: str
    priority: int


class ToolOptions:
    def __init__(
        self,
        read_only: bool | None = None,
        idempotent: bool | None = None,
        open_world: bool | None = None,
        destructive: bool | None = None,
    ) -> None:
        self.read_only = read_only
        self.idempotent = idempotent
        self.open_world = open_world
        self.destructive = destructive

    def to_annotations(self) -> dict[str, bool]:
        data: dict[str, bool] = {}
        if self.read_only is not None:
            data["readOnlyHint"] = bool(self.read_only)
        if self.idempotent is not None:
            data["idempotentHint"] = bool(self.idempotent)
        if self.open_world is not None:
            data["openWorldHint"] = bool(self.open_world)
        if self.destructive is not None:
            data["destructiveHint"] = bool(self.destructive)
        return data


def _wrap_handler(func: Callable[..., Any]) -> Callable[..., Any]:
    def wrapped(args: Any, ctx: Any) -> Any:
        if isinstance(ctx, Context):
            return func(args, ctx)
        if isinstance(ctx, dict) and "tool" in ctx:
            return func(args, ToolContext(ctx))
        if isinstance(ctx, dict) and "event" in ctx:
            return func(args, HookContext(ctx))
        return func(args, Context(ctx if isinstance(ctx, dict) else {}))

    return wrapped


def _schema_to_jsonable(value: Any) -> Any:
    if value is None:
        return {"type": "object"}
    if hasattr(value, "to_json"):
        return value.to_json()
    if isinstance(value, dict):
        return {key: _schema_to_jsonable(item) for key, item in value.items()}
    if isinstance(value, list):
        return [_schema_to_jsonable(item) for item in value]
    return value


class _SchemaField:
    def __init__(
        self,
        type_name: str,
        description: str = "",
        required: bool = False,
        default: Any = None,
        enum: list[Any] | None = None,
        minimum: int | float | None = None,
        maximum: int | float | None = None,
    ) -> None:
        self.required = required
        self._data: dict[str, Any] = {"type": type_name}
        if description:
            self._data["description"] = description
        if default is not None:
            self._data["default"] = default
        if enum is not None:
            self._data["enum"] = enum
        if minimum is not None:
            self._data["minimum"] = minimum
        if maximum is not None:
            self._data["maximum"] = maximum

    def to_json(self) -> dict[str, Any]:
        return self._data.copy()


class _ArraySchema:
    def __init__(self, items: Any = None, description: str = "", required: bool = False) -> None:
        self.items = items or _SchemaField("string")
        self.description = description
        self.required = required

    def to_json(self) -> dict[str, Any]:
        data = {"type": "array", "items": _schema_to_jsonable(self.items)}
        if self.description:
            data["description"] = self.description
        return data


class _ObjectSchema:
    def __init__(self, properties: dict[str, Any] | None = None, description: str = "") -> None:
        self._properties: dict[str, Any] = {}
        self._required: list[str] = []
        self._description = description
        if properties:
            for key, value in properties.items():
                self.field(key, value)

    def field(self, name: str, schema: Any, required: bool | None = None) -> "_ObjectSchema":
        key = str(name)
        self._properties[key] = schema
        is_required = getattr(schema, "required", False) if required is None else required
        if is_required and key not in self._required:
            self._required.append(key)
        return self

    def require(self, *names: str) -> "_ObjectSchema":
        for name in names:
            key = str(name)
            if key not in self._required:
                self._required.append(key)
        return self

    def to_json(self) -> dict[str, Any]:
        data: dict[str, Any] = {
            "type": "object",
            "properties": {key: _schema_to_jsonable(value) for key, value in self._properties.items()},
        }
        if self._description:
            data["description"] = self._description
        if self._required:
            data["required"] = list(self._required)
        return data


class _Schema:
    def String(self, description: str = "", required: bool = False, default: Any = None, enum: list[Any] | None = None) -> _SchemaField:
        return _SchemaField("string", description, required, default, enum)

    def Integer(
        self,
        description: str = "",
        required: bool = False,
        default: Any = None,
        enum: list[Any] | None = None,
        minimum: int | None = None,
        maximum: int | None = None,
    ) -> _SchemaField:
        return _SchemaField("integer", description, required, default, enum, minimum, maximum)

    def Number(
        self,
        description: str = "",
        required: bool = False,
        default: Any = None,
        enum: list[Any] | None = None,
        minimum: int | float | None = None,
        maximum: int | float | None = None,
    ) -> _SchemaField:
        return _SchemaField("number", description, required, default, enum, minimum, maximum)

    def Boolean(self, description: str = "", required: bool = False, default: Any = None) -> _SchemaField:
        return _SchemaField("boolean", description, required, default)

    def Array(self, items: Any = None, description: str = "", required: bool = False) -> _ArraySchema:
        return _ArraySchema(items, description, required)

    def Object(self, properties: dict[str, Any] | None = None, description: str = "") -> _ObjectSchema:
        return _ObjectSchema(properties, description)


def register_tool(
    name: str,
    description: str = "",
    schema: Any = None,
    handler: Callable[..., Any] | None = None,
    options: ToolOptions | dict[str, bool] | None = None,
):
    def deco(func: Callable[..., Any]) -> Callable[..., Any]:
        return func

    return deco if handler is None else handler


def tool(name: str, description: str = "", schema: Any = None, options: ToolOptions | dict[str, bool] | None = None):
    return register_tool(name, description, schema, None, options)


def register_hook(event: str, handler: Callable[..., Any] | None = None, priority: int = 10):
    def deco(func: Callable[..., Any]) -> Callable[..., Any]:
        return func

    return deco if handler is None else handler


def hook(event: str, priority: int = 10):
    return register_hook(event, None, priority)


class MemoryIndex:
    def __init__(self, mode: str = "zh") -> None:
        self.mode = mode

    def add_doc(self, id: object, text: object, metadata: dict[str, Any] | None = None, file: str = "", line_start: int = 1) -> None:
        return None

    def set_dir(self, root: str, glob: list[str] | None = None, chunk: str = "markdown_heading") -> None:
        return None

    def build(self) -> None:
        return None

    def invalidate(self) -> None:
        return None

    def rebuild(self) -> None:
        self.invalidate()
        self.build()

    def search(self, keyword: object, top_k: int = 6) -> list[dict[str, Any]]:
        return []


class _Search:
    MemoryIndex = MemoryIndex

    def build_index_from_dir(
        self,
        root: str,
        glob: list[str] | None = None,
        mode: str = "zh",
        chunk: str = "markdown_heading",
        lazy: bool = True,
    ) -> MemoryIndex:
        idx = MemoryIndex(mode)
        idx.set_dir(root, glob, chunk)
        if not lazy:
            idx.build()
        return idx


class _Fs:
    def read_text(self, path: object) -> str:
        with open(str(path), "r", encoding="utf-8") as f:
            return f.read()

    def write_text(self, path: object, text: object, append: bool = False) -> None:
        mode = "a" if append else "w"
        with open(str(path), mode, encoding="utf-8") as f:
            f.write(str(text))

    def scandir(self, path: object) -> list[dict[str, Any]]:
        entries: list[dict[str, Any]] = []
        with _os.scandir(str(path)) as it:
            for item in it:
                entries.append({
                    "name": item.name,
                    "path": item.path,
                    "is_file": item.is_file(),
                    "is_dir": item.is_dir(),
                })
        return entries

    def listdir(self, path: object) -> list[str]:
        return _os.listdir(str(path))

    def exists(self, path: object) -> bool:
        return _os.path.exists(str(path))

    def isfile(self, path: object) -> bool:
        return _os.path.isfile(str(path))

    def isdir(self, path: object) -> bool:
        return _os.path.isdir(str(path))

    def walk(
        self,
        top: object,
        topdown: bool = True,
        onerror: Callable[[Exception], Any] | None = None,
        followlinks: bool = False,
    ):
        yield from _os.walk(str(top), topdown=topdown, onerror=onerror, followlinks=followlinks)


log = _Log()
hooks = _Hooks()
schema = _Schema()
search = _Search()
fs = _Fs()
