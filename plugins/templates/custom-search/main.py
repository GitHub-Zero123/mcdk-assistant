import mcdk_assistant as mcdk

DOC_INDEX = None

SEARCH_SCHEMA = (
    mcdk.schema.Object()
    .field(
        "keyword",
        mcdk.schema.String(
            "要搜索的关键词，例如 packet、缓存、热更新、组件生命周期",
            required=True,
        ),
    )
    .field(
        "top_k",
        mcdk.schema.Integer(
            "返回结果数量，默认 5，范围 1-10",
            minimum=1,
            maximum=10,
            default=5,
        ),
    )
)


def _as_int(value, fallback):
    try:
        return int(value)
    except Exception:
        return fallback


def _get_doc_index(ctx):
    global DOC_INDEX
    if DOC_INDEX is None:
        DOC_INDEX = mcdk.search.build_index_from_dir(
            ctx.plugin_dir + "/docs",
            glob=["**/*.md"],
            mode="zh",
            chunk="markdown_heading",
            lazy=True,
        )
    return DOC_INDEX


@mcdk.tool("demo_tech_search", "搜索 custom_search_demo 插件内置的伪技术文档。", SEARCH_SCHEMA)
def demo_tech_search(args: dict, ctx: mcdk.ToolContext):
    keyword = str(args.get("keyword", "")).strip()
    top_k = _as_int(args.get("top_k", 5), 5)
    if top_k < 1:
        top_k = 1
    if top_k > 10:
        top_k = 10

    if not keyword:
        return {
            "error": "keyword is required",
            "hint": "试试搜索 packet、缓存、热更新、组件生命周期、脚本桥接",
        }

    results = _get_doc_index(ctx).search(keyword, top_k)
    return {
        "plugin": ctx.plugin_id,
        "tool": ctx.tool,
        "query": keyword,
        "count": len(results),
        "results": results,
    }
