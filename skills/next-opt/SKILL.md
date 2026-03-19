---
name: next-opt
description: 在 Python 中手动驱动生成器或迭代器时，优先使用 for 循环；必须手动推进时，优先使用带默认值的 next()，避免在热路径中用 try/except StopIteration 作为常态结束判断。
---

# next() 迭代优化

用于生成器、迭代器、协程调度、微线程推进等场景。

结论：

- 能用 `for` 就用 `for`
- 必须手动推进时，优先 `next(it, default)`
- 不要在热路径中用 `try/except StopIteration` 做常态结束判断

原因：

- `for` 循环由解释器在 C 层处理，通常最快且最稳
- `next(it, default)` 可避免显式进入异常路径
- `try/except StopIteration` 依赖异常结束，开销更高

适用场景：

- 外部调度器按帧推进生成器
- 需要手动暂停、恢复、限步执行的迭代器
- 事件驱动或 tick 驱动的生成器消费

反例：

```python
while 1:
    try:
        next(gen)
    except StopIteration:
        break
```

案例：

```python
for item in gen:
    pass
```

```python
while next(gen, None) is not None:
    pass
```
