# MCDK-ASSISTANT

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)]()

面向 NetEase Minecraft / Bedrock 开发场景的通用 MCP Server。

聚合文档检索、原版资源搜索、参考速查与扩展分析能力，使 AI 能以更工程化的方式参与 Minecraft 开发流程。

<p align="center">
  <img src="docs/agent-mcp-workflow.svg" alt="MCDK Assistant MCP 与 LLM Agent 的工程协作关系" width="100%">
</p>

MCDK-ASSISTANT 不直接替代编辑器或 AI Agent，而是作为一个面向 Minecraft 工程语境的能力层：把知识库、原版资产、JSON UI、NBT、模型、动画和 Python2 Addon 分析能力整理成稳定的 MCP 工具，让 Agent 在“查资料、理解结构、定位文件、生成修改、回读验证”的闭环里少猜测、多验证。

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
🧩 JSON UI 分析 | 支持控件结构查询、属性搜索与问题诊断；涉及资源修改的能力仅在完整版提供
🧠 解决方案层 | 检索时自动浮现「可运行组合范式 + 踩坑」，让 AI 照着写而非盲猜（默认开，`--no-solution` 可关；需预编译 bin）

> 默认分发以 `LITE` 为主，聚焦检索、搜索、参考与分析能力。

## 🎯 适用场景

- 查询网易版独占资料、原版资源、组件和接口参考
- 让 AI 辅助分析 JSON UI 结构与常见问题
- 获取模型、动画与 JSON UI 的参考信息
- 快速说明网易版与国际版差异

## 📦 版本与分发策略

项目当前提供三个可执行目标：

能力版本 | 可执行文件 | 说明
--- | --- | ---
LITE 版 | `mcdk-asst-lite` | 默认发布版本，聚焦资料检索、原版资源搜索和参考说明，不包含本地文件修改能力
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

### 🛠️ 关于 `cpp-mcp`

这里使用的不是纯上游原版，而是项目内维护的定制版本。
已针对本项目的本地服务场景做过适配和改造，升级、替换或同步时不应按上游原版直接无差异处理。
