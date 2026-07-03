"""Development-time helpers for MCDK Assistant plugins.

The real module is injected by the MCDK Assistant pocketPy runtime. At runtime,
the engine directly redirects pocketPy ``print(...)`` plus ``sys.stdout`` and
``sys.stderr`` writes to the host stderr so stdio MCP stdout stays protocol-only.

This file is only for IDE completion, local imports, and simple CPython-side
smoke checks.
"""

from __future__ import annotations

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


def register_tool(name: str, description: str = "", schema: dict[str, Any] | None = None, handler: Callable[..., Any] | None = None):
    def deco(func: Callable[..., Any]) -> Callable[..., Any]:
        return func

    return deco if handler is None else handler


def tool(name: str, description: str = "", schema: dict[str, Any] | None = None):
    return register_tool(name, description, schema)


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


log = _Log()
hooks = _Hooks()
search = _Search()
