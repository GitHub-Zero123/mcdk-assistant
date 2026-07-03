# 插件初始化模板

`hello/` 是最小插件模板：

```text
hello/
  manifest.json
  main.py
```

复制该目录到安装目录的 `plugins/` 下，然后重命名目录和 `manifest.json`
里的 `id`、`name`、`description` 即可开始开发。

`custom-search/` 是自定义搜索工具示例：

```text
custom-search/
  manifest.json
  main.py
  docs/
    async-packet-pipeline.md
    cache-hot-reload.md
    script-bridge-lifecycle.md
```

它注册 `demo_tech_search` tool，并使用 `mcdk.search.build_index_from_dir(...)`
懒构建插件自带 Markdown 文档索引。复制到运行时 `plugins/` 目录后，可以搜索
`packet`、`缓存`、`热更新`、`组件生命周期` 等关键词测试。
