# 基于QuMod加载器创建混合项目
通过`QuModLibs.QuMod`提供的API可以轻松的完成混合管理。

## 原版注册
### 简易系统注册
```python
# modMain.py
from .QuModLibs.QuMod import *

myQuMod = EasyMod()
myQuMod.Server("...")
myQuMod.Client("...")

# 服务端与客户端的封装系统类注册
SERVER_REG_NATIVE_PY_SYSTEM(命名空间, 系统名, 路径) # 相当于 serverApi.RegisterSystem(...)
CLIENT_REG_NATIVE_PY_SYSTEM(命名空间, 系统名, 路径) # 相当于 clientApi.RegisterSystem(...)

# 特殊的封装版本 路径会自动绑定当前目录(相对路径)
myQuMod.regNativePyServer(...)
myQuMod.regNativePyClient(...)
```

### 复杂业务处理
```python
# modMain.py
from .QuModLibs.QuMod import *

myQuMod = EasyMod()
myQuMod.Server("...")
myQuMod.Client("...")

# 若SERVER/CLIENT_REG_NATIVE_PY_SYSTEM不能满足需求
SERVER_REG_NATIVE_PY_SYSTEM(...)
CLIENT_REG_NATIVE_PY_SYSTEM(...)

# 可以通过注册一个初始化函数来解决问题
@REG_SERVER_INIT_CALL
def SERVER_INIT():
    serverApi.RegisterSystem(...)
    serverApi.RegisterComponent(...)
    ...

@REG_CLIENT_INIT_CALL
def CLIENT_INIT():
    clientApi.RegisterSystem(...)
    clientApi.RegisterComponent(...)
    ...

```

## 互相调用
不管是原版业务还是QuMod亦或者其他框架，都运行在同一个线程环境下。
### NeServer.py
```python
import mod.server.extraServerApi as serverApi
from QMServer import TEST_FUNC, TEST_COMP    # 所有的QuMod模块都可以直接import访问
ServerSystem = serverApi.GetServerSystemCls()

class MyServer(ServerSystem):
    def __init__(self, namespace, systemName):
        ServerSystem.__init__(self, namespace, systemName)
        mcNamespace, mcSystemName = serverApi.GetEngineNamespace(), serverApi.GetEngineSystemName()
        self.ListenForEvent(mcNamespace, mcSystemName, "PlayerAttackEntityEvent", self, self.PlayerAttackEntityEvent)

    def PlayerAttackEntityEvent(self, args={}):
        playerId = args["playerId"]
        victimId = args["victimId"]
        TEST_FUNC(playerId)         # 调用QMServer.py中的TEST_FUNC
        TEST_COMP().bind(victimId)  # 为命中实体绑定QuMod组件 实现持续造成伤害
```
### QMServer.py
```python
from .QuModLibs.Server import *
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp

def TEST_FUNC(entityId):
    print("这是个空函数: {}".format(entityId))

class TEST_COMP(QBaseEntityComp):
    def onBind(self):
        QBaseEntityComp.onBind(self)
        # 0.5s后自动解除组件
        self.addTimer(QBaseEntityComp.Timer(lambda: self.unBind(), time=0.5))
    
    def update(self):
        QBaseEntityComp.update(self)
        # 持续造成伤害
        comp = compFactory.CreateHurt(self.entityId)
        comp.Hurt(1, serverApi.GetMinecraftEnum().ActorDamageCause.EntityAttack, None, None, False)
```
### QuMod调用NeServer
```python
from .QuModLibs.Server import *

def TestFunc():
    # 对于单例系统方法 需要Get后再访问
    systemNode = serverApi.GetSystem(...)
    systemNode.xxx()

def TestFunc2():
    # 对于定义出来的函数/非系统类 可以直接import
    from NeServer import xxx
    xxx()
```

## 混合编写
```python
from .QuModLibs.Server import *
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp
ServerSystem = serverApi.GetServerSystemCls()

# 由于和QuMod相关业务放在同一个文件 只需要注册一次System即可加载整个文件的其他业务
class MyServer(ServerSystem):
    def __init__(self, namespace, systemName):
        ServerSystem.__init__(self, namespace, systemName)
        mcNamespace, mcSystemName = serverApi.GetEngineNamespace(), serverApi.GetEngineSystemName()
        self.ListenForEvent(mcNamespace, mcSystemName, "PlayerAttackEntityEvent", self, self.PlayerAttackEntityEvent)

    def PlayerAttackEntityEvent(self, args={}):
        playerId = args["playerId"]
        victimId = args["victimId"]
        TEST_COMP().bind(victimId)  # 为命中实体绑定QuMod组件 实现持续造成伤害

class TEST_COMP(QBaseEntityComp):
    def onBind(self):
        QBaseEntityComp.onBind(self)
        # 0.5s后自动解除组件
        self.addTimer(QBaseEntityComp.Timer(lambda: self.unBind(), time=0.5))
    
    def update(self):
        QBaseEntityComp.update(self)
        # 持续造成伤害
        comp = compFactory.CreateHurt(self.entityId)
        comp.Hurt(1, serverApi.GetMinecraftEnum().ActorDamageCause.EntityAttack, None, None, False)
```
::: tip 总结
任何一个合法的QuMod模块都是一个合法的原版MOD模块，反之亦然。
:::
