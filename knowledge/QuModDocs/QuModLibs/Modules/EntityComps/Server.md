# 服务端组件类扩展 <Badge type="tip" text="^1.3.0" />
适用于服务端组件类的扩展功能。

::: tip 更新入口建议
服务端组件推荐优先使用 `onGameTick()` 编写稳定业务逻辑。`update()` 会经过 `getNeedUpdate()` 判定，当前实现会参考 `GetServerTickTime()` 去重，实际频率受网易 ModSDK / 底层 API 行为影响，不建议把它假定为固定逻辑帧。
:::

## QEntityAttrData 实体数据类
为了解决生物内存周期下持久的储存数据片段，引入了该管理类。
```python
from .QuModLibs.Modules.EntityComps.Server import QEntityAttrData
data = QEntityAttrData("dataKey", needRestore=False, defaultValue=None)
"""
    @needRestore -是否需要持久化存档
    @defaultValue - 数据默认值
"""
data.bindEntityId(entityId) # 实体数据对象需要绑定到一个实体身上才能操作管理 和组件类一样允许rebuild
data.setValue(1024)         # 设置数据
data.getValue()
```
::: tip 数据隔离
`QEntityAttrData`管理的实体资源数据会根据MOD名称建立隔离，多个MOD之间互不干涉。
:::
在服务端的QBaseEntityComp中该功能默认被封装包含，可以直接使用：
```python
from .QuModLibs.Modules.EntityComps.Server import QBaseEntityComp

class TestComp(QBaseEntityComp):
    def __init__(self):
        QBaseEntityComp.__init__(self)
        # 创建共享数据(跨组件共享) 注意:跨组件共享数据getValue请勿使用缓存 确保一致性
        self.data1 = self.createSharedAttr(attrName="default", defaultValue=0, needRestore=False)
        # 创建私有数据(当前类型组件独占)
        self.data2 = self.createPrivateAttr(attrName="default2", defaultValue=None, needRestore=False)
    
    def onGameTick(self):
        QBaseEntityComp.onGameTick(self)
        # 记录Tick
        self.data1.setValue(self.data1.getValue(False) + 1)
        self.data2.setValue(self.data2.getValue() + 1)
```
::: tip 注意事项
在init下使用createSharedAttr/createPrivateAttr定义数据管理对象时，会在onBind期间自动绑定对应的生物进行管理，因此这是合法的。
:::

## QConstraintNodeComp 约束型组件类 <Badge type="warning" text="实验性" />
约束型组件类提供了父子集绑定机制，用于建立复杂的状态机组件集。
```python
from .QuModLibs.Modules.EntityComps.Server import QConstraintNodeComp

class RootAI(QConstraintNodeComp):
    """ 根组件节点 """
    def __init__(self):
        QConstraintNodeComp.__init__(self)
        self.baseCon = BaseCon()
        self.attackCon = AttackCon()
        self.hurtCon = HurtCon()

    def onBind(self):
        QConstraintNodeComp.onBind(self)
        # addConstraintNode 用于添加并绑定一个组件到自己身上(可以绑定多个)
        self.addConstraintNode(self.baseCon)

class BaseCon(QConstraintNodeComp):
    """ 常规控制器 """
    def __init__(self):
        QConstraintNodeComp.__init__(self)

    def onHurt(self):
        # 受伤处理逻辑 可自行绑定事件监听在合适的时候调用
        parent = self.getParent()   # type: RootAI
        # transferConstraint方法用于解除并切换绑定到另外一个节点
        self.transferConstraint(parent.hurtCon)

class HurtCon(QConstraintNodeComp):
    """ 受伤管理 """
    def __init__(self):
        QConstraintNodeComp.__init__(self)
    
    def onBind(self):
        QConstraintNodeComp.onBind(self)
        # 受伤持续1s后回到BaseCon状态
        self.addTimer(QConstraintNodeComp.Timer(self.toDefault, time=1.0))
    
    def toDefault(self):
        parent = self.getParent()   # type: RootAI
        self.transferConstraint(parent.baseCon)

class AttackCon(QConstraintNodeComp):
    """ 战斗管理 """
    def __init__(self):
        QConstraintNodeComp.__init__(self)
        # 这里就不演示了
    ...
```
