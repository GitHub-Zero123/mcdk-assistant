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

命令协议已调整为“资料源即子命令”，例如 `wiki minecraft:food`、`api ListenForEvent`、`assets stair --rp`。
旧名不作为 alias 注册，以免重新增加 MCP tool 数量；旧 `search --scope ...` 写法也不再保留，避免 Agent 把资料源选择藏进参数里。

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
minecraft_docs
```

唯一入参：

```json
{
  "command": "help"
}
```

设计意图：

- tool 名称强调资料/文档查询语义，而不是完整开发能力，方便模型判断何时调用；
- tool 描述保持精简，只提示“Minecraft addon/mod 开发时先调用 `help`”；
- 具体能力全部放到 `command` 子命令中；
- 原有独立 tool 不再注册，从源头减少 MCP tools/list 注入上下文的体积。

## 已完成改动

### 1. 新增通用命令解析器

文件：[`src/tools/command_parser.hpp`](src/tools/command_parser.hpp)

用途：为 `minecraft_docs` 以及未来其他合并后的单工具入口提供通用命令解析能力。

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

文件：[`src/tools/register_minecraft_docs.hpp`](src/tools/register_minecraft_docs.hpp)

该文件注册单个 MCP tool：

```text
minecraft_docs
```

内部通过 `dispatch_minecraft_docs(...)` 分派命令。

当前支持命令：

```text
help
all     <关键词...> [--top <n>]
api     <关键词...> [--top <n>]
event   <关键词...> [--top <n>]
enum    <关键词...> [--top <n>]
wiki    <关键词...> [--top <n>]
dev     <关键词...> [--top <n>]
qumod   <关键词...> [--top <n>]
netease <关键词...> [--top <n>]
assets  <关键词...> [--top <n>] [--assets <0|1|2>] [--bp|--rp]
read   <path> [--start <n>] [--end <n>]
list   [path]
netease diff
netease jsonui
diff
jsonui
```

资料源子命令映射关系：

| 子命令 | 复用方法 |
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

注意：`all` 只聚合文档索引，不搜索 GameAssets；游戏资产只通过 `assets` 子命令进入。

### 5. 切换服务注册入口

文件：[`src/app/server_runtime.cpp`](src/app/server_runtime.cpp)

已改为注册：

```cpp
mcdk::register_minecraft_docs_tools(srv, search_svc, effective_knowledge_dir);
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
- `minecraft_docs` 子命令实际调用验证；
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

- 出现 `minecraft_docs`；
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

### 3. `minecraft_docs` 子命令冒烟

建议调用：

```text
help
wiki minecraft:food --top 8
wiki "minecraft:food block"
all 自定义方块
api ListenForEvent
assets stair --bp
assets stair --rp --top 5
list BedrockWiki
read BedrockWiki/items/items-intro.md --start 1 --end 40
netease diff
netease jsonui
diff
jsonui
```

非法输入也要检查：

```text
wiki
bogus
netease
```

预期：返回 help 或友好错误，不崩溃。

## 后续路线

### 第一阶段（已完成）

搜索 / 资料库 / 网易参考速查 → 已合并到 `minecraft_docs`。

### 第二阶段（已完成，2026-06-24）

像素画 28 个独立工具 → 已合并到 `minecraft_pixelart` 单工具（命令式）。

详见下方"第二阶段：像素画合并记录"。

### 第三阶段（已完成，2026-06-24）

JSON UI 7 个独立工具 → 已合并到 `minecraft_ui` 单工具（命令式）。

详见下方"第三阶段：JSON UI 合并记录"。

### 第四阶段（已完成，2026-06-24）

模型 6 个独立工具 → 已合并到 `minecraft_model` 单工具（命令式）。
动画 4 个独立工具 → 已合并到 `minecraft_animation` 单工具（命令式）。

详见下方"第四阶段：模型 + 动画合并记录"。

### 第五阶段（建议，仅剩）

Python 分析 2 个独立工具待整合（`scan_py2_mod_architecture` + `scan_py2_import_chain`）。
数量少但工作流配对清晰（先总览后深挖），且是唯一在 lite 版也启用的非资料工具，
整合后 lite 版 tools/list 更干净。

建议总原则：

- 一个能力族只暴露一个 MCP tool；
- tool 描述只讲"何时使用 + help"；
- 详细用法全部放到 `help` 子命令；
- 旧注册代码注释保留，不直接删除；
- 尽量复用现有 handler / service，不重写业务逻辑。

---

## 第二阶段：像素画合并记录（2026-06-24）

### 目的

将 `register_pixel_art.hpp` 中 28 个像素画独立工具合并为单一命令式入口
`minecraft_pixelart`，进一步降低 MCP tools/list 注入上下文的 token 占用。
本次与搜索性能优化工作完全隔离，未触碰 search 模块与 `canvas_manager.hpp` 核心。

### 工具命名

入口名定为 `minecraft_pixelart`（曾考虑 `minecraft_canvas`，但"canvas"偏底层容器语义，
`pixelart` 更准确表达"像素画处理"能力族）。

### 子命令协议

原 28 工具 → 子命令映射：

| 旧工具 | 新子命令 |
|---|---|
| canvas_new / canvas_load / canvas_save / canvas_info / canvas_preview | new / load / save / info / preview |
| draw_pixel / draw_pixels_batch / draw_line / draw_rect / draw_circle | pixel / batch / line / rect / circle |
| fill_flood / fill_rect / fill_gradient | flood / (rect --filled) / gradient |
| apply_outline / apply_shadow / apply_dithering / apply_palette_quantize | outline / shadow / dither / quantize |
| pixelate | pixelate |
| read_pixel / read_area / extract_palette / replace_color | rpixel / rarea / palette / recolor |
| transform_flip / transform_rotate / transform_scale / transform_crop | flip / rotate / scale / crop |

参数风格：长 flag（`--x --y --color --filled`），与 `minecraft_docs` 一致。
`fill_rect` 与 `draw_rect` 合并为 `rect`，用 `--filled` 区分实心/空心。
批量像素 `draw_pixels_batch` 的 pixels 数组通过 `--pixels '<JSON>'` 传入。

flag 别名：`--w=--width`、`--h=--height`、`--bg=--background`、`--tol=--tolerance`、
`--filled=--fill`、`--from=--old-color/--old`、`--to=--new-color/--new`、`--no-dither=--nodither`、
`--color-a=--ca`、`--color-b=--cb`、`--offset-x=--ox`、`--offset-y=--oy`。

### 已完成改动

#### 1. 新增统一入口 tool

文件：[`src/tools/register_minecraft_pixelart.hpp`](src/tools/register_minecraft_pixelart.hpp)

结构仿 `register_minecraft_docs.hpp`：

- `namespace minecraft_pixelart_detail`
- `pixelart_help_text()` 完整命令帮助
- `text_result()` / `image_result()` / `error_with_help()` 标准返回辅助
- 每个 dispatch 自由函数内部直接调用 `get_canvas()` 单例的 public 方法，
  不抽 handler 函数（canvas 操作是过程式的，逐子命令内联更清晰）
- `dispatch_minecraft_pixelart(command)` 主分发器
- `register_minecraft_pixelart_tools(srv)` 注册单 tool，唯一入参 `command`

`preview` 子命令返回 image 内容；`rarea`/`palette` 返回 JSON 文本；其余返回操作结果文本。

#### 2. 注释旧注册

文件：[`src/tools/register_pixel_art.hpp`](src/tools/register_pixel_art.hpp)

- 顶部注释更新为"已合并到 minecraft_pixelart"，附 28 工具 → 子命令映射表
- `register_pixel_art_tools()` 函数体整段用 `/* ... */` 注释保留
- `jstr/jint/jfloat/jbool/ok/err` 辅助函数保留不动（新入口未复用，但便于回滚）

#### 3. 切换服务注册入口

文件：[`src/app/server_runtime.cpp`](src/app/server_runtime.cpp)

`#ifndef MCDK_LITE` 块内：

```cpp
mcdk::register_jsonui_tools(srv);
// mcdk::register_pixel_art_tools(srv);   // 已合并
mcdk::register_minecraft_pixelart_tools(srv);
mcdk::register_model_tools(srv);
mcdk::register_animation_tools(srv);
```

仅改 full 版；lite 版原本就没注册像素画，不动。

### 已知行为差异（非缺陷）

**路径反斜杠转义**：`command_parser.hpp` 的词法分析会把 `\<char>` 当转义序列处理，
因此 Windows 反斜杠路径（如 `D:\mod\icon.png`）会被吃掉反斜杠。
已在 `pixelart_help_text()` 中明确提示"路径请使用正斜杠"。
这是 command_parser 的既有设计（docs 阶段就存在），不在本次修改范围。
若后续要根治，应在 command_parser 层面统一处理，而非各 dispatch 单独修补。

### 验证状态

已完成：

- `mcdk-assistant`（full 版）MSVC release 编译通过；
- stdio MCP 冒烟测试 42 项全部通过（脚本 `tests/minecraft_pixelart_stdio_probe.py`）。

测试覆盖：

- tools/list 包含 `minecraft_pixelart`，旧 26 个像素画工具不再注册；
- help / 空 command / 前导 `/` 三种触发方式；
- 画布生命周期：new → info → pixel → rpixel → save → PNG 实际生成 → load → info；
- 绘制形状：rect --filled + rarea 区域读取；
- 变换链：pixel → flip H → rpixel 验证像素位移；
- batch 批量绘制（JSON 数组参数）；
- 非法输入容错：bogus 子命令 / 缺参数 / 非法角度，均返回 help 友好错误；
- preview 返回 image 内容；
- 其余子命令（line/circle/flood/gradient/outline/shadow/pixelate/rotate/scale/crop/recolor/quantize/dither/palette）冒烟通过。

测试用法：

```bat
python tests/minecraft_pixelart_stdio_probe.py
python tests/minecraft_pixelart_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
```

另有可视化观测脚本 `tests/minecraft_pixelart_render_demo.py`，会生成实际 PNG 样例
（棋盘格/圆/渐变/笑脸/描边/像素化/阴影/变换链）供肉眼查看，并打印发现的 tool 列表。

---

## 第三阶段：JSON UI 合并记录（2026-06-24）

### 目的

将 `register_jsonui.hpp` 中 7 个 JSON UI 独立工具合并为单一命令式入口
`minecraft_ui`。本阶段与像素画阶段一样，完全不触碰 search 模块与底层编辑器核心。

### 子命令协议

原 7 工具 → 子命令映射：

| 旧工具 | 新子命令 |
|---|---|
| get_jsonui_reference | help（help 同时返回命令用法 + 速查手册） |
| generate_ui_fullstack | gen |
| diagnose_ui | diag |
| query_ui_control | query |
| dump_ui_tree | tree |
| search_ui_content | search |
| patch_ui_file | patch |

参数风格：长 flag（`--file --content --path --ops` 等），与 docs/pixelart 一致。
内容来源统一为 `--file <绝对路径>` 或 `--content <JSON文本>` 二选一（替代旧的 `file_path`/`json_content`）。
patch 的 ops 数组通过 `--ops '<JSON>'` 传入。

### 已完成改动

#### 1. 新增统一入口 tool

文件：[`src/tools/register_minecraft_ui.hpp`](src/tools/register_minecraft_ui.hpp)

结构仿 `register_minecraft_docs.hpp` / `register_minecraft_pixelart.hpp`：

- `namespace minecraft_ui_detail`
- `ui_reference_text()` 速查手册（从原 `JSONUI_REFERENCE_TEXT` 搬迁并更新工具引用措辞）
- `ui_help_text()` 命令用法
- `help_result()` 同时返回命令用法 + 速查手册（AI 一次拿到全部知识）
- 各 `dispatch_*` 自由函数：gen/diag/query/tree/search/patch 各一个
- **query/tree/search 三个工具原本是内联在 register_jsonui.hpp 的 lambda 里**，
  本次将其逻辑整体搬迁到本文件的 dispatch 函数，改造为基于 `ParsedCommand`。
  tree 用 `std::function` 实现自递归（lambda 无法直接自递归）。
- `register_minecraft_ui_tools(srv)` 注册单 tool，唯一入参 `command`

#### 2. 注释旧注册

文件：[`src/tools/register_jsonui.hpp`](src/tools/register_jsonui.hpp)

- 顶部注释更新为"已合并到 minecraft_ui"，附 7 工具 → 子命令映射表
- `register_jsonui_tools()` 函数体用 `#if 0 ... #endif` 整段注释保留
  （用 `#if 0` 而非 `/* */` 是因为函数体内含 `"/* 覆写属性 */"` 等字符串/注释，
  块注释嵌套会提前闭合）
- `JSONUI_REFERENCE_TEXT` / `resolve_json_content` 保留不动（新入口不依赖，便于回滚）

#### 3. 切换服务注册入口

文件：[`src/app/server_runtime.cpp`](src/app/server_runtime.cpp)

`#ifndef MCDK_LITE` 块内注释 `register_jsonui_tools`，新增 `register_minecraft_ui_tools`。

### help 文本准确性要点（已审查）

- patch 的 8 种 op 参数已与 `ui_patcher.h` 的真实结构对齐：
  `set_prop` 用 `props` 对象（非 key/value）、`remove_prop` 用 `keys` 数组、
  `merge_ctrl` 用 `key+props` 等。help 与速查手册都给出了正确示例。
- 通用规则提示：`--file`/`--content` 二选一、Windows 路径用正斜杠、参数含空格用引号包裹。
- tree 子命令标注"推荐修改 UI 前先调用了解整体结构"。
- help 子命令同时返回速查手册，避免 AI 需要两次调用。

### 验证状态

已完成：

- `mcdk-assistant`（full 版）MSVC release 编译通过；
- stdio MCP 冒烟测试 30 项全部通过（脚本 `tests/minecraft_ui_stdio_probe.py`）。

测试覆盖：

- tools/list 包含 `minecraft_ui`，旧 7 个 JSON UI 工具不再注册；
- help / 空 command / 前导 `/` / reference 四种触发方式；
- gen 生成全栈模板（json_ui + python_class 双段输出验证）+ 缺参数报错；
- query 顶层列表 / 指定控件 / 深层路径 / 不存在路径报错；
- tree 完整树 / depth 限制 / root 指定路径 / search 匹配；
- search 匹配纹理 / 绑定名 / 无匹配 / 缺 keyword 报错；
- diag 正常 UI 通过 / 问题 UI 报错；
- patch set_prop 成功 + 读回验证 size 已修改 + 缺参数 / 非法 JSON 报错；
- bogus 子命令 / 缺内容参数容错。

测试用法：

```bat
python tests/minecraft_ui_stdio_probe.py
python tests/minecraft_ui_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
```

---

## 第四阶段：模型 + 动画合并记录（2026-06-24）

### 目的

将 `register_model.hpp` 的 6 个模型工具合并为 `minecraft_model`，
将 `register_animation.hpp` 的 4 个动画工具合并为 `minecraft_animation`。
命名按"不绝对强调准确"原则，用通用的 model/animation 而非强调格式的
geometry/animations。本阶段同样完全不触碰 search 模块。

### 关键技术决策：op 执行逻辑抽取

模型/动画的 `model_op`/`anim_op` 工具原本各自内联了一个 `exec_one_op` lambda
（op 分发器），用于把扁平参数路由到 `op_*` 函数。本次将其抽取为自由函数：

- `model_editor.hpp` 新增 `inline std::string exec_model_op(json& geo, const json& src)`
- `animation_editor.hpp` 新增 `inline std::string exec_anim_op(json& root, const json& src)`

原 register_*.hpp 的 lambda 改为转发调用，**新旧入口共用同一逻辑**，
行为完全一致（含向量参数的 `json::parse` 字符串解析）。

### 子命令协议

#### minecraft_model（6 → 1）

| 旧工具 | 子命令 |
|---|---|
| get_model_reference | help（速查手册 + 命令用法） |
| model_parse | parse (--file \| --content) [--id] |
| model_get_bone | bone (--file \| --content) --bone [--id] |
| model_op | op --file [--id] --ops '<JSON数组>' |
| model_mirror_symmetry | mirror --file --src --dst [--id] [--no-uv] |
| model_create_from_template | create --save --id --template [--tw --th] |

#### minecraft_animation（4 → 1）

| 旧工具 | 子命令 |
|---|---|
| get_animation_reference | help（速查手册 + 命令用法） |
| animation_parse | parse (--file \| --content) [--filter] |
| animation_get_bone_channel | bone (--file \| --content) --anim --bone [--channel] |
| anim_op | op --file --ops '<JSON数组>' |

op 统一只用 `--ops '<JSON数组>'`（无单次 op 扁平参数模式，简化 AI 认知）。
向量/通道值参数在 ops 数组里传 JSON 字符串，内部 `json::parse` 解析（与原工具一致）。

### 已完成改动

#### 1. op 执行逻辑抽取（关键复用基础设施）

- `model_editor.hpp`：新增 `exec_model_op(json& geo, const json& src)`，10 种 op 分发
- `animation_editor.hpp`：新增 `exec_anim_op(json& root, const json& src)`，9 种 op 分发
- `register_model.hpp` / `register_animation.hpp`：原 `exec_one_op`/`exec_one_anim_op`
  lambda 改为转发调用（虽然旧注册已 #if 0 注释，但保留转发便于理解）

#### 2. 新增统一入口 tool

- `register_minecraft_model.hpp`：6 个子命令 dispatch + 速查手册 + 命令用法
- `register_minecraft_animation.hpp`：4 个子命令 dispatch + 速查手册 + 命令用法

结构仿 register_minecraft_ui.hpp，help 同时返回命令用法 + 速查手册。

#### 3. 注释旧注册

- `register_model.hpp`：`register_model_tools()` 用 `#if 0`/`#endif` 注释保留
- `register_animation.hpp`：同上
- `MODEL_REFERENCE_TEXT`/`ANIMATION_REFERENCE_TEXT`/辅助函数保留

#### 4. 切换服务注册入口

`server_runtime.cpp` 的 `#ifndef MCDK_LITE` 块内注释旧注册，新增新入口。

### 验证状态

已完成：

- `mcdk-assistant`（full 版）MSVC release 编译通过；
- 模型+动画 stdio 测试 27 项全部通过；
- 全量回归：像素画 42 项 + UI 30 项 + 模型/动画 27 项 = **99 项全部通过，无回归**。

测试覆盖（`tests/minecraft_model_anim_stdio_probe.py`）：

- tools/list：minecraft_model/minecraft_animation 出现，旧 10 工具消失；
- 模型：create humanoid → parse 验证骨骼 → op add_bone+add_cube → parse 验证生效 →
  bone 验证 cube → mirror leftArm→testRight → help → 非法输入容错；
- 动画：op add_anim+set_keyframe（文件自动创建）→ parse 验证 → bone 查看 keyframe →
  bone 指定 channel → help → 非法输入容错。

逆向审查：14 个 help 关键引导点全部命中（向量参数 JSON 字符串说明、
op 操作完整列表、--file/--content 二选一、路径正斜杠、文件自动创建等）。

测试用法：

```bat
python tests/minecraft_model_anim_stdio_probe.py
```

## 注意事项

1. 目前 `.mcp.json` 是项目级配置，可能会被纳入 git；是否提交由维护者决定。
2. `command_parser.hpp` 是后续所有合并工作的关键基础设施，修改时要注意保持无业务依赖。
3. `register_search.hpp` / `register_netease.hpp` 中虽然旧注册被注释，但 handler / 文本函数仍是新入口依赖，不能删除。
4. `register_pixel_art.hpp` 中旧注册被注释（`/* */` 块注释），但 `jstr/jint/jfloat/jbool/ok/err` 辅助函数保留（便于回滚）；新入口 `register_minecraft_pixelart.hpp` 不依赖这些辅助，直接调用 `get_canvas()`。
5. `register_jsonui.hpp` 中旧注册被注释（`#if 0` 预处理指令，因函数体含 `"/* */"` 字符串），`JSONUI_REFERENCE_TEXT` / `resolve_json_content` 保留；新入口 `register_minecraft_ui.hpp` 自带速查手册与内容解析，不依赖这些符号。
6. `minecraft_docs` 名称是当前确认结果：强调资料/文档查询语义，避免与完整开发能力混淆。
7. `minecraft_pixelart` 强调像素画处理语义，不用偏底层的 `minecraft_canvas`。
8. `minecraft_ui` 覆盖 JSON UI 全栈（手册/生成/诊断/查询/树/搜索/补丁），命名不绝对强调"准确"，留有余地。
9. `minecraft_model` / `minecraft_animation` 命名按"不绝对强调准确"原则用通用名（非 geometry/animations）。
   op 执行逻辑已抽取为 `exec_model_op`/`exec_anim_op` 自由函数，新旧入口共用。
   旧注册用 `#if 0` 注释保留，`MODEL_REFERENCE_TEXT`/`ANIMATION_REFERENCE_TEXT` 保留。
10. 本次没有改 `CMakeLists.txt`，因为新增的是 header-only 文件，通过 `server_runtime.cpp` 间接包含。
