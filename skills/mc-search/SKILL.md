---
name: mc-search
description: 在处理我的世界 MOD、Bedrock、网易版、JSON UI、模型、动画、原版资源等任务时，优先通过 MCP 资料库检索接口、字段、路径、格式规则和示例，避免编造事实。
---

# 先查资料库

涉及以下内容时，先检索再回答：

- ModAPI 接口、事件、枚举、回调参数
- 网易版差异、教程、独占规则
- JSON UI 属性、绑定、命名空间、补丁规则
- 原版资源路径、命名、示例文件
- QuMod 相关内容

工具选择：

- `search_api` / `search_event` / `search_enum` / `search_all`：ModAPI
- `search_netease_guide`：网易资料
- `search_wiki`：Bedrock Wiki
- `search_qumod`：QuMod
- `search_game_assets`：原版资源
- `get_netease_diff` / `get_jsonui_reference` / `get_animation_reference` / `get_model_reference` / `get_netease_jsonui`：速查说明

要求：

- 先检索，再下结论
- 资料不足时直接说明，不要硬猜
- 生成代码或 JSON 前，凡涉及字段、事件、路径、格式，先查资料
- 文本润色、纯重构、纯创意讨论可不检索
