# ShaderSDK / 渲染龙开发引导

ShaderSDK（渲染龙 / RenderDragon 系列）是 `minecraft_docs` 下的文档子集。

## Addon 工程接入

以下为本项目维护的工程接入提示：

- 渲染龙在网易 **3.10 正式版**中加入 Addon 解析。
- 材质 bin 的 Addon 解析路径为 **`RP/materials/bins/Actor.material.bin`**，其中 `RP` 表示资源包根目录。
- 开发工程时，建议在工程配置中编译**全平台兼容的 bin 文件**，并验证各目标平台的加载与渲染效果。

## 查询方式

通过 `minecraft_docs` 的 `command` 参数调用：

```text
shader StandardSurfaceOutput
shadersdk 材质 --top 5
renderdragon Uniform
list ShaderSDK
read ShaderSDK/0-概述.md
read ShaderSDK/API参考/6-根类型与材质映射.md
```

`shader`、`shadersdk`、`shader-sdk`、`renderdragon` 指向同一分类；`all` 也包含此分类。

## 官方文档与更新

[官方 ShaderSDK 概述](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/0-概述.html?catalog=1)及 `API参考/` 下文档由爬虫获取，各页末尾保留来源链接。当前上游文档范围为 CSS r21u12 API 参考，工程接入提示单独在本页维护。

在仓库根目录执行：

```sh
pip install requests beautifulsoup4 markdownify mdformat mdformat-gfm
python automation/update_shadersdk_docs.py --dry-run
python automation/update_shadersdk_docs.py
python automation/update_shadersdk_docs.py --refresh --rebuild-index
```

爬虫沿用 ModAPI 自动化工具的缓存、重试与 Markdown 格式化流程。全部页面转换成功后才更新知识库，并刷新目录时间以使索引失效。它不会删除本地补充文档或覆盖本 README。新增分类使用 v8 通用索引缓存，发布时应使用新版索引编译器重新生成缓存。
