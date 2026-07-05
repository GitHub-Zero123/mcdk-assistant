# UI 动画系统模块

`QuModLibs.Modules.UI.Anims` 提供 UI 控件动画托管能力。现代项目推荐使用 `QAnimManager.bindRAIINode(...)` 接入 `ScreenNodeWrapper` / `QEScreenNode` 的 RAII 生命周期，旧式手动 Tick 管理放在本文末尾，仅供维护历史项目参考。

::: tip 现代推荐
优先使用 `QAnimManager.bindRAIINode(uiNode)`。它会在 UI 创建时自动监听帧事件，并在 UI 销毁时自动释放监听和动画对象。
:::

## QAnimManager <Badge type="tip" text="现代推荐" />

动画管理器用于统一托管多个控件的动画对象。通过 `getControlAnimObj(path)` 获取指定控件的 `QAnimsControl` 后，即可添加位置、大小、不透明度、打字机等动画变换。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Client import *
from .QuModLibs.Modules.UI.EnhancedUI import QEScreenNode
from .QuModLibs.Modules.UI.Anims import QAnimManager, QPosTransform, QSizeTransform

@QEScreenNode.autoRegister("test_ui.main")
class TestUI(QEScreenNode):
    def __init__(self, *args):
        QEScreenNode.__init__(self, *args)
        self.imgPath = "/panel/image"
        self.animSystem = QAnimManager.bindRAIINode(self)

    @QEScreenNode.OnClick("/panel/move")
    def onMove(self):
        animObj = self.animSystem.getControlAnimObj(self.imgPath)
        animObj.changeTransformAnim(
            QPosTransform(animObj.getPos(), (400, 100), useTime=0.8)
        )
        animObj.changeTransformAnim(
            QSizeTransform(animObj.getScale(), animObj.controlInfo._baseScale, useTime=0.8, resizeChildren=True)
        )
```

::: warning 使用条件
`bindRAIINode` 只能绑定到继承 `QDRAIIEnv` 的 UI 节点，例如 `ScreenNodeWrapper` / `QEScreenNode`。非 RAII 界面只能使用本文末尾的旧式绑定方式。
:::

## QAnimsControl

`QAnimsControl` 是单个控件的动画对象，通常不需要手动创建，而是通过 `QAnimManager.getControlAnimObj(path)` 获取。

### 常用方法

```python
animObj = self.animSystem.getControlAnimObj("/panel/image")

animObj.addTransformAnim(QPosTransform(...))
animObj.changeTransformAnim(QSizeTransform(...))
animObj.removeTransformAnim(transformObj)
animObj.matchTransformWithClass(QPosTransform)
animObj.update(0.033, forceUpdate=True)
```

- `addTransformAnim(transformObj)`：添加动画变换对象。
- `changeTransformAnim(transformObj)`：添加动画，并覆盖同类型旧动画，通常比直接 `addTransformAnim` 更安全。
- `removeTransformAnim(transformObj)`：移除指定动画对象。
- `matchTransformWithClass(cls)`：查找当前控件上指定类型的动画对象，不存在时返回 `None`。
- `update(delayTime, forceUpdate=True)`：推进动画计算。使用 `QAnimManager` 时通常不需要手动调用。

::: tip 注意事项
单个 `QTransform` 对象只能同时绑定到一个控件。需要复用动画配置时，请创建新的变换对象。
:::

## QTransform

`QTransform` 是动画变换基类。内置动画均继承自它，并支持链式设置缓动与完成回调。

### 完成回调

```python
QPosTransform((0, 0), (100, 100), 0.8).setFinishAnimBackCall(
    lambda: print("动画完成")
)
```

### 缓动模式

```python
from .QuModLibs.Modules.UI.Anims import QPosTransform, QTransform

QPosTransform((0, 0), (100, 100), 0.8).setEasingMode(
    QTransform.EasingMode.EaseOutQuad
)

# 也可以修改全局默认缓动类型。
QTransform.EasingMode.Default = QTransform.EasingMode.EaseOutQuad
```

当前内置缓动：

- `QTransform.EasingMode.Linear`：线性。
- `QTransform.EasingMode.EaseInSide`：渐入。
- `QTransform.EasingMode.EaseOutQuad`：渐出。

## 内置动画类型

```python
from .QuModLibs.Modules.UI.Anims import QPosTransform, QSizeTransform, QAlphaTransform, QuTypeWriter

QPosTransform((0, 0), (100, 100), 0.8)
QSizeTransform((10, 10), (100, 100), 0.8, resizeChildren=False)
QAlphaTransform(0.0, 1.0, 0.8)
QuTypeWriter("你好, 世界", 1.2, childPath="", syncSize=False)
```

- `QPosTransform`：坐标动画。
- `QSizeTransform`：大小动画，可选择是否重算子节点。
- `QAlphaTransform`：透明度动画，仅支持图片/文字控件。
- `QuTypeWriter`：打字机动画，仅支持文字控件。

## 旧式手动管理 <Badge type="danger" text="已过时" />

早期写法直接创建 `QAnimsControl`，并在 Tick / 帧事件中手动调用 `update()`。该方式需要自行管理监听和释放，新项目不推荐使用。

```python
# -*- coding: utf-8 -*-
from .QuModLibs.Client import *
from .QuModLibs.UI import EasyScreenNodeCls
from .QuModLibs.Modules.UI.Anims import QAnimsControl, QPosTransform

@EasyScreenNodeCls.Binding("test_ui.main")
class TestUI(EasyScreenNodeCls):
    def __init__(self):
        self.imgPath = "/panel/image"
        self.imgAnimObj = QAnimsControl.bindControl(self, self.imgPath)

    @EasyScreenNodeCls.Listen("OnScriptTickClient")
    def onTick(self, _={}):
        self.imgAnimObj.update()

    @EasyScreenNodeCls.OnClick("/panel/move")
    def onMove(self):
        self.imgAnimObj.changeTransformAnim(
            QPosTransform(self.imgAnimObj.getPos(), (400, 100), useTime=0.8)
        )
```

## 旧式 bindNode <Badge type="danger" text="已过时" />

`QAnimManager.bindNode(uiNode)` 通过旧式 Auto Tick 生命周期管理动画，主要用于非 QuModLibs UI 或历史项目维护。新项目请使用 `bindRAIINode`。

```python
from .QuModLibs.UI import EasyScreenNodeCls
from .QuModLibs.Modules.UI.Anims import QAnimManager

@EasyScreenNodeCls.Binding("test_ui.main")
class TestUI(EasyScreenNodeCls):
    def __init__(self):
        self.animSystem = QAnimManager.bindNode(self)
```
