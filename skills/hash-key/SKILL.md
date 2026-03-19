---
name: hash-key
description: 在 Python 性能敏感代码中，进行字典或集合高频查找时，优先设计轻量、稳定、可复用的 key，善于利用元组，避免字符串拼接。
---

# 哈希搜索优化

用于字典、集合、缓存表、注册表等高频查找场景。

规则：

- 善于利用元组组织多字段 key
- 高频查找优先使用轻量、稳定的 key
- 不要在查询时临时字符串拼接构造 key
- 多字段 key 优先用元组等不可变类型
- 仅在确有必要时再引入自定义 key 或额外哈希优化

原因：

- 字符串拼接会创建新对象，增加分配和哈希开销
- 元组 key 通常更直接、更轻量，也更易读
- key 结构越简单，字典和集合查找越稳定
- 过度设计自定义 key 会提高维护成本

反例：

```python
def getSystem(namespace, systemName):
    return SYSTEM_MAP.get(namespace + "::" + systemName)
```

案例：

```python
def getSystem(namespace, systemName):
    return SYSTEM_MAP.get((namespace, systemName))
```
