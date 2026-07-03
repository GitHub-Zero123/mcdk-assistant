# 异步 Packet 管线设计草案

## 背景

这是一份用于插件搜索测试的伪技术文档。它描述一个名为 `NebulaPacketBus` 的假想网络管线，用来模拟服务端和脚本层之间的异步数据交换。

核心目标是让 packet 处理链路具备可观测、可限流、可重放的能力。每一个 packet 都会携带 `trace_id`、`channel`、`schema_version` 和 `payload_hash`。

## Packet 生命周期

Packet 进入网关后先经过 `DecodeStage`，再进入 `ValidateStage`，最后由 `DispatchStage` 投递给业务处理器。

如果 `schema_version` 低于当前运行时版本，管线会调用 `CompatAdapter` 做一次轻量转换。转换失败时不会直接丢弃 packet，而是写入 `dead_letter_queue` 便于开发者排查。

## 背压策略

当单个 channel 的排队数量超过 `soft_limit` 时，系统会开始降低非关键 packet 的优先级。超过 `hard_limit` 后，新的低优先级 packet 会被拒绝，并返回 `QUEUE_SATURATED`。

测试关键词：packet、异步、管线、背压、trace_id、dead_letter_queue。
