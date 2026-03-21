# MCDK-ASSISTANT

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg)]()

面向 NetEase Minecraft / Bedrock 开发场景的通用 MCP Server。

聚合文档检索、原版资源搜索、参考速查与扩展分析能力，使 AI 能以更工程化的方式参与 Minecraft 开发流程。

## 生态项目
- [QuMod](http://qumod.cc)：QuMod 主站点，汇总文档、资源、项目动态与相关内容
- [MCDK](https://github.com/GitHub-Zero123/MCDevTool)：轻量化网易 MOD 开发调试工具，支持后端内核与 VS Code 插件两种形态
- [MCDevTools for VSCode](https://marketplace.visualstudio.com/items?itemName=dofes.mcdev-tools)：MCDK 的 VS Code 插件形态，便于在编辑器内完成调试与开发辅助

## ✨ 核心能力

能力 | 说明
--- | ---
🔎 智能文档检索 | 支持知识库、网易教程、ModAPI、QuMod、Bedrock Wiki 等资料搜索
🧭 原版资源搜索 | 模糊搜索行为包 / 资源包原版资产，支持按文件名和内容定位
📘 参考速查 | 快速获取网易版差异、JSON UI / 动画 / 模型参考资料
🧩 JSON UI 分析 | 支持控件结构查询、属性搜索与问题诊断；涉及资源修改的能力仅在完整版提供

> 默认分发以 `LITE` 为主，聚焦检索、搜索、参考与分析能力。
<!-- >
> 完整版额外提供模型、动画、像素画等本地资源读写与编辑能力；这部分涉及敏感文件操作，默认不提供 release 包，需自行编译。 -->

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
完整版 | `mcdk-assistant` | 提供 JSON UI、模型、动画、像素画等本地资源读写与编辑能力；相关能力涉及敏感操作，默认不提供 release 包，需自行编译
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
    "mcdk-assistant": {
      "command": "/path/to/mcdk-asst-lite.exe",
      "args": ["--stdio"],
      "alwaysAllow": ["*"],
      "timeout": 30
    }
  }
}
```

#### Claude Desktop / 其他支持 stdio 的客户端

```jsonc
{
  "mcpServers": {
    "mcdk-assistant": {
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

### 🛠️ 关于 `cpp-mcp`

这里使用的不是纯上游原版，而是项目内维护的定制版本。
已针对本项目的本地服务场景做过适配和改造，升级、替换或同步时不应按上游原版直接无差异处理。
