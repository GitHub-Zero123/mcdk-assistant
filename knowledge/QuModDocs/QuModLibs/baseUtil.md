# Util 基础工具包
QuMod中通用的工具模块，适用于服务端/客户端。

## TRY_EXEC_FUN 尝试执行
用于带有异常捕获性的尝试执行，当发生错误将完整的输出错误日志。
```python
from .QuModLibs.Util import TRY_EXEC_FUN

def errFunc():
    # 一个异常函数 0不能作为除数
    return 1 / 0

TRY_EXEC_FUN(errFunc)   # 执行并捕获异常输出到控制台 不会影响下文执行
print("OK")
```

## getObjectPathName 获取包路径名称 <Badge type="tip" text="^1.3.0" />
通常用于函数/类，获取在整个包中的绝对路径。
```python
# MyMod/Client.py
from .QuModLibs.Util import getObjectPathName

def myFunc():
    pass

print(getObjectPathName(myFunc))    # 将输出: MyMod.Client.myFunc
```

## @QThrottle 调用间隔限制 <Badge type="tip" text="^1.3.0" />
修饰一个函数最小调用间隔时间，期间重复调用将会被忽略。
```python
from .QuModLibs.Util import QThrottle

@QThrottle(0.2)
def myFunc():
    print("被调用了")

for _ in range(1000):
    myFunc()
```

## Vec3 向量类型
为了方便向量运算，在双系统端侧都默认包含该类型。
```python
from .QuModLibs.Server import *

# 将实体位置向上移动1格
Entity(entityId).Pos = Entity(entityId).Vec3Pos.addVec(
    Vec3(0, 1, 0)
)

# 获取实体左右向量
axis = Vec3(0, 1, 0)                        # 旋转轴
f = Entity(entityId).Vec3DirFromRot         # 正方向
l = f.copy().rotateVector(axis, -90)        # 左方向
r = f.copy().rotateVector(axis, 90)         # 右方向
```
::: tip 注意事项
向量类的所有操作都是对自我修改的，并且返回self链式访问，若需创建副本需要调用copy方法。
:::

## QBox3D 三维包围盒类型 <Badge type="tip" text="^1.3.1" />
为了方便碰撞检测，在双系统端侧都默认包含该类型。
```python
from .QuModLibs.Server import *
box1 = Entity(entityId_1).getBox3D()
box2 = Entity(entityId_2).getBox3D()

print(box1.overlapsAABB(box2))  # 检测两个实体是否碰撞 相较于使用pos坐标计算 碰撞检测对大体积生物支持更加友好

# 同时 QBox3D 也可以用与索敌计算
def box3DEntityMatch(playerId, boxData=QBox3D.createNullBox3D(), useBodyRot=True, useBodyPos=True):
    """ 3D_BOX 目标匹配 """
    pObj = Entity(playerId)
    if useBodyRot:
        boxData.setRot(pObj.Rot[1], Vec3(0, 1, 0))
    if useBodyPos:
        boxData.setCenterPos(pObj.Vec3Pos)
    for entityId in compFactory.CreateGame(playerId).GetEntitiesAround(playerId, int(round(boxData.xyzMaxLength(), 2)), {}):
        if entityId == playerId:
            continue
        if boxData.overlapsSAT(Entity(entityId).getBox3D()):
            yield entityId

# 实现 大圣蓄力的索敌 (x轴宽3.2, y轴上下各5.5, z轴前后各5.0)
for entityId in box3DEntityMatch(playerId, QBox3D(Vec3(3.2, 11.0, 10), Vec3(0, 0, 0))):
    print(entityId)
```

## QStruct 结构模型 <Badge type="tip" text="^1.3.1" />
结构模型用于解决Call通信中自定义数据类型的传输亦或者持久化保存对象。
```python
# Common.py 通用文件(不应该涉及任何API)
from .QuModLibs.Util import QStruct

class People(QStruct):
    # 定义一个数据模型
    def __init__(self, name="", age=18):   # 注: QStruct不需要调用父级初始化
        # type: (str, int) -> None
        self.name = name
        self.age = age

# ----------------------------------
# Client.py 客户端文件
from .QuModLibs.Client import *
from Common import People

Call("Test", People("张三", 18))

# ----------------------------------
# Server.py 服务端文件
from .QuModLibs.Server import *
from Common import People

@AllowCall
def Test(peo=People()):
    print(peo.name)
    print(peo.age)

```
::: tip 注意事项
QStruct在通过网络发包时将会序列化保存特征，并在接收端解析，如果涉及的结构类所在文件涉及异端API将会发生异常，因此QStruct应定义在一个无关的共享模块中。
:::
容器的异常处理
```python
# Common.py
from .QuModLibs.Util import QStruct

class People(QStruct):
    def __init__(self, name="", age=18):
        self.name = name
        self.age = age

# ----------------------------------
# Client.py
from .QuModLibs.Client import *
from Common import People

dataList = [
    People("张三", 18),
    People("李四", 18)
]

# 根级带有容器嵌套时将发生异常
Call("Test", dataList)

# 解决方法 使用包装类型(其他容器同理)
from .QuModLibs.Util import QRefStruct, QListStruct
Call("Test", QListStruct(dataList))

# 或者使用万能引用(最通用)
Call("Test", QRefStruct(dataList))

# PS: 不管是容器包装类型还是万能引用 在传输后都会被自动解包
```
::: warning 容器嵌套异常
出于性能考虑，Call并不会深层次分析对象是否为QStruct类型，因此当根级是容器时应使用对应的容器结构类型包装或使用万能引用以便识别处理。
:::

## 运行时线程分析 <Badge type="tip" text="^1.3.1" /> <Badge type="warning" text="实验性" />
通过初始化环境的线程ID计算，获取运行时所在线程。
```python
# modMain.py
from .QuModLibs.QuMod import *
myMod = EasyMod()
...

START_THREAD_ANALYSIS() # 需显性声明分析线程 将在加载器内部计算线程ID
```
在服务端/客户端调用时查询所在环境(适用于通用模块的不同系统端分发)：
```python
# Server / Client
from .QuModLibs.IN import IS_SERVER_THREAD, IS_CLIENT_THREAD, GET_THREAD_TYPE

def SERVER_FUNC():
    # 服务端功能
    if IS_SERVER_THREAD():
        import QuModLibs.Server as sAPI     # 受Python内部缓存机制的影响 局部import并不会重复加载
        sAPI.serverApi.xxx().doSomeThing()
        return
    raise RuntimeError("该函数仅适用于服务端线程")

def CLIENT_FUNC():
    # 客户端功能
    if IS_CLIENT_THREAD():
        import QuModLibs.Client as cAPI
        cAPI.clientApi.xxx().doSomeThing()
        return
    raise RuntimeError("该函数仅适用于客户端线程")

def ADD_TIMER(delay=0.5, func=lambda: None):
    # 通用功能 为不同的环境实现不同的业务逻辑
    threadType = GET_THREAD_TYPE()
    if threadType == 0:
        # 当处于服务端时
        import QuModLibs.Server as sAPI
        comp = sAPI.compFactory.CreateGame(sAPI.levelId)
        return comp.AddTimer(delay, func)
    elif threadType == 1:
        # 当处于客户端时
        import QuModLibs.Client as cAPI
        comp = cAPI.compFactory.CreateGame(cAPI.levelId)
        return comp.AddTimer(delay, func)
    return None
```
