# QuickLobbyManager <Badge type="tip" text="^1.3.8" />
快速便捷的联机大厅管理方案。
```python
# 包含所有依赖
from .QuModLibs.Modules.MultiplayerLobby.Server import *
```

## 基础功能
使用`QuickLobbyManager`创建快捷管理器。
```python
from .QuModLibs.Modules.MultiplayerLobby.Server import *

if serverApi.GetPlatform() != -1:
    # 自动检测 启用DEBUG模式 可以在单机环境下模拟调试(也可以用AUTO_DEBUG_MODE函数处理)
    BaseLobbyManager.LOCAL_DEBUG_MODE = True
    # BaseLobbyManager.LOCAL_DEBUG_SAVE_LEVEL = False

@QuickLobbyManager.Init
class MyManager(QuickLobbyManager):
    def initMemoryMap(self):
        # 分配初始化玩家数据 以便后续缓存映射 (所有需要映射操作的数据都应该在此处声明)
        return {
            "money": 100,
            "money2": 0,
        }

    def onPlayerMmpUpdate(self, playerId):
        QuickLobbyManager.onPlayerMmpUpdate(self, playerId)
        playerData = self.getPlayerAllMMPData(playerId)
        print("玩家数据更新: {} -> {}".format(playerId, playerData))
        # 若有需要 可在此事件下同步玩家数据到客户端(当mmp数据改变时触发该事件)
        Call(playerId, "syncSerData", playerData)
```

### 键值对数据操作
在特定场景下处理用户数据：
```python
class MyManager(QuickLobbyManager):
    ...

    # 当玩家攻击实体时 获得1货币
    @QuickLobbyManager.Listen("PlayerAttackEntityEvent")
    def PlayerAttackEntityEvent(self, args={}):
        playerId = args["playerId"]
        # Quick管理器会将后台数据分配在服务端内存空间上映射操作 并由内部周期性同步确保性能优化
        # 因此需要使用getMemoryValue / setMemoryValue系列方法操作数据
        comp = self.getPlayerComp(playerId)
        nowMoney = comp.getMemoryValue("money")
        comp.setMemoryValue("money", nowMoney + 1)

        # 除了手动获取组件实例 也允许直接使用函数过程接口操作
        # nowMoney = self.getPlayerMMPData(playerId, "money")
        # self.setPlayerMMPData(playerId, "money", nowMoney+1)
```
::: tip 关于QuickLobbyManager
内存数据操作 <-> 智能后台同步，像属性修改一样简单便捷。
:::

基本组件操作接口：
```python
# 获取特定玩家绑定的管理组件
comp = self.getPlayerComp(playerId)  # 服务类外部操作时可以直接QuickLobbyPlayerComp.getComp(playerId)
# 获取映射数据
comp.getMemoryValue("key", None)
# 设置映射数据
comp.setMemoryValue("key", 100)
# 改变映射数据(lambda运算)
comp.changeMemoryValue("key", lambda v: v+1)
# 显性声明需要同步特定key值
comp.asyncMemoryKey("key")
# 显性声明需要同步所有key值
comp.asyncAllMemoryDatas()
```

基本过程式封装接口(基于组件接口)：
```python
# 过程式封装接口可以直接在服务类中操作玩家数据
# 获取与设置数据
self.getPlayerMMPData(playerId, "key")
self.setPlayerMMPData(playerId, "key", 100)
self.changePlayerMMPData(playerId, "key", lambda v: v+100)

# 显性声明同步特定key
self.updatePlayerMMPKey(playerId, "key")
```
::: danger 注意事项
由于内部映射缓存机制 若直接操作原对象再set则不会触发更新响应 推荐原对象操作+显性声明同步一并使用
:::

### 全局数据查询 <Badge type="tip" text="^1.3.10" />
适用于全局后台数据只读操作：
```python
class MyManager(QuickLobbyManager):
    def initGlobalMemoryMap(self):
        # 与玩家数据管理一样 需要分配一个初始映射表以便托管处理
        # 特殊: 当处于DEBUG本地调试模式时 将始终返回默认值(可自行判断模拟)
        return {
            "notice": [],   # 公告数据
            "activity": {}  # 活动数据
        }

def getNotice():
    # 使用getGlobalDataMap获取同步的只读数据
    globalData = MyManager.access().getGlobalDataMap()
    return globalData["notice"] # 注: 如果在后台同步完毕之前就获取会直接返回默认值，因此建议实时查询而非提前缓存
```

### 常用事件功能
通过重写特定方法，您可以轻易的完成事件监听。
```python
class MyManager(QuickLobbyManager):
    # 常用事件
    def onPlayerMmpUpdate(self, playerId):
        """ 用户数据更新触发 可在此处编写客户端同步逻辑(玩家初始化加入游戏也会触发一次) """
        QuickLobbyManager.onPlayerMmpUpdate(self, playerId)
        Call(playerId, "syncSerData", self.getPlayerAllMMPData(playerId))

    def onPlayerAutomatedOrderProc(self, playerId, info=OrderINFO()):
        """ 用户购买商品触发 此处可处理商品交易业务 若无任何异常抛出 默认将会自动[发货] """
        QuickLobbyManager.onPlayerAutomatedOrderProc(self, playerId, info)
        if info.cmd == "BUY1":
            # 金币+100
            self.changePlayerMMPData(playerId, "money2", lambda v: v+100)

    # 不那么重要的事件
    def onPlayerEnterGameSession(self, playerId):
        """ 玩家加入大厅游戏完毕时触发 """
        pass

    def onLobbyStorePurchase(self, playerId):
        """ 玩家在局内购买物品触发 """
        pass

    def onLobbyEnvInit(self):
        """ 联机大厅环境初始化时触发(至少一个玩家加入时) """
        pass
```

## 组件绑定
所有的服务管理器都提供了[模板特化]功能允许绑定参数并创建一个新类。
```python
# 对于复杂的业务需求 使用面向对象的组件将更利于维护
class MyPlayerComp(QuickLobbyPlayerComp):
    pass

@QuickLobbyManager.Init
class MyManager(QuickLobbyManager[MyPlayerComp]):
    # 使用 类[绑定] 语句创建一个特化类 将使用特定组件类作为玩家管理组件
    pass
```

### 编写组件管理
```python
class MyPlayerComp(QuickLobbyPlayerComp):
    def onMmpUpdate(self):
        """ mmp更新事件 对应服务类onMmpUpdateonPlayerMmpUpdate """
        QuickLobbyPlayerComp.onMmpUpdate(self)

    def update(self):
        """ update事件 每秒20次 """
        # 组件在绑定到玩家时就会立即进入tick循环状态 此时并不一定完成初始化远程数据同步 因此建议使用userUpdate
        QuickLobbyPlayerComp.update(self)

    def userUpdate(self):
        """ 安全的更新逻辑 每秒20次 确保在初始化数据之后才会进行 """
        QuickLobbyPlayerComp.userUpdate(self)

    def onFirstDataUpdate(self):
        """ 远程后台初始化拉取数据事件 (可作为业务逻辑入口) """
        QuickLobbyPlayerComp.onFirstDataUpdate(self)

    def onAutomatedOrderProc(self, orderIn=OrderINFO()):
        """ 自动订单处理时触发 """
        QuickLobbyPlayerComp.onAutomatedOrderProc(orderIn)

    def onLobbyStorePurchase(self):
        """ 玩家在局内购买物品触发 """
        QuickLobbyPlayerComp.onLobbyStorePurchase(self)

    def onUserDataUpdate(self):
        """ 用户数据更新时触发(网络数据响应) """
        # 与onMmpUpdate的区别是 它是内部真实同步数据的响应事件 而非映射操作的更新
        QuickLobbyPlayerComp.onUserDataUpdate(self)
```

## 本地测试
联机大厅服务基类提供了本地测试的模拟环境：
```python
# 本地测试模式 将直接模拟远程请求参数
# 启用 LOCAL_DEBUG_MODE 后 代码无需任何修改即可支持本地调试
BaseLobbyManager.LOCAL_DEBUG_MODE = True

# 其他调试选项
BaseLobbyManager.LOCAL_DEBUG_SAVE_LEVEL = False          # 调试模式下是否保存数据到存档
BaseLobbyManager.LOCAL_DEBUG_FORCE_SIMULATION_UID = 0    # 若非0 调试模式下将强制模拟该UID
BaseLobbyManager.LOCAL_DEBUG_GLOBAL_DATAS = {            # 调试模式下的全局数据模拟(高版本引入)
    "notice": ["欢迎使用联机大厅服务!"],
    "activity": {}
}
```

### 商品订单模拟
```python
# 模拟创建特定商品订单给特定玩家ID 该方法仅在DEBUG环境下生效 正式环境不会执行任何逻辑
QuickLobbyManager.DEBUG_CREATE_PLAYER_ORDER(
    playerId, "BUY1"
)
```

## 线上测试
使用`NeLobbyAutoKit.exe`自动化自测工具（可在仓库/Tools工具中获取）。

### 配置参数
**TARGET_CONFIG.json** 用于声明目标参数：
```json
// 该文件放置在exe同级以便读取解析
{
    // 可在开发者平台F12调试模式->网络 随便点击一个MOD查看可以捕获到me字头的请求 其中包含你的账号cookie
    "cookie": "...",
    "modId": "...",                 // MOD_ID或联机大厅项目ID
    "targetPath": "D:/xxx/xxxx",    // 项目路径
    // 打包忽略文件 若当前工具放置在项目中 请确保忽略自身
    "ignore": [
        "*.exe",
        "TARGET_CONFIG.json",
        "*/TARGET_CONFIG.json",
        "*/.mcs/*",
        "*/.git/*",
        "*/.gitignore",
        "*/studio.json",
        "*/work.mcscfg"
    ]
}
```

### 目录问题
联机大厅的压缩文件结构中需要额外套一个`顶级文件夹`，因此`targetPath`需要注意填写正确的路径，否则自测将不通过（退一级目录即可）。

### 运行
在满足配置条件后双击即可启动，将自动压缩打包项目并发布自测，若出现异常/安全执行完毕将弹出Toast提示窗。 <Badge type="warning" text="win10+" />

::: danger 注意事项
自动化工具打包可能会忽略空文件夹 请确保entities/textures不为空。
:::
