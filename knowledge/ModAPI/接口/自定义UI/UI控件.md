# UI控件

## AddEntityMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  增加实体位置标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| entityId | str | 实体Id |
| texturePath | str | 头顶ICON贴图，如textures/blocks/border |
| size | `tuple(float,float)` | 贴图大小，默认为(4,4) |
| enableRotation | bool | 是否启用实体朝向，默认为False |
| isRevertZRot | bool | 是否翻转实体Z轴旋转，默认为False |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否增加成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.AddEntityMarker(entityId, "textures/ui/custom_head")

```

## AddEntityTextMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  在小地图上增加实体文本标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| entityId | str | 实体Id |
| text | str | 文本的内容，可以支持[样式代码 (opens new window)](https://zh.minecraft.wiki/w/%E6%A0%BC%E5%BC%8F%E5%8C%96%E4%BB%A3%E7%A0%81)（§可以设置文字的颜色、格式等，该种用法更加灵活多变） |
| scale | float | 文本缩放倍数，等于文本控件json中的font_scale_factor参数，默认缩放倍数为1.0 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否增加成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.AddEntityTextMarker(entityId, "just test", 1.0)

```

## AddHoverEventParams

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  开启按钮的悬浮回调功能，不调用该函数则按钮无悬浮回调

- 参数

  无

- 返回值

  无

- 备注

  - 只有PC能生效，且只有PushScreen生成UI时生成的鼠标能产生悬浮回调！

- 示例

```python
buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddHoverEventParams()

```

## AddOption

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  添加下拉框项，若添加成功则返回True，否则返回False

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| showName | str | 显示文本，必填参数 |
| icon | str | 贴图路径，若填写则在下拉框项前端会显示该icon，默认为None |
| userData | any | 自定义数据，在选中该下拉框项时会跟随回调函数传回，默认为None |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否添加成功 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
success = comboBoxUIControl.AddOption("测试项", "textures/ui/test_icon", {"mydata": "netease"})

```

## AddStaticMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  增加地图上静态位置的标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| key | str | 标记Id |
| vec2 | `tuple(float,float)` | 地图位置二维坐标(x,z) |
| texturePath | str | 贴图路径 |
| size | `tuple(float,float)` | 贴图大小，默认为(4,4) |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否增加成功 |

- 备注

  - 如使用该接口请勿将地图缩小倍数设置过大（建议ZoomOut设置后的地图倍数不小于原地图大小的0.5倍），以免造成地图缩小后静态标记位置失效等问题。

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.AddStaticMarker("this_is_marker_key", (10,2), "textures/blocks/border", (3,3))

```

## AddStaticTextMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  在小地图上增加静态文本的标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| key | str | 标记Id |
| vec2 | `tuple(float,float)` | 地图位置二维坐标(x,z) |
| text | str | 文本的内容，可以支持[样式代码 (opens new window)](https://zh.minecraft.wiki/w/%E6%A0%BC%E5%BC%8F%E5%8C%96%E4%BB%A3%E7%A0%81)（§可以设置文字的颜色、格式等，该种用法更加灵活多变） |
| scale | float | 文本缩放倍数，等于文本控件json中的font_scale_factor参数，默认缩放倍数为1.0 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否增加成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.AddStaticTextMarker("this_is_marker_key", (10,2), "just test", 1.0)

```

## AddTouchEventParams

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  开启按钮回调功能，不调用该函数则按钮无回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| args | dict | 默认为None，详细说明请见备注。 |

- 返回值

  无

- 备注

  - AddTouchEventParams参数args说明：

| 关键字 | 数据类型 | 说明 |
| --- | --- | --- |
| isSwallow | bool | 默认为True, 按钮是否吞噬事件；或为Ture时，点击按钮时，点击事件不会穿透到世界。如破坏方块、镜头转向不会被响应 |

- 示例

```python
buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})

```

## ClearOptions

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  清空下拉框

- 参数

  无

- 返回值

  无

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.ClearOptions()

```

## ClearSelection

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  清除当前选中，使下拉框恢复未选中内容状态

- 参数

  无

- 返回值

  无

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.ClearSelection()

```

## DisableTextShadow

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  关闭文本控件显示阴影

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to diable text2 textShadow
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
ret = labelUIControl.DisableTextShadow()

```

## EnableTextShadow

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  使文本控件显示阴影

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to enable text2 textShadow
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
ret = labelUIControl.EnableTextShadow()

```

## GetAnchorFrom

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  判断控件相对于父节点的哪个锚点来计算位置与大小

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 控件计算位置大小所依赖的父节点锚点位置信息，具体返回值的意义可参考SetAnchorFrom接口的备注 |

- 示例

```python
# we want to get image1 anchorFrom
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
anchorFrom = baseUIControl.GetAnchorFrom()

```

## GetAnchorTo

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件自身锚点位置信息

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 该控件自身锚点位置信息，具体返回值的意义可参考SetAnchorTo接口的备注 |

- 示例

```python
# we want to get image1 anchorTo
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
anchorTo = baseUIControl.GetAnchorTo()

```

## GetChildByName

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  根据子控件的名称获取BaseUIControl实例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| childName | str | 子节点名称 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 子控件的BaseUIControl实例 |

- 示例

```python
# text1的BaseUIControl实例获得text2的BaseUIControl实例
text1Path = "/text1"
text1Control = uiNode.GetBaseUIControl(text1Path)
text2Control = text1Control.GetChildByName("text2")

```

## GetChildByPath

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  根据相对路径获取BaseUIControl实例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| childPath | str | 相对当前BaseUIControl路径的路径 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 子控件的BaseUIControl实例 |

- 示例

```python
# 根据路径"/text1"的BaseUIControl实例获得路径为"/text1/text2/text3"的BaseUIControl实例
text1Path = "/text1"
text1Control = uiNode.GetBaseUIControl(text1Path)
text3Control = text1Control.GetChildByPath("/text2/text3")

```

## GetClipDirection

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片控件的裁剪方向

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 图片控件的裁剪方向，返回值的意义可见SetClipDirection接口的备注 |

- 示例

```python
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.GetClipDirection()

```

## GetClipOffset

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件的裁剪偏移信息

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件的裁剪偏移信息，第一项为横轴，第二项为纵轴 |

- 示例

```python
# we want to get image1 clipOffset
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
clipOffset = baseUIControl.GetClipOffset()

```

## GetClipsChildren

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  根据控件路径返回某控件是否开启裁剪内容

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 该控件是否已开启裁剪内容 |

- 示例

```python
# we want to know whether image1 enables clipsChildren
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
clipsChildrenEnabled = baseUIControl.GetClipsChildren()

```

## GetCurrentSliceIndex

客户端

method in mod.client.ui.controls.selectionWheelUIControl.SelectionWheelUIControl

- 描述

  获取轮盘当前选择的切片的index，一般是在SetHoverCallback和SetTouchUpCallback中使用，表示当前鼠标悬浮或者点击的轮盘切片index

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 轮盘当前选择的切片的index，-1表示轮盘当前没有选择任何切片 |

- 示例

```python
selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()
selectionWheelControl.GetCurrentSliceIndex()

```

## GetEditText

客户端

method in mod.client.ui.controls.textEditBoxUIControl.TextEditBoxUIControl

- 描述

  获取edit_box输入框的文本信息，获取失败会返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 文本信息 |

- 备注

  - 获取失败通常是由于路径填写错误，或该控件不是edit_box类型

- 示例

```python
# we want to get edit2 content
editBoxPath = "/panel/edit2"
textEditBoxUIControl = uiNode.GetBaseUIControl(editBoxPath).asTextEditBox()
text = textEditBoxUIControl.GetEditText()

```

## GetFullPosition

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件的锚点坐标，支持比例值以及绝对值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| axis | str | 要获取的轴向，可选的值有`["x","y"]`，"x"表示获取控件的x坐标，"y"表示获取控件的y坐标 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| dict | 控件的大小信息，详见备注 |

- 备注

  - 参见SetFullPosition接口，控件的大小信息描述可由 absoluteValue， followType， relativeValue三个属性描述
    因此返回值的结构如下：

| 键值 | 类型 |
| --- | --- |
| "absoluteValue" | float |
| "followType" | str |
| "relativeValue" | float |

- 示例

```python
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
# 获取image1的锚点x坐标
ret = baseUIControl.GetFullPosition(axis="x")

```

## GetFullSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件的大小，支持百分比以及绝对值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| axis | str | 要获取的轴向，可选的值有`["x","y"]`，"x"表示获取控件的宽度，"y"表示获取控件的高度 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| dict | 控件的大小信息，详见备注 |

- 备注

  - 参见SetFullSize接口，控件的大小信息描述可由 absoluteValue， followType， relativeValue， fit四个属性描述
    因此返回值的结构如下：

| 键值 | 类型 |
| --- | --- |
| "absoluteValue" | float |
| "followType" | str |
| "relativeValue" | float |
| "fit" | bool |

- 示例

```python
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
# 获取image1的宽度信息
ret = baseUIControl.GetFullSize(axis="x")

```

## GetGlobalPosition

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件全局坐标

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件全局坐标信息，第一项为横轴，第二项为纵轴 |

- 示例

```python
# we want to get text2 global position
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
textPosition = baseUIControl.GetGlobalPosition()

```

## GetGlobalRotateAngle

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片通过RotateAround函数设置进去的角度值

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 获取图片通过RotateAround函数设置进去的角度值 |

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
print imageControl.GetGlobalRotateAngle()

```

## GetGlobalRotatePoint

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片通过RotateAround函数设置进去的point值

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 获取图片通过RotateAround函数设置进去的point值 |

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
print imageControl.GetGlobalRotatePoint()

```

## GetGridItem

客户端

method in mod.client.ui.controls.gridUIControl.GridUIControl

- 描述

  根据网格位置获取元素控件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| x | int | 元素在网格的横坐标 |
| y | int | 元素在网格的纵坐标 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 网格的子节点控件 |

- 备注

  - 获取网格子节点需要注意一些细节，详见《开发指南-界面与交互-UI说明文档》中对Grid控件的描述

- 示例

```python
# we want to get element positioned at (0, 0)
gridPath = "/grid1"
gridUIControl = uiNode.GetBaseUIControl(gridPath).asGrid()
gridItem_0 = gridUIControl.GetGridItem(0, 0)
if gridItem_0:
    print gridItem_0.GetSize()

```

## GetIsModal

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  判断当前面板是否为模态框

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 当前面板是否为模态框 |

- 示例

```python
# we want to know whether inputPanel1 isModal
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
inputPanel.GetIsModal()

```

## GetIsSwallow

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  判断当前面板输入是否会吞噬事件，isSwallow为Ture时，点击时，点击事件不会穿透到世界。如破坏方块、镜头转向不会被响应

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 当前当前面板输入是否会吞噬事件 |

- 示例

```python
# we want to know whether inputPanel1 isSwallow
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
inputPanel.GetIsSwallow()

```

## GetMaxSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件所允许的最大的大小值

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件所允许的最大的大小值，第一项为横轴，第二项为纵轴 |

- 备注

  - 返回值为(0, 0)表示无限制

- 示例

```python
# we want to get image1 maxSize
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
imageMaxSize = baseUIControl.GetMaxSize()

```

## GetMinSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件所允许的最小的大小值

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件所允许的最小的大小值，第一项为横轴，第二项为纵轴 |

- 备注

  - 返回值为(0, 0)表示无限制

- 示例

```python
# we want to get image1 minSize
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
imageMinSize = baseUIControl.GetMinSize()

```

## GetModelId

客户端

method in mod.client.ui.controls.neteasePaperDollUIControl.NeteasePaperDollUIControl

- 描述

  获取渲染的骨骼模型Id

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 骨骼模型Id，失败或者不存在返回-1 |

- 备注

  - 注意：请不要在RenderEntity/RenderSkeletonModel调用之后立即执行。
    骨骼模型Id可用于一下情形：
    1.绑定一个另外的骨骼模型；
    2.绑定序列帧动画；
    3.绑定特效粒子动画

- 示例

```python
import mod.client.extraClientApi as clientApi
path = '/demoPanel/paper_doll'
doll = uiNode.GetBaseUIControl(path).asNeteasePaperDoll()
modelId = doll.GetModelId()
if modelId == -1:
    return
#用途1：绑定一个另外的骨骼模型
modelComp = clientApi.GetEngineCompFactory().CreateModel(modelId)
newModelId = modelComp.BindModelToModel("rightHand", "gun") # 把名称为gun的骨骼模型挂接到rightHand骨骼上
#用途2：绑定序列帧动画
sfxId = clientSystem.CreateEngineSfxFromEditor("effects/example_sfx.json") # 创建特效
comp = clientApi.GetEngineCompFactory().CreateFrameAniSkeletonBind(sfxId)
comp.Bind(modelId, "root", (0, 1, 0), (0, 0, 0)) # 把特效绑定到骨骼模型的骨骼节点上
frameComp = clientApi.GetEngineCompFactory().CreateFrameAniControl(sfxId)
frameComp.Play() # 播放动画
clientSystem.DestroyEntity(sfxId) # 销毁动画
#用途3：绑定特效粒子动画
particleEntityId = clientSystem.CreateEngineParticle("effects/example_particle.json", (0,0,0)) # 创建特效
comp = clientApi.GetEngineCompFactory().CreateParticleSkeletonBind(particleEntityId)
comp.Bind(modelId, "root", (0, 1, 0), (0, 0, 0)) # 把特效绑定到骨骼模型的骨骼节点上
particleComp = clientApi.GetEngineCompFactory().CreateParticleControl(particleEntityId)
particleComp.Play() # 播放动画
clientSystem.DestroyEntity(particleEntityId) # 销毁动画

```

## GetOffsetDelta

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  获得点击面板的拖拽偏移量

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件的拖拽偏移量，第一项为横轴，第二项为纵轴 |

- 备注

  - 点击面板拖拽功能及拖拽偏移量相关，详见InputPanel

- 示例

```python
# we want to get inputPanel1's offset_delta
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
offset_delta = inputPanel.GetOffsetDelta()

```

## GetOptionCount

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  获得选项数量

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 当前下拉框选项数量 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
totalNum = comboBoxUIControl.GetOptionCount()

```

## GetOptionIndexByShowName

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据展示文本查找对应下拉框项的索引位置，若找不到返回-1

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| name | str | 显示文本 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 索引位置 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
index = comboBoxUIControl.GetOptionIndexByShowName("测试项")

```

## GetOptionShowNameByIndex

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据索引位置查找当前栈式文本，若找不到返回None

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| index | int | 索引位置 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 显示文本 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
text = comboBoxUIControl.GetOptionShowNameByIndex(0)

```

## GetOrientation

客户端

method in mod.client.ui.controls.stackPanelUIControl.StackPanelUIControl

- 描述

  获取stackPanel的排列方向

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | stackPanel的排列方向，取值有限为`["vertical", "horizontal"]`，分别表示垂直方向，水平方向 |

- 示例

```python
# we want to get stackPanel1 orientation
stackPanelPath = "/stackPanel1"
stackPanel = uiNode.GetBaseUIControl(stackPanelPath).asStackPanel()
stackPanel.GetOrientation()

```

## GetPath

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  返回当前控件的相对路径，路径从画布节点开始算起

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 当前控件的相对路径，路径从画布节点开始算起 |

- 示例

```python
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
path = baseUIControl.GetPath()

```

## GetPosition

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件相对父节点的坐标

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件相对父节点的坐标信息，第一项为横轴，第二项为纵轴 |

- 示例

```python
# we want to get text2 position
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
textPosition = baseUIControl.GetPosition()

```

## GetPropertyBag

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取PropertyBag

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | PropertyBag |

- 示例

```python
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
propertyBag = baseUIControl.GetPropertyBag()

```

## GetRotateAngle

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片相对自身的旋转锚点旋转的角度

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 获取图片相对自身的旋转锚点旋转的角度 |

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
print imageControl.GetRotateAngle()

```

## GetRotatePivot

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片相对自身的旋转锚点

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 获取图片相对自身的旋转锚点，第一项为横轴，第二项为纵轴 |

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
print imageControl.GetRotatePivot()

```

## GetRotateRect

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  获取图片当前的四个边角点

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(tuple(float,float)`,`tuple(float,float)`,`tuple(float,float)`,`tuple(float,float)`) | 获取图片当前的四个边角点(每个点都是一个tuple) |

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
print imageControl.GetRotateRect()

```

## GetScrollViewContentControl

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  返回该scroll_view内容的BaseUIControl实例

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 该scroll_view内容的BaseUIControl实例 |

- 示例

```python
# we want get scroll_view content
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
contentUIControl = scrollViewUIControl.GetScrollViewContentControl()

```

## GetScrollViewContentPath

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  返回该scroll_view内容的路径

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 该scroll_view内容的路径 |

- 备注

  - scroll_view的路径是动态的，在触控模式和键鼠模式（按f11可切换的两种操作方式）下的路径不同，每次使用scroll_view及其子控件的路径，都应该重新调用一次本接口。
    如果不想让路径随操作模式变化，可以指定$touch变量为true，此时路径将固定为touch_path。

  ```json
  "scroll_view0@common.scrolling_panel": {
      // 手动指定$touch变量为true
      "$touch": true
      ...
  }
  ```

- 示例

```python
# we want get scroll_view content path
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
path = scrollViewUIControl.GetScrollViewContentPath()

```

## GetScrollViewPercentValue

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  获取当前scroll_view内容的百分比位置

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 当前scroll_view内容的百分比位置 |

- 示例

```python
# we want get scroll_view percent
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
scrollViewUIControl.GetScrollViewPercentValue()

```

## GetScrollViewPos

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  获得当前scroll_view最上方内容的位置

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 当前scroll_view最上方内容的位置 |

- 示例

```python
# we want get scroll_view pos
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
scrollViewUIControl.GetScrollViewPos()

```

## GetSelectOptionIndex

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  获得当前选中项的索引，所无选中项则返回-1

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 当前下拉框选中项索引 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
currentIndex = comboBoxUIControl.GetSelectOptionIndex()

```

## GetSelectOptionShowName

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  获得当前选中项的展示文本，所无选中项则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 当前选中项的展示文本 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
currentText = comboBoxUIControl.GetSelectOptionShowName()

```

## GetSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  获取控件的大小

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 该控件的大小信息，第一项为横轴，第二项为纵轴 |

- 示例

```python
# we want to get text2 size
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
text2Size = baseUIControl.GetSize()

```

## GetSliceCount

客户端

method in mod.client.ui.controls.selectionWheelUIControl.SelectionWheelUIControl

- 描述

  获取轮盘可以选择的总切片数量

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 轮盘可以选择的总切片数量 |

- 示例

```python
selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()
selectionWheelControl.GetSliceCount()

```

## GetSliderValue

客户端

method in mod.client.ui.controls.sliderUIControl.SliderUIControl

- 描述

  获取滑动条的值，失败返回0

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 滑动条的值 |

- 示例

```python
# we want to get slider value
sliderPath = "/panel/slider0"
sliderUIControl = uiNode.GetBaseUIControl(sliderPath).asSlider()
value = sliderUIControl.GetSliderValue()

```

## GetText

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  获取Label的文本信息，获取失败会返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 文本信息 |

- 备注

  - 获取失败通常是由于路径填写错误，或该控件不是Label类型

- 示例

```python
# we want to get text2 content
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.GetText()

```

## GetTextAlignment

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  获取文本控件的文本对齐方式

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 文本控件的文本对齐方式，具体返回值的意义可参考SetTextAlignment接口的备注 |

- 示例

```python
# we want to get text2 textAlignment
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.GetTextAlignment()

```

## GetTextColor

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  获取Label文本颜色

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float,float,float)` | 获取文本的颜色信息(r, g, b, a), 取值`[0, 1]` |

- 示例

```python
# we want to get text2 color
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.GetTextColor()

```

## GetTextLinePadding

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  获取文本控件的行间距

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 文本控件的行间距，单位为像素 |

- 示例

```python
# we want to get text2 textLinePadding
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.GetTextLinePadding()

```

## GetToggleState

客户端

method in mod.client.ui.controls.switchToggleUIControl.SwitchToggleUIControl

- 描述

  获取Toggle开关控件的状态

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| toggle_path | str | 实际toggle控件相对路径，由UI编辑器生成的开关控件该参数即为默认值"/this_toggle" |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否处于开启状态 True:开启状态 False:关闭状态 |

- 示例

```python
togglePath = "/toggle1"
switchToggleUIControl = uiNode.GetBaseUIControl(togglePath).asSwitchToggle()
print switchToggleUIControl.GetToggleState()

```

## GetUiItem

客户端

method in mod.client.ui.controls.itemRendererUIControl.ItemRendererUIControl

- 描述

  获取ItemRenderer控件显示的物品，ItemRenderer控件的配置方式详见控件介绍ItemRenderer

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| dict | 物品字典，详见备注（包含itemName:物品identifier, auxValue:物品附加值, isEnchanted:是否附魔) |

- 备注

  - 返回值具体如下：

| 键值 | 类型 | 内容 |
| --- | --- | --- |
| "itemName" | str | 物品identifier |
| "auxValue" | int | 物品附加值 |
| "isEnchanted" | bool | 是否显示附魔效果 |

- 示例

```python
#获取木板block信息
itemRenderPath = "/panel/item_renderer"
itemRendererBaseControl = uiNode.GetBaseUIControl(itemRenderPath)
itemRendererControl = itemRendererBaseControl.asItemRenderer()
itemInfoDict = itemRendererControl.GetUiItem()

```

## GetVisible

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  根据控件路径返回某控件是否已显示

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 该控件是否已显示 |

- 示例

```python
# 我们获得panel下面的text2是否显示
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
textVisible = baseUIControl.GetVisible()

```

## IsAnimEndCallbackRegistered

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  控件是否对名称为animName的动画进行了注册回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| animName | str | 动画的名称，请不要包含动画的命名空间 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否对名称为animName的动画进行了注册回调 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
print control.IsAnimEndCallbackRegistered("offset_animation")

```

## IsTextShadowEnabled

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  判断文本控件是否显示阴影

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 文本控件是否显示阴影 |

- 示例

```python
# we want to know whether text2 has shadow
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.IsTextShadowEnabled()

```

## PauseAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  暂停动画，暂停后的动画会停在当前的状态

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 动画的属性名称，默认值为"all"，表示暂停所有动画，不为"all"的时候表示单个动画的暂停，比如propertyName=="size"时，表示暂停尺寸属性上的动画 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否暂停成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
# 暂停所有动画
control.PauseAnimation()
# 暂停尺寸动画
# control.PauseAnimation("size")
# 暂停位移动画
# control.PauseAnimation("offset")
# 其他属性也是支持暂停的，具体可参见备注

```

## PlayAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  继续播放动画，从动画当前状态开始播放

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 动画的属性名称，默认值为"all"，表示播放所有动画，不为"all"的时候表示单个动画的播放，比如propertyName=="size"时，表示播放尺寸属性上的动画 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否播放成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
# 播放所有动画
control.PlayAnimation()
# 播放尺寸动画
# control.PlayAnimation("size")
# 播放位移动画
# control.PlayAnimation("offset")
# 其他属性也是支持播放的，具体可参见备注

```

## RegisterCloseComboBoxCallback

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  注册关闭下拉框事件回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callback | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 示例

```python
# 注册关闭下拉框事件回调，在点击关闭下拉框时会调用注册的回调函数
def onCloseComboBoxCallback(args):
    pass

comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.RegisterCloseComboBoxCallback(onCloseComboBoxCallback)

```

## RegisterOpenComboBoxCallback

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  注册展开下拉框事件回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callback | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 示例

```python
# 注册展开下拉框事件回调，在点击展开下拉框时会调用注册的回调函数
def onOpenComboBoxCallback(args):
    pass

comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.RegisterOpenComboBoxCallback(onOpenComboBoxCallback)

```

## RegisterSelectItemCallback

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  注册选中下拉框内容事件回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callback | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - onSelectItemCallback参数说明：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| index | int | 当前选中项的索引 |
| showName | str | 当前选中项的显示文本 |
| userData | Unknown | 当前选中项的自定义数据，若添加下拉框项时未传入自定义数据则此处为None |

- 示例

```python
# 注册选中下拉框内容事件回调，在点击关闭下拉框时会调用注册的回调函数
def onSelectItemCallback(index, showName, userData):
    pass

comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.RegisterSelectItemCallback(onSelectItemCallback)

```

## RemoveAnimEndCallback

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  移除动画播放结束后的回调

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| animName | str | 动画的名称，请不要包含动画的命名空间 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否移除成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
# 移除属性动画offset_animation播放结束后的回调
control.RemoveAnimEndCallback("offset_animation")

```

## RemoveAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  删除单一属性的动画，删除后的值与当前状态有关，建议删除后重新设置该属性值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 要删除动画的属性名称，无默认值，值必须为单一属性（不能填"all"） |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否删除成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
# 删除 位移 属性上的动画
control.RemoveAnimation("offset")

```

## RemoveEntityMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  删除实体位置标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| entityId | str | 实体Id |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否删除成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.RemoveEntityMarker(entityId)

```

## RemoveEntityTextMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  在小地图上删除实体文本标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| entityId | str | 实体Id |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否删除成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.RemoveEntityTextMarker(entityId)

```

## RemoveOptionByIndex

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据提供的索引移除对应下拉框项，移除成功则返回True，否则返回False

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| index | int | 索引 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否移除成功 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
success = comboBoxUIControl.RemoveOptionByIndex(0)

```

## RemoveOptionByShowName

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据提供的展示文本移除对应下拉框项，移除成功则返回True，否则返回False

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| showName | str | 展示文本 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否移除成功 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
success = comboBoxUIControl.RemoveOptionByShowName("测试项")

```

## RemoveStaticMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  删除静态位置标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| key | str | 标记的Id |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否删除成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.RemoveStaticMarker(entityId)

```

## RemoveStaticTextMarker

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  在小地图上删除静态文本标记

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| key | str | 标记Id |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否删除成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.RemoveStaticTextMarker("this_is_marker_key")

```

## RenderBlockGeometryModel

客户端

method in mod.client.ui.controls.neteasePaperDollUIControl.NeteasePaperDollUIControl

- 描述

  渲染网格体模型

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| params | dict | 渲染参数，详细说明请见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 备注

  - 网格体模型使用CombineBlockPaletteToGeometry生成
  - 每次进入游戏需要重新调用本接口渲染网格体模型，可使用SerializeBlockPalette和DeserializeBlockPalette实现调色板保存并重载调色板，重新生成网格体模型进行渲染。
  - RenderBlockGeometryModel参数params说明：
    params参数解释如下：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| block_geometry_model_name | str | 网格体模型名称, 可用CombineBlockPaletteToGeometry返回值 |
| scale | float | 渲染缩放比例，默认为1.0 |
| init_rot_y | float | 初始Y方向的朝向 |
| init_rot_x | float | 初始x方向的朝向 |
| init_rot_z | float | 初始z方向的朝向 |
| molang_dict | dict | molang表达式字典，其中key为str，value为float |
| rotation_axis | tuple | 选择旋转环绕的轴，如(0,1,0)依次代表x、y、z轴，此为绕y轴旋转，该属性只在netease_paper_doll_renderer控件的rotation属性为“freedom_gesture”时起效 |

- 示例

```python
import mod.client.extraClientApi as clientApi
path = '/demoPanel/paper_doll'
param = {
    "block_geometry_model_name": "my_geometry_model",
    "scale": 0.5,
    "init_rot_y": 60,
    "rotation_axis":(0,1,1),
    "molang_dict": {"variable.liedownamount": 1}
}
doll = uiNode.GetBaseUIControl(path).asNeteasePaperDoll()
doll.RenderBlockGeometryModel(param)

```

## RenderEntity

客户端

method in mod.client.ui.controls.neteasePaperDollUIControl.NeteasePaperDollUIControl

- 描述

  渲染实体

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| params | dict | 渲染参数，详细说明请见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 备注

  - RenderEntity参数params说明：
    params参数解释如下：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| entity_id | str | 渲染生物的实体Id，与实体的identifier二者选其一即可。如果与entity_identifier同时定义，则优先使用entity_id。 |
| entity_identifier | str | 渲染生物的identifier，与实体Id二者选其一即可。如果与entity_id同时定义，则优先使用entity_id。 |
| scale | float | 渲染缩放比例，默认为1.0 |
| render_depth | int | 渲染深度，对于玩家默认-50，普通生物-15，该参数可解决UI遮挡剔除问题 |
| init_rot_y | float | 初始Y方向的朝向 |
| init_rot_x | float | 初始x方向的朝向 |
| init_rot_z | float | 初始z方向的朝向 |
| molang_dict | dict | molang表达式字典，其中key为str，value为float |
| rotation_axis | tuple | 选择旋转环绕的轴，如(0,1,0)依次代表x、y、z轴，此为绕y轴旋转，该属性只在netease_paper_doll_renderer控件的rotation属性为“freedom_gesture”时起效 |

- 示例

```python
path = '/demoPanel/paper_doll'
# 使用entity_id进行渲染
param = {
    "entity_id": "-8589934591",
    "scale": 0.5,
    "render_depth": -50,
    "init_rot_y": 60,
    "rotation_axis":(0,1,1),
    "molang_dict": {"variable.liedownamount": 1}
}
doll = uiNode.GetBaseUIControl(path).asNeteasePaperDoll()
doll.RenderEntity(param)
# 使用identifier进行渲染
param = {
    "entity_identifier": "minecraft:cow",
    "scale": 0.5,
    "render_depth": -50,
    "init_rot_y": 60,
    "rotation_axis":(0,1,1),
    "molang_dict": {"variable.liedownamount": 1}
}
doll = uiNode.GetBaseUIControl(path).asNeteasePaperDoll()
doll.RenderEntity(param)

```

## RenderSkeletonModel

客户端

method in mod.client.ui.controls.neteasePaperDollUIControl.NeteasePaperDollUIControl

- 描述

  渲染骨骼模型（不依赖实体）

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| params | dict | 渲染参数，详细说明请见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 备注

  - RenderSkeletonModel参数params说明：
    params参数解释如下：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| skeleton_model_name | str | 骨骼模型名称 |
| animation | str | 骨骼动作名称，默认为idle |
| animation_looped | bool | 骨骼动作是否循环播放，默认True |
| scale | float | 渲染缩放比例，默认为1.0 |
| render_depth | int | 渲染深度，对于玩家默认-50，普通生物-15，该参数可解决UI遮挡剔除问题 |
| init_rot_y | float | 初始Y方向的朝向 |
| init_rot_x | float | 初始x方向的朝向 |
| init_rot_z | float | 初始z方向的朝向 |
| molang_dict | dict | molang表达式字典，其中key为str，value为float |
| rotation_axis | tuple | 选择旋转环绕的轴，如(0,1,0)依次代表x、y、z轴，此为绕y轴旋转，该属性只在netease_paper_doll_renderer控件的rotation属性为“freedom_gesture”时起效 |
| light_direction | tuple | 可选参数。控制骨骼模型在纸娃娃中显示时的光照方向，x控制光照的左右方向，y控制光照的上下方向，z控制光照的前后方向，取值为-1,0,1。不填写该参数时模型默认从底部打光。该属性仅对没有自定义材质的骨骼模型生效。如果该骨骼模型定义了自定义材质，则无效。如需要对使用自定义材质的骨骼模型控制其光照方向，可以参考官方骨骼模型vertex shader中getLightColor使用到HIDE_COLOR的这部分代码。该参数对部分安卓低端设备，极低端设备无效。 |

- 示例

```python
import mod.client.extraClientApi as clientApi
path = '/demoPanel/paper_doll'
param = {
    "skeleton_model_name": "ty_yuanshenghuli_0_0",
    "animation": "idle_stand",
    "scale": 0.5,
    "render_depth": -50,
    "init_rot_y": 60,
    "rotation_axis":(0,1,1),
    "molang_dict": {"variable.liedownamount": 1},
    # 控制光照方向为从模型的正右边往左边打光，以及从模型的正前方往后打光
    "light_direction": ( 1.0, 0.0, 1.0 )
}
doll = uiNode.GetBaseUIControl(path).asNeteasePaperDoll()
doll.RenderSkeletonModel(param)

```

## RepaintMiniMap

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  重新绘制小地图

- 参数

  无

- 返回值

  无

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.RepaintMiniMap()

```

## Rotate

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  图片相对自身的旋转锚点进行旋转

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| angle | float | 旋转角度 |

- 返回值

  无

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageControl.Rotate(30)

```

## RotateAround

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  图片相对全局坐标系中某个固定的点进行旋转

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| point | `tuple(float,float)` | 全局坐标点 |
| angle | float | 旋转角度 |

- 返回值

  无

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageControl.RotateAround((50, 50), 30)

```

## SetAlpha

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置节点的透明度，仅对image和label控件生效

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| alpha | float | 透明度，取值0-1之间，0表示完全透明，1表示完全不透明 |

- 返回值

  无

- 示例

```python
# 设置text2的透明度为半透明
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
baseUIControl.SetAlpha(0.5)

```

## SetAnchorFrom

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件相对于父节点的锚点

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| anchorFrom | str | 相对于父节点的锚点，可选的值详见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - anchorFrom可选的值

| 值 | 解释 |
| --- | --- |
| "top_left" | 相对于父节点的左上角 |
| "top_middle" | 相对于父节点的上边中间 |
| "top_right" | 相对于父节点的右上角 |
| "left_middle" | 相对于父节点的左边中间 |
| "center" | 相对于父节点的中间 |
| "right_middle" | 相对于父节点的右边中间 |
| "bottom_left" | 相对于父节点的底部左边 |
| "bottom_middle" | 相对于父节点的部中间 |
| "bottom_right" | 相对于父节点的底部右边 |

- 示例

```python
# 设置image1控件相对于父节点的左上角
imagePath = "/panel/image1"
anchorFrom = "top_left"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetAnchorFrom(anchorFrom)

```

## SetAnchorTo

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件自身锚点位置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| anchorTo | str | 控件自身锚点位置，可选的值详见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - anchorTo可选的值

| 值 | 解释 |
| --- | --- |
| "top_left" | 自身锚点位于左上角 |
| "top_middle" | 自身锚点位于上边中间 |
| "top_right" | 自身锚点位于右上角 |
| "left_middle" | 自身锚点位于左边中间 |
| "center" | 自身锚点位于中间 |
| "right_middle" | 自身锚点位于右边中间 |
| "bottom_left" | 自身锚点位于底部左边 |
| "bottom_middle" | 自身锚点位于底部中间 |
| "bottom_right" | 自身锚点位于底部右边 |

- 示例

```python
# 设置image1控件锚点到自身中间
imagePath = "/panel/image1"
anchorTo = "center"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetAnchorTo(anchorTo)

```

## SetAnimEndCallback

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置动画播放结束后的回调，每次设置都会覆盖上一次的设置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| animName | str | 动画的名称，请不要包含动画的命名空间 |
| func | function | 回调，无参数无返回值的函数 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")

def callback():
    print "offset_animation end"

# 设置属性动画offset_animation播放结束后的回调
control.SetAnimEndCallback("offset_animation", callback)

```

## SetAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  给单一属性设置动画，已有重复的会设置失败，需要先remove

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 要设置的动画的属性名称，无默认值，值必须为单一属性（不能填"all"） |
| namespace | str | 动画的命名空间，类似于自定义控件，动画也是可以定义到某个命名空间的，详见备注 |
| animName | str | 动画的名称，详见备注 |
| autoPlay | bool | 动画添加后是否自动播放，默认值为False，表示不进行播放 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否添加成功 |

- 备注

  - UI属性动画相关，详见属性动画
  - 需要注意，设置动画本质上是拷贝动画数据到控件的动画组件里面，所以如果动画数据发生了改变（比如通过接口RegisterUIAnimations修改了动画数据），如果想要控件应用修改需要再次调用setAnimation进行更新

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")

# 如果动画不在json中定义，也可以通过接口进行动态注册
data = {
    "namespace": "CustomAnimations",
    "alpha_animation_1": {
        "anim_type": "alpha",
        "duration": 1.5,
        "from": 0,
        "to": 1,
        "next": "@namespace.alpha_animation_2"
    },
    "alpha_animation_2": {
        "anim_type": "alpha",
        "duration": 1.5,
        "from": 1,
        "to": 0,
        "next": "@namespace.alpha_animation_1"
    }
}
import mod.client.extraClientApi as clientApi
clientApi.RegisterUIAnimations(data, True)

# 给 alpha属性 添加 透明度动画
control.SetAnimation("alpha", "CustomAnimations", "alpha_animation_1")

```

## SetButtonHoverInCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置鼠标进入按钮时触发的悬浮回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 将鼠标移入添加了悬浮回调的控件中会触发该事件，该鼠标指PushScreen生成UI后显示的鼠标，F11生成的鼠标无法生效
  - OnButtonHoverInCallback接收到的参数：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| isHoverIn | int | 鼠标是否为进入悬浮回调，1为移入，0为移出 |
| PrevButtonDownID | str | 上一个被点击Down的按钮的ID，如果没有取值为"-1" |
| TouchPosX | float | 鼠标进入按钮时屏幕上的UI坐标X值 |
| TouchPosY | float | 鼠标进入按钮时屏幕上的UI坐标Y值 |
| ButtonPath | str | 鼠标进入的按钮的ComponentPath |
| AddHoverEventParams | dict | 在调用AddHoverEventParams接口时传入的参数字典 |

- 示例

```python
def onButtonHoveInCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddHoverEventParams()
buttonUIControl.SetButtonHoverInCallback(onButtonHoveInCallback)

```

## SetButtonHoverOutCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置鼠标退出按钮时触发的悬浮回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 将鼠标移出添加了悬浮回调的控件中会触发该事件，该鼠标指PushScreen生成UI后显示的鼠标，F11生成的鼠标无法生效
  - OnButtonHoverInCallback接收到的参数：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| isHoverIn | int | 鼠标是否为进入悬浮回调，1为移入，0为移出 |
| PrevButtonDownID | str | 上一个被点击Down的按钮的ID，如果没有取值为"-1" |
| ButtonPath | str | 鼠标退出的按钮的ComponentPath |
| AddHoverEventParams | dict | 在调用AddHoverEventParams接口时传入的参数字典 |

- 示例

```python
def OnButtonHoverOutCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddHoverEventParams()
buttonUIControl.SetButtonHoverOutCallback(OnButtonHoverOutCallback)

```

## SetButtonScreenExitCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置按钮所在画布退出时若鼠标仍未抬起时触发回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：

- 示例

```python
def onButtonScreenExitCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonScreenExitCallback(onButtonScreenExitCallback)

```

## SetButtonTouchCancelCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置触控在按钮范围外弹起时触发的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：

- 示例

```python
def onButtonTouchCancelCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchCancelCallback(onButtonTouchCancelCallback)

```

## SetButtonTouchDownCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置按钮按下时触发的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - onButtonTouchDownCallback参数args说明：

| 参数 | 类型 | 解释 |
| --- | --- | --- |
| #collection_name | str | 按钮所属的集合名称 |
| #collection_index | int | 按钮在集合所属的集合序号 |
| ButtonState | int | 按钮的状态：Up为0，Down为1，默认是-1，建议使用New |
| TouchEvent | int | 按钮的状态新版本：Up为0，Down为1，Cancel为3，Move为4，默认是-1 |
| PrevButtonDownID | str | 上一个被点击Down的按钮的ID，如果没有取值为"-1" |
| TouchPosX | float | 按钮被点击时屏幕上的UI坐标X值 |
| TouchPosY | float | 按钮被点击时屏幕上的UI坐标Y值 |
| ButtonPath | str | 被点击的按钮的ComponentPath |
| AddTouchEventParams | dict | 在调用AddTouchEventParams接口时传入的参数字典 |

```
事件之间的关系如下图所示：
![事件说明](data:image/png;base64,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)
```

- 示例

```python
def onButtonTouchDownCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchDownCallback(onButtonTouchDownCallback)

```

## SetButtonTouchMoveCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置按下后触控移动时触发的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：
  - 该事件只在触屏模式下触发

- 示例

```python
def onButtonTouchMoveCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchMoveCallback(onButtonTouchMoveCallback)

```

## SetButtonTouchMoveInCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置按下按钮后进入控件时触发的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：
  - 键鼠模式下，鼠标移入会触发该事件。触屏模式下，鼠标按下按钮会触发该事件

- 示例

```python
def onButtonTouchMoveInCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchMoveInCallback(onButtonTouchMoveInCallback)

```

## SetButtonTouchMoveOutCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置按下按钮后退出控件时触发的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：
  - 键鼠模式下，鼠标移出会触发该事件。触屏模式下，鼠标松开按钮会触发该事件

- 示例

```python
def onButtonTouchMoveOutCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchMoveOutCallback(onButtonTouchMoveOutCallback)

```

## SetButtonTouchUpCallback

客户端

method in mod.client.ui.controls.buttonUIControl.ButtonUIControl

- 描述

  设置触控在按钮范围内弹起时的回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数 |

- 返回值

  无

- 备注

  - 其他相关说明见SetButtonTouchDownCallback接口：

- 示例

```python
def onButtonTouchUpCallback(args):
    pass

buttonPath = "/panel/test_btn"
buttonUIControl = uiNode.GetBaseUIControl("/panel/test_btn").asButton()
buttonUIControl.AddTouchEventParams({"isSwallow":True})
buttonUIControl.SetButtonTouchUpCallback(onButtonTouchUpCallback)

```

## SetClipDirection

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片控件的裁剪方向

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| clipDirection | str | 图片控件的裁剪方向，可选的值详见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - clipDirection可选的值

| 值 | 解释 |
| --- | --- |
| "fromLeftToRight" | 从左到右 |
| "fromRightToLeft" | 从右到左 |
| "fromOutsideToInside" | 从外到内 |
| "fromTopToBottom" | 从上到下 |
| "fromBottomToTop" | 从下到上 |

- 示例

```python
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
clipDirection = "fromOutsideToInside"
ret = imageUIControl.SetClipDirection(clipDirection)

```

## SetClipOffset

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件的裁剪偏移信息

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| clipOffset | `tuple(float,float)` | 该控件的裁剪偏移信息，第一项为横轴，第二项为纵轴 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to set image1 maxSize
imagePath = "/panel/image1"
clipOffset = (20, 20)
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetClipOffset(clipOffset)

```

## SetClipsChildren

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件是否开启裁剪内容

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| clipsChildren | bool | True表示开启，False表示关闭 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to enable image1 clipsChildren
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetClipsChildren(True)

```

## SetCurrentSliceIndex

客户端

method in mod.client.ui.controls.selectionWheelUIControl.SelectionWheelUIControl

- 描述

  设置轮盘选择的切片

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| index | int | 轮盘选择的切片的index，取值范围为 [-1, GetSliceCount() - 1]，-1表示轮盘无选择 |

- 返回值

  无

- 示例

```python
selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()
selectionWheelControl.SetCurrentSliceIndex(-1)

```

## SetEditText

客户端

method in mod.client.ui.controls.textEditBoxUIControl.TextEditBoxUIControl

- 描述

  设置edit_box输入框的文本信息

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| text | str | 文本的内容 |

- 返回值

  无

- 示例

```python
# we want to clear edit2 content
editBoxPath = "/panel/edit2"
text = ""
textEditBoxUIControl = uiNode.GetBaseUIControl(editBoxPath).asTextEditBox()
textEditBoxUIControl.SetEditText(text)

```

## SetEditTextMaxLength

客户端

method in mod.client.ui.controls.textEditBoxUIControl.TextEditBoxUIControl

- 描述

  设置输入框的最大输入长度

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| maxLength | int | 输入框可输入的最大长度，取值\[0, +∞) |

- 返回值

  无

- 示例

```python
# we want to set text_edit_box max input length 10
editTextPath = "/panel2/text_edit_box"
textEditBoxUIControl = uiNode.GetBaseUIControl(editBoxPath).asTextEditBox()
textEditBoxUIControl.SetEditTextMaxLength(10)

```

## SetFullPosition

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件的锚点坐标（全局坐标），支持比例值以及绝对值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| axis | str | 设置的轴向，可选的值有`["x","y"]`，"x"表示设置控件锚点的x坐标，"y"表示设置控件锚点的y坐标 |
| paramDict | dict | 设置的参数，具体见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - paramDict的键值

| 参数名 | 参数类型 | 解释 |
| --- | --- | --- |
| "followType" | str | 跟随类型，默认值为"none"，可以不写该键值 |
| "relativeValue" | float | 相对于跟随控件的比例值，默认值为0，可以不写该键值 |
| "absoluteValue" | float | 设置的绝对值，默认值为0，可以不写该键值 |

- 控件的大小支持复杂的计算来实现自适应布局，形式为“absoluteValue + relativeValue * 跟随值”。
  其中跟随值由跟随的控件以及当前设置的属性共同决定。比如当前设置的是控件的x坐标，并且跟随控件为父控件，而跟随值为父控件的宽度。
  而跟随的控件是特定的，是与控件本身有一定关系的，比如控件的父控件，子控件等，可以通过设置followType的值来指定。
- followType可选的值

| 值 | 解释 |
| --- | --- |
| "none" | 无跟随，实际计算的时候只考虑absoluteValue |
| "parent" | 跟随控件为父控件 |
| "maxChildren" | 跟随控件为最大子控件（设置x则为最大宽度，设置y则为最大高度），relativeValue无论如何设置都为1.0 |
| "maxSibling" | 跟随控件为最大兄弟控件（设置x则为最大宽度，设置y则为最大高度），relativeValue无论如何设置都为1.0 |
| "children" | 跟随值等于所有子节点之和（如果是x则计算的是子节点的宽度之和，如果是y则计算的是子节点的高度之和） |
| "x" | 跟随值等于控件本身的宽度，该值仅当 axis == "y" 才生效 |
| "y" | 跟随值等于控件本身的高度，该值仅当 axis == "x" 才生效 |

- 设置跟随类型的时候请务必小心，不要造成依赖循环，比如父控件x坐标依赖子控件的宽度，而子控件的宽度又依赖于父控件这类情况，这样即使设置成功，结果也是未知的
- 示例

```python
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
# 设置image1的位置在父控件的中间
ret = baseUIControl.SetFullPosition(axis="x", paramDict={"followType":"parent", "relativeValue":0.5})
ret = baseUIControl.SetFullPosition(axis="y", paramDict={"followType":"parent", "relativeValue":0.5})

```

## SetFullSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件的大小，支持比例形式以及绝对值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| axis | str | 设置的轴向，可选的值有`["x","y"]`，"x"表示设置控件的宽度，"y"表示设置控件的高度 |
| paramDict | dict | 设置的参数，具体见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - paramDict的键值

| 参数名 | 参数类型 | 解释 |
| --- | --- | --- |
| "fit" | bool | 是否自适应父控件，默认值为False，可以不写该键值 |
| "followType" | str | 跟随类型，默认值为"none"，可以不写该键值 |
| "relativeValue" | float | 相对于跟随控件的比例值，默认值为0，可以不写该键值 |
| "absoluteValue" | float | 设置的绝对值，默认值为0，可以不写该键值 |

- 控件的大小支持复杂的计算来实现自适应布局，形式为“absoluteValue + relativeValue * 跟随值”。
  其中跟随值由跟随的控件以及当前设置的属性共同决定。比如当前设置的是控件的宽度，并且跟随控件为父控件，则跟随值为父控件的宽度。
  而跟随的控件是特定的，是与控件本身有一定关系的，比如控件的父控件，子控件等，可以通过设置followType的值来指定。
- followType可选的值

| 值 | 解释 |
| --- | --- |
| "none" | 无跟随，实际计算的时候只考虑absoluteValue |
| "parent" | 跟随控件为父控件 |
| "maxChildren" | 跟随控件为最大子控件（设置宽度则为最大宽度，设置高度则为最大高度），relativeValue无论如何设置都为1.0 |
| "maxSibling" | 跟随控件为最大兄弟控件（设置宽度则为最大宽度，设置高度则为最大高度），relativeValue无论如何设置都为1.0 |
| "children" | 跟随值等于所有子节点之和 |
| "x" | 跟随值等于控件本身的宽度，该值仅当 axis == "y" 才生效 |
| "y" | 跟随值等于控件本身的高度，该值仅当 axis == "x" 才生效 |

- fit参数用来指定是否自适应父控件，如果是自适应父控件，则absoluteValue，followType，relativeValue参数均会失效，控件的值直接取自父控件的值
- 设置跟随类型的时候请务必小心，不要造成依赖循环，比如父控件宽度依赖子控件的宽度，而子控件的宽度又依赖于父控件这类情况，这样即使设置成功，结果也是未知的
- 示例

```python
imagePath = "/panel/image1"
baseUIControl = uiNode.GetBaseUIControl(imagePath)
# 设置image1的宽度为父节点的一半，
ret = baseUIControl.SetFullSize(axis="x", paramDict={"followType":"parent", "relativeValue":0.5})
# 设置image1的高度为所有子节点的高度的2倍并且多20像素
ret = baseUIControl.SetFullSize(axis="y", paramDict={"absoluteValue":20, "followType":"children", "relativeValue":2})
# 设置image1的宽，高为父亲节点的宽，高
ret = baseUIControl.SetFullSize(axis="x", {"fit":True})
ret = baseUIControl.SetFullSize(axis="y", {"fit":True})

```

## SetGridDimension

客户端

method in mod.client.ui.controls.gridUIControl.GridUIControl

- 描述

  设置Grid控件的大小

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| dimension | `tuple(int,int)` | 设置网格的横向与纵向大小 |

- 返回值

  无

- 示例

```python
# we want change grid dimension
gridPath = "/grid1"
gridUIControl = uiNode.GetBaseUIControl(gridPath).asGrid()
gridUIControl.SetGridDimension((2, 2))

```

## SetHighestY

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  设置绘制地图的最大高度

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| highestY | int | 绘制高度值 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 备注

  - 动态调整高度值后，已经绘制过的区块不会刷新为新的高度值，只有没有绘制过的区块会以新的高度值来绘制。

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.SetHighestY(255)

```

## SetHoverCallback

客户端

method in mod.client.ui.controls.selectionWheelUIControl.SelectionWheelUIControl

- 描述

  设置轮盘选择切片触发回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数，并且无参数 |

- 返回值

  无

- 示例

```python
def onSelectionWheelHover():
    pass

selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()
selectionWheelControl.SetHoverCallback(onSelectionWheelHover)

```

## SetImageAdaptionType

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片控件的图片适配方式以及信息

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| imageAdaptionType | str | 图片控件的图片适配方式，可选的值详见备注 |
| imageAdaptionData | `tuple(float,float,float,float)` | 如果图片不是九宫适配方式，无需传该值，否则需要设置，tuple的每个值分别代表九宫格切割的四个参数：左，右，上，下 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - imageAdaptionType可选的值如下

| 值 | 解释 |
| --- | --- |
| "normal" | 普通适配，不开启九宫并保持宽高比 |
| "filled" | 填充适配，不开启九宫并保持宽高比 |
| "oldNineSlice" | 旧版九宫格适配 |
| "originNineSlice" | 原版九宫格适配 |

- 示例

```python
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
# 普通适配
ret = imageUIControl.SetImageAdaptionType("normal")
# 原版九公格适配，并设置边距
ret = imageUIControl.SetImageAdaptionType("originNineSlice", (0, 0, 0, 0))

```

## SetIsModal

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  设置当前面板是否为模态框

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isModal | bool | 当前面板是否为模态框 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to set inputPanel1 isModal
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
ret = inputPanel.SetIsModal(True)

```

## SetIsSwallow

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  设置当前面板输入是否会吞噬事件，isSwallow为Ture时，点击时，点击事件不会穿透到世界。如破坏方块、镜头转向不会被响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isSwallow | bool | 设置当前面板输入是否会吞噬事件 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to set inputPanel1 isSwallow
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
ret = inputPanel.SetIsSwallow(True)

```

## SetLayer

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件节点的层级，可以通过传入空字符串（""）的方式来调整整个JSON的基础层级

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| layer | int | 设置层级 |
| syncRefresh | bool | 是否需要同步刷新，默认值为True。置True为游戏在同一帧根据该控件的层级进行相关计算，置False则在下一帧进行计算。如同一帧有大量SetLayer操作建议置False，操作结束后调用一次ScreenNode.UpdateScreen接口刷新界面及相关控件数据 |
| forceUpdate | bool | 是否需要强制刷新，默认值为True。置True则按照syncRefresh逻辑进行同步或者下一帧刷新，置False则当前帧和下一帧均不刷新，需要手动调用UpdateScreen进行刷新。如有大量SetLayer操作且非在同一帧执行，建议设置为False,需要更新时再调用UpdateScreen接口刷新界面及相关控件数据 |

- 返回值

  无

- 示例

```python
# 设置text2的控件的层级
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
baseUIControl.SetLayer(2)
# 同一帧设置若干控件的层级
text1Path = "/panel/text1"
uiNode.GetBaseUIControl(text1Path).SetLayer(1,False)
text2Path = "/panel/text2"
uiNode.GetBaseUIControl(text2Path).SetLayer(2,False)
text3Path = "/panel/text3"
uiNode.GetBaseUIControl(text3Path).SetLayer(3,False)
# 不同帧设置若干控件的层级
def _setLyerText1(path, layer):
    uiNode.GetBaseUIControl(text1Path).SetLayer(1,False,False)
def _updateScreen():
    uiNode.UpdateScreen(True)
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
comp.AddTimer(100, _setLyerText1, "/panel/text1", 1)
comp.AddTimer(200, _setLyerText1, "/panel/text2", 2)
comp.AddTimer(300, _setLyerText1, "/panel/text3", 3)
comp.AddTimer(300, _updateScreen)

```

## SetMaxSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件所允许的最大的大小值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| maxSize | `tuple(float,float)` | 该控件所允许的最大的大小值，第一项为横轴，第二项为纵轴 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - maxSize = (0, 0)表示无限制

- 示例

```python
# we want to set image1 maxSize
imagePath = "/panel/image1"
imageMaxSize = (10, 10)
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetMaxSize(imageMaxSize)

```

## SetMinSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件所允许的最小的大小值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| minSize | `tuple(float,float)` | 该控件所允许的大小值，第一项为横轴，第二项为纵轴 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - minSize = (0, 0)表示无限制

- 示例

```python
# we want to set image1 minSize
imagePath = "/panel/image1"
imageMinSize = (10, 10)
baseUIControl = uiNode.GetBaseUIControl(imagePath)
ret = baseUIControl.SetMinSize(imageMinSize)

```

## SetOffsetDelta

客户端

method in mod.client.ui.controls.inputPanelUIControl.InputPanelUIControl

- 描述

  设置点击面板的拖拽偏移量

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| offset_delta | `tuple(float,float)` | 该控件的拖拽偏移量，第一项为横轴，第二项为纵轴 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 |

- 备注

  - 点击面板拖拽功能及拖拽偏移量相关，详见InputPanel

- 示例

```python
# we want to set inputPanel's offset_delta
inputPanelPath = "/inputPanel1"
inputPanel = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()
success = inputPanel.SetOffsetDelta((100, 100))

```

## SetOrientation

客户端

method in mod.client.ui.controls.stackPanelUIControl.StackPanelUIControl

- 描述

  设置stackPanel的排列方向

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| orientation | str | stackPanel的排列方向，取值有限为`["vertical", "horizontal"]`，分别表示垂直方向，水平方向 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to set stackPanel1 orientation
stackPanelPath = "/stackPanel1"
stackPanel = uiNode.GetBaseUIControl(stackPanelPath).asStackPanel()
orientation = "vertical"
stackPanel.SetOrientation(orientation)

```

## SetPosition

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件相对父节点的坐标

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| pos | `tuple(float,float)` | 该控件相对父节点的坐标信息，第一项为横轴，第二项为纵轴 |

- 返回值

  无

- 示例

```python
# we want to set text2 position
text2Path = "/panel/text2"
pos = (10, 10)
baseUIControl = uiNode.GetBaseUIControl(text2Path)
baseUIControl.SetPosition(pos)

```

## SetPropertyBag

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置PropertyBag,将使用字典中的每个值来覆盖原本PropertyBag中的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| params | dict | 需要设置的属性字典，字典键值对的值仅支持设置字符串、数字、布尔值类型，其余类型会导致设置失败 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 |

- 示例

```python
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
params = {"text": "hello world", "number": 123, "boolean": True}
success = baseUIControl.SetPropertyBag(params)

```

## SetRotatePivot

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片自身旋转锚点，该点并不是固定的点，而是相对于自身位置的点

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| pivot | `tuple(float,float)` | 相对于自身位置的锚点，第一项为横轴x，第二项为纵轴y，锚点实际坐标=图片的position + anchor * (width, height) |

- 返回值

  无

- 备注

  - 自身的旋转锚点是一个相对坐标，它是根据图片当前所在位置和大小进行计算的，所以一旦设置后，就会每帧都进行位置的计算。如果不调用该函数，默认状态下图片的旋转锚点是 (0.5, 0.5)

- 示例

```python
imagePath = "/imagePath"
imageControl = uiNode.GetBaseUIControl(imagePath).asImage()
# 设置旋转锚点为图片的左上角
imageControl.SetRotatePivot((0, 0))
# 设置旋转锚点为图片的右上角
imageControl.SetRotatePivot((1, 0))
# 设置旋转锚点为图片的右下角
imageControl.SetRotatePivot((1, 1))
# 设置旋转锚点为图片的左下角
imageControl.SetRotatePivot((0, 1))

```

## SetScrollViewPercentValue

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  设置当前scroll_view内容的百分比位置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| percent_value | int | 需要跳转到的百分比位置，一般设置的位置会出现在scroll_view的最上方。该值取值范围0-100 |

- 返回值

  无

- 示例

```python
# we want set scroll_view percent pos
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
scrollViewUIControl.SetScrollViewPercentValue(20)

```

## SetScrollViewPos

客户端

method in mod.client.ui.controls.scrollViewUIControl.ScrollViewUIControl

- 描述

  设置当前scroll_view内容的位置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| pos | float | 需要跳转到的位置，一般设置的位置会出现在scroll_view的最上方。 |

- 返回值

  无

- 示例

```python
# we want set scroll_view pos
scrollViewPath = "/scroll_view0"
scrollViewUIControl = uiNode.GetBaseUIControl(scrollViewPath).asScrollView()
scrollViewUIControl.SetScrollViewPos(100)

```

## SetSelectOptionByIndex

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据提供的索引选中对应下拉框项

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| index | int | 索引 |

- 返回值

  无

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.SetSelectOptionByIndex(0)

```

## SetSelectOptionByShowName

客户端

method in mod.client.ui.controls.neteaseComboBoxUIControl.NeteaseComboBoxUIControl

- 描述

  根据提供的展示文本选中对应下拉框项

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| name | str | 索引 |

- 返回值

  无

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()
comboBoxUIControl.SetSelectOptionByShowName("测试项")

```

## SetSize

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件的大小

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| size | `tuple(float,float)` | 该控件的大小信息，第一项为横轴，第二项为纵轴 |
| resizeChildren | bool | 是否同时调整子控件尺寸，默认为False |

- 返回值

  无

- 示例

```python
# we want to set text2 size
text2Path = "/panel/text2"
text2Size = (10, 10)
baseUIControl = uiNode.GetBaseUIControl(text2Path)
baseUIControl.SetSize(text2Size)

```

## SetSliderValue

客户端

method in mod.client.ui.controls.sliderUIControl.SliderUIControl

- 描述

  设置滑动条的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| value | float | 滑动条的值 |

- 返回值

  无

- 示例

```python
# we want to set slider value
sliderPath = "/panel/slider0"
sliderUIControl = uiNode.GetBaseUIControl(sliderPath).asSlider()
sliderUIControl.SetSliderValue(0.5) # 百分比滑动条
# sliderUIControl.SetSliderValue(5) # 固定格类型滑动条

```

## SetSprite

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  给图片控件换指定贴图

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| texturePath | str | 贴图的路径，需要从resource_pack下面的textures目录开始 |

- 返回值

  无

- 备注

  - 给ImageButton换贴图的时候注意使用子控件路径(default / hover / pressed)获得ImageUIControl

- 示例

```python
# we want to set image_button textures
imageButtonPath = "/image_button"
buttonUIControl = uiNode.GetBaseUIControl(imageButtonPath).asButton()
buttonDefaultUIControl = buttonUIControl.GetChildByName("default").asImage()
buttonHoverUIControl = buttonUIControl.GetChildByName("hover").asImage()
buttonPressedUIControl = buttonUIControl.GetChildByName("pressed").asImage()
buttonDefaultUIControl.SetSprite("textures/button01_default")
buttonHoverUIControl.SetSprite("textures/button01_hover")
buttonPressedUIControl.SetSprite("textures/button01_pressed")

```

## SetSpriteClipRatio

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片的裁剪区域比例（不改变控件尺寸）。可以配合image控件的clip_ratio属性控制方向。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| clipRatio | float | 图片的裁剪比例（范围0到1），裁剪精度与图片分辨率相关 |

- 返回值

  无

- 示例

```python
# 我们想对UI编辑器创建的进度条设置进度
# 编辑器进度条包含两个image控件（名为empty_progress_bar和filled_progress_bar）
progress = 0.8  # 用于模拟进度时，下面的裁剪比例需设为(1-进度)，才能得到正确的视觉效果
imagePath = "/progress_bar0/filled_progress_bar"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpriteClipRatio(1.0 - progress)

```

## SetSpriteColor

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片颜色

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| color | `tuple(float,float,float)` | 图片颜色rgb，取值`[0, 1]` |

- 返回值

  无

- 示例

```python
# 我们想对耐久度条随耐久度变化颜色，满的时候为绿色，空的时候为红色，其中barPath为耐久度条路径
durabilityRatio = 0.8  # 耐久度比例，1为满耐久
barPath = "/image_bar"
imageUIControl = uiNode.GetBaseUIControl(barPath).asImage()
imageUIControl.SetSpriteColor((1 - durabilityRatio, durabilityRatio, 0))

```

## SetSpriteGray

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  给图片控件置灰，比直接SetSprite一张灰图片效率要高

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| gray | bool | True为将图片置灰，False为恢复原色 |

- 返回值

  无

- 示例

```python
# we want set image gray
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpriteGray(True)

```

## SetSpritePlatformFrame

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片为我的世界移动端启动器当前帐号的头像框

- 参数

  无

- 返回值

  无

- 备注

  - 该接口仅支持移动端调用，其他平台该接口无效：
  - 当我的世界移动端启动器账号的头像框是动态头像框时，该头像框图片将无法被加载，该问题将在下个版本修复：

- 示例

```python
# 我们想将当前图片控件设置为我的世界移动端启动器当前帐号的头像框
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpritePlatformFrame()

```

## SetSpritePlatformHead

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片为我的世界移动端启动器当前帐号的头像

- 参数

  无

- 返回值

  无

- 备注

  - 该接口仅支持移动端调用，其他平台该接口无效：

- 示例

```python
# 我们想将当前图片控件设置为我的世界移动端启动器当前帐号的头像
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpritePlatformHead()

```

## SetSpriteUV

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片的起始uv，与json中的"uv"属性作用一致

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| uv | `tuple(float,float)` | 图片的左上角为(0,0)，向右为x轴，向下为y轴 |

- 返回值

  无

- 示例

```python
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpriteUV((10, 0))

```

## SetSpriteUVSize

客户端

method in mod.client.ui.controls.imageUIControl.ImageUIControl

- 描述

  设置图片的uv大小，与json中的"uv_size"属性作用一致

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| uvSize | `tuple(float,float)` | 图片向右为x轴，向下为y轴 |

- 返回值

  无

- 示例

```python
imagePath = "/image"
imageUIControl = uiNode.GetBaseUIControl(imagePath).asImage()
imageUIControl.SetSpriteUVSize((40, 30))

```

## SetText

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  设置Label的文本信息

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| text | str | 文本的内容，可以支持[样式代码 (opens new window)](https://zh.minecraft.wiki/w/%E6%A0%BC%E5%BC%8F%E5%8C%96%E4%BB%A3%E7%A0%81)（§可以设置文字的颜色、格式等，该种用法更加灵活多变） |
| syncSize | bool | 是否设置文本时同步更新文本框大小，默认值为False |

- 返回值

  无

- 示例

```python
# we want to set text2 content
text2Path = "/panel/text2"
text = "Hello World!"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.SetText(text)

```

## SetTextAlignment

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  设置文本控件的文本对齐方式

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| textAlignment | str | 文本控件的文本对齐方式，可选的值详见备注 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - textAlignment可选的值

| 值 | 解释 |
| --- | --- |
| "left" | 文本左对齐（水平方向） |
| "right" | 文本右对齐（水平方向） |
| "center" | 文本居中对齐（水平方向） |

- 示例

```python
# we want to align text2 to center
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
textAlignment = "center"
ret = labelUIControl.SetTextAlignment(textAlignment)

```

## SetTextColor

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  设置Label文本的颜色

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| color | `tuple(float,float,float)` | 文本的颜色信息(r, g, b)，取值`[0, 1]` |

- 返回值

  无

- 备注

  - 该接口不支持传入alpha通道改变透明度，建议使用SetAlpha接口

- 示例

```python
# we want to set text2 green
text2Path = "/panel/text2"
color = (0, 1, 0)
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.SetTextColor(color)

```

## SetTextFontSize

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  设置Label中文本字体的大小

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| scale | float | label的font_size的作用是Label中的默认字体大小，取值有限为[small normal large]，这个scale是在这个默认字体的基础上进行缩放字体大小，默认字体大小为1.0 |

- 返回值

  无

- 示例

```python
# set text font size
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
labelUIControl.SetTextFontSize(0.8)

```

## SetTextLinePadding

客户端

method in mod.client.ui.controls.labelUIControl.LabelUIControl

- 描述

  设置文本控件的行间距

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| textLinePadding | float | 文本控件的行间距，单位为像素 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
# we want to set text2 textLinePadding
text2Path = "/panel/text2"
labelUIControl = uiNode.GetBaseUIControl(text2Path).asLabel()
textLinePadding = 20
ret = labelUIControl.SetTextLinePadding(textLinePadding)

```

## SetToggleState

客户端

method in mod.client.ui.controls.switchToggleUIControl.SwitchToggleUIControl

- 描述

  设置Toggle开关控件的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| is_on | bool | 设置Toggle开关控件是打开还是关闭状态 |
| toggle_path | str | 实际toggle控件相对路径，由UI编辑器生成的开关控件该参数即为默认值"/this_toggle" |

- 返回值

  无

- 示例

```python
togglePath = "/toggle1"
switchToggleUIControl = uiNode.GetBaseUIControl(togglePath).asSwitchToggle()
switchToggleUIControl.SetToggleState(True)

```

## SetTouchEnable

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  设置控件是否可点击交互

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| enable | bool | False为不响应，True为恢复响应 |

- 返回值

  无

- 示例

```python
# we want to set image_button unable
imageButtonPath = "/image_button"
baseUIControl = uiNode.GetBaseUIControl(imageButtonPath)
baseUIControl.SetTouchEnable(False)

```

## SetTouchUpCallback

客户端

method in mod.client.ui.controls.selectionWheelUIControl.SelectionWheelUIControl

- 描述

  设置轮盘选择切片并且鼠标按下抬起后触发回调函数

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| callbackFunc | function | 回调函数，必须是UI的类函数，并且无参数 |

- 返回值

  无

- 示例

```python
def onSelectionWheelTouchUp():
    pass

selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()
selectionWheelControl.SetTouchUpCallback(onSelectionWheelTouchUp)

```

## SetUiItem

客户端

method in mod.client.ui.controls.itemRendererUIControl.ItemRendererUIControl

- 描述

  设置ItemRenderer控件显示的物品，ItemRenderer控件的配置方式详见控件介绍ItemRenderer

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| itemName | str | 物品identifier |
| auxValue | int | 物品附加值 |
| isEnchanted | bool | 可选参数，是否显示附魔效果，默认为False不显示 |
| userData | dict | 可选参数，如果是灾厄旗帜或焰火之星等带有userData的需要传入该参数才能正确显示，目前已知仅有灾厄旗帜和焰火之星需要传 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置结果，True为成功 |

- 示例

```python
#设置显示为羊毛wool
itemRenderPath = "/panel/item_renderer"
itemName = "minecraft:wool"
auxValue = 0
itemRendererBaseControl = uiNode.GetBaseUIControl(itemRenderPath)
itemRendererControl = itemRendererBaseControl.asItemRenderer()
itemRendererControl.SetUiItem(itemName, auxValue)

```

## SetValue

客户端

method in mod.client.ui.controls.progressBarUIControl.ProgressBarUIControl

- 描述

  设置进度条的进度

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| progress | float | 进度，取值`[0, 1]` |

- 返回值

  无

- 示例

```python
# we want to set progress
progressBarPath = "/panel/progress_bar"
progressBarUIControl = uiNode.GetBaseUIControl(progressBarPath).asProgressBar()
progressBarUIControl.SetValue(0.8)

```

## SetVisible

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  根据控件路径选择是否显示某控件，可以通过传入空字符串（""）的方式来调整整个JSON的显示/隐藏

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| visible | bool | False为隐藏该控件，True为显示该控件 |
| forceUpdate | bool | 是否需要强制刷新，默认值为True。置True则按照马上进行刷新，新的visible状态生效。置False，则需要再次调用UpdateScreen使新状态生效。如有大量SetVisible操作且非在同一帧执行，建议设置为False,需要更新时再调用UpdateScreen接口刷新界面及相关控件数据 |

- 返回值

  无

- 备注

  - 特殊情况说明：
    当该接口的调用来自滚动列表内容控件中的按钮回调，且调用目的是隐藏滚动列表控件或其父节点控件时，将forceUpdate参数置True会导致滚动列表数据异常。若没有刷新界面的必要请将forceUpdate参数置False，若有请使用定时器延迟执行手动UpdateScreen接口。

- 示例

```python
# 隐藏panel的子控件text2
text2Path = "/panel/text2"
baseUIControl = uiNode.GetBaseUIControl(text2Path)
baseUIControl.SetVisible(False)
# 在非同一帧，隐藏panel的子控件text1, text2, text3
parentPath = "/panel"
textPrefabPath = "/panel/text_prefab"
def _tickSetVisible(pathName):
    baseUIControl = uiNode.GetBaseUIControl(pathName)
    baseUIControl.SetVisible(False, False)
def _tickUpdateScreen():
    uiNode.UpdateScreen(True)
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
comp.AddTimer(100, _tickSetVisible, "/panel/text1")
comp.AddTimer(200, _tickSetVisible, "/panel/text2")
comp.AddTimer(300, _tickSetVisible, "/panel/text3")
comp.AddTimer(300, _tickUpdateScreen)

```

## StopAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  停止动画，动画将恢复到第一段动画片段的from状态

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 动画的属性名称，默认值为"all"，表示停止所有动画，不为"all"的时候表示单个动画的停止，比如propertyName=="size"时，表示停止尺寸属性上的动画 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否停止成功 |

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 获取控件
control = uiNode.GetBaseUIControl("/image")
# 停止所有动画
control.StopAnimation()
# 停止尺寸动画
# control.StopAnimation("size")
# 停止位移动画
# control.StopAnimation("offset")
# 其他属性也是支持停止的，具体可参见备注

```

## ZoomIn

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  放大地图

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| value | float | 在原有基础上的增量值，可以控制放大速度，默认为0.05 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.ZoomIn(0.2)

```

## ZoomOut

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  缩小地图

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| value | float | 在原有基础上的减少值，可以控制缩小速度，默认为0.05 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 备注

  - 客户端地图区块加载有限，如果地图UI界面太大或者缩小地图倍数太大，会导致小地图无法显示未加载的区块。

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.ZoomOut(0.2)

```

## ZoomReset

客户端

method in mod.client.ui.controls.minimapUIControl.MiniMapUIControl

- 描述

  恢复地图放缩大小为默认值

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否成功 |

- 示例

```python
path = "/demoPanel/netease_mini_map"
map = uiNode.GetBaseUIControl(path).asMiniMap()
map.ZoomReset(0.2)

```

## asButton

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为ButtonUIControl实例，如当前控件非button类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ButtonUIControl | ButtonUIControl实例 |

- 示例

```python
buttonPath = "/button"
buttonBaseControl = uiNode.GetBaseUIControl(buttonPath)
buttonControl = buttonBaseControl.asButton()

```

## asGrid

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为GridUIControl实例，如当前控件非grid类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| GridUIControl | GridUIControl实例 |

- 示例

```python
gridPath = "/grid"
gridBaseControl = uiNode.GetBaseUIControl(gridPath)
gridControl = gridBaseControl.asGrid()

```

## asImage

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为ImageUIControl实例，如当前控件非image类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ImageUIControl | ImageUIControl实例 |

- 示例

```python
imagePath = "/image"
imageBaseControl = uiNode.GetBaseUIControl(imagePath)
imageControl = imageBaseControl.asImage()

```

## asInputPanel

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为InputPanelUIControl实例，如当前控件非inputPanel类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| InputPanelUIControl | InputPanelUIControl实例 |

- 示例

```python
inputPanelPath = "/inputPanel"
inputPanelUIControl = uiNode.GetBaseUIControl(inputPanelPath).asInputPanel()

```

## asItemRenderer

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为ItemRenderer实例，如当前控件非custom类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ItemRendererUIControl | ItemRendererUIControl实例 |

- 示例

```python
itemRendererPath = "/item_renderer"
itemRendererBaseControl = uiNode.GetBaseUIControl(itemRendererPath)
itemRendererControl = itemRendererBaseControl.asItemRenderer()

```

## asLabel

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为LabelUIControl实例，如当前控件非Label类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| LabelUIControl | LabelUIControl实例 |

- 示例

```python
text1Path = "/text1"
text1BaseControl = uiNode.GetBaseUIControl(text1Path)
text1LabelControl = text1BaseControl.asLabel()

```

## asMiniMap

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为MiniMapUIControl实例，如当前控件非小地图类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| MiniMapUIControl | MiniMapUIControl实例 |

- 示例

```python
path = "/mini_map"
miniMapBaseControl = uiNode.GetBaseUIControl(path)
miniMapControl = miniMapBaseControl.asMiniMap()

```

## asNeteaseComboBox

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为NeteaseComboBoxUIControl实例，如当前控件非panel类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| NeteaseComboBoxUIControl | NeteaseComboBoxUIControl实例 |

- 示例

```python
comboBoxPath = "/panel/comboBoxPanel"
comboBoxUIControl = uiNode.GetBaseUIControl(comboBoxPath).asNeteaseComboBox()

```

## asNeteasePaperDoll

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为NeteasePaperDollUIControl实例，如当前控件非custom类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| NeteasePaperDollUIControl | NeteasePaperDollUIControl实例 |

- 示例

```python
paperDollPath = "/paper_doll"
paperDollBaseControl = uiNode.GetBaseUIControl(paperDollPath)
paperDollControl = paperDollBaseControl.asNeteasePaperDoll()

```

## asProgressBar

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为ProgressBarUIControl实例，如当前控件非panel类型则返回None

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| fillImagePath | str | 进度条填充图片路径，默认为"/filled_progress_bar",该参数影响该控件API的效果 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ProgressBarUIControl | ProgressBarUIControl实例 |

- 示例

```python
progressBarPath = "/progress_bar"
progressBarBaseControl = uiNode.GetBaseUIControl(progressBarPath)
progressBarControl = progressBarBaseControl.asProgressBar()

```

## asScrollView

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为ScrollViewUIControl实例，如当前控件非scrollview类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ScrollViewUIControl | ScrollViewUIControl实例 |

- 示例

```python
scrollViewPath = "/scroll_view"
scrollViewBaseControl = uiNode.GetBaseUIControl(scrollViewPath)
scrollViewControl = scrollviewBaseControl.asScrollView()

```

## asSelectionWheel

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为SelectionWheelUIControl实例，如当前控件非selectionWheel类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| SelectionWheelUIControl | SelectionWheelUIControl实例 |

- 示例

```python
selectionWheelPath = "/selectionWheel"
selectionWheelControl = uiNode.GetBaseUIControl(selectionWheelPath).asSelectionWheel()

```

## asSlider

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为SliderUIControl实例，如当前控件非滑动条类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| SliderUIControl | SliderUIControl |

- 示例

```python
path = "/slider0"
sliderBaseControl = uiNode.GetBaseUIControl(path)
sliderControl = sliderBaseControl.asSlider()

```

## asStackPanel

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为StackPanelUIControl实例，如当前控件非stackPanel类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| StackPanelUIControl | StackPanelUIControl实例 |

- 示例

```python
stackPanelPath = "/stackPanel"
stackPanelUIControl = uiNode.GetBaseUIControl(stackPanelPath).asStackPanel()

```

## asSwitchToggle

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为SwitchToggleUIControl实例，如当前控件非panel类型或非toggle则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| SwitchToggleUIControl | SwitchToggleUIControl实例 |

- 示例

```python
switchTogglePath = "/switch_toggle"
switchToggleBaseControl = uiNode.GetBaseUIControl(switchTogglePath)
switchToggleControl = switchToggleBaseControl.asSwitchToggle()

```

## asTextEditBox

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  将当前BaseUIControl转换为TextEditBoxUIControl实例，如当前控件非editbox类型则返回None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| TextEditBoxUIControl | TextEditBoxUIControl实例 |

- 示例

```python
textEditBoxPath = "/text_edit_box"
textEditBoxBaseControl = uiNode.GetBaseUIControl(textEditBoxPath)
textEditBoxControl = textEditBoxBaseControl.asTextEditBox()

```

## resetAnimation

客户端

method in mod.client.ui.controls.baseUIControl.BaseUIControl

- 描述

  重置该控件的动画

- 参数

  无

- 返回值

  无

- 备注

  - UI属性动画相关，详见属性动画

- 示例

```python
# 重置/text1控件上的属性动画
text1Path = "/text1"
text1Control = uiNode.GetBaseUIControl(text1Path)
text1Control.resetAnimation()

```
