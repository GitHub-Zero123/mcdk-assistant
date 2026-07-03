# MCDK Assistant 插件目录

把插件放在本目录下，每个插件一个子目录：

```text
plugins/
  hello/
    manifest.json
    main.py
```

`manifest.json` 最小示例：

```json
{
  "id": "hello",
  "name": "Hello Plugin",
  "version": "0.1.0",
  "entry": "main.py",
  "description": "示例插件"
}
```

`main.py` 示例：

```python
import mcdk_assistant as mcdk

@mcdk.tool("hello", "返回一个问候", options=mcdk.ToolOptions(read_only=True, idempotent=True))
def hello(args, ctx):
    print("hello plugin called")
    return "hello"
```

插件中的 `print()`、`sys.stdout.write()` 和 `sys.stderr.write()` 会被引擎转发到宿主日志。在 stdio MCP 模式下不会污染 stdout 协议流。

开发补全库位于安装目录的 `plugins-dev/completion`，可加入 IDE 的 Python analysis path。
