# GLRender 资源管理扩展
玩家资源渲染同步管理解决方案。

## 启用扩展
### modMain注册
创建EasyMod 模组开发的初始化入口，并启用include属性中的GLRender项。

```python
# modMain.py
from .QuModLibs.QuMod import *

testMod = EasyMod()
testMod.include.glRender = True	# 启用 GLRender

```

## 静态注册节点 (初始化同步)
通过类加载的方式静态注册全局节点 (推荐)。

```python
# modMain.py
from .QuModLibs.QuMod import *
from .QuModLibs.Include.GL_Render.SharedRes import loadQueryFromClass

testMod = EasyMod()
testMod.include.gl_render = True

# 推荐您在开发期间定义一个全局配置文件 存放该class以便后续的setQuery直接访问
class CustomQuery:
    QUERY1 = "query.mod.xxx"
    QUERY2 = "query.mod.xxx"

loadQueryFromClass(CustomQuery)		# loadQueryFromClass 将通过反射获取到类中的所有静态属性

```
## 动态注册节点 (运行时同步)
GLR允许您在运行时动态设置不存在的节点(将会自动注册)。

```python
# Server.py
from .QuModLibs.Server import *
from .QuModLibs.Include.GL_Render.Server import GL_Service

@Listen("ServerItemTryUseEvent")
def ServerItemTryUseEvent(args={}):
    """ 尝试使用物品事件 """
    playerId = args["playerId"]
    # 通过玩家ID拿到资源操作对象
    playerRes = GL_Service.getPlayerRes(playerId)
    # 当节点不存在时将会自动注册节点
    playerRes.setQuery("query.mod.xxx", 2024.0)
```

## 玩家资源操作对象
通过 **getPlayerRes** 方法拿到操作对象后可进行一系列操作：
```python
# Server.py
from .QuModLibs.Server import *
from .QuModLibs.Include.GL_Render.Server import GL_Service, GL_RES_OPT

playerId = "..."
playerRes = GL_Service.getPlayerRes(playerId)						# 拿到玩家资源操作对象

# ========== 节点操作 ==========
playerRes.setQuery("query.mod.xxx", 100.0)
playerRes.setLineQuery("query.mod.line_query", 10.0, 0.5)			# 线性插值节点到 10.0 (0.5s)
playerRes.getQuery("query.mod.xxx")									# 获取节点值 仅支持GLR设置的节点

# ========== 脚本层动画播放 ==========
playerRes.playAnim("animation.my_mod_default.test", blendTime=0.5)	# 播放动画 并设置入场过渡时间
playerRes.stopAnim("animation.my_mod_default.test", blendTime=0.5)	# 停止播放动画 并设置退场过渡时间
playerRes.playCMDAnim("animation.test")		# 通过CMD命令播放瞬时动画(CMD动画可能受资源重建影响被刷新)
playerRes.initPyAnims(["animation.xxx", "animation.xxx"]) # 批量初始化需要动态播放的动画 避免运行时渲染重建

# ========== 资源操作处理 ==========
playerRes.addOptPass(GL_RES_OPT.GL_GEOMETRY("default", "xxx"))
playerRes.addOptPass(GL_RES_OPT.GL_TEXTURE("default", "xxx"))
playerRes.addOptPass(...)		# 更多资源操作请查阅 GL_RES_OPT (部分资源可以通过设置为None实现删除资源)
playerRes.restDefaultSkin()		# 重置基本皮肤资源 (在操作default后的复原)
playerRes.restAnim("animKey")	# 刷新动画资源请求 (使其回归到时间线0.0) 不会重建渲染
```

## GL_RES_OPT 资源操作标识

应用于 **addOptPass** 的资源设置支持。

```python
# Server.py
from .QuModLibs.Server import *
from .QuModLibs.Include.GL_Render.Server import GL_Service, GL_RES_OPT

GL_RES_OPT.GL_MATERIAL				# 材质资源
GL_RES_OPT.GL_ANIM					# 动画资源(包含控制器支持)
GL_RES_OPT.GL_GEOMETRY				# 模型资源
GL_RES_OPT.GL_PARTICLE_EFFECT		# 粒子资源
GL_RES_OPT.GL_RENDER_CONTROLLER		# 渲染控制器资源
GL_RES_OPT.GL_PLAYER_SKIN			# 玩家皮肤资源
GL_RES_OPT.GL_TEXTURE				# 纹理资源
GL_RES_OPT.GL_SCRIPT_ANIMATE		# anim加载资源
GL_RES_OPT.GL_SOUND_EFFECT			# 音效资源

```

## 加载JSON渲染资源
基于JSON表加载、释放资源。

```python
# Server.py
from .QuModLibs.Server import *
from .QuModLibs.Include.GL_Render.Server import GL_Service, GL_RES_OPT, JSONRenderData

playerId = "..."
playerRes = GL_Service.getPlayerRes(playerId)

# 直接创建JSONRenderData对象
renderData = JSONRenderData(
    materials = {
        "xxx": "xxx"
	},
    geometry = {
        "xxx": "xxx"
	},
    animations = {
        "xxx": "xxx"
	},
    render_controllers = [
        "xxx",
        {
            "xxx": "query.life_time 1145.0"
        }
    ]
)

# 或是使用解包语句直接加载JSON内容 (使用两个星号解包展开dict)
# renderData = JSONRenderData(**{"materials": {...}})

playerRes.loadJSONRenderData(renderData)	# 加载渲染对象合集

# 在不需要使用时可以直接释放相关资源
playerRes.releaseJSONRenderData(renderData)
# PS: 受API限制默认提供的释放处理不一定满足实际项目需求 仅供参考
# 如动画加载资源无法API释放 若释放了相关引用的动画将可能报错 部分场景可以定义一个空动画资源置换而不是删除

```
