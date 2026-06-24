# v0.2.0 更新日志

## MCP 稳定性、Tool 分层与 NBT 工具

该版本重点修复 stdio 模式下并发调用工具时可能出现的脏数据流污染问题，并对 MCP Tool 暴露面进行分层重构。新版本以更少的初始上下文占用提供更清晰的能力入口，同时新增 Minecraft NBT 文件工具，支持 `.mcstructure` 等 NBT 文件的查看、创建与编辑。

### 修复 stdio 并发调用崩溃

- 修复 stdio 模式下并发调用 MCP Tool 时，响应数据流可能互相污染的问题
- 解决脏数据流导致 JSON-RPC 响应异常、客户端解析失败或服务进程崩溃的风险
- 强化并发调用场景下的输出边界，避免工具结果串流到其他请求
- 提升 Codex、Claude Code、Roo 等 stdio MCP 客户端在高频工具调用下的稳定性
- 推荐使用 stdio 模式作为本地开发默认接入方式，无需额外 Node.js 桥接

### 重构 MCP Tool 分层设计

- 重构全量 MCP Tool 暴露方式，由大量独立顶层 Tool 调整为“能力族入口 + command 子命令”的分层结构
- 资料、JSON UI、模型、动画、像素画、NBT 等能力均采用统一的命令式入口
- 各入口提供 `/help` 或 `help` 子命令，按需返回完整教程与速查手册
- 通过启发式设计减少 MCP tools/list 阶段注入给 AI 客户端的初始上下文
- 降低 Agent 在工具选择阶段的噪声，减少误选旧工具、重复工具或过细粒度工具的概率
- 旧的分散式工具入口不再作为默认运行时暴露面，避免长期占用上下文窗口

### 新增 NBT 系列工具

- 新增统一入口：`minecraft_nbt(command="...")`
- 支持查看 NBT 文件结构：`view --file <path>`
- 支持创建基础 NBT 文件：`create --file <path> --template mcstructure|empty`
- 支持编辑 NBT 文件：`edit --file <path> --save <path> --ops '<JSON array>'`
- 支持 `.mcstructure`、`level.dat`、未压缩 `.nbt` 等文件格式
- 支持 tree 与 tagged-JSON 两种查看方式，便于复制子树后继续编辑
- 支持 set / remove / rename / add 等基础编辑操作，并支持批量 `ops` 一次性写回
- 编辑命令默认可直接写回原文件，建议实验阶段使用 `--save` 输出到副本
- 写入类命令不会自动创建父目录，目标目录必须已存在

### 文档与引导优化

- 各合并入口补充命令式 `/help` 指引，强调参数含空格或 JSON 数组/对象时需要使用引号包裹
- 资料库入口补充网易 ModSDK / ModAPI 的开发语义提醒：
  - ModSDK / ModAPI 多为 C 接口封装，通常不是异常驱动设计
  - 编写示例代码时优先按返回值、回调或文档约定判断成败
  - Mod 环境下客户端线程和服务端线程共享同一个 VM 上下文，跨线程初始化模块时需避免意外执行对方侧代码
- README 增加 Codex 与 Claude Code 的 MCP 配置示例
- README 增加 Agent 与 MCDK MCP 协作关系示意图，说明资料检索、资产定位、文件编辑与回读验证的典型闭环

### 推荐迁移方式

- 本地只需要资料检索、参考速查和资产搜索时，优先启用 `mcdk-asst-lite`
- 需要 JSON UI、NBT、模型、动画、像素画等本地资源读写能力时，再切换到完整版 `mcdk-assistant`
- 同一 AI 客户端中通常只启用 LITE 版或完整版中的一个，避免重复暴露同类工具
- 对新版命令式工具不熟悉时，优先调用对应入口的 `/help`：
  - `minecraft_docs(command="/help")`
  - `minecraft_ui(command="/help")`
  - `minecraft_model(command="/help")`
  - `minecraft_animation(command="/help")`
  - `minecraft_nbt(command="/help")`
