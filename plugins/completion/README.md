# MCDK Assistant 插件 Python API

本目录是给插件作者使用的 API 补全库。
源码树中路径为 `plugins/completion`；安装产物中路径为 `plugins-dev/completion`。
把对应的 `completion` 目录加到 IDE 的 Python analysis path 后，插件代码即可
`import mcdk_assistant` 并获得补全、类型提示和中文悬停文档。

运行时，MCDK Assistant 会在 pocketPy 中注入真实的 `mcdk_assistant` 模块；
这里的 Python 文件只用于开发体验，不参与嵌入式运行时。

最小插件示例：

```python
import mcdk_assistant as mcdk

HELLO_SCHEMA = (
    mcdk.schema.Object()
    .field("name", mcdk.schema.String("要问候的名字", default="Steve"))
)

@mcdk.tool(name="hello", description="返回一个问候", schema=HELLO_SCHEMA)
def hello(args, ctx):
    return "hello " + str(args.get("name", "Steve"))

@mcdk.hook(mcdk.hooks.MINECRAFT_DOCS_SEARCH_AFTER_RENDER, priority=10)
def after_render(args, ctx):
    return None
```
