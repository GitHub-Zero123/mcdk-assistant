# 通用服务功能

Service 提供一种面向对象的服务管理方式，类似于原生 `SystemCls`，但更轻量。它是可选的管理层抽象，不是 QuMod 业务开发的默认组织方式。

::: warning 默认从模块级组织开始
大多数业务直接依赖 Python 模块即可：模块天然单例，配合静态 `@Listen("EventName")`、`@AllowCall`、普通函数和模块级状态，通常已经足够清晰。即便需要 class，也往往只是普通业务类、数据类或小型管理类，不一定需要 `BaseService`。
:::

默认写法可以保持为模块级监听与模块级状态：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Server import *

_playerCache = {}

@Listen("AddServerPlayerEvent")
def onAddPlayer(args={}):
    playerId = args["id"]
    _playerCache[playerId] = {}

@Listen("DelServerPlayerEvent")
def onDelPlayer(args={}):
    playerId = args["id"]
    _playerCache.pop(playerId, None)
```

::: tip 启发提示
当逻辑只是“监听事件后执行一段业务”，优先写成模块级函数。只有当您已经明确需要可启动/停止的长期管理对象、统一回收注解/定时器，或需要服务内的本地事件广播时，再回来看 `BaseService`。
:::

## BaseService 服务类
如果普通模块或普通 class 已不足以表达管理关系，可以基于 `BaseService` 创建服务：
```python
# -*- coding: utf-8 -*-
from .QuModLibs.Server import *
from .QuModLibs.Modules.Services.Server import BaseService

@BaseService.Init
class MyService(BaseService):
    def __init__(self):
        BaseService.__init__(self)
        print("服务实例化")
    
    def onCreate(self):
        # 实例化过程正常走完没有遇到报错时将会触发该方法
        BaseService.onCreate(self)
        print("服务创建成功")
    
    def onServiceStop(self):
        # 游戏关闭时会强制停用所有服务
        BaseService.onServiceStop(self)
        print("服务停用触发")

    def onServiceUpdate(self):
        # 随服务管理器 Tick 回调触发，具体节奏取决于端侧 OnScriptTick 事件
        BaseService.onServiceUpdate(self)
    
    def call(self):
        print("执行逻辑")
        
	# 使用默认提供的注解注册游戏监听 您也可以自定义注解
    @BaseService.Listen("ServerItemTryUseEvent")
    def tryUseItem(self, args={}):
        # 服务类继承了定时器加载器 因此此处可以使用内部定时器(在服务关闭时将会跟随销毁)
        self.addTimer(BaseService.Timer(self.call, time=2.0))
        print(serverApi.GetPlayerList())

    @BaseService.LoopTimer(1.5)
    def onTick(self):
        print("tick")
    
    @BaseService.REG_API("MyService/MY_API")
    def MY_API(self):
        print("该方法将允许其他服务调用 通过self.syncRequest(...)")

```
::: tip 关于服务
服务是一种单例的管理模式，同一个服务类至多存在一个实例，通过类方法 **start** 启用 **stop**关闭或是使用装饰器`Init`初始化持久工作
此外服务提供了`注解加载器`功能方便用户开发。
:::

基于`AutoStopService`创建定时关闭服务:
```python
# -*- coding: utf-8 -*-
from .QuModLibs.Server import *
from .QuModLibs.Modules.Services.Server import AutoStopService

class TEST_SERVICE(AutoStopService):
    def __init__(self):
        AutoStopService.__init__(self)
        self.STOP_SERVICE_MAX_TIME = 60.0	# 规定60s未通过access访问自动关闭
        print("服务实例化")
    
    def onCreate(self):
        AutoStopService.onCreate(self)
        print("服务创建成功")
    
    def onServiceStop(self):
        AutoStopService.onServiceStop(self)
        print("服务停用触发")
    
    def onServiceAutoStop(self):
        AutoStopService.onServiceAutoStop(self)
        print("服务因长时间无访问自动关闭触发 随后触发onServiceStop")

    def onServiceUpdate(self):
        # 随服务管理器 Tick 回调触发，具体节奏取决于端侧 OnScriptTick 事件
        AutoStopService.onServiceUpdate(self)
    
    def doSomeThing(self):
        """ 实现一些功能 利用定时关闭特性可以对内缓存数据 """
        pass

@Listen("xxxx")
def TEST_FUN(_={}):
    # 在一定条件下使用服务业务功能
    TEST_SERVICE.access().doSomeThing()		# access类方法 未启用服务时将会启用服务并返回实例
```

基于`BaseEvent`定义您的服务事件

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Server import *
from .QuModLibs.Modules.Services.Server import BaseService, BaseEvent

class C_T_RAP_AND_BASKETBALL(BaseEvent):
    """ 定义一个事件 """
    def __init__(self, playerId):
        BaseEvent.__init__(self)
        self.playerId = playerId
        self.stop_rap = False	# 是否停止rap 接收方可以修改以便发起方读取结果

@BaseService.Init
class MyService(BaseService):
    """ 服务1 发起事件广播 """
    def __init__(self):
        BaseService.__init__(self)
        
    @BaseService.Listen("ServerItemTryUseEvent")
    def ServerItemTryUseEvent(self, args={}):
		playerId = args["playerId"]
        eventData = C_T_RAP_AND_BASKETBALL(playerId)	# 创建事件对象
        self.broadcast(eventData)						# 广播事件 (仅支持服务访问)
        if eventData.stop_rap:
            print("{} 取消了RAP".format(playerId))
        else:
            # doSomeThing
            ...

@BaseService.Init
class Service2(BaseService):
    """ 服务2 接受事件广播 """
    def __init__(self):
        BaseService.__init__(self)
        
    @BaseService.ServiceListen(C_T_RAP_AND_BASKETBALL)
    def C_T_RAP_AND_BASKETBALL_EVENT(self, data):
        # type: (C_T_RAP_AND_BASKETBALL) -None
        data = C_T_RAP_AND_BASKETBALL.getData(data)		# 显性转换类型(不是必要的) 仅辅助补全提示
        # 事件内置的上下文临时属性 通常情况下可能用不上
        # 上下文属性为临时结果 请勿尝试储存结果
        index = data._T_CONTEXT_INDEX			# 返回你是第几个收到事件通知的
        fromService = data._T_FROM_SERVICE		# 返回广播此事件的服务是谁
        if data.playerId == "-114514":
            data.stop_rap = True	# 你不许跳
        
```
::: tip 自定义服务事件
服务事件仅适用于当前系统环境下的本地广播，不会跨MOD/系统端侧，如需通信请使用Call，如需设计开放性API(MOD通信)请使用动态导入并制定一份API文件。
:::

## BaseBusiness 子业务类
`BaseBusiness` 提供服务内多个运行时实例的解决方案。它适合“一个服务管理多个玩家/实体/任务对象”的场景；如果业务对象不需要跟随服务生命周期统一启动和停止，普通 class 往往更简单。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Server import *
from .QuModLibs.Modules.Services.Server import BaseService, BaseBusiness

class PlayerRuntimerBusiness(BaseBusiness):
    """ 玩家运行时业务 """
    def __init__(self, playerId):
        BaseBusiness.__init__(self)
        self.playerId = playerId

    def onCreate(self):
        """ 业务构建成功触发 """
        parent = self.getParentService() # 获取父级服务对象
        # parent对象 请勿随意储存避免互相引用导致垃圾回收困难(即便Python会试图使用分代回收 在一定的时机) 遵循用时再获取即可

    def onStopBefore(self):
        """ 业务终止之前触发 """
        pass

    def onStop(self):
        """ 业务终止触发 业务终止之后将无法再获取parent对象 """
        pass
    
    def onTick(self):
        BaseBusiness.onTick(self)
        print("玩家ID: {}".format(self.playerId))

	@BaseBusiness.Listen("__ANY_EVENT__")
    def AnyEvent(self, args={}):
        """ 同服务一样 子业务也能使用注解管理资源 并跟随生命周期一同释放 """
        pass

@BaseService.Init
class MyService(BaseService):
    def __init__(self):
        BaseService.__init__(self)
        self.playerBusinessMap = {}		# 定义字典以便快速查表优化性能
        # 尽管服务内具有业务储存管理 由于储存方式并非使用map 想要高性能查询仍需自己定制化
        print("服务实例化")
    
    def onCreate(self):
        BaseService.onCreate(self)
        print("服务创建成功")
    
    def onServiceStop(self):
        BaseService.onServiceStop(self)
        print("服务停用触发")

    def onServiceUpdate(self):
        BaseService.onServiceUpdate(self)
    
    @BaseService.Listen("AddServerPlayerEvent")
    def AddServerPlayerEvent(self, args={}):
        """ 系统级 玩家加入游戏事件 """
        playerId = args["id"]
        # 对内添加一个玩家运行时业务 此业务享有相关生命周期事件
        businessObj = PlayerRuntimerBusiness(playerId)
        self.addBusiness(businessObj)
        self.playerBusinessMap[playerId] = businessObj
        
	@BaseService.Listen("DelServerPlayerEvent")
	def DelServerPlayerEvent(self, args={}):
        """ 系统级 玩家离开游戏事件 """
        playerId = args["id"]
        if playerId in self.playerBusinessMap:
            self.removeBusiness(self.playerBusinessMap[playerId])
            del self.playerBusinessMap[playerId]

```
::: tip 注意事项
子业务运行在服务之下，当服务被关闭时所有子业务都将终止，通过这一机制您可以方便的管理对内资源。
:::
