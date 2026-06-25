# ModSDK AI 速查

本文是面向 AI Agent 的网易 Minecraft ModSDK 精炼开发约束。目标不是替代完整教程，而是在生成代码前快速对齐加载模型、端侧边界和常见坑。

适用范围：

- 入口加载器、`modMain.py`、`@Mod.Binding`、`RegisterSystem` 等规则主要适用于原版 ModSDK 加载器。
- 三方 Mod 框架例如 QuModLibs 可能封装了自己的 `modMain.py` 注册风格，应优先遵循对应框架的入口约定。
- Python 2 编码、代码风格、端侧边界、事件参数、跨端通信安全等约束适用于大多数网易 MC MOD 开发环境。

## 基本概念

- `BH` 表示行为包 `Behavior-Pack`。
- ModSDK 使用 Python 2.7.18，源码默认按 ASCII 解析；含中文时文件顶部必须写：

```python
# -*- coding: utf-8 -*-
```

- ModSDK/ModAPI 多为 C 接口封装，通常不是异常驱动设计；不要把 `try/except` 当常规兜底逻辑，优先按返回值、回调、事件参数和接口约定判断。
- 客户端线程和服务端线程共享同一个 VM 上下文，并非完全隔离；跨线程初始化模块时要避免意外执行对方端代码。
- 虽然运行时是 Python 2.7，但不建议到处写 `try: xrange` / `except NameError: range` 这类环境探测兼容层；它会让用户 IDE/编辑器的静态分析和提示变差。除非项目已有明确兼容策略，否则应优先遵循当前项目代码风格，有些项目会刻意按 Python 3 风格书写。
- `print` 默认建议写成 `print("message")`。在 Python 2 中，只要不是多参数输出，带括号写法会在字节码解析环节折叠为普通打印表达式，兼容性更稳；但如果用户项目已经统一使用 `print "message"` 语句式，应继续延续既有风格。

## 入口脚本

游戏运行时会扫描 BH 内的 `modMain.py` 并执行。BH 根目录等价于 ModSDK 模块解析的 import root，入口通常放在合法 Python 包目录下：

```text
BH/<python_package>/modMain.py
```

典型入口：

```python
# -*- coding: utf-8 -*-
from mod.common.mod import Mod
import mod.server.extraServerApi as serverApi
import mod.client.extraClientApi as clientApi

@Mod.Binding(name="my_mod", version="1.0.0")
class ModEntry(object):
    @Mod.InitServer()
    def serverInit(self):
        serverApi.RegisterSystem("my_mod", "server", "my_mod.Server.MyServerSystem")

    @Mod.InitClient()
    def clientInit(self):
        clientApi.RegisterSystem("my_mod", "client", "my_mod.Client.MyClientSystem")

    @Mod.DestroyServer()
    def serverDestroy(self):
        pass

    @Mod.DestroyClient()
    def clientDestroy(self):
        pass
```

要点：

- `@Mod.Binding` 标记的类会被加载器自动构造。
- 类名和方法名不重要，加载器识别的是 `@Mod.*` 装饰器。
- 多个 Mod 使用相同 `name` 时可能只加载版本更高者；通常应保证 `name` 唯一。
- `RegisterSystem(namespace, systemName, "package.module.Class")` 的第三个参数是 Python import 路径，不是文件系统路径。
- `package.module.Class` 从 BH 根目录开始解析，例如 `my_mod.Server.MyServerSystem` 对应 `BH/my_mod/Server.py` 中的 `class MyServerSystem`。
- 包名和模块名应使用合法 Python 标识符，避免中文、空格、连字符和数字开头。
- `modMain.py` 同时导入 `mod.server.extraServerApi` 和 `mod.client.extraClientApi` 是常见且安全的特例；这两个模块是端侧 API 的封装跳板，未调用具体接口时不会执行端侧业务，因此不属于跨线程初始化风险。

## 加载器反射模型

ModSDK 引擎侧加载流程大致如下：

1. 遍历 BH 下的 Python 包目录。
2. 按是否存在 `modMain.py` 判定入口包。
3. import 并执行目标 `modMain.py`。
4. 对 `modMain` 模块做 `dir` 反射，收集所有带 `@Mod.Binding` 标记的 class。
5. 对每个匹配 class 执行无参构造。
6. 继续反射实例方法，按端侧调用带 `@Mod.InitServer()`、`@Mod.InitClient()`、`@Mod.DestroyServer()`、`@Mod.DestroyClient()` 标记的方法。

约束：

- 一个 `modMain.py` 中可以存在多个 `@Mod.Binding` class，它们都会被加载器识别。
- 客户端线程和服务端线程会各自走一遍上述流程；通常同一个 Mod class 会被构造两次，而不是两端复用同一个对象。
- 不要把 `@Mod.Binding` class 的实例属性当作跨端共享状态；如果在 `__init__` 里记录属性，需要明确它只属于当前端侧本次构造出的实例。
- `@Mod.Binding(name=..., version=...)` 是早期设计产物，通常几乎不参与引擎侧计算；更主要的价值是人为阅读和区分，仍建议保持唯一、稳定、可读。

## 系统类

系统类是实际业务逻辑承载体。服务端和客户端分别继承各自端侧基类。

服务端：

```python
# -*- coding: utf-8 -*-
import mod.server.extraServerApi as serverApi
ServerSystem = serverApi.GetServerSystemCls()

class MyServerSystem(ServerSystem):
    def __init__(self, namespace, systemName):
        ServerSystem.__init__(self, namespace, systemName)
```

客户端：

```python
# -*- coding: utf-8 -*-
import mod.client.extraClientApi as clientApi
ClientSystem = clientApi.GetClientSystemCls()

class MyClientSystem(ClientSystem):
    def __init__(self, namespace, systemName):
        ClientSystem.__init__(self, namespace, systemName)
```

端侧选择：

- 服务端处理真实数据：血量、位置、属性、物理、掉落、规则校验。
- 客户端处理本地表现：渲染、GUI、输入、本地玩家视角。
- 单人房主同时加载服务端和客户端；联机房主也同时加载两端，其他玩家通常仅加载客户端；网络服服务端由网易服务器运行。

## 事件监听

监听原版事件时通常使用引擎命名空间和系统名：

```python
mcNamespace = serverApi.GetEngineNamespace()
mcSystemName = serverApi.GetEngineSystemName()
self.ListenForEvent(mcNamespace, mcSystemName, "PlayerAttackEntityEvent", self, self.onAttack)
```

回调示例：

```python
def onAttack(self, args={}):
    playerId = args["playerId"]
    victimId = args["victimId"]
```

要点：

- `ListenForEvent(namespace, systemName, eventName, self, callback)` 用于注册监听。
- 回调必须是系统实例上的方法。
- 游戏关闭时监听会自动取消，一般不需要手动回收。
- 事件参数以对应事件定义为准；不要凭事件名猜测 `args` 字段。
- 事件也可来自其他 Mod 的自定义广播，因此命名空间和系统名用于区分来源。

## 常用实体操作模式

运行时 `entityId` 是游戏对象引用，不等同于 JSON 标识符。操作实体通常依赖事件参数中的 `playerId`、`victimId` 等运行时 ID。

示例：玩家攻击僵尸时反伤玩家。

```python
def onAttack(self, args={}):
    playerId = args["playerId"]
    victimId = args["victimId"]

    typeComp = serverApi.GetEngineCompFactory().CreateEngineType(victimId)
    if typeComp.GetEngineTypeStr() == "minecraft:zombie":
        hurtComp = serverApi.GetEngineCompFactory().CreateHurt(playerId)
        hurtComp.Hurt(
            3,
            serverApi.GetMinecraftEnum().ActorDamageCause.EntityAttack,
            victimId,
            None,
            False
        )
```

部分接口需要 `levelId`：

```python
levelId = serverApi.GetLevelId()
```

## 跨端通信

常用接口：

- `NotifyToClient`：服务端向指定客户端发送事件。
- `NotifyToServer`：客户端向服务端发送事件。
- `NotifyToMultiClients`：服务端向多个客户端发送事件。
- `BroadcastToAllClient`：服务端向所有客户端广播事件。

服务端发送到客户端：

```python
self.NotifyToClient(playerId, "playerAttackEntity", {"targetId": victimId})
```

客户端监听服务端自定义事件：

```python
self.ListenForEvent("test", "ser1", "playerAttackEntity", self, self.onServerAttack)

def onServerAttack(self, args):
    targetId = args["targetId"]
```

客户端发送到服务端：

```python
self.NotifyToServer("playerJumpRelease", {"clPlayerId": playerId})
```

服务端监听客户端自定义事件：

```python
self.ListenForEvent("test", "cli1", "playerJumpRelease", self, self.onClientJump)
```

安全与性能：

- 网络型 Mod 不要相信客户端传来的关键数据，服务端必须二次校验。
- 跨端通信会立即发包，高频广播会明显影响性能，应控制频率。
- 自定义事件数据尽量使用基础 Python 类型。

## AI 生成代码检查清单

- 文件顶部是否有 `# -*- coding: utf-8 -*-`。
- 是否先识别当前项目使用原版 ModSDK 加载器还是 QuModLibs 等三方框架，并遵循对应入口注册风格。
- `modMain.py` 是否只负责绑定入口和注册系统，业务逻辑是否放到系统类文件。
- 服务端逻辑是否使用 `serverApi` 和 `ServerSystem`。
- 客户端逻辑是否使用 `clientApi` 和 `ClientSystem`。
- `RegisterSystem` 的类路径是否是从 BH 根目录解析的 Python import 路径，例如 `my_mod.Server.MyServerSystem` 对应 `BH/my_mod/Server.py`。
- 是否避免在模块 import 阶段执行跨端敏感逻辑。
- 是否按事件定义读取 `args`，而不是猜参数名。
- 是否区分运行时 `entityId` 和 JSON 标识符。
- 是否避免用 `try/except` 掩盖 ModAPI 调用设计问题。
- 是否避免无必要的 Python 2/3 环境探测兼容层，并遵循当前项目既有风格。
- `print` 写法是否优先使用 `print(...)`，或延续用户项目已有的语句式风格。
- 是否控制跨端通信频率，并对客户端输入做服务端校验。
