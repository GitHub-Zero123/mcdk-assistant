# 基本客户端 API

基础客户端模块会导出常用客户端 API、事件监听、跨端通信工具以及若干 QuMod 便捷封装。通常在 `Client.py` 中直接导入：

```python
from .QuModLibs.Client import *
```

::: tip 事件名写法
监听游戏事件时，推荐直接使用字符串事件名，例如 `@Listen("ClientItemTryUseEvent")`。旧式 `Events.xxx` 写法需要维护类属性映射，后续文档不再推荐。
:::

## IsServerUser

`IsServerUser` 用于标记当前客户端玩家是否为房主玩家。它适合单机或房主客户端专属逻辑，例如只允许房主执行某些客户端侧操作。

```python
# Client.py
from .QuModLibs.Client import *

@Listen("ClientItemTryUseEvent")
def ClientItemTryUseEvent(args):
    if not IsServerUser:
        return

    itemDict = args["itemDict"]
    itemName = itemDict["itemName"]
    if itemName == "minecraft:diamond_sword":
        # 房主客户端专属逻辑
        ...
```

::: warning 网络游戏须知
在联机大厅或服务器类网络游戏中，不存在“房主客户端”这一稳定概念，此时 `IsServerUser` 通常为 `False`。权威业务逻辑仍应放在服务端。
:::

## Request

`Request` 是带回调的远程请求封装。相比 `Call` 的单向调用，`Request` 更适合需要服务端返回结果的客户端交互。

```python
# Client.py
from .QuModLibs.Client import *

def onResponse(data):
    print("返回数据结果: {}".format(data))

Request("ServerFunc", ("张三",), {}, onResponse)
```

参数说明：

- `Key`：远程可执行对象名称。
- `args`：位置参数元组。单个参数请写成 `("张三",)`。
- `kwargs`：关键字参数字典。
- `onResponse`：收到响应后的回调函数。

::: tip 元组参数
Python 中 `("张三")` 只是字符串表达式，不是元组；单元素元组必须写成 `("张三",)`。
:::

## CallOTClient

`CallOTClient` 用于让当前客户端请求服务端转发调用到其他玩家客户端。

```python
# Client.py
from .QuModLibs.Client import *

CallOTClient(playerId, "ClientFuncName", "hello")
```

常见用途：

- 向指定玩家客户端同步表现层事件。
- 触发其他玩家客户端上的已授权函数。
- 在多人房间中做轻量客户端间协作。

::: warning 安全边界
`CallOTClient` 仍然需要经过服务端转发。不要把关键游戏判定只放在客户端，涉及奖励、伤害、背包、存档等权威逻辑时，应由服务端校验和执行。
:::
