# System

## GetClientSystemCls

客户端

method in mod.client.extraClientApi

- 描述

  用于获取客户端system基类。实现新的system时，需要继承该接口返回的类

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| type(ClientSystem) | 客户端系统类 |

- 示例

```python
import mod.client.extraClientApi as clientApi
ClientSystem = clientApi.GetClientSystemCls()
class FpsClientSystem(ClientSystem):
    def __init__(self, namespace, systemName):
        ClientSystem.__init__(self, namespace, systemName)

```

## GetServerSystemCls

服务端

method in mod.server.extraServerApi

- 描述

  用于获取服务器system基类。实现新的system时，需要继承该接口返回的类

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| type(ServerSystem) | 服务端系统类 |

- 示例

```python
import mod.server.extraServerApi as serverApi
ServerSystem = serverApi.GetServerSystemCls()
class FpsServerSystem(ServerSystem):
    def __init__(self, namespace, systemName):
        ServerSystem.__init__(self, namespace, systemName)

```

## GetSystem

服务端客户端

### 服务端接口

method in mod.server.extraServerApi

- 描述

  获取已注册的系统

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| nameSpace | str | 命名空间，建议为mod名字 |
| systemName | str | 系统名称，自定义名称，可以使用英文、拼音和下划线，建议尽量个性化 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ServerSystem | 返回具体系统的实例 |

- 示例

```python
import mod.server.extraServerApi as serverApi
serverSystem = serverApi.GetSystem("TutorialMod", "TutorialServerSystem")

```

### 客户端接口

method in mod.client.extraClientApi

- 描述

  用于获取其他系统实例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| nameSpace | str | 系统注册的命名空间，一般为mod名字 |
| systemName | str | 要获取的系统名称 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ClientSystem | 返回具体系统的实例，如果获取不到则返回 None |

- 示例

```python
import mod.client.extraClientApi as clientApi
ClientSystem = clientApi.GetClientSystemCls()
class FpsClientSystem(ClientSystem):
    def __init__(self, namespace, systemName):
        ClientSystem.__init__(self, namespace, systemName)
        self.tutorialSystem = clientApi.GetSystem("TutorialMod", "TutorialClientSystem")

```

## RegisterSystem

服务端客户端

### 服务端接口

method in mod.server.extraServerApi

- 描述

  用于将系统注册到引擎中，引擎会创建一个该系统的实例，并在退出游戏时回收。系统可以执行我们引擎赋予的基本逻辑，例如监听事件、执行Tick函数、与客户端进行通讯等。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| nameSpace | str | 命名空间，建议为mod名字 |
| systemName | str | 系统名称，自定义名称，可以使用英文、拼音和下划线，建议尽量个性化 |
| clsPath | str | 组件类路径，路径从脚本的第一层开始算起 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ServerSystem | 返回具体系统的实例 |

- 示例

```python
import mod.server.extraServerApi as serverApi
# 系统system的注册是在modMain.py的MOD类中
# 服务端系统system的注册方式
@Mod.InitServer()
def TutorialServerInit(self):
    serverApi.RegisterSystem("TutorialMod", "TutorialServerSystem", "tutorialScripts.tutorialServerSystem.TutorialServerSystem")

```

### 客户端接口

method in mod.client.extraClientApi

- 描述

  用于将系统注册到引擎中，引擎会创建一个该系统的实例，并在退出游戏时回收。系统可以执行我们引擎赋予的基本逻辑，例如监听事件、执行Tick函数、与服务端进行通讯等。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| nameSpace | str | 命名空间，建议为mod名字 |
| systemName | str | 系统名称，自定义名称，可以使用英文、拼音和下划线，建议尽量个性化 |
| clsPath | str | 组件类路径，路径从脚本的第一层开始算起 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ClientSystem | 返回具体系统的实例 |

- 示例

```python
import mod.client.extraClientApi as clientApi
# 系统system的注册是在modMain.py的MOD类中
# 客户端系统system的注册方式
@Mod.InitClient()
def TutorialClientInit(self):
    clientApi.RegisterSystem("TutorialMod", "TutorialClientSystem", "tutorialScripts.tutorialClientSystem.TutorialClientSystem")

```
