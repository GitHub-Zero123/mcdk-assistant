# MCDK-ASSISTANT

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)]()

面向 NetEase Minecraft / Bedrock 开发场景的通用 MCP Server。

聚合文档检索、原版资源搜索、参考速查、项目分析与代码审查能力，使 AI 能以更工程化的方式参与 Minecraft 开发流程。

<p align="center">
  <img src="docs/agent-mcp-workflow.svg" alt="MCDK Assistant MCP 与 LLM Agent 的工程协作关系" width="100%">
</p>

MCDK-ASSISTANT 不直接替代编辑器或 AI Agent，而是作为一个面向 Minecraft 工程语境的能力层：把知识库、原版资产、JSON UI、NBT、模型、动画、Python2 Addon 项目分析与代码审查能力整理成稳定的 MCP 工具，让 Agent 在“查资料、理解结构、定位文件、生成修改、审查回改、回读验证”的闭环里少猜测、多验证。

<p align="center">
  <img src="docs/tool-layering.svg" alt="MCDK Assistant MCP Tool 分层设计" width="100%">
</p>

MCP Tool 暴露面采用分层式设计：顶层只保留少量能力族入口，细粒度教程、子命令和操作参数延迟到 `/help` 或具体 `command` 调用时展开，从而减少 AI 客户端在 `tools/list` 阶段被迫接收的初始上下文。

### 🧠 解决方案启发
文档检索能告诉 AI"某个接口长什么样"，却答不了"怎么把这些接口拼成一个能跑的功能"——调用顺序、参数来源、端侧边界、常见坑，资料里都没有。**解决方案层**补的正是这一层：一个**经实测、端到端、拿来即用的开发范式库**，在检索时按需浮现相关范式与踩坑，让 AI 从"查到接口却盲猜用法"变成"照着可运行范式写"；范式之间还能按依赖从接口级组装成功能级。

<p align="center">
  <img src="docs/solution-search-demo.svg" alt="mcdk-solutions：检索即浮现可运行范式，并按依赖组装成完整功能" width="100%">
</p>

预编译成高性能索引，命中才现、不打扰纯资料检索。内容与写作规范见 [solutions/README.md](solutions/README.md)。

## 生态项目
- [QuMod](http://qumod.cc)：QuMod 主站点，汇总文档、资源、项目动态与相关内容
- [MCDK](https://github.com/GitHub-Zero123/MCDevTool)：轻量化网易 MOD 开发调试工具，支持后端内核与 VS Code 插件两种形态
- [MCDevTools for VSCode](https://marketplace.visualstudio.com/items?itemName=dofes.mcdev-tools)：MCDK 的 VS Code 插件形态，便于在编辑器内完成调试与开发辅助

## ✨ 核心能力

能力 | 说明
--- | ---
🔎 智能文档检索 | 支持知识库、网易教程、ModAPI、QuMod、Bedrock Wiki、BedrockDev 等资料搜索
🧭 原版资源搜索 | 模糊搜索行为包 / 资源包原版资产，支持按文件名和内容定位
📘 参考速查 | 快速获取网易版差异、JSON UI / 动画 / 模型参考资料
🐍 Python MOD 分析与审查 | `minecraft_py` 支持行为包架构分析、引用链追踪和 Python2 Addon 结构性代码审查
🧩 JSON UI 分析 | 支持控件结构查询、属性搜索与问题诊断；涉及资源修改的能力仅在完整版提供
🧠 解决方案层 | 检索时自动浮现「可运行组合范式 + 踩坑」，让 AI 照着写而非盲猜（默认开，`--no-solution` 可关；需预编译 bin）

> 默认分发以 `LITE` 为主，聚焦检索、搜索、参考、项目分析与代码审查能力。

### 🐍 Python AI 代码审查

`minecraft_py` 除了 `arch` / `imports` 项目分析外，还提供 `review` 子命令，用于让 AI 在写完或改完 Python2 MOD 代码后自查，再按报告回改。
它适合作为 Agent 修改代码后的自检步骤：优先审查刚改动的包或模块，输出按规则分组的结构性问题与可执行建议。

<details>
<summary><strong>诊断范围：十二条规则</strong></summary>

审查规则以低误报、可复核和可执行为目标，优先覆盖 Python2 Addon 中容易导致运行失败、维护成本上升或 AI 生成代码退化的结构性问题。

| rule_id | 级别 | 可执行性 | tier | 触发条件(默认) | 分寸 / 豁免 |
|---|---|---|---|---|---|
| `encoding.missing-utf8-declaration` | warning | **must-fix** | 1 | 文件带非 ASCII 字节,首两行却无 PEP263 编码声明,也无 UTF-8 BOM —— Py2 一 import 就 `SyntaxError` | **唯有含非 ASCII 才查**;空文件 / 纯空白 / 纯 ASCII 天然免检;BOM 即声明;识得 `# -*- coding: utf-8 -*-`、`# coding=utf-8`、shebang 后第二行、`# vim: set fileencoding=utf-8 :` 等多种版式 |
| `try.masking.bare-except` | hint\|risk\|warning | advisory-verify | 1 | 裸 `except:` 未 re-raise;轻重看 try 体大小:≤5 行→hint、≥20 行→warning、居中→risk | 未必是错——可能是 PC / 服务器环境差异下的有意防御,只请核实 |
| `try.masking.broad-except-swallow` | hint\|risk\|warning | advisory-verify | 1 | `except Exception/BaseException` 且吞掉(无实质兜底),分级同上 | 若有真实回退兜底,则不问 |
| `implicit-global.mutable-default` | risk | advisory-verify | 1 | 可变默认参数(`list`/`dict`/`set`)在体内被**累积式**修改(`append`/`+=`) | 不认 `x[k]=v`/`x.attr=v`(往引擎 / 调用方 dict 塞字段是惯用法);豁免约定名 `args`/`data`/`event`/`kwargs`… 及配置追加项 |
| `implicit-global.global-write` | risk | advisory-verify | 1 | 函数 `global x` 且写 `x`,且 `x` 为**公开名**(无下划线前缀) | **下划线私有全局径直放行**——缓存 / 单例 / 计数器是模块封装的正道;并顺手建议"若仅本模块自用,加个下划线即可" |
| `stub.placeholder` | warning / hint | should-fix / advisory | 1 | 函数体仅 `raise NotImplementedError`(warning)或 `...`(hint) | 接口 / 抽象命名类(`^I[A-Z]`、`Base*`/`Abstract*`、`*Interface`/`*ABC`)与 `@abstractmethod` 豁免;空 `pass`/`return None` 不问 |
| `stub.shallow-impl` | risk | advisory-verify | 2 | 函数体 ≥3 条业务语句,**却所有 `return` 皆固定值、且完全不碰任何参数** —— 疑似假实现 | 放过 1 行 `return 固定值`(Py2 无 .pyi,裸写补全库触发类型推导);排除 `return None`/非空集合;`_`/`__` 占位参、dunder、接口类豁免 |
| `signature.too-many-params` | hint / warning | advisory-verify | 2 | 非 `self`/`cls` 参数 > 5;6~7 个→hint、≥8 个→warning | `__init__`/`__new__` 豁免(构造器天然聚合);`*args`/`**kwargs` 不计;引擎固定签名可调阈值或整族关闭 |
| `logic-blob.large-function` | risk / warning | advisory-verify | 2 | 单函数行跨度 ≥50→risk、≥100→warning | 事件分发 / UI 回调类的长函数,可酌情放过 |
| `logic-blob.large-file` | warning | advisory-verify | 2 | 单文件代码行 ≥1000 **且** `def+class` ≥25(双重判定) | 纯数据 / 配置文件(体量大却极少定义)自然不触发 |
| `duplicate-function.cross-module` | warning | advisory-verify | 3 | 相同结构指纹(抹去标识符 / 字面量,只留控制流 + 调用名)、≥5 语句、指纹长 ≥10、散落 ≥2 文件、≥2 处 | 跳过 dunder;要求确有逻辑(控制流或 ≥2 次调用),免得把样板 `__init__` 错聚成"重复" |
| `comment-doc.unowned-todo` | hint | advisory-verify | 2 | 无主的 `TODO`/`FIXME`/`HACK`/`XXX` | 带括号者(疑似署名,如 `TODO(alice)`)不问 |

> **tier** 是精度分层:**1** 确定性词法 / 句法 · **2** 度量阈值 · **3** 启发式 / 相似度。级别越低,越是"证据确凿"。

### 降噪的底线:开放世界

这套规则刻意不做企业 linter 那一套"事无巨细的挑刺",而是守住几条底线,让报告只说要紧话:

- **QuModLibs** 等静态三方库默认不看(`--include-third-party` 可开)。
- **项目外与游戏引擎的 API,不解析、不报错**——只查结构与用法,绝不因"找不到定义"就发难。
- 空 `__init__.py`、空文件,不算解析失败。
- 输出**确定性排序** + `finding_key` 稳定去重——同一份代码反复审,结果始终如一,闭环方能收敛。

</details>

## 🎯 适用场景

- 查询网易版独占资料、原版资源、组件和接口参考
- 让 AI 辅助分析 JSON UI 结构与常见问题
- 让 AI 在修改 Python2 Addon 后运行结构性代码审查，按明确报告闭环回改
- 分析行为包入口、注册链、引用关系和目录职责
- 获取模型、动画与 JSON UI 的参考信息
- 快速说明网易版与国际版差异

## 📦 版本与分发策略

项目当前提供三个可执行目标：

能力版本 | 可执行文件 | 说明
--- | --- | ---
LITE 版 | `mcdk-asst-lite` | 默认发布版本，聚焦资料检索、原版资源搜索、参考说明、Python 项目分析与代码审查，不包含本地文件修改能力
完整版 | `mcdk-assistant` | 提供 JSON UI、NBT、模型、动画、像素画等本地资源读写与编辑能力；相关能力涉及敏感操作，但会占用更多初始上下文，建议仅在确认需要的情况下使用。
Server 版 | `mcdk-asst-server` | 在 LITE 能力基础上扩展后台请求记录与统计接口，适合服务化部署

默认端口为 `18766`（HTTP 模式）。

- SSE 地址：`http://127.0.0.1:18766/sse`

### 🏗️ 编译与测试状态

| 操作系统 | 架构 | 编译器 | 测试状态 | 备注 |
| --- | --- | --- | :---: | --- |
| **Windows 11** | x86_64 | MSVC v143 | ✅ Passing | VS 2022+ / CMake 3.29+ |
| **Ubuntu 24.04** | x86_64 | GCC 13.3.0 | ✅ Passing | WSL2 (Kernel 6.6+) |

## 🚀 快速开始

### 方式一：stdio 模式（推荐，无需 Node.js）

LITE 版和完整版支持 `--stdio` 参数，直接以 stdio 传输模式启动，AI 客户端通过 spawn 子进程建立连接，**无需额外部署**。

#### VSCode / Roo / Copilot

在项目根目录的 `.roo/mcp.json` 中配置（Roo 扩展专用）：

```jsonc
{
  "mcpServers": {
    "mcdk_assistant_stdio": {
      "command": "/path/to/mcdk-asst-lite.exe",
      "args": ["--stdio"],
      "alwaysAllow": ["*"],
      "timeout": 30
    }
  }
}
```

#### Codex

Codex 的 MCP 配置写在用户目录的 `config.toml` 中。Windows 默认路径：

```text
C:\Users\<你的用户名>\.codex\config.toml
```

也可以在 Codex IDE 扩展右上角齿轮菜单中选择 `Codex Settings > Open config.toml` 打开同一个文件。

Windows 下可直接运行：

```powershell
notepad $env:USERPROFILE\.codex\config.toml
```

在文件末尾追加以下配置。下面示例使用 LITE 版；如果需要 JSON UI、NBT、模型、动画、像素画等本地读写能力，把 `command` 改成完整版 `mcdk-assistant.exe`。

```toml
[mcp_servers.mcdk-asst-lite]
command = "D:/Zero123/CPP/CMAKE/mcdk-assistant/build/x64-msvc-release/mcdk-asst-lite.exe"
args = ["--stdio"]
enabled = true
startup_timeout_sec = 30
tool_timeout_sec = 120

# 可选：完整版，默认关闭。需要本地资源编辑能力时再启用。
[mcp_servers.mcdk-assistant]
command = "D:/Zero123/CPP/CMAKE/mcdk-assistant/build/x64-msvc-release/mcdk-assistant.exe"
args = ["--stdio"]
enabled = false
startup_timeout_sec = 30
tool_timeout_sec = 120
```

保存后重启 Codex，或开启新会话。在 Codex CLI/TUI 中可用 `/mcp` 查看连接状态。通常只启用 LITE 版和完整版中的一个，避免重复暴露同类工具。

#### Claude Desktop / 其他支持 stdio 的客户端

```jsonc
{
  "mcpServers": {
    "mcdk_assistant_stdio": {
      "command": "/path/to/mcdk-asst-lite.exe",
      "args": ["--stdio"]
    }
  }
}
```

---

### 方式二：HTTP / SSE 模式

直接运行可执行文件，不带 `--stdio` 参数，默认以 HTTP 模式启动。

#### 支持 SSE 的客户端

```jsonc
{
  "mcpServers": {
    "minecraft_mod_assistant": {
      "url": "http://127.0.0.1:18766/sse",
      "name": "Minecraft Mod Assistant MCP Server (MCDK)",
      "alwaysAllow": ["*"],
      "timeout": 30
    }
  }
}
```

#### VSCode / Copilot（通过 mcp-remote 桥接）

```jsonc
{
  "servers": {
    "minecraft_mod_assistant": {
      "command": "npx",
      "args": [
        "mcp-remote",
        "http://127.0.0.1:18766/sse",
        "--transport",
        "sse-only"
      ]
    }
  }
}
```

这一步依赖本地 `Node.js` 环境。

---

### 验证连接

可在 AI 客户端中输入类似指令：

- 搜索 `minecraft:food` 组件用法
- 搜索网易版和国际版 JSON UI 的主要差异
- 查找某个原版资源文件或动画资源
- 分析 Python 行为包结构：`minecraft_py(command="arch D:/mc/addons/MyAddon/behavior_pack --depth 2")`
- 审查刚修改的 Python 模块：`minecraft_py(command="review D:/mc/addons/MyAddon/behavior_pack --scope my_mod/client --format summary")`

如果能返回对应资料或搜索结果，说明 MCP Server 已连接成功。

## 📁 目录要求

- `dicts/`：分词词典
- `knowledge/`：可选的资料目录，用于提供知识库、教程、原版资产及参考资料
- 索引缓存：支持使用预生成的二进制索引文件启动，以减少对原始资料目录的依赖并提升加载速度

程序按“可执行文件所在目录”寻找这些资源；在提供索引缓存的情况下，不强制依赖完整的原始资料目录。

## 🔗 第三方依赖

项目当前主要依赖以下第三方库：

- `cpp-mcp`
  MCP 通信框架，仓库内位于 `libs/cpp-mcp/`
- `nlohmann/json`
  JSON 解析与序列化
- `cppjieba`
  中文分词与检索支持
- `limonp`
  `cppjieba` 依赖
- `stb_image` / `stb_image_write`
  PNG 读写与像素画处理
- `cpp-httplib`
  由 `cpp-mcp` 间接使用的 HTTP 能力
- `tree-sitter`
  通用语法分析与结构树构建
- `tree-sitter-python`
  Python 语言的 Tree-sitter 语法定义，用于解析 Python 代码
- `tree-sitter-typescript`
  TypeScript 的 Tree-sitter 语法定义，用于解析 SAPI `.d.ts` 声明文件并构建符号索引
- `pocketPy`
  Python脚本引擎使用的三方解释器，服务于轻量级插件系统
- `tomlplusplus`
  Toml配置文件解析使用。
### 🛠️ 关于 `cpp-mcp`

这里使用的不是纯上游原版，而是项目内维护的定制版本。
已针对本项目的本地服务场景做过适配和改造，升级、替换或同步时不应按上游原版直接无差异处理。
