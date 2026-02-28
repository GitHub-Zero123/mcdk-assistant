# 国际版 vs 网易版差异映射工具设计

## 方案

在 `main.cpp` 中硬编码差异文本，注册无参数 MCP 工具 `get_netease_diff`，调用即返回。

## 差异文本内容

```
网易版与国际版文件夹命名差异：
行为包：items -> netease_items_beh, biomes -> netease_biomes, blocks -> netease_blocks, features -> netease_features, feature_rules -> netease_feature_rules, recipes -> netease_recipes
资源包：items -> netease_items_res
```

## 代码改动

仅修改 `src/main.cpp`，在现有工具注册之后新增 `get_netease_diff` 工具。
