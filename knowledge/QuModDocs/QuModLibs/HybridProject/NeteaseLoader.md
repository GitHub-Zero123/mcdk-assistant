# 基于原版加载器创建混合项目
任何一个QuMod模块都可以作为库被直接import加载。

## 原版注册
```python
# -*- coding: utf-8 -*-
from mod.common.mod import Mod
import mod.server.extraServerApi as serverApi
import mod.client.extraClientApi as clientApi

@Mod.Binding(name = "MOD_MAIN", version = "1.0.0")
class MOD_MAIN(object):
    @Mod.InitServer()
    def serverInit(self):
        serverApi.RegisterSystem(...)
        # 对于QuMod的模块 只需要通过import即可完成加载
        import QServer

    @Mod.InitClient()
    def clientInit(self):
        clientApi.RegisterSystem(...)
        # 注：并不一定要在初始化时import加载 需根据实际需求 在子系统文件中的import同样生效
        import QClient

    @Mod.DestroyServer()
    def serverDestroy(self):
        pass

    @Mod.DestroyClient()
    def clientDestroy(self):
        pass
```

## 互相调用 (同上一篇)
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
通过import可以直接加载并引用目标模块，同一个模块不会多次加载，是共享性的。
:::
