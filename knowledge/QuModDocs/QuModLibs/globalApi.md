# 通用系统端 API

通用系统端 API 适用于服务端和客户端。通过基础端侧模块导入后，`serverApi` / `clientApi`、`levelId`、`compFactory`、事件监听、跨端通信等常用能力会被一并提供。

```python
from .QuModLibs.Server import *
# from .QuModLibs.Client import *
```

::: tip compFactory
高版本中 `compFactory` 已经被基础端侧模块直接包含。文档示例推荐使用 `compFactory.CreateXxx(...)`，无需反复书写 `serverApi.GetEngineCompFactory()` 或 `clientApi.GetEngineCompFactory()`。
:::

## 游戏事件监听

QuMod 提供静态监听和动态监听两种常用方式。新代码推荐直接使用字符串事件名。

::: warning 关于 Events.xxx
`Events.xxx` 属于旧式事件名辅助写法，需要频繁维护类属性映射。文档后续统一推荐字符串事件名，例如 `@Listen("ServerItemTryUseEvent")`。
:::

### 静态监听

静态监听适合模块级常驻逻辑。监听会在游戏关闭时自动销毁，但不支持运行时动态取消。

```python
# Server.py
from .QuModLibs.Server import *

@Listen("OnScriptTickServer")
def OnScriptTickServer(args={}):
    print("tick")

@Listen("OnCarriedNewItemChangedServerEvent")
def OnCarriedNewItemChangedServerEvent(args={}):
    playerId = args["playerId"]
    comp = compFactory.CreateCommand(levelId)
    comp.SetCommand("/say 我切换了手持物品", playerId)
```

::: tip 游戏事件
事件触发时机和参数请以[网易我的世界 API 事件索引表](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/1-ModAPI/%E4%BA%8B%E4%BB%B6/%E4%BA%8B%E4%BB%B6%E7%B4%A2%E5%BC%95%E8%A1%A8.html?catalog=1)为准。
:::

### 动态监听

动态监听适合类实例、临时业务或需要手动注销的场景。

```python
from .QuModLibs.Server import *

class TestCls:
    def __init__(self):
        ListenForEvent("OnScriptTickServer", self, self.onTick)

    def onTick(self, args={}):
        print("tick")

    def free(self):
        UnListenForEvent("OnScriptTickServer", self, self.onTick)
```

## 游戏销毁处理

当游戏关闭时，一些业务需要做回收处理，例如保存数据、释放外部资源或取消自定义状态。

### DestroyFunc <Badge type="tip" text="^1.3.0" />

```python
@DestroyFunc
def onGameClose():
    print("游戏关闭时触发")
```

### QuDestroy <Badge type="danger" text="已废弃" />

旧版 `QuDestroy` 仅适用于动态注册文件，不推荐新项目继续使用。

```python
def QuDestroy():
    print("旧版销毁入口")
```

## 跨系统端通信

跨端通信通过网络发包完成。QuMod 用 `AllowCall` + `Call` 简化了服务端与客户端之间的函数调用。

### AllowCall

被 `AllowCall` 标记的函数会登记为可远程调用对象；普通本地调用不受影响。

```python
@AllowCall
def serverFunc(name):
    print(name)
```

### Call

```python
# 客户端调用服务端
Call("serverFunc", "Steve")

# 服务端调用指定客户端
Call(playerId, "clientFunc", "hello")

# 服务端广播给所有客户端
Call("*", "clientFunc", "hello")
```

::: warning 调用频率
远程通信涉及 I/O 开销，请勿在高频 Tick 中无节制调用。本地环境下 `Call` 也会经过环回流程。
:::

### MultiClientsCall <Badge type="tip" text="^1.2.0" />

`MultiClientsCall` 是服务端独占能力，用于批量调用多个客户端函数，比手动循环 `Call` 更适合批量发包场景。

```python
MultiClientsCall(["playerId1", "playerId2"], "clientFunc", "hello")
```

## Entity 实体类

`Entity` 是 QuMod 提供的实体快捷封装，用于读写常见实体属性。

```python
from .QuModLibs.Server import *

entity = Entity(entityId)

print(entity.Health.Value)
print(entity.Health.Max)
entity.Health.Value = 20

print(entity.Pos)
print(entity.FootPos)
entity.Pos = (0, 64, 0)

print(entity.Rot)
entity.Rot = (10, -30)

print(entity.Identifier)
print(entity.Dm)
print(entity.DirFromRot)
```
