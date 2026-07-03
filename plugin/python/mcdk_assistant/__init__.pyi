# -*- coding: utf-8 -*-
"""MCDK Assistant 插件 Python API 类型补全库。

本文件只服务于 IDE 补全、悬停文档和类型检查；运行时的真实
`mcdk_assistant` 模块由 MCDK Assistant 的 pocketPy 引擎注入。
插件发布时不需要把本目录复制进插件包。
"""

from typing import Callable, Dict, List, Mapping, Optional, Sequence, TypeVar, Union, overload

Json = Union[None, bool, int, float, str, List["Json"], Dict[str, "Json"]]
"""可被 JSON 序列化的基础值类型。插件 tool/hook 参数和返回值都按 JSON 传递。"""

JsonObject = Dict[str, Json]
"""JSON 对象类型。"""

ToolHandler = Callable[[JsonObject, JsonObject], Json]
"""工具处理函数签名：`handler(args, ctx) -> result`。"""

HookHandler = Callable[[JsonObject, JsonObject], Optional[Json]]
"""Hook 处理函数签名：返回 `None` 表示不修改数据，返回 JSON 表示替换当前 payload。"""

_ToolFunc = TypeVar("_ToolFunc", bound=ToolHandler)
_HookFunc = TypeVar("_HookFunc", bound=HookHandler)

class Log:
    """插件日志接口。

    在运行时，引擎会直接接管 pocketPy 的 `print(...)` 输出回调，并替换
    `sys.stdout` / `sys.stderr` 流对象。因此插件里的裸 `print(...)`、
    `sys.stdout.write(...)`、`sys.stderr.write(...)` 以及本日志接口都会被统一
    转发到宿主侧 stderr。

    这样在 stdio MCP 模式下，插件随便打印也不会污染 stdout 上的 JSON-RPC
    协议流；tool 的正式结果只取函数 `return` 值。
    """

    def info(self, text: object) -> None:
        """输出普通信息日志。"""
        ...

    def warn(self, text: object) -> None:
        """输出警告日志。"""
        ...

    def error(self, text: object) -> None:
        """输出错误日志。"""
        ...

class SearchResult(JsonObject):
    """内存模糊搜索结果。

    字段包括命中的文本片段、来源文件、起止行号和 BM25 分数。
    """

    text: str
    file: str
    line_start: int
    line_end: int
    score: float

class MemoryIndex:
    """插件可复用的内存模糊搜索索引。

    索引数据只存在内存中，不落盘；目录源默认懒构建，第一次 `search()`
    或显式 `build()` 时才会扫描和构建索引。
    """

    def __init__(self, mode: str = "zh") -> None:
        """创建一个内存索引。

        Args:
            mode: 分词模式。`"zh"` 使用中文分词，`"en"` 使用英文分词，
                `"auto"` 会按查询文本自动选择。
        """
        ...

    def add_doc(
        self,
        id: object,
        text: object,
        metadata: Optional[Mapping[str, object]] = None,
        file: str = "",
        line_start: int = 1,
    ) -> None:
        """向索引追加一段文本。

        Args:
            id: 文档标识；当 `file` 为空时也会作为来源文件名。
            text: 文档正文。
            metadata: 可选元数据，当前识别 `file` 和 `line_start`。
            file: 来源文件名。
            line_start: 文本片段起始行号，1-based。
        """
        ...

    def set_dir(
        self,
        root: str,
        glob: Optional[Sequence[str]] = None,
        chunk: str = "markdown_heading",
    ) -> None:
        """设置目录数据源。

        Args:
            root: 要扫描的目录。
            glob: 文件匹配规则，默认 `["**/*.md", "**/*.txt"]`。
            chunk: 切片策略。当前推荐使用默认的 `"markdown_heading"`。

        注意：设置目录源后不会立即构建索引，除非之后显式调用 `build()`。
        """
        ...

    def build(self) -> None:
        """立即构建或重建索引。"""
        ...

    def invalidate(self) -> None:
        """标记索引为脏数据，下次搜索前会重新构建。"""
        ...

    def rebuild(self) -> None:
        """等价于 `invalidate()` 后再 `build()`。"""
        ...

    def search(self, keyword: object, top_k: int = 6) -> List[SearchResult]:
        """搜索索引。

        如果索引还没构建，会在本次调用中自动懒构建。

        Args:
            keyword: 搜索关键词。
            top_k: 返回结果数量上限。
        """
        ...

class Search:
    """搜索能力命名空间。"""

    MemoryIndex: type[MemoryIndex]

    def build_index_from_dir(
        self,
        root: str,
        glob: Optional[Sequence[str]] = None,
        mode: str = "zh",
        chunk: str = "markdown_heading",
        lazy: bool = True,
    ) -> MemoryIndex:
        """基于目录创建一个内存索引。

        Args:
            root: 要扫描的目录。
            glob: 文件匹配规则，默认 Markdown 和文本文件。
            mode: 分词模式，见 `MemoryIndex`。
            chunk: 切片策略。
            lazy: 是否懒构建。默认 `True`，第一次搜索时才构建。
        """
        ...

log: Log
"""插件日志对象。"""

search: Search
"""搜索能力命名空间，包含内存索引构建 API。"""

@overload
def register_tool(
    name: str,
    description: str = "",
    schema: Optional[Mapping[str, Json]] = None,
    handler: None = None,
) -> Callable[[_ToolFunc], _ToolFunc]: ...

@overload
def register_tool(
    name: str,
    description: str = "",
    schema: Optional[Mapping[str, Json]] = None,
    handler: _ToolFunc = ...,
) -> _ToolFunc: ...

def register_tool(
    name: str,
    description: str = "",
    schema: Optional[Mapping[str, Json]] = None,
    handler: Optional[_ToolFunc] = None,
) -> Union[Callable[[_ToolFunc], _ToolFunc], _ToolFunc]:
    """注册一个 MCP tool。

    推荐插件按“单能力单 tool”暴露，方便用户和模型理解能力边界。

    Args:
        name: tool 名称。应尽量短且稳定。
        description: tool 描述，会暴露给 MCP 客户端。
        schema: JSON Schema inputSchema。省略时默认为 `{"type": "object"}`。
        handler: 可选处理函数；省略时返回装饰器。

    Handler:
        处理函数签名为 `func(args, ctx)`。插件的 stdout/stderr 会被引擎兜底
        转发到 stderr；tool 的正式结果只取函数 return 值。
    """
    ...

def tool(
    name: str,
    description: str = "",
    schema: Optional[Mapping[str, Json]] = None,
) -> Callable[[_ToolFunc], _ToolFunc]:
    """`register_tool(...)` 的装饰器简写。"""
    ...

@overload
def register_hook(
    event: str,
    handler: None = None,
    priority: int = 10,
) -> Callable[[_HookFunc], _HookFunc]: ...

@overload
def register_hook(
    event: str,
    handler: _HookFunc = ...,
    priority: int = 10,
) -> _HookFunc: ...

def register_hook(
    event: str,
    handler: Optional[_HookFunc] = None,
    priority: int = 10,
) -> Union[Callable[[_HookFunc], _HookFunc], _HookFunc]:
    """注册一个 Hook 监听器。

    Args:
        event: Hook 事件名，例如 `"minecraft_docs.search.before"` 或
            `"minecraft_docs.search.after_render"`。
        handler: 可选处理函数；省略时返回装饰器。
        priority: 执行优先级，默认 `10`；越小越先执行，最低建议为 `0`。

    Return:
        Hook 函数返回 `None` 表示不修改当前 payload；返回 JSON 对象或字符串
        表示由引擎按该事件的规则改写数据。
    """
    ...

def hook(event: str, priority: int = 10) -> Callable[[_HookFunc], _HookFunc]:
    """`register_hook(...)` 的装饰器简写。"""
    ...
