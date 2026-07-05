# 异步组件模块 <Badge type="tip" text="^1.4.0" />
通用的异步组件模块 用于处理异步相关的业务逻辑。
```python
from .QuModLibs.Modules.Promise.Server import *
# from .QuModLibs.Modules.Promise.Client import *
```

## Promise 对象
Promise的框架下可以实现任意异步组件。
```python
# 通过以下案例了解 sleepAsync 组件的实现原理
def sleepAsync(duration):
    """
        1. 构造Promise对象需要传入一个执行器(executor)
        2. Promise会给执行器resolve函数和reject函数作为参数
        3. 调用resolve时Promise状态为成功, reject则为失败
        4. 当resolve和reject任意一个函数被调用时这个Promise结束, 并结束所属函数等待
        5. resolve和reject都接受一个参数作为Promise的返回值, 默认为None
    """
    # 执行器在Promise被yield返回或者使用then和catch时开始执行 
    # [后续讲解非阻塞的Promise] 会使用到then和catch
    def executor(resolve, reject):
        # 这个执行器会构造一个计时器, 计时器使用sleep的duration参数计时
        comp = compFactory.CreateGame(levelId)
        # 计时结束调用Promise的resolve, Promise成功
        # 计时是个简单操作不会有失败的情况, 也不需要Promise返回参数, 直接调用即可
        comp.AddTimer(duration, resolve)
    return Promise(executor) # 所有的异步组件的返回值类型都必须是Promise
```

## 自定义异步组件
一个简单的角色技能示例。
```python
# 技能描述 - 开启技能后3s内被怪物攻击就杀死怪物, 如果没有技能结束, 类似反弹
def Bounce(playerId):
    def callBack(args):
        if args["entityId"] == playerId:
            # 如果是玩家则返回True, 结束事件等待
            return True

    # 执行器也可以使用asyncRunner装饰器使用异步组件
    @asyncRunner
    def executor(resolve, reject):
        # 如果超过最大等待时间, 结束事件等待, 并将Promise状态设为失败
        state, value = yield waitEvent("ActuallyHurtServerEvent", callBack, 3.0)
        # 如果event的状态为True则调用resolve并传入攻击者的id否则调用reject
        resolve(value["srcId"]) if state else reject()

    return Promise(executor)
```
应用自定义异步组件：
```python
@asyncRunner
def playerUseSkill(playerId):
    # 此处Player类为另行封装
    player = Player(playerId)
    # 如果正在使用技能则直接return不执行后续代码
    if player.isUsing:
        return
    # 将正在使用技能的状态设置为True
    # 将玩家大小设置为1.5方便知道技能是否开启
    player.isUsing = True
    player.scale(1.5)
    state, entityId = yield Bounce(playerId) # 等待技能结果
    # entityId为Bounce的resolve的传入值
    player.skill = False
    player.scale(1.0)
    # 如果Bounce的Promise状态为成功销毁攻击者
    DestroyEntity(entityId) if state else None
```
