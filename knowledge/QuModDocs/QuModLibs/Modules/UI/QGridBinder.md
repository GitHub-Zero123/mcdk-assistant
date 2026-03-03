# QGridBinder 与 QGridData 网格绑定解决方案（UI网格开发）

> **⚠️ 重要提示：在使用 QuModLibs 框架进行网格（Grid）或列表网格（ScrollView+Grid）UI 开发时，必须阅读本文档。**
> QGridBinder + QGridData 是 QuModLibs 中处理所有网格/列表类 UI 的标准方案，覆盖背包、商店、排行榜、物品选择等绝大多数涉及动态列表的界面场景。
> **不要** 手动监听 `GridComponentSizeChangedClientEvent` 事件来实现网格渲染 — 请使用本模块提供的声明式 API。

QuModLibs 提供的现代化网格/列表网格开发解决方案，用于自动化管理界面中的 Grid（网格）和 ScrollView+Grid（列表网格/滚动网格）元素的渲染更新。

```python
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData
```

---

## 核心概念

在网易 MC JSON UI 中，`grid`（网格控件）的子项是引擎动态创建的，开发者无法在 JSON 中静态定义每个格子的内容。当网格尺寸改变（`SetGridDimension`）时，引擎会触发 `GridComponentSizeChangedClientEvent` 事件通知子项已创建/变化，开发者需要在此事件中遍历所有子项并更新其显示内容。

**QGridBinder + QGridData** 将上述流程封装为声明式 API：
- `QGridData` — 网格数据描述对象，定义网格路径、渲染回调等参数
- `QGridBinder` — 网格绑定器，自动监听事件、驱动渲染更新、管理生命周期

两者配合使用，开发者只需关注"如何渲染每个格子"，无需手动监听事件和管理路径。

### ⚠️ 异步池化渲染与复用机制

网格/列表网格的渲染是**异步池化**的，这意味着：

1. **格子控件会被引擎复用**：当列表滚动时，滑出屏幕的格子控件会被回收并重新分配给新进入屏幕的格子。同一个 `viewPath` 可能在不同时刻对应不同的 `index`。
2. **渲染时机不可预测**：引擎并不是在 `SetGridDimension` 后立即渲染所有格子，而是通过事件异步通知。格子的创建和销毁由引擎内部的池化机制决定。
3. **必须在回调中处理每个格子**：由于复用机制，**不能**在回调外部通过缓存的路径直接操作格子控件 — 路径对应的格子内容可能已经变化。所有格子的显示逻辑都**必须**在 `bindFunc` 回调中完成。

```python
# ❌ 错误用法 - 在回调外部保存路径并操作
savedPaths = {}
def onRender(viewPath, index):
    savedPaths[index] = viewPath  # 保存路径

def laterUpdate(index):
    # 危险！此路径对应的格子可能已被引擎复用给其他 index
    self.GetBaseUIControl(savedPaths[index] + "/label").asLabel().SetText("更新")

# ✅ 正确用法 - 所有操作在回调中完成
def onRender(viewPath, index):
    # 在回调中直接操作，此时 viewPath 和 index 的对应关系是确定的
    if index < len(self.dataList):
        self.GetBaseUIControl(viewPath + "/label").asLabel().SetText(self.dataList[index])
```

> **结论**：永远在 `bindFunc` / `incrementalCallback` 回调内操作格子控件，不要依赖缓存的路径在回调外部修改格子内容。如需更新特定格子，使用 `updateOnceRender` 或 `updateRender` 主动触发回调（用户滚动也会触发）。

### ⚠️ 格子可见性控制与等宽渲染

网格采用**等宽渲染机制**：`SetGridDimension((cols, rows))` 设置的是网格矩阵尺寸，引擎会按 `cols × rows` 创建所有格子，即使数据不足以填满。因此当数据量不是 `cols` 的整数倍时，末尾会出现多余的空格子，需要手动隐藏。

**关键限制**：在 `bindFunc` 回调触发期间，格子的根 `viewPath` 控件正在被引擎接管，**无法直接设置其可见性**。但可以通过操作其**内部子控件**的可见性来实现隐藏效果。

```python
def onRender(viewPath, index):
    # type: (str, int) -> None
    if index < len(self.dataList):
        # 有数据 - 显示内容面板
        # 注意：SetVisible 第二个参数必须设为 False，避免触发全局 UI 刷新（性能关键）
        self.GetBaseUIControl(viewPath + "/content_panel").SetVisible(True, False)
        self.GetBaseUIControl(viewPath + "/content_panel/label").asLabel().SetText(
            self.dataList[index]
        )
    else:
        # 无数据 - 隐藏内容面板（隐藏多余的空格子）
        # ⚠️ 不能对 viewPath 本身 SetVisible，只能对子控件操作
        # ⚠️ 第二个参数 False 非常重要：避免每个格子都触发一次全局 UpdateScreen 造成严重卡顿
        self.GetBaseUIControl(viewPath + "/content_panel").SetVisible(False, False)
```

> **性能要点**：`SetVisible(visible, param2)` 的第二个参数控制是否触发全局 UI 刷新。在网格回调中对每个格子操作时，**务必设为 `False`**，否则 N 个格子会触发 N 次全局刷新，造成严重性能问题。

### 完整案例：3 列网格 + 隐藏多余格子

假设有一个 3 列网格，当数据只有 4 项时，需要 2 行（`ceil(4/3) = 2`），第二行第 2、3 个格子需要隐藏。

```python
# -*- coding: utf-8 -*-
import math
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("myui.main")
class GridVisibilityUI(ScreenNodeWrapper):
    COLUMNS = 3  # 每行 3 列

    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        self.dataList = ["A", "B", "C", "D"]  # 4 项数据，不是 3 的倍数
        self.gridBinder = QGridBinder(self, QGridData(
            "/panel/grid",
            bindFunc=self.onRender
        ))

    def Create(self):
        ScreenNodeWrapper.Create(self)
        self.refreshGrid()

    def Destroy(self):
        ScreenNodeWrapper.Destroy(self)

    def refreshGrid(self):
        """刷新网格尺寸"""
        count = len(self.dataList)
        # 计算行数：向上取整
        # ⚠️ Py2 中 math.ceil 返回 float，必须转 int，否则 SetGridDimension 会报错
        rows = int(math.ceil(count / float(self.COLUMNS)))
        self.gridBinder.setGridDimension((self.COLUMNS, rows))
        # 网格实际会渲染 COLUMNS * rows = 3 * 2 = 6 个格子
        # 但数据只有 4 项，所以 index=4 和 index=5 的格子内容需要隐藏

    def onRender(self, viewPath, index):
        # type: (str, int) -> None
        """渲染回调"""
        contentPanel = self.GetBaseUIControl(viewPath + "/content")
        if index < len(self.dataList):
            # 有数据 - 显示内容
            contentPanel.SetVisible(True, False)  # 第二个参数 False 避免全局刷新
            self.GetBaseUIControl(viewPath + "/content/label").asLabel().SetText(
                self.dataList[index]
            )
        else:
            # 多余格子 - 隐藏内容（index=4, index=5 会走到这里）
            contentPanel.SetVisible(False, False)  # 第二个参数 False 非常重要
```

> **Py2 注意事项**：
> - `math.ceil()` 在 Python 2 中返回 `float`（如 `2.0`），而 `SetGridDimension` 需要 `int`，必须用 `int()` 转换
> - 整除计算使用 `count / float(self.COLUMNS)` 确保浮点除法（Py2 中两个 int 相除是整除）
> - 也可以用纯整数方案：`rows = (count + self.COLUMNS - 1) // self.COLUMNS`

---

## QGridData 数据模型

### 构造参数

```python
QGridData(
    path,                       # type: str         网格控件在 UI 中的路径
    isScrollGrid=False,         # type: bool        是否为滚动网格（ScrollView 嵌套 Grid）
    bindFunc=lambda *_: None,   # type: function    渲染回调 (viewPath: str, index: int) -> None
    incrementalCallback=lambda *_: None,  # type: function  增量渲染回调 (viewPath: str, index: int) -> None
    bindUpdateBeforeFunc=lambda *_: None, # type: function  每轮更新前置回调 () -> None
    bindUpdateFinishFunc=lambda *_: None, # type: function  每轮更新完成回调 () -> None
    bindGridConName="",         # type: str         显式声明网格格子控件名（可选）
)
```

### 参数详解

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `path` | `str` | ✅ | 网格控件路径。普通网格直接填 grid 路径；滚动网格填 ScrollView 路径 |
| `isScrollGrid` | `bool` | ❌ | 是否为列表网格渲染模式（ScrollView + Grid 组合） |
| `bindFunc` | `function(str, int)` | ✅ | **核心渲染函数**，每个格子每次更新都会调用，接收 `(viewPath, index)` |
| `incrementalCallback` | `function(str, int)` | ❌ | **增量渲染函数**，仅在格子首次出现（或缓存清理后）时调用，适合做初始化逻辑 |
| `bindUpdateBeforeFunc` | `function()` | ❌ | 一轮更新开始前的前置回调，适合准备数据 |
| `bindUpdateFinishFunc` | `function()` | ❌ | 一轮更新完成后的后置回调，适合延迟刷新等后处理 |
| `bindGridConName` | `str` | ❌ | 当网格格子控件名本身包含数字结尾时，会影响 index 计算，此时需显式声明控件名 |

> **💡 实际开发提示**：99% 的情况下只需要传入 `path` 和 `bindFunc` 两个参数即可完成网格开发。其余参数（`incrementalCallback`、`bindUpdateBeforeFunc`、`bindUpdateFinishFunc`）仅在需要入场动画、批量前置数据准备等进阶场景时才需要用到。**先从最简形式开始，按需添加即可。**

### 回调函数签名

```python
def bindFunc(viewPath, index):
    # type: (str, int) -> None
    """
    核心渲染回调 - 每次网格更新时对每个可见格子调用
    
    参数:
        viewPath: 当前格子的完整 UI 路径（可直接用于 GetBaseUIControl 等操作）
        index:    当前格子的索引（从 0 开始）
    """
    pass

def incrementalCallback(viewPath, index):
    # type: (str, int) -> None
    """
    增量渲染回调 - 仅在格子首次渲染时调用（基于路径+索引缓存判断）
    适合执行只需一次的初始化操作，如绑定按钮事件
    
    注意: 调用 clearIncrementalCache() 后，所有格子将重新触发增量回调
    """
    pass
```

### 方法列表

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `updateRender(uiNode)` | `None` | 手动触发完整渲染更新 |
| `updateOnceRender(viewPath, index=None)` | `None` | 单一格子渲染更新，可指定路径和索引 |
| `clearIncrementalCache()` | `None` | 清理增量缓存，下次更新时所有格子都会触发 `incrementalCallback` |
| `getRealPath(uiNode)` | `str` | 获取网格的真实路径（滚动网格会自动获取内容路径） |
| `getRealComponentPath(uiNode)` | `str` | 获取含组件路径的完整路径（用于事件路径匹配） |
| `childsGen(uiNode)` | `generator` | 子节点生成器，yield `(index, gridPath)` |
| `getPosWithPath(path)` | `int` | 从路径字符串中提取格子索引 |
| `setGridPathBasedOnScrollView(path)` | `self` | 设置基于滚动视图的网格路径偏移（高级用法） |

---

## QGridBinder 绑定器

`QGridBinder` 继承自 `QRAIIControlFuntion`，支持 RAII 自动生命周期管理。当宿主 UI 节点（`ScreenNodeWrapper`）销毁时，绑定器会自动清理监听和缓存。

### 构造参数

```python
QGridBinder(
    uiNode,             # type: ScreenNodeWrapper    UI 节点实例
    gridData=None,      # type: QGridData | None     网格数据对象（可后续通过 setGridData 设置）
    parentPath="",      # type: str                  父路径（通常留空）
)
```

### 方法列表

| 方法 | 返回值 | 说明 |
|------|--------|------|
| `setGridData(gridData)` | `QGridBinder` | 绑定 QGridData 对象（链式调用） |
| `start()` | `QGridBinder` | 手动启动绑定器（非 RAII 模式时使用） |
| `stop()` | `QGridBinder` | 手动停止绑定器（释放监听） |
| `updateRender()` | `QGridBinder` | 主动触发渲染更新 |
| `updateOnceRender(viewPath, index=None)` | `QGridBinder` | 单一格子渲染更新 |
| `clearIncrementalCache()` | `QGridBinder` | 清理增量缓存 |
| `setGridDimension(size)` | `QGridBinder` | 设置网格尺寸 `(columns, rows)` |
| `getRealPath()` | `str` | 获取网格真实路径 |
| `setButtonClickHandler(buttonPath, onClick)` | `QGridBinder` | 为格子内按钮设置点击回调 |
| `bindButtonClickHandler(buttonPath)` | `decorator` | 装饰器方式绑定按钮点击 |

---

## 使用模式

### 模式一：RAII 自动管理（推荐）

当 UI 类继承自 `ScreenNodeWrapper`（内置 RAII 环境）时，`QGridBinder` 在 `__init__` 中创建即可自动跟随 UI 生命周期：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("myui.main")
class MyUI(ScreenNodeWrapper):
    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        # 数据源
        self.itemList = ["苹果", "面包", "钻石", "铁剑", "盾牌"]
        # 创建 QGridBinder 并绑定 QGridData
        # 当 gridData 不为 None 时，会自动注册到 RAII 环境
        self.gridBinder = QGridBinder(self, QGridData(
            "/panel/grid",              # 网格控件路径
            bindFunc=self.onGridRender  # 渲染回调
        ))

    def onGridRender(self, viewPath, index):
        # type: (str, int) -> None
        """网格渲染回调 - 设置每个格子的显示内容"""
        if index < len(self.itemList):
            label = self.GetBaseUIControl(viewPath + "/label")
            if label:
                label.asTextEditBox().SetEditText(self.itemList[index])

    def Create(self):
        ScreenNodeWrapper.Create(self)  # 触发 RAII 资源加载（自动 start）
        # 设置网格大小
        self.gridBinder.setGridDimension((1, len(self.itemList)))

    def Destroy(self):
        ScreenNodeWrapper.Destroy(self)  # 触发 RAII 资源释放（自动 stop）
```

> **原理**：`ScreenNodeWrapper.Create()` 会调用 `setDRAIIEnvState(True)` + `loadALLDRAIIRes()`，使所有已注册的 RAII 资源（包括 QGridBinder）自动执行 `_loadResource()` → `createControl()` → `onCreate()` → 开始监听事件。`Destroy()` 则自动释放。

### 模式二：链式延迟绑定（推荐）

先创建 QGridBinder，再通过 `setGridData` 链式设置：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("myui.main")
class MyUI(ScreenNodeWrapper):
    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        self.itemList = []

    def Create(self):
        ScreenNodeWrapper.Create(self)
        # 在 Create 中创建并启动（手动管理）
        self.gridBinder = QGridBinder(self).setGridData(
            QGridData(
                "/panel/grid",
                bindFunc=self.onGridRender,
                bindUpdateFinishFunc=self.onRenderFinish
            )
        ).start()  # 手动启动
        self.refreshData()

    def onGridRender(self, viewPath, index):
        # type: (str, int) -> None
        if index < len(self.itemList):
            self.GetBaseUIControl(viewPath + "/name").asLabel().SetText(
                self.itemList[index]
            )

    def onRenderFinish(self):
        """每轮渲染更新完成后执行"""
        print("网格渲染完成，共 {} 项".format(len(self.itemList)))

    def refreshData(self):
        """刷新数据并触发重新渲染"""
        self.itemList = ["物品A", "物品B", "物品C"]
        self.gridBinder.setGridDimension((1, len(self.itemList)))

    def Destroy(self):
        self.gridBinder.stop()  # 手动停止
        ScreenNodeWrapper.Destroy(self)
```

### 模式三：手动 start/stop

对于不继承 RAII 环境的 UI 类，需要手动管理生命周期：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

class SomeUI(object):
    """非 ScreenNodeWrapper 的场景"""
    def __init__(self, uiNode):
        self.uiNode = uiNode
        self.gridBinder = QGridBinder(uiNode).setGridData(
            QGridData("/panel/grid", bindFunc=self.onRender)
        )

    def onRender(self, viewPath, index):
        pass

    def show(self):
        self.gridBinder.start()

    def hide(self):
        self.gridBinder.stop()
```

---

## 滚动网格（ScrollView + Grid）

当网格嵌套在 ScrollView 内时，需要设置 `isScrollGrid=True`：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("myui.main")
class ScrollGridUI(ScreenNodeWrapper):
    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        self.dataList = list(range(100))  # 100 条数据
        self.gridBinder = QGridBinder(self, QGridData(
            "/panel/scrollView",        # 传入 ScrollView 的路径（而不是 Grid 的）
            isScrollGrid=True,          # 声明为滚动网格
            bindFunc=self.onRender,
            incrementalCallback=self.onFirstRender
        ))

    def onFirstRender(self, viewPath, index):
        # type: (str, int) -> None
        """仅在格子首次渲染时调用（增量回调）"""
        # 适合做按钮绑定等一次性初始化
        buttonPath = viewPath + "/btn"
        self.gridBinder.setButtonClickHandler(
            buttonPath,
            lambda: self.onItemClick(index)
        )

    def onRender(self, viewPath, index):
        # type: (str, int) -> None
        """每次更新都会调用"""
        if index < len(self.dataList):
            self.GetBaseUIControl(viewPath + "/label").asLabel().SetText(
                "第 {} 项".format(self.dataList[index])
            )

    def onItemClick(self, index):
        print("点击了第 {} 项".format(index))

    def Create(self):
        ScreenNodeWrapper.Create(self)
        # 获取滚动网格内的真实 Grid 路径，设置维度
        realPath = self.gridBinder.getRealPath()
        self.GetBaseUIControl(realPath).asGrid().SetGridDimension(
            (1, len(self.dataList))
        )

    def Destroy(self):
        ScreenNodeWrapper.Destroy(self)
```

### 非直接嵌套的滚动网格

当 ScrollView 和 Grid 之间还有其他父级控件时，使用 `setGridPathBasedOnScrollView` 设置偏移路径：

```python
# JSON UI 结构:
# scrollView
#   └─ content_panel (ScrollView 内容面板)
#       └─ wrapper      ← 额外的中间层
#           └─ grid

gridData = QGridData(
    "/panel/scrollView",
    isScrollGrid=True,
    bindFunc=self.onRender
)
# 设置从 ScrollView 内容路径到 Grid 的相对路径
gridData.setGridPathBasedOnScrollView("/wrapper/grid")
```

---

## 增量渲染机制

`QGridData` 内置增量渲染缓存，通过 `incrementalCallback` 实现只在格子首次出现时执行特定逻辑：

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("myui.main")
class IncrementalUI(ScreenNodeWrapper):
    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        self.gridBinder = QGridBinder(self, QGridData(
            "/panel/grid",
            bindFunc=self.onRender,
            incrementalCallback=self.onIncrementalRender,
            bindUpdateBeforeFunc=self.onBeforeUpdate,
            bindUpdateFinishFunc=self.onAfterUpdate
        ))

    def onBeforeUpdate(self):
        """每轮更新开始前调用 - 准备数据"""
        print("开始更新网格...")

    def onIncrementalRender(self, viewPath, index):
        # type: (str, int) -> None
        """增量回调 - 仅首次渲染时触发"""
        # 适合做一次性操作：绑定事件、创建子控件等
        print("格子 {} 首次渲染".format(index))

    def onRender(self, viewPath, index):
        # type: (str, int) -> None
        """完整回调 - 每次更新都触发"""
        # 适合做数据刷新：更新文本、图片等
        pass

    def onAfterUpdate(self):
        """每轮更新结束后调用 - 后处理"""
        print("网格更新完成")

    def resetAllGridItems(self):
        """重置所有格子 - 清除增量缓存后下次更新将全部重新触发 incrementalCallback"""
        self.gridBinder.clearIncrementalCache()
        self.gridBinder.updateRender()  # 主动触发重新渲染

    def Create(self):
        ScreenNodeWrapper.Create(self)

    def Destroy(self):
        ScreenNodeWrapper.Destroy(self)
```

### 增量渲染的调用顺序

每次网格更新（由事件触发或手动调用 `updateRender`）时，回调执行顺序为：

```
bindUpdateBeforeFunc()                      ← 1. 前置回调（一次）
    ├─ incrementalCallback(path, 0)         ← 2. 增量回调（仅新格子）
    ├─ bindFunc(path, 0)                    ← 3. 渲染回调
    ├─ incrementalCallback(path, 1)         ← 仅新格子
    ├─ bindFunc(path, 1)
    ├─ ...
    └─ bindFunc(path, N)
bindUpdateFinishFunc()                      ← 4. 后置回调（一次）
```

---

## 单一格子更新

当只需要更新某个特定格子而不触发全量刷新时，使用 `updateOnceRender`：

```python
def updateSingleItem(self, index):
    """更新单个格子"""
    # 方式一：通过 gridBinder（推荐）
    # 需要知道格子的 viewPath
    for i, gridPath in self.gridBinder._bindGridData.childsGen(self):
        if i == index:
            self.gridBinder.updateOnceRender(gridPath, index)
            break

    # 方式二：直接通过 QGridData
    # gridData.updateOnceRender(viewPath, index)
```

---

## 按钮绑定

QGridBinder 提供了便捷的按钮点击绑定方法，适合在格子内设置按钮：

```python
# 方式一：直接设置
self.gridBinder.setButtonClickHandler(
    viewPath + "/button",   # 按钮的完整路径
    lambda: self.onButtonClick(index)
)

# 方式二：装饰器
@self.gridBinder.bindButtonClickHandler(viewPath + "/button")
def onClick():
    print("按钮被点击")
```

### ⚠️ 必须在 bindFunc 中每次重新绑定按钮

由于网格的**复用机制**，格子控件会被引擎回收并重新分配给不同的 index。这意味着之前绑定的按钮回调可能已经指向错误的 index。因此，**按钮回调必须在 `bindFunc`（每次渲染回调）中重新设置，而不是放在 `incrementalCallback`（增量回调）中**。

```python
# ❌ 错误用法 - 只在增量回调中绑定一次，复用后 index 会错乱
def onIncrementalRender(self, viewPath, index):
    self.gridBinder.setButtonClickHandler(
        viewPath + "/btn", lambda: self.onClick(index)
    )

def onRender(self, viewPath, index):
    # 只更新显示，没有重新绑定按钮 → 复用后点击会触发错误的 index
    self.GetBaseUIControl(viewPath + "/content/label").asLabel().SetText(
        self.dataList[index]
    )

# ✅ 正确用法 - 在 bindFunc 中每次都重新绑定
def onRender(self, viewPath, index):
    # type: (str, int) -> None
    if index < len(self.dataList):
        self.GetBaseUIControl(viewPath + "/content/label").asLabel().SetText(
            self.dataList[index]
        )
        # 每次渲染都重新设置按钮回调，确保 index 正确
        self.gridBinder.setButtonClickHandler(
            viewPath + "/btn",
            lambda: self.onClick(index)
        )
```

> **⚠️ 千万不要尝试"优化"按钮绑定**：不要缓存已绑定的路径、不要跳过已绑定的格子、不要做任何"避免重复绑定"的优化 — 复用机制会导致所有这类优化产生 bug。每次 `bindFunc` 被调用时，都必须无条件地重新设置按钮回调。

---

## 与 bindGridConName 相关

当网格格子控件名本身包含数字时（如 `item_panel2`），QGridData 从路径末尾提取数字计算 index 时可能产生错误。此时需要显式声明控件名：

```python
# 假设网格子项控件名为 "item_panel2"
# 引擎生成的路径为: /grid/item_panel21, /grid/item_panel22, ...
# 不声明 bindGridConName 时，提取出的数字为 21, 22（错误）

gridData = QGridData(
    "/panel/grid",
    bindFunc=self.onRender,
    bindGridConName="item_panel2"  # 显式声明控件名
)
# 声明后，提取逻辑会跳过 "item_panel2" 部分，正确提取 1, 2, ...
```

---

## 完整实战示例：物品背包界面

```python
# -*- coding: utf-8 -*-
from .QuModLibs.UI import ScreenNodeWrapper
from .QuModLibs.Modules.UI.Client import QGridBinder, QGridData

@ScreenNodeWrapper.autoRegister("backpack.main")
class BackpackUI(ScreenNodeWrapper):
    """物品背包界面 - 展示玩家物品列表"""
    GRID_COLUMNS = 4    # 每行 4 个格子

    def __init__(self, namespace, name, param):
        ScreenNodeWrapper.__init__(self, namespace, name, param)
        self.items = []     # type: list[dict]  物品数据
        # 创建滚动网格绑定器
        self.gridBinder = QGridBinder(self, QGridData(
            "/root/scroll_view",
            isScrollGrid=True,
            bindFunc=self.renderItem,
            incrementalCallback=self.initItem,
            bindUpdateBeforeFunc=self.prepareRender,
            bindUpdateFinishFunc=self.finishRender
        ))

    def Create(self):
        ScreenNodeWrapper.Create(self)
        self.loadItems()

    def Destroy(self):
        ScreenNodeWrapper.Destroy(self)

    # ====== 数据操作 ======

    def loadItems(self):
        """加载物品数据并刷新网格"""
        self.items = [
            {"name": "钻石剑", "count": 1, "icon": "textures/items/diamond_sword"},
            {"name": "金苹果", "count": 64, "icon": "textures/items/apple_golden"},
            {"name": "末影珍珠", "count": 16, "icon": "textures/items/ender_pearl"},
            # ... 更多物品
        ]
        rows = (len(self.items) + self.GRID_COLUMNS - 1) // self.GRID_COLUMNS
        self.gridBinder.setGridDimension((self.GRID_COLUMNS, rows))

    def addItem(self, itemData):
        """添加物品并刷新"""
        self.items.append(itemData)
        rows = (len(self.items) + self.GRID_COLUMNS - 1) // self.GRID_COLUMNS
        self.gridBinder.setGridDimension((self.GRID_COLUMNS, rows))

    # ====== 渲染回调 ======

    def prepareRender(self):
        """前置处理 - 可在此准备缓存数据"""
        pass

    def initItem(self, viewPath, index):
        # type: (str, int) -> None
        """增量回调 - 首次渲染时绑定按钮"""
        self.gridBinder.setButtonClickHandler(
            viewPath + "/btn_item",
            lambda: self.onItemClick(index)
        )

    def renderItem(self, viewPath, index):
        # type: (str, int) -> None
        """渲染回调 - 更新格子显示"""
        # 注意：viewPath 根控件在回调期间被引擎接管，不能直接 SetVisible
        # 必须操作其内部子控件（如 content_panel）来控制可见性
        contentPanel = self.GetBaseUIControl(viewPath + "/content_panel")
        if index < len(self.items):
            item = self.items[index]
            # 显示内容面板（第二个参数 False 避免触发全局 UI 刷新，性能关键）
            contentPanel.SetVisible(True, False)
            # 设置物品名
            nameControl = self.GetBaseUIControl(viewPath + "/content_panel/item_name")
            if nameControl:
                nameControl.asLabel().SetText(item["name"])
            # 设置数量
            countControl = self.GetBaseUIControl(viewPath + "/content_panel/item_count")
            if countControl:
                countControl.asLabel().SetText(str(item["count"]))
            # 设置图标
            iconControl = self.GetBaseUIControl(viewPath + "/content_panel/item_icon")
            if iconControl:
                iconControl.asImage().SetSprite(item["icon"])
        else:
            # 超出数据范围的多余格子 - 隐藏内容面板
            # ⚠️ 不能对 viewPath 本身 SetVisible，只能对子控件操作
            # ⚠️ 第二个参数 False 非常重要：避免每个格子都触发全局 UpdateScreen
            contentPanel.SetVisible(False, False)

    def finishRender(self):
        """后置处理"""
        pass

    # ====== 交互回调 ======

    def onItemClick(self, index):
        if index < len(self.items):
            print("选择了物品: {}".format(self.items[index]["name"]))
```

---

## API 速查表

### QGridData

```python
# 创建
gridData = QGridData(path, isScrollGrid, bindFunc, incrementalCallback, 
                     bindUpdateBeforeFunc, bindUpdateFinishFunc, bindGridConName)

# 方法
gridData.updateRender(uiNode)                       # 触发完整渲染
gridData.updateOnceRender(viewPath, index)           # 单格渲染
gridData.clearIncrementalCache()                     # 清理增量缓存
gridData.getRealPath(uiNode)                         # 获取真实路径
gridData.getRealComponentPath(uiNode)                # 获取组件路径
gridData.childsGen(uiNode)                           # 子节点迭代器 -> (index, path)
gridData.getPosWithPath(path)                        # 路径提取索引
gridData.setGridPathBasedOnScrollView(path)          # 设置滚动视图内偏移路径
```

### QGridBinder

```python
# 创建（RAII 自动管理）
binder = QGridBinder(uiNode, gridData)

# 创建（手动管理）
binder = QGridBinder(uiNode)
binder.setGridData(gridData).start()

# 方法
binder.setGridData(gridData)                         # 设置网格数据 -> QGridBinder
binder.start()                                       # 启动 -> QGridBinder
binder.stop()                                        # 停止 -> QGridBinder
binder.updateRender()                                # 触发渲染 -> QGridBinder
binder.updateOnceRender(viewPath, index)             # 单格渲染 -> QGridBinder
binder.clearIncrementalCache()                       # 清理缓存 -> QGridBinder
binder.setGridDimension((cols, rows))                # 设置维度 -> QGridBinder
binder.getRealPath()                                 # 获取路径 -> str
binder.setButtonClickHandler(buttonPath, onClick)    # 按钮点击 -> QGridBinder
binder.bindButtonClickHandler(buttonPath)            # 装饰器绑定 -> decorator
```

---

## 常见问题

### Q: index 从 0 还是 1 开始？
A: `index` 从 **0** 开始。内部实现是 `getPosWithPath` 提取路径末尾数字后减 1。

### Q: bindFunc 和 incrementalCallback 的区别？
A: `bindFunc` 在每次网格更新时对所有可见格子都会调用，适合数据刷新；`incrementalCallback` 仅在格子路径首次出现时调用（有缓存机制），适合一次性操作如事件绑定。

### Q: 如何动态改变网格数量？
A: 调用 `setGridDimension((cols, rows))` 改变维度后，引擎会自动触发 `GridComponentSizeChangedClientEvent`，QGridBinder 会自动响应并执行渲染回调。

### Q: 滚动网格的 path 填什么？
A: 填 **ScrollView 控件的路径**，而不是内部 Grid 的路径。QGridData 会自动通过 `GetScrollViewContentPath()` 获取真实的 Grid 路径。

### Q: 什么时候需要 clearIncrementalCache？
A: 当数据源发生根本性变化（如重新排序、替换整个列表）时，需要清理缓存以确保 `incrementalCallback` 对所有格子重新触发。单纯改变数量（增加/减少）无需清理。

### Q: QGridBinder 支持链式调用吗？
A: 大部分方法返回 `self`（QGridBinder），支持链式调用：
```python
self.gridBinder.setGridData(data).start().setGridDimension((4, 10)).updateRender()
```
