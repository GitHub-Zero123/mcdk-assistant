# MCDK pocketPy 插件系统计划

## 定位

插件系统用于扩展 MCDK MCP Server 的自定义能力。插件运行在 MCDK 进程内，由 pocketPy 执行 Python 3 子集脚本。

插件不是游戏内脚本，也不和网易 MC ModSDK 运行环境发生关系。

核心目标：

- 插件用 Python API 动态注册 MCP tool。
- 插件用 Python API 动态注册 hook。
- manifest 只负责基础元信息和入口脚本，不负责声明 tool/hook 细节。
- stdio 模式下，引擎侧兜底接管插件 stdout/stderr/异常输出，避免污染 JSON-RPC stdout 协议流。

## 插件目录

插件目录位于可执行文件同级：

```text
plugins/
  hello/
    manifest.json
    main.py
```

## Manifest MVP

manifest 第一版保持最小：

```json
{
  "id": "hello",
  "name": "Hello Plugin",
  "version": "0.1.0",
  "entry": "main.py",
  "description": "Example plugin for custom MCP tools"
}
```

字段：

- `id`：插件 ID，只允许小写字母、数字、下划线、中划线。
- `name`：展示名。
- `version`：插件版本。
- `entry`：相对插件目录的入口 Python 文件。
- `description`：可选，插件描述。用于插件列表、日志、未来管理工具展示，不作为具体 MCP tool 描述。

manifest 不声明 tools，不声明 hooks，不声明 schema。  
这些都由 Python 入口脚本通过 `mcdk_assistant` API 动态注册。

## Python API 设计

入口脚本直接 import 宿主注入的 `mcdk_assistant` 模块：

```python
import mcdk_assistant as mcdk
```

### 注册 Tool

推荐 decorator 形式：

```python
import mcdk_assistant as mcdk

@mcdk.tool(
    name="hello",
    description="Hello custom MCP tool",
    schema={
        "type": "object",
        "properties": {
            "command": {"type": "string"}
        },
        "required": ["command"]
    }
)
def hello(params, ctx):
    print("plugin log")
    command = params.get("command", "help")
    return {
        "content": [
            {"type": "text", "text": "hello: " + command}
        ]
    }
```

也可以提供函数式注册，方便动态构造：

```python
def hello(params, ctx):
    return "hello"

mcdk.register_tool(
    name="hello",
    description="Hello custom MCP tool",
    schema={"type": "object"},
    handler=hello
)
```

对外 MCP tool 名默认加命名空间：

```text
plugin_<plugin_id>_<tool_name>
```

例如：

```text
plugin_hello_hello
```

第一版不允许插件覆盖内置 tool，也不允许多个插件注册同名 public tool。

### 注册 Hook

hook 也使用 decorator：

```python
import mcdk_assistant as mcdk

@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_AFTER_RENDER)
def append_note(args, ctx):
    text = args["text"]
    args["text"] = text + "\n\n[plugin note] custom reminder"
    return args
```

如果不修改数据，返回 `None`：

```python
@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_AFTER_RENDER)
def observe(args, ctx):
    print("scope =", args.get("scope"))
    return None
```

hook 语义：

- `return None`：不修改当前事件数据。
- `return dict`：用返回值作为新的事件数据。
- 抛异常：记录 traceback，忽略该 hook 的修改，继续主流程。

可选支持优先级：

```python
@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_AFTER_RENDER, priority=100)
def rewrite(args, ctx):
    return args
```

第一版需要支持 priority：

- 默认 `priority=10`。
- 最低值为 `0`。
- 数值越小越先执行。
- 同 priority 下按插件加载顺序、同插件内注册顺序执行。

示例：

```python
@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_BEFORE, priority=0)
def earliest(args, ctx):
    return args

@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_BEFORE)
def normal(args, ctx):
    return None
```

## Tool 返回值适配

Python tool handler 的正式返回值由 C++ 宿主接收并转换为 MCP result：

- `dict`：作为 MCP result 使用。
- `str`：包装为 `content: [{ "type": "text", "text": value }]`。
- `None`：返回空成功文本或固定成功消息。
- 其他类型：第一版建议报错，避免隐式行为过多。

插件中的 `print()`、`sys.stdout.write()`、`sys.stderr.write()` 不参与 MCP result。它们只是日志输出。

## 输出流兜底

这是 stdio 模式下的硬约束。

MCP stdio transport 中：

```text
stdout = JSON-RPC 协议流
stderr = 日志 / 诊断输出
```

插件作者可以正常 `print()`、写 `sys.stdout`、写 `sys.stderr` 或抛异常。  
引擎侧必须在 pocketPy VM 初始化时重绑定 Python 输出流，由 `PluginOutputRouter` 统一接管。

输出路由：

```text
plugin print / sys.stdout.write / sys.stderr.write / traceback
        |
        v
PluginOutputRouter
        |
        +-- stdio mode: 全部写宿主 stderr
        |
        +-- http/sse mode: 走普通日志 sink / console 策略
```

规则：

- stdio 模式下，插件输出永远不写宿主 stdout。
- 插件 tool 的正式 MCP result 只来自 Python 函数 `return`。
- traceback 进入日志，不污染协议流。
- `PluginOutputRouter` 写日志需要加锁。

## Hook MVP

第一版只接入 `minecraft_docs` 搜索链路。

事件：

```text
docs.search.before_query
docs.search.after_results
docs.search.after_render
```

数据流：

```text
parse command
 -> docs.search.before_query
 -> SearchService
 -> docs.search.after_results
 -> render MCP result
 -> docs.search.after_render
 -> return MCP result
```

### docs.search.before_query

输入：

```json
{
  "scope": "wiki",
  "keyword": "food",
  "top_k": 6,
  "command": "wiki food --top 6"
}
```

插件可以返回修改后的 dict：

```python
@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_BEFORE)
def expand_query(args, ctx):
    if args["keyword"] == "food":
        args["keyword"] = "custom food item"
        args["top_k"] = 8
        return args
    return None
```

### docs.search.after_results

输入：

```json
{
  "scope": "wiki",
  "keyword": "food",
  "results": [
    {
      "type": "text",
      "text": "...",
      "file": "BedrockWiki/items/...",
      "line_start": 1,
      "line_end": 20,
      "score": 12.3
    }
  ]
}
```

插件可过滤、追加、重排结果。

第一版需要限制：

- 最大结果数。
- 单条结果最大文本长度。
- hook 后总输出最大长度。

### docs.search.after_render

输入：

```json
{
  "scope": "wiki",
  "keyword": "food",
  "text": "..."
}
```

插件可改写最终文本：

```python
@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_AFTER_RENDER)
def append_team_rule(args, ctx):
    if args.get("scope") == "netease":
        args["text"] += "\n\n团队规范：示例代码优先使用项目已有封装。"
        return args
    return None
```

## 宿主注入模块

C++ 宿主在 pocketPy 环境中注入 `mcdk_assistant` 模块。

第一版 API：

```text
mcdk_assistant.tool(name, description="", schema=None)
mcdk_assistant.register_tool(name, description, schema, handler)
mcdk_assistant.hook(event, priority=10)
mcdk_assistant.register_hook(event, handler, priority=10)
mcdk_assistant.log.info(text)
mcdk_assistant.log.warn(text)
mcdk_assistant.log.error(text)
```

`ctx` 第一版建议提供：

```text
ctx.plugin_id
ctx.plugin_dir
ctx.log.info(text)
ctx.log.warn(text)
ctx.log.error(text)
```

后续再考虑：

```text
ctx.search_docs(scope, keyword, top_k)
ctx.read_knowledge(path, start, end)
ctx.config.get(key, default)
```

## 插件侧内存模糊搜索

插件需要复用 MCDK 内部的模糊搜索能力，用于构建自己的临时资料检索，而不是只能调用内置 knowledge 索引。

目标场景：

- 插件扫描某个特定目录。
- 插件把文件内容构建成内存索引。
- 不落盘，不生成缓存文件。
- 使用类似 ModSDK 中文资料搜索的中文分词 + BM25 模糊搜索能力。
- 插件 tool 或 hook 中可以查询该索引。

### API 草案

推荐提供 `mcdk.search` 子模块：

```python
import mcdk_assistant as mcdk

index = mcdk.search.build_index_from_dir(
    root="D:/project/private_docs",
    glob=["**/*.md", "**/*.txt"],
    mode="zh",
    chunk="markdown_heading",
    lazy=True
)

@mcdk.tool(
    name="private_docs",
    description="Search private project docs",
    schema={
        "type": "object",
        "properties": {
            "keyword": {"type": "string"},
            "top_k": {"type": "number"}
        },
        "required": ["keyword"]
    }
)
def private_docs(params, ctx):
    return index.search(
        params["keyword"],
        top_k=int(params.get("top_k", 6))
    )
```

也提供手动喂数据的形式，便于插件从非文件来源构建索引：

```python
index = mcdk.search.MemoryIndex(mode="zh")
index.add_doc(
    id="doc-1",
    text="这里是自定义资料正文",
    metadata={"file": "virtual.md", "line_start": 1}
)

results = index.search("自定义资料", top_k=6)
```

### 懒构建

插件侧内存索引默认应支持懒构建，避免 server 启动时被插件索引拖慢。

默认策略：

- `build_index_from_dir(..., lazy=True)` 只记录目录、glob、分词模式、chunk 策略，不立即扫描和构建 BM25。
- `MemoryIndex.add_doc(...)` 只追加文档并标记 dirty。
- 第一次调用 `index.search(...)` 时自动构建。
- 调用 `index.build()` 时显式立即构建。
- 索引已构建后再次 `add_doc(...)`，标记 dirty；下一次 `search(...)` 自动增量重建或全量重建。第一版可以全量重建。
- 多线程并发首次 search 时，只允许一个线程执行构建，其它线程等待或复用构建结果。

API：

```python
index = mcdk.search.build_index_from_dir(
    root="D:/project/private_docs",
    glob=["**/*.md"],
    mode="zh",
    lazy=True
)

# 可选：插件希望启动时预热，则显式调用
index.build()

# 默认：首次查询才真正扫描目录并构建
results = index.search("注册事件", top_k=6)
```

目录索引状态建议：

```text
created       已创建，未扫描
building      正在构建
ready         可查询
dirty         文档已变更，下一次查询需重建
failed        上次构建失败，下一次查询可重试
```

目录懒扫描说明：

- 第一版不要求监控文件变化。
- `build_index_from_dir` 的目录扫描可以放到首次 `build/search`。
- 如果插件需要刷新，提供 `index.invalidate()` 或 `index.rebuild()`。
- `index.search(...)` 发现状态为 `failed` 时，可以重试一次；仍失败则抛异常并写日志。

示例：

```python
index = mcdk.search.build_index_from_dir(
    root=ctx.plugin_dir + "/docs",
    glob=["**/*.md"],
    mode="zh"
)

@mcdk.tool(name="docs", description="Search plugin docs")
def docs(params, ctx):
    # 第一次调用这个 tool 时才构建索引
    return {"content": index.search(params["keyword"], top_k=6)}
```

### 搜索模式

第一版建议支持：

```text
mode="zh"      使用 cppjieba 分词，适合中文资料、ModSDK 风格文档
mode="en"      使用英文 tokenization
mode="auto"    简单自动判断
```

实现上应复用现有搜索模块的核心能力：

- `BM25Engine`
- `DocFragment`
- 中文分词逻辑尽量复用 `SearchService` 中的 cppjieba 初始化和 stop words
- 英文 tokenization 复用现有规则

不要复用 `SearchService` 的固定 category 结构，因为插件索引是临时、独立、内存态的。

建议抽一个轻量组件：

```text
src/search/memory_search_index.hpp
src/search/memory_search_index.cpp
```

职责：

- 管理一组 `DocFragment`
- 分词
- 构建 BM25
- 返回统一搜索结果 JSON

### 返回结果

`MemoryIndex.search(...)` 返回 MCP 友好的结构：

```python
[
    {
        "type": "text",
        "text": "...matched fragment...",
        "score": 12.3,
        "id": "doc-1",
        "file": "private/a.md",
        "line_start": 10,
        "line_end": 24,
        "metadata": {}
    }
]
```

插件 tool 可以直接包装：

```python
def private_docs(params, ctx):
    results = index.search(params["keyword"], top_k=6)
    return {"content": results}
```

### 生命周期与资源限制

内存索引不落盘，随插件 runtime 生命周期释放。

需要限制：

- 单插件最大索引数。
- 单索引最大文档数。
- 单索引最大总文本大小。
- 单条 fragment 最大大小。
- 构建索引耗时日志。
- 懒构建首次查询最大等待时间。
- 构建失败后的重试策略。

第一版可以先给保守默认值，并允许后续配置化。

## C++ 模块划分

建议新增：

```text
src/plugins/plugin_manifest.hpp
src/plugins/plugin_manifest.cpp
src/plugins/plugin_manager.hpp
src/plugins/plugin_manager.cpp
src/plugins/plugin_runtime.hpp
src/plugins/plugin_runtime.cpp
src/plugins/plugin_output_router.hpp
src/plugins/plugin_output_router.cpp
src/plugins/plugin_hooks.hpp
src/plugins/plugin_hooks.cpp
src/plugins/plugin_tool_registry.hpp
src/plugins/plugin_tool_registry.cpp
```

职责：

- `PluginManifest`：读取和校验最小 manifest。
- `PluginManager`：扫描插件目录、维护插件列表、初始化 runtime。
- `PluginRuntime`：封装 pocketPy VM、注入 `mcdk_assistant` 模块、加载入口脚本。
- `PluginOutputRouter`：接管插件 stdout/stderr/traceback。
- `PluginHooks`：维护 hook 订阅表和事件调用。
- `PluginToolRegistry`：把 Python API 动态注册的 tool 注册到 `mcp::server`。

## CMake 接入

增加开关：

```cmake
option(MCDK_WITH_PLUGINS "Enable pocketPy plugin system" ON)
```

检测逻辑：

- 如果 `libs/pocketpy` 或约定路径存在，启用插件系统。
- 否则自动关闭，输出 status，主工程照常编译。

编译定义：

```text
MCDK_WITH_PLUGINS
```

所有插件相关 include 和注册逻辑都放在该宏下。

## ServerRuntime 接入点

启动阶段：

```text
detect_runtime_paths()
 -> exe_dir
 -> plugins_dir = exe_dir / "plugins"
 -> PluginManager::load_all(plugins_dir)
```

注册阶段：

```text
register built-in tools
load plugin runtimes
register plugin tools collected from Python API
attach plugin hooks to selected built-in dispatchers
```

插件 tools 建议在内置 tools 之后注册，避免覆盖内置能力。

## 并发与生命周期

第一版保守策略：

- 每个插件一个 runtime/VM。
- 每个 runtime 一个 mutex，调用 Python tool/hook 时加锁。
- 插件输出路由加锁。
- 不做热重载。
- 不在请求处理中卸载插件。
- 如果 pocketPy 无法安全中断执行，第一版记录 timeout 限制，不假装已经有强制中断能力。

后续可考虑：

- runtime pool。
- 外部进程隔离插件。
- instruction budget / timeout。

## 错误模型

插件加载错误：

- manifest 缺失或非法：跳过插件，写日志。
- entry 不存在：跳过插件，写日志。
- 入口脚本执行失败：跳过插件，写日志。

tool 调用错误：

- Python handler 抛异常：traceback 走 `PluginOutputRouter`，MCP 返回 error。
- 返回值非法：MCP 返回 error。
- 插件异常不导致 server 崩溃。

hook 调用错误：

- 单个插件 hook 失败不影响主流程。
- traceback 写日志。
- 当前 hook 返回忽略，继续执行后续 hook。

## 测试计划

新增 stdio probe：

```text
tests/plugin_stdio_probe.py
```

覆盖：

- pocketPy 缺失时工程仍可编译。
- 插件目录为空时 server 正常启动。
- manifest 错误时 server 正常启动并记录错误。
- Python decorator 注册 tool 成功。
- Python decorator 注册 hook 成功。
- tools/list 包含插件 tool。
- 插件 tool 返回 dict/string/None。
- 插件 `print()` 在 stdio 下不污染 stdout。
- 插件 `sys.stdout.write()` / `sys.stderr.write()` 在 stdio 下不污染 stdout。
- 插件异常不崩 server，traceback 进入 stderr。
- 并发调用插件 tool 不串 MCP 响应。
- `docs.search.before_query` 可改 keyword/top_k。
- `docs.search.after_results` 可追加结果。
- `docs.search.after_render` 可改最终文本。
- hook 返回 `None` 时不修改数据。

示例插件：

```text
plugins/hello_tool/
plugins/docs_text_rewriter/
plugins/query_expander/
```

## 实施阶段

### 阶段 1：文档与构建开关

- 新增本计划文档。
- 增加 `MCDK_WITH_PLUGINS` CMake 选项和 pocketPy 检测。
- pocketPy 不存在时不编译插件模块。

### 阶段 2：Manifest 与插件扫描

- 实现最小 `PluginManifest`。
- 实现 `PluginManager::scan_plugins`。
- 输出插件加载日志。

### 阶段 3：pocketPy Runtime 与宿主模块

- 接入 pocketPy。
- 注入 `mcdk_assistant` 模块。
- 支持 `@mcdk.tool(...)` / `mcdk.register_tool(...)`。
- 支持 `@mcdk.hook(...)` / `mcdk.register_hook(...)`。
- 加载入口脚本并收集注册结果。

### 阶段 4：输出流兜底

- 实现 `PluginOutputRouter`。
- 重绑定 Python stdout/stderr。
- stdio 模式下全部转发到宿主 stderr。
- 加并发锁。

### 阶段 5：插件 Tool 注册

- 根据 Python API 收集到的 tool 注册 MCP tool。
- 调 Python handler。
- 实现返回值适配和异常兜底。

### 阶段 6：Docs Hook 接入

- 在 `minecraft_docs` 搜索链路加入 3 个 hook 点。
- 支持 query/result/render text 的结构化改写。
- hook 返回 `None` 表示不修改。
- hook 异常不影响主流程。

### 阶段 7：测试与示例

- 增加 stdio probe。
- 增加示例插件。
- 补充 README 或 docs 中的插件开发说明。

## 需要继续确认的问题

- pocketPy 在当前工程中的实际目录：`libs/pocketpy` 还是其他路径。
- pocketPy 输出重绑定的最佳 API 形态，需要结合实际版本确认。
- 是否允许插件声明 public tool name，还是第一版强制 `plugin_<plugin_id>_<tool_name>`。
- hook 执行顺序是否需要第一版支持 `priority`。
- timeout 能否在 pocketPy 内安全实现；若不能，第一版应明确限制。
