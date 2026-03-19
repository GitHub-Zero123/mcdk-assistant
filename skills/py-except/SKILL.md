---
name: py-except
description: 在编写 Python 性能敏感代码，尤其是 tick、循环、事件热路径时，避免把异常处理当作常态控制流；优先使用显式判断，并避免裸 except。
---

# Python 异常处理优化

用于性能敏感代码，尤其是 tick、循环、事件回调。

规则：

- 不要把 `try/except` 当常态分支使用
- 热路径里优先 `if` 判断，不要频繁依赖异常
- 禁止裸 `except:`，至少捕获明确异常类型
- 异常只用于真正罕见、不可预期的错误
- 频繁路径中，缺失值、空对象、边界情况都应显式处理

原因：

- 异常触发会创建异常对象并收集 traceback，开销明显高于普通判断
- 裸 `except` 会吞掉真实错误，降低可调试性
- 用异常代替分支会让代码语义变差

反例：

```python
try:
    x, y, z = GET_ENTITY_POS(entityId)
except:
    pass
```

推荐：

```python
pos = GET_ENTITY_POS(entityId)
if pos is not None:
    x, y, z = pos
```
