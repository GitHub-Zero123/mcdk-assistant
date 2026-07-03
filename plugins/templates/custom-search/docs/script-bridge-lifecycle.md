# 脚本桥接生命周期

## 初始化

插件运行时会先加载 `manifest.json`，再执行入口脚本 `main.py`。入口脚本通过 `mcdk_assistant` 注册 tool 和 hook。

对于 Python 插件，推荐在模块顶层完成轻量注册，把昂贵工作延迟到 tool 被调用时执行。例如搜索索引可以先创建为 lazy index，等首次查询再构建。

## 输出流

脚本里的 `print()`、`sys.stdout.write()` 和 `sys.stderr.write()` 都应该由宿主统一接管。在 stdio MCP 模式下，这些输出会进入 stderr，避免污染 stdout 上的 JSON-RPC 协议流。

tool 的正式结果只取函数 `return` 值。插件作者可以随便打印调试信息，宿主侧负责兜底。

## 组件生命周期

一个假想组件 `ScriptBridgeComponent` 有四个阶段：`created`、`registered`、`active`、`disposed`。

组件进入 `registered` 后可以暴露工具，进入 `active` 后可以处理请求。进入 `disposed` 后应释放内存索引、关闭临时句柄，并停止接受新的 hook 调用。

测试关键词：脚本桥接、生命周期、stdio、print、tool、hook、组件生命周期。
