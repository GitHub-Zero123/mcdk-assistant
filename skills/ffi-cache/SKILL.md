---
name: ffi-cache
description: 在 Python 性能敏感代码中，尤其是通过 FFI 调用 C、C++ 扩展或 MC-MODSDK 接口时，优先缓存高频结果，避免重复跨语言边界获取大对象或容器数据。
---

# FFI 缓存优化

用于 tick、循环、事件回调等性能敏感路径中的 FFI 调用。

规则：

- 高频 FFI 调用优先考虑缓存
- 大容器、大对象、配置、状态快照不要每次都跨语言重新取
- 用事件、脏标记或版本号驱动缓存刷新
- 变化频繁且必须实时的数据，再直接调用 FFI
- 缓存策略要和数据更新源绑定，避免无效刷新
- 不要为了性能过度增加状态复杂度，仅在高频需求下使用这类优化

原因：

- FFI 调用有跨语言边界成本
- C/C++ 侧构造大容器并转换为 Python 对象开销高
- 热路径重复拉取同一批数据会明显拖慢吞吐
- 过度缓存会提高维护成本并增加状态同步风险

反例：

```python
for _ in range(100000):
    entities = list(serverApi.GetEngineActor())
```

案例：

```python
_WORLD_ENTITY_CACHE = []
_NEED_REFRESH = True

@Listen("AddEntityServerEvent")
def AddEntityServerEvent(_=None):
    global _NEED_REFRESH
    _NEED_REFRESH = True

@Listen("EntityRemoveEvent")
def EntityRemoveEvent(_=None):
    global _NEED_REFRESH
    _NEED_REFRESH = True

def GET_WORLD_ENTITY_LIST():
    global _NEED_REFRESH, _WORLD_ENTITY_CACHE
    if _NEED_REFRESH:
        _NEED_REFRESH = False
        _WORLD_ENTITY_CACHE = list(serverApi.GetEngineActor())
    return _WORLD_ENTITY_CACHE
```
