# MCDK-ASSISTANT

面向 NetEase Minecraft / Bedrock 开发场景的通用 MCP Server，基于 `C++20` 构建。

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

> 默认分发以 `LITE` / `Server` 为主，聚焦检索、搜索、参考与分析能力。
>
> 完整版额外提供模型、动画、像素画等本地资源读写与编辑能力；这部分涉及敏感文件操作，默认不提供 release 包，需自行编译。

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

默认端口为 `18766`。

- SSE 地址：`http://127.0.0.1:18766/sse`

## 🚀 快速开始

### 1. 配置 MCP 客户端

以支持 SSE 的客户端为例：

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

### 2. VSCode / Copilot 接入

当前 VSCode 不能直接连接 SSE，可通过 `mcp-remote` 做桥接：

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

### 3. 验证连接

可在 AI 客户端中输入类似指令：

- 搜索 `minecraft:food` 组件用法
- 搜索网易版和国际版 JSON UI 的主要差异
- 查找某个原版资源文件或动画资源

如果能返回对应资料或搜索结果，说明 MCP Server 已连接成功。

<!-- ## 🧰 工具范围说明

### LITE / Server 默认提供

工具分类 | 说明
--- | ---
🔎 文档与知识检索 | `search_api`、`search_event`、`search_enum`、`search_all`、`search_netease_guide`、`search_wiki`
🧭 原版资源搜索 | `search_game_assets`
📘 网易 / 参考资料 | `get_netease_diff`、`get_jsonui_reference`、`get_animation_reference`、`get_model_reference`、`get_netease_jsonui` -->

<!-- ### 仅完整版提供

以下能力涉及本地文件读写、资源修改或画布操作等**敏感能力**，不包含在默认发布的 LITE 版本中：

工具分类 | 说明
--- | ---
🧩 JSON UI | `diagnose_ui`、`dump_ui_tree`、`query_ui_control`、`search_ui_content`、`patch_ui_file`、`generate_ui_fullstack`
🦴 模型 | `model_parse`、`model_get_bone`、`model_op`、`model_mirror_symmetry`、`model_create_from_template`
🎞️ 动画 | `animation_parse`、`animation_get_bone_channel`、`anim_op`
🎨 像素画 | `canvas_new`、`canvas_load`、`canvas_save`、`canvas_preview`、`draw_pixels_batch`、`fill_rect`、`apply_outline`、`extract_palette` 等

如需让 AI 直接修改 `JSON UI`、模型、动画或贴图资源，应使用自行编译的完整版。 -->

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
