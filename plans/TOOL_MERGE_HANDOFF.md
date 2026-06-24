# MCDK MCP Tool 合并优化交接说明

## 2026-06-24 后续更新

统一资料入口工具已从 `minecraft_dev` 更名为 `minecraft_docs`。

原因：当前阶段合并的是“搜索 / 资料库 / 网易参考速查”能力，工具内容本质上是文档与资料查询，不是完整 Minecraft 开发能力族。新名称更准确，也能避免后续 JSON UI、模型、动画、Python 分析等能力继续合并时语义过宽。

当前入口：

```text
minecraft_docs
```

唯一入参仍为：

```json
{
  "command": "help"
}
```

命令协议、子命令语义与原 `minecraft_dev` 相同。旧名不作为 alias 注册，以免重新增加 MCP tool 数量。

## 目的

本次工作的核心目标是降低 MCP 工具描述对上下文 token 的长期占用。

原项目中资料库相关能力拆成了多个独立 tool，例如：

- `search_api`
- `search_event`
- `search_enum`
- `search_all`
- `search_wiki`
- `search_bedrock_dev`
- `search_qumod`
- `search_netease_guide`
- `search_game_assets`
- `read_knowledge`
- `list_knowledge`
- `get_netease_diff`
- `get_netease_jsonui`

每个 tool 的 `name / description / inputSchema` 都会注入到模型上下文中，资料类 tool 数量多、描述长，会持续浪费 token。

当前方向是：

> 将 MCP tool 按能力族逐步合并为“单一入口 tool + command 命令式子命令”的形式。

本次是第一阶段，只合并**搜索 / 资料库 / 网易参考速查**相关工具。后续 JSON UI、模型、动画、Python 分析等其他 tool 家族也应按同样思路继续合并。

## 当前阶段设计

新增统一 tool：

```text
minecraft_dev
```

唯一入参：

```json
{
  "command": "help"
}
```

设计意图：

- tool 名称强调 Minecraft 开发语义，而不是品牌名，方便模型判断何时调用；
- tool 描述保持精简，只提示“Minecraft addon/mod 开发时先调用 `help`”；
- 具体能力全部放到 `command` 子命令中；
- 原有独立 tool 不再注册，从源头减少 MCP tools/list 注入上下文的体积。

## 已完成改动

### 1. 新增通用命令解析器

文件：[`src/tools/command_parser.hpp`](src/tools/command_parser.hpp)

用途：为 `minecraft_dev` 以及未来其他合并后的单工具入口提供通用命令解析能力。

主要能力：

- `tokenize_command_line(const std::string&)`
  - shell 风格词法分析；
  - 支持空白分词；
  - 支持 `"..."` 与 `'...'` 包裹的带空格参数；
  - 支持反斜杠转义；
  - 未闭合引号按读到行尾容错。

- `parse_command(const std::string&)`
  - 解析子命令；
  - 剥离可选前导 `/`，所以 `/help` 与 `help` 等价；
  - 支持 `--key value`、`--key=value`、`-s value` 形式 flag；
  - 位置参数会以空格拼回 `positional`。

- `flag_str / flag_int / has_flag`
  - 提供路由器读取 flag 的便捷函数。

注意：该解析器无 MCP 依赖，可复用于后续所有 tool 合并工作。

### 2. 重构搜索工具 handler

文件：[`src/tools/register_search.hpp`](src/tools/register_search.hpp)

已将原 lambda 中的业务逻辑抽成可复用自由函数：

- `handle_search(...)`
- `handle_search_game_assets(...)`
- `handle_read_knowledge(...)`
- `handle_list_knowledge(...)`

这些函数仍复用原有 `SearchService`，没有重写检索逻辑。

原 `register_search_tools(...)` 注册代码已整段注释保留，不删除，便于审视和回滚。

### 3. 重构网易参考速查文本

文件：[`src/tools/register_netease.hpp`](src/tools/register_netease.hpp)

已将原两个 tool 内嵌文本暴露为函数：

- `netease_diff_text()`
- `netease_jsonui_text()`

原 `register_netease_tools(...)` 注册代码已整段注释保留，不删除，便于审视和回滚。

### 4. 新增统一入口 tool

文件：[`src/tools/register_minecraft_dev.hpp`](src/tools/register_minecraft_dev.hpp)

该文件注册单个 MCP tool：

```text
minecraft_dev
```

内部通过 `dispatch_minecraft_dev(...)` 分派命令。

当前支持命令：

```text
help
search <关键词...> [--scope <s>] [--top <n>] [--assets <0|1|2>]
read   <path> [--start <n>] [--end <n>]
list   [path]
netease diff
netease jsonui
```

`search --scope` 映射关系：

| scope | 复用方法 |
|---|---|
| `all` | `SearchService::search_all` |
| `api` | `SearchService::search_api` |
| `event` | `SearchService::search_event` |
| `enum` | `SearchService::search_enum` |
| `wiki` | `SearchService::search_wiki` |
| `dev` | `SearchService::search_bedrock_dev` |
| `qumod` | `SearchService::search_qumod` |
| `netease` | `SearchService::search_netease_guide` |
| `assets` | `SearchService::search_game_assets` |

### 5. 切换服务注册入口

文件：[`src/app/server_runtime.cpp`](src/app/server_runtime.cpp)

已改为注册：

```cpp
mcdk::register_minecraft_dev_tools(srv, search_svc, effective_knowledge_dir);
```

并注释保留旧注册：

```cpp
// mcdk::register_search_tools(...)
// mcdk::register_netease_tools(...)
```

其他功能性工具注册保持不变。

### 6. 添加本地 MCP 配置

文件：[`./.mcp.json`](.mcp.json)

已配置当前构建产物作为项目级 MCP server：

```json
{
  "mcpServers": {
    "mcdk_assistant_local": {
      "command": "D:\\Zero123\\CPP\\CMAKE\\mcdk-assistant\\build\\x64-msvc-release\\mcdk-asst-lite.exe",
      "args": ["--stdio"],
      "timeout": 30
    }
  }
}
```

用途：后续验证时可让 Claude Code / MCP 客户端直接连接当前项目构建出的 lite 版。

注意：当前会话未继续做 MCP 运行验证，用户要求“先别验证，准备交接”。下次接手时可能需要重启 Claude Code 或重新加载 MCP 配置。

## 当前验证状态

已完成：

- `mcdk-asst-lite` 编译通过；
- `mcdk-assistant` 编译通过。

构建时曾通过 Visual Studio `vcvars64.bat` 环境执行。普通 shell 直接构建会因为没有 MSVC include 环境而找不到 `<filesystem>`，这不是代码问题。

暂未完成：

- MCP tools/list 实际运行验证；
- `minecraft_dev` 子命令实际调用验证；
- 确认旧 13 个资料类 tool 不再出现在 MCP tools/list 中。

## 下次接手建议验证清单

### 1. 构建

在 VS 开发环境中构建：

```bat
call "D:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d D:\Zero123\CPP\CMAKE\mcdk-assistant
cmake --build build\x64-msvc-release --target mcdk-asst-lite
cmake --build build\x64-msvc-release --target mcdk-assistant
```

### 2. MCP tools/list

确认工具列表中：

- 出现 `minecraft_dev`；
- 不再出现以下旧资料类工具：
  - `search_api`
  - `search_event`
  - `search_enum`
  - `search_all`
  - `search_wiki`
  - `search_bedrock_dev`
  - `search_qumod`
  - `search_netease_guide`
  - `search_game_assets`
  - `read_knowledge`
  - `list_knowledge`
  - `get_netease_diff`
  - `get_netease_jsonui`

### 3. `minecraft_dev` 子命令冒烟

建议调用：

```text
help
search minecraft:food --scope wiki --top 8
search "minecraft:food block" --scope wiki
search 自定义方块
search stair --scope assets --assets 1
list BedrockWiki
read BedrockWiki/items/items-intro.md --start 1 --end 40
netease diff
netease jsonui
```

非法输入也要检查：

```text
search
bogus
netease
```

预期：返回 help 或友好错误，不崩溃。

## 后续路线

### 第一阶段（当前）

搜索 / 资料库 / 网易参考速查 → 已合并到 `minecraft_dev`。

### 第二阶段（建议）

把 JSON UI 相关 tool 合并为一个命令式入口，例如：

```text
minecraft_ui
```

或继续收编到 `minecraft_dev ui ...` 分层命令，待设计确认。

### 第三阶段（建议）

模型、动画、像素画、Python 分析等功能性 tool 继续按能力族合并。

建议总原则：

- 一个能力族只暴露一个 MCP tool；
- tool 描述只讲“何时使用 + help”；
- 详细用法全部放到 `help` 子命令；
- 旧注册代码注释保留，不直接删除；
- 尽量复用现有 handler / service，不重写业务逻辑。

## 注意事项

1. 目前 `.mcp.json` 是项目级配置，可能会被纳入 git；是否提交由维护者决定。
2. `command_parser.hpp` 是后续所有合并工作的关键基础设施，修改时要注意保持无业务依赖。
3. `register_search.hpp` / `register_netease.hpp` 中虽然旧注册被注释，但 handler / 文本函数仍是新入口依赖，不能删除。
4. `minecraft_dev` 名称是当前确认结果：强调 Minecraft 开发语义，便于模型选择工具。
5. 本次没有改 `CMakeLists.txt`，因为新增的是 header-only 文件，通过 `server_runtime.cpp` 间接包含。
