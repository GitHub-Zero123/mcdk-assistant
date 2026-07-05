# QBaseEntityComp <Badge type="tip" text="^1.3.0" />
通用的实体组件功能 适用于服务端/客户端。
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp
# from .QuModLibs.Modules.EntityComps.Client import QBaseEntityComp
```

## 基本构造
`QBaseEntityComp`在设计上遵循先构造后绑定原则。
```python
from .QuModLibs.Server import *
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp

class MyComp(QBaseEntityComp):
    """ 自定义组件 一定时间后杀死生物 """
    def __init__(self, deathTime=2.0):
        QBaseEntityComp.__init__(self)  # 请确保父类方法被正确调用以便初始化功能
        self.deathTime = deathTime
    
    def onBind(self):
        """ 生物绑定触发 """
        QBaseEntityComp.onBind(self)
        print("成功绑定生物 {}".format(self.entityId))  # 组件在绑定后即可拿到 entityId
        self.addTimer(QBaseEntityComp.Timer(self.kill, time=self.deathTime))         # 同理 组件类也继承了的定时器有自己的生命周期(随着组件绑定/解除绑定开始/销毁)
    
    def kill(self):
        # 组件在绑定后也能拿到entityObj对象 以便快速操作
        self.entityObj.Kill()

    def onUnBind(self):
        """ 生物解除绑定触发 """
        QBaseEntityComp.onUnBind(self)
        print("组件解除绑定")
    
    def onGameTick(self):
        """ 推荐：稳定逻辑更新入口 """
        QBaseEntityComp.onGameTick(self)
        # 建议将组件的主要逻辑写在 onGameTick 中。
        pass

    def update(self):
        """ 兼容入口：不建议作为主要逻辑帧 """
        QBaseEntityComp.update(self)
        # update 会受到底层 API / getNeedUpdate 判定影响，不保证在所有场景下都是固定逻辑帧频率。
        pass

# 在玩家攻击生物时施加组件
@Listen("PlayerAttackEntityEvent")
def PlayerAttackEntityEvent(args={}):
    comp = MyComp(3.0)  # 组件在不绑定生物时 什么功能都不会有 就像一个常规对象
    comp.bind(args["victimId"]) # 绑定成功后 组件进入管理模式 持续进行tick更新
```

## 组件生命周期
了解组件的生命周期有助于开发理解。
```
组件无绑定: 跟随Python垃圾回收器释放 (不作讨论)；
组件有绑定: 跟随生物内存状态更新。

onGameTick/update: 生物处于当前渲染区块内时更新(通过JSON组件设置持久化更新同样生效)。
onUnBind: 生物被系统回收(不在内存状态中,但不一定被杀死)/脚本主动解绑/游戏关闭。
(onUnBind中可以通过方法getUnBindINFO拿到解绑原因)

关于生物内存状态的特殊情况(以下内容基于网易接口数据讨论 不代表真实的游戏底层)
处于主世界的生物: 在内存不足/一些其他条件因素(如区块距离玩家特远)释放；
处于非主世界的生物: 当前维度中不存在任何玩家时一定会被释放；
相关事件对应AddEntityServerEvent, EntityRemoveEvent。

客户端差异:
对于客户端而言，离开自己的渲染区块就会释放，同理重新进入渲染视野又会触发增加。

玩家实体:
在服务端无论如何都不会触发自动释放(始终渲染) 除非游戏关闭。
```
```text
补充说明(2025/09/20)
基岩版的区块卸载与JE不同，并不会因为远离就立即卸载，而是会先冻结一段时间，具体释放机制未知，
因此实体被冻结在区块中哪怕此时区块理应被卸载也会有一段时间能够被操作（这才有了判定实体内存状态的必要性）。
```

## 多例组件模式
默认的情况下，同一个组件类在单个生物上至多持有一个实例，通过权限声明可以启用多例模式。
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp, QEntityCompFlags
# QEntityCompFlags 包含了所有可以设置的声明

@QBaseEntityComp.setFlags(QEntityCompFlags.ALLOW_DUPLICATES)    # 允许重复持有特定类型的组件
class MyComp(QBaseEntityComp):
    # 此时 该组件将允许重复构造实例添加在单个生物身上
    ...
```

## 渲染状态忽略
默认的情况下，组件仅在生物渲染时触发更新方法，通过权限声明可以改变这一机制(通常来说不会这么做，更多的是在JSON中设置)。
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp, QEntityCompFlags

# 通过位运算可以同时使用多个声明
@QBaseEntityComp.setFlags(QEntityCompFlags.ALLOW_DUPLICATES | QEntityCompFlags.IGNORE_RENDERING_STATUS)
class MyComp(QBaseEntityComp):
    # 此时 该组件将允许重复构造实例添加在单个生物身上 并且忽略渲染状态(但不会忽略内存状态 被系统释放后依然会掉组件)
    ...
```

## 按类型绑定生物
在了解组件生命周期后，可知无论如何生物都有可能被系统暂时回收，在重新回到加载时会触发对应的事件，对于持久化组件绑定特定生物的需求
可以使用内置的`regEntity`装饰器完成(暂不支持玩家)。
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp, QEntityCompFlags

@QBaseEntityComp.regEntity("minecraft:zombie", "minecraft:husk", ...)
@QBaseEntityComp.setFlags(QEntityCompFlags.IGNORE_RENDERING_STATUS)
class MyComp(QBaseEntityComp):
    ...
```

## 获取实体持有组件
组件类提供了多种实体持有组件获取方法，可根据需求选择使用。
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp

class MyComp(QBaseEntityComp):
    pass

MyComp.getComp(entityId)    # 基于实体id获取单个组件 失败/无组件则返回None

MyComp.getSubComp(entityId)    # 基于实体id获取单个组件 相较于getComp, getSubComp支持带有继承关系的子类组件获取

for comp in MyComp.getComps(entityId):
    # 获取实体身上的所有该组件实例(浅拷贝) 适用于读写操作
    pass

for comp in MyComp.getCompsGen(entityId):
    # [生成器过程时] 获取实体身上的所有该组件实例 适用于只读操作
    pass

for comp in MyComp.getSubComps(entityId):
    # 同理 获取全部带有继承关系的组件实例(浅拷贝)
    pass

for comp in MyComp.subCompsGen(entityId):
    # [生成器过程时] 获取全部带有继承关系的组件实例
    pass

# 一种特殊的获取 当不存在任何此类型的组件时将抛出一个万能对象而不是None 万能对象的一切操作都将合法
MyComp.getTempSubComp(entityId)
```

## 主动销毁实体组件
组件类也提供了多种组件销毁方法(严格来说是解绑)。
```python
comp = MyComp.getComp(entityId)
if comp:
    # 调用实例方法解除绑定
    comp.unBind()

# 注: 以下方法均不支持子类组件销毁 需自行搭配获取与unBind方法实现
MyComp.removeComp(entityId) # 使用类方法而不是实例方法解除组件绑定(该方法会自动判断存在与否)
MyComp.removeAllComps(entityId) # 删除所有特定类型的组件(多例模式)
```

## rebind 复用组件
组件的绑定机制使得单个组件实例能够被复用绑定到任意生物身上。
```python
comp = MyComp()         # 构造单个组件复用

comp.rebind(entityId1)  # 绑定到实体1
comp.rebind(entityId2)  # 从实体1解除绑定并绑定到实体2
```

## ECS高阶用法
仅演示设计思路：
```python
# 角色扮演类
class 通用变身(QBaseEntityComp):
    def onBind(self):
        构造技能UI()    # 可搭配UI管理器确保切换维度也能自动恢复

    def onUnBind(self):
        销毁技能UI()

    def defaultAttact(self):
        # 由用户UI输入调用 普通攻击逻辑
        pass

class 广智(通用变身):
    def onBind(self):
        super(...).onBind()
        # 实现资源处理 修改外观 动画

    def onUnBind(self):
        super(...).onUnBind()
        # 实现资源恢复

    def defaultAttact(self):
        # 实现普通攻击
        pass

class 石猴(通用变身):
    def onBind(self):
        super(...).onBind()
        # 实现资源处理 修改外观 动画

    def onUnBind(self):
        super(...).onUnBind()
        # 实现资源恢复

    def defaultAttact(self):
        # 实现普通攻击
        pass
```

::: tip 更新入口建议
组件内部每轮更新会先触发 `onGameTick()`，随后才根据 `getNeedUpdate()` 判断是否触发 `update()`。因此推荐把稳定业务逻辑写在 `onGameTick()` 中；`update()` 更适合兼容旧代码或确实需要跟随底层引擎去重策略的场景。
:::
