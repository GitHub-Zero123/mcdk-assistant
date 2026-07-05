# UI 基本功能扩展

UI 功能扩展模块用于对象化管理界面控件、动态绘制、网格渲染与分页等场景。

::: tip 推荐路径
在支持 RAII 的界面类（如 `ScreenNodeWrapper` / `QEScreenNode`）中，现代化网格渲染推荐使用 `QGridBinder` + `QGridData`。旧式手动监听、`QUIAutoCanvas` 与 `QUIAutoControlFuntion` 的 Tick 检测策略仅建议用于兼容旧项目。
:::

## QGridData

### 网格对象参数

`QGridData` 保存网格渲染所需的路径、回调与索引解析配置。它本身不负责监听生命周期，通常搭配 `QGridBinder`、`QUICanvas.listenQGridRender` 或旧式 `QGridAdapter` 使用。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Modules.UI.Client import QGridData

gridDataObj = QGridData(
    "/panel/grid",
    isScrollGrid=False,
    bindFunc=lambda viewPath, index: None,
    incrementalCallback=lambda viewPath, index: None,
    bindUpdateBeforeFunc=lambda: None,
    bindUpdateFinishFunc=lambda: None,
    bindGridConName="",
)

```

- `path`：Grid 路径；`isScrollGrid=True` 时通常填写 ScrollView 路径。
- `isScrollGrid`：声明为滚动网格，内部会通过 ScrollView 获取实际内容路径。
- `bindFunc`：渲染回调，类型为 `(viewPath: str, index: int) -> None`。发起更新时会遍历当前已渲染控件并调用。
- `incrementalCallback`：增量渲染回调，参数同 `bindFunc`，只在某个格子首次进入当前渲染集合时触发。
- `bindUpdateBeforeFunc`：一轮更新前触发。
- `bindUpdateFinishFunc`：一轮更新结束后触发，可用于延迟刷新或批量收尾。
- `bindGridConName`：格子控件名。涉及路径末尾数字解析，控件名本身带数字结尾时建议显式设置。

### 手动实现网格管理

基于原版游戏事件 `GridComponentSizeChangedClientEvent` 可以手动完成对接。大多数业务不需要这样做；该方式适合自定义生命周期、非标准 UI 节点或调试底层网格事件时使用。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Client import *
from .QuModLibs.Modules.UI.Client import QGridData

gridDataObj = QGridData("/panel/grid", ...)	# 网格对象参数

# 监听网格变化事件
@Listen("GridComponentSizeChangedClientEvent")
def GridComponentSizeChangedClientEvent(args={}):
    path = str(args["path"])
    uiNode = ...	# 提供一个uiNode作上下文 通过getRealComponentPath方法计算刷新判定
    if path.endswith(gridDataObj.getRealComponentPath(uiNode)):
        # 当符合条件时调用updateRender触发绑定的bindFunc完成网格处理关联
        gridDataObj.updateRender(uiNode)
# 为了方便动态的使用 通常需要 手动 ListenForEvent / UnListenForEvent 管理 此处仅作演示

```

`QUICanvas` 已提供 `listenQGridRender` / `unListenQGridRender`，`QGridBinder` 又在此基础上接入了 RAII 生命周期。除非确实要控制底层监听，否则优先使用封装后的绑定器。

## QUICanvas
QUI画布绘制类，提供控件逻辑的封装复用功能，同时充当一个协议。

### 构建一个基本Canvas

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QUICanvas

# ======== 自定义Canvas ========
class MyCanvas(QUICanvas):
    def __init__(self, uiNode, parentPath):
        QUICanvas.__init__(self, uiNode, parentPath)
        self.drawDefName = "namespace.controlName"	# drawDefName 属性用于关联绑定一个JSON自定义控件

    def onCreate(self):
        QUICanvas.onCreate(self)	# 父类的onCreate方法实现了self.drawDefName控件的创建也可以不使用默认的机制
        print("成功绘制, 路径为: " + self._conPath)
        # self.getBaseUIControl() 也可以通过该方法直接拿到控件对象

    def onDestroy(self):
        QUICanvas.onDestroy(self)
        print("外部调用removeControl时触发 通常做后置回收处理 大多数情况下不需要 在一些涉及事件监听的需求中用到")

# ======== 自定义UI应用自定义Canvas ========
@ScreenNodeWrapper.autoRegister("xxx.main")
class MyUI(ScreenNodeWrapper):
    def __init__(self, *args):
        ScreenNodeWrapper.__init__(self, *args)
        self._myCanvas = MyCanvas(self, "/panel")	# 绑定到当前UI的特定父节点

    def Create(self):
        ScreenNodeWrapper.Create(self)
        self._myCanvas.createControl()	# 创建Canvas

	def Destroy(self):
        ScreenNodeWrapper.Destroy(self)
        # removeControl(justOnDestroy = False)		# justOnDestroy默认False 将会执行控件删除逻辑
        self._myCanvas.removeControl(True)			# UI界面销毁时请置为True(不执行控件删除将跟随UI一起销毁)

```
```text
Q: 我是否一定要调用removeControl完成回收工作 ?
答: 并不一定 倘若此Canvas并不涉及需要回收处理的逻辑且不需要动态删除则可以忽略调用 (为防歧义最好统一调用)
```

### 进阶版Canvas
在Canvas中操作uiNode，并注册网格监听事件，完成渲染处理。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QUICanvas

# ======== 自定义Canvas ========
class MyCanvas(QUICanvas):
    def __init__(self, uiNode, parentPath):
        QUICanvas.__init__(self, uiNode, parentPath)
        self.drawDefName = "namespace.controlName"
        self._QGridData = None	# type: QGridData | None
        self.dataList = []		# 绑定数据列表

    def renderUpdate(self, viewPath, i):
        # type: (str, int) -> None
        # 异步渲染加载触发 其中i代表下标 viewPath返回控件路径
        data = self.dataList[i]
        
    def onCreate(self):
        self.clearParent()		  # 该方法用于清理parent中所有的控件 以便独占位置
        QUICanvas.onCreate(self)
        uiNode = self.getUiNode() # 为防止内存泄漏 父节点的引用储存采用弱引用机制 该方法用于解包弱引用
        
        # 调用uiNode的方法 实现一个带有网格列表渲染的Canvas
        self._QGridData = QGridData(self._conPath, True, bindFunc=self.renderUpdate)
        # 设置网格渲染数目
        uiNode.GetBaseUIControl(
            self._QGridData.getRealPath(uiNode)
        ).asGrid().SetGridDimension((1, len(self.dataList)))
        
        # 使用内置的实现方法监听网格渲染
        self.listenQGridRender(self._QGridData)

    def onDestroy(self):
        QUICanvas.onDestroy(self)
        # 使用内置的实现方法回收监听网格渲染
        self.unListenQGridRender(self._QGridData)
```

## QUIControlFuntion
QUI画布绘制类的派生类，相比QUICanvas，该class更多是处理已有控件逻辑，为其赋予特定功能而不是构建新的控件。

### 基本了解
```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QUIControlFuntion

class MyControlFuntion(QUIControlFuntion):
    def onCreate(self):
        QUIControlFuntion.onCreate(self)	# QUIControlFuntion并不会为您创建新的控件
        uiNode = self.getUiNode()
        parentPath = self._parentPath
        # 为parentPath编写业务
        ...
```

## [智能对象] QUIAutoCanvas <Badge type="danger" text="已过时" />

`QUIAutoCanvas` 是 `QUICanvas` 的派生类，提供基于 Tick 的存活检测与自动回收机制。该策略属于旧式 Auto Tick 管理方式，现代项目不再推荐使用。

::: danger 过时说明
Auto 系列对象通过 Tick 检测 UI 节点是否仍存活，因此释放时机可能滞后一帧或多帧，并会引入额外的 Tick 管理成本。新项目请优先使用 RAII 策略；网格场景推荐 `QGridBinder`，动态画布场景推荐 `QRAIICanvas` / `QECanvas`。
:::

### 基本了解
```python
# -*- coding: utf-8 -*-
from .QuModLibs.Modules.UI.Client import QUIAutoCanvas
class MyCanvas(QUIAutoCanvas):
    def __init__(self, uiNode, parentPath):
        QUIAutoCanvas.__init__(self, uiNode, parentPath)
        self.drawDefName = "namespace.controlName"
        self._QGridData = None	# type: QGridData | None
        self.dataList = []		# 绑定数据列表

    def renderUpdate(self, viewPath, i):
        # type: (str, int) -> None
        # 异步渲染加载触发 其中i代表下标 viewPath返回控件路径
        data = self.dataList[i]
        
    def onCreate(self):
        self.clearParent()
        QUIAutoCanvas.onCreate(self)
        uiNode = self.getUiNode()
        self._QGridData = QGridData(self._conPath, True, bindFunc=self.renderUpdate)
        uiNode.GetBaseUIControl(
            self._QGridData.getRealPath(uiNode)
        ).asGrid().SetGridDimension((1, len(self.dataList)))
        
        # 使用内置的实现方法监听网格渲染
        self.listenQGridRender(self._QGridData)

    def onDestroy(self):
        QUIAutoCanvas.onDestroy(self)
        self.unListenQGridRender(self._QGridData)

# 旧式 Auto Tick 示例：仅建议维护旧项目时参考。
```

## [智能对象] QUIAutoControlFuntion <Badge type="danger" text="已过时" />

`QUIAutoControlFuntion` 是 `QUIControlFuntion` 的派生类，提供基于 Tick 的存活检测与自动回收机制。该类属于旧式 Auto Tick 管理方式，现代项目不再推荐使用。

### 基本了解

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QUIAutoControlFuntion

class TextTimer(QUIAutoControlFuntion):
    """ 文本记时器 """	# 旧式 Auto Tick 示例：仅建议维护旧项目时参考。
    def __init__(self, uiNode, parentPath):
        QUIAutoControlFuntion.__init__(self, uiNode, parentPath)
        self._tick = 0
    
    def onTick(self):
        QUIAutoControlFuntion.onTick(self)
        self._tick += 1
        self.getBaseUIControl().asLabel().SetText(str(round(self._tick / 30.0, 1)))

@ScreenNodeWrapper.autoRegister("testUI3.main")
class TEST_UI3(ScreenNodeWrapper):
    def __init__(self, *args):
        ScreenNodeWrapper.__init__(self, *args)
        # 使用此功能 为特定文本控件赋予计时器机制
        self.textTimer = TextTimer(self, "/panel/image/text")
    
    def Create(self):
        ScreenNodeWrapper.Create(self)
        self.textTimer.createControl()
```

::: danger 过时说明
智能对象基于 Tick 检测 UI 节点存活性，因此释放并不一定及时。新项目请优先使用 RAII 策略；网格场景推荐 `QGridBinder`，普通控件托管推荐 `QRAIIControlFuntion` / `QEControlFuntion`。
:::

## [RAII] QGridBinder <Badge type="tip" text="现代推荐" />

网格数据绑定器，用于自动管理 Grid / ScrollView + Grid 的渲染监听。它是当前现代化推荐的网格管理入口，适合替代旧式 `QGridAdapter`、手动事件监听与 Auto Tick 网格管理方案。

### 代码演示

```python
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("testUI3.main")
class TEST_UI4(ScreenNodeWrapper):
    def __init__(self, *args):
        ScreenNodeWrapper.__init__(self, *args)
        self.dataList = []
        # ScreenNodeWrapper 支持 RAII，上下文创建和销毁时会自动启停绑定器。
        self.gridBinder = QGridBinder(
            self,
            QGridData("/panel/scroll_view", True, bindFunc=self.renderView),
        )

    def Create(self):
        ScreenNodeWrapper.Create(self)
        self.gridBinder.setGridDimension((1, len(self.dataList)))

    def renderView(self, viewPath="", index=0):
        if index >= len(self.dataList):
            return
        data = self.dataList[index]
        ...
```

### 常用方法

- `setGridData(gridData)`：绑定 `QGridData`，返回自身。
- `setGridDimension(size)`：调整 Grid 元素数量，内部调用原生 `SetGridDimension`。
- `updateRender()`：主动刷新当前已渲染控件，并触发 `bindFunc`。
- `updateOnceRender(viewPath, index=None)`：只刷新单个控件路径。
- `clearIncrementalCache()`：清理增量渲染缓存。
- `start()` / `stop()`：在不支持 RAII 的 UI 节点中手动启停绑定器。
