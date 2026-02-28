# UI界面

## BindVirtualWorldModel

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  绑定虚拟世界中的模型

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| bindToObjId | int | 绑定的模型对象的id |
| offset | `tuple(float,float,float)` | UI与绑定实体的偏移量 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否修改成功 True:成功 False:失败 |

- 备注

  - 绑定的对象销毁时UI不会销毁，而是会隐藏起来，建议复用或者销毁

- 示例

```python
# 若是被绑定的UI需要创建多份，则需要使用此方式进行创建:
uiNode=clientApi.CreateUI("modNamespace","testUI", {
    "bindEntityId": clientApi.GetLocalPlayerId()
})
virtualWorldComp = clientApi.GetEngineCompFactory().CreateVirtualWorld(clientApi.GetLevelId())
virtualWorldComp.VirtualWorldCreate()
objId = virtualWorldComp.ModelCreateObject("datiangou", "run")
succ = uiNode.BindVirtualWorldModel(objId, (0.0, 0.0, 0.0))

```

## ChangeBindAutoScale

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置已绑定实体的UI是否根据绑定实体与本地玩家间的距离动态缩放，**只对已绑定实体的UI界面生效，如何将UI与实体绑定详见CreateUI接口**

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| autoScale | int | 1:动态缩放 0:不动态缩放 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 True:成功 False:失败 |

- 备注

  - 注意：
    绑定实体的UI缩放仅作用于根节点（比如"main"节点）下的子节点。

    建议在根节点下挂载一个panel类型的节点，然后将所有需要缩放的UI节点设置为百分比属性并挂载在这个panel节点下。

- 示例

```python
succ = uiNode.ChangeBindAutoScale(1)

```

## ChangeBindEntityId

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  修改绑定的实体id，**只对已绑定实体的UI界面生效，如何将UI与实体绑定详见CreateUI接口**

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| entityId | str | 绑定的实体id |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否修改成功 True:成功 False:失败 |

- 示例

```python
succ = uiNode.ChangeBindEntityId(entityId)

```

## ChangeBindOffset

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  修改与绑定实体之间的偏移量，**只对已绑定实体的UI界面生效，如何将UI与实体绑定详见CreateUI接口**

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| offset | `tuple(float,float,float)` | 偏移量 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否修改成功 True:成功 False:失败 |

- 备注

  - 不建议在第一人称视角下，将本地玩家绑定UI的偏移量设为(0, 0, 0)

- 示例

```python
succ = uiNode.ChangeBindOffset((0, 3, 0))

```

## Clone

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  克隆一个已有的控件，修改它的名称，并将它挂接到指定的父节点上，目前文本、图片、按钮控件的克隆控件表现正常，其他复杂控件的克隆控件可能存在运行问题，建议在json编写的过程中，手动一份对应控件使用。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始的控件路径 |
| parentPath | str | 为从main节点开始，父节点的控件路径 |
| newName | str | 为克隆成功后创建的新控件名称，新控件的路径为parentPath/newName |
| syncRefresh | bool | 是否需要同步刷新，默认值为True。置True为游戏在同一帧计算该控件的Size等相关数据，置False则在下一帧进行计算。**如同一帧有大量clone操作建议置False，操作结束后调用一次UpdateScreen接口刷新界面及相关控件数据** |
| forceUpdate | bool | 是否需要强制刷新，默认值为True。置True则按照syncRefresh逻辑进行同一帧或者下一帧刷新，置False则当前帧和下一帧均不刷新，需要手动调用UpdateScreen进行刷新。如有大量Clone操作且非在同一帧执行，建议设置为False,需要更新时再调用UpdateScreen接口刷新界面及相关控件数据 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否克隆成功 |

- 示例

```python
# 克隆text2控件，重命名为text3并以panel为父控件
parentPath = "/panel"
text2Path = "/panel/text2"
text3Name = "text3"
uiNode.Clone(text2Path, parentPath, text3Name)
# 以panel为父控件，以text_prefab为模板克隆若干控件
parentPath = "/panel"
textPrefabPath = "/panel/text_prefab"
for i in range(0, 10):
    uiNode.Clone(textPrefabPath, parentPath, "text" + str(i), False)
uiNode.UpdateScreen(True)
# 在非同一帧，以panel为父控件，text_prefab为模板克隆若干控件
parentPath = "/panel"
textPrefabPath = "/panel/text_prefab"
def _tickClone(newName):
    uiNode.Clone(textPrefabPath, parentPath, newName , False, False)
def _tickUpdateScreen():
    uiNode.UpdateScreen(True)
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
comp.AddTimer(100, _tickClone, "text1")
comp.AddTimer(200, _tickClone, "text2")
comp.AddTimer(300, _tickUpdateScreen)

```

## Create

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  UI生命周期函数，当UI创建成功时调用。

- 参数

  无

- 返回值

  无

## CreateChildControl

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  在当前画布中创建子控件，如果该子控件已经存在则返回已存在的子控件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| defName | str | UI控件路径，格式为"namespace.controlName"。namespace对应uiJson文件中"namespace"对应的值，UI编辑器生成的uiJson文件该值等于文件名。controlName对应想创建的控件的名称。 |
| childName | str | 所创建的子控件的名称 |
| parentControl | BaseUIControl | 指定所创建的子节点的父节点，默认值为None，表示直接创建在当前画布根节点下。 |
| forceUpdate | bool | 是否需要强制刷新，默认值为True。置True则进行同一帧或者下一帧刷新，置False则当前帧和下一帧均不刷新，需要手动调用UpdateScreen进行刷新。如有大量CreateChildControl操作且在同一帧执行，建议设置为False,需要更新时再调用UpdateScreen接口刷新界面及相关控件数据 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 创建的子控件节点 |

- 示例

```python
# 在父控件"/panel"下创建命名空间为"ui0"的自定义控件"myImage"，并取名为"myChild"
parentControl = uiNode.GetBaseUIControl("/panel")
if parentControl:
    childNode = uiNode.CreateChildControl("ui0.myImage", "myChild", parentControl, True)
    print childNode

```

## Destroy

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  UI生命周期函数，当UI销毁时调用。

- 参数

  无

- 返回值

  无

## GetAllChildrenPath

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取所有子节点的路径list

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| parentPath | str | 为从main节点开始，父节点的控件路径 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `list(str)` | 返回父节点下的子节点的路径，会递归返回所有子节点，若节点无子节点，返回空list |

- 示例

```python
# get panel's all children path
node.GetAllChildrenPath("/panel")

```

## GetBaseUIControl

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  根据路径获取BaseUIControl实例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| path | str | 当前控件的路径 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| BaseUIControl | 路径对应控件的BaseUIControl实例 |

- 示例

```python
# 根据路径获得BaseUIControl实例
text2Path = "/panel/text2"
text2UIControl = uiNode.GetBaseUIControl(text2Path)

```

## GetBindAutoScale

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取该绑定实体的UI是否动态缩放，未绑定的UI将传回默认值1

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 1:动态缩放 0:不动态缩放 |

- 示例

```python
autoScale = uiNode.GetBindAutoScale()

```

## GetBindEntityId

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取该UI绑定的实体id，未绑定的UI将传回默认值None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 绑定的实体id |

- 示例

```python
entityId = uiNode.GetBindEntityId()

```

## GetBindOffset

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取该UI绑定实体的偏移量，未绑定的UI将传回默认值(0, 0, 0)

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float,float)` | 偏移量 |

- 示例

```python
offset = uiNode.GetBindOffset()

```

## GetBindWorldPosition

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取该UI绑定的worldPosition，未绑定返回默认值None

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(int,tuple(float,float,float)`) | (维度,(地点))，无设置则为None |

- 示例

```python
position = uiNode.GetBindWorldPosition()

```

## GetChildrenName

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取子节点的名称list

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| parentPath | str | 为从main节点开始，父节点的控件路径 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `list(str)` | 返回父节点下的子节点的名称，不会递归返回所有子节点，若节点无子节点，返回空list |

- 示例

```python
# get panel's children name
node.GetChildrenName("/panel")

```

## GetIsHud

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获得本界面的输入模式

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| int | 返回1表示该界面不屏蔽游戏操作，返回0则屏蔽。 |

- 示例

```python
# 我们需要获得本界面的输入模式
isHud = uiNode.GetIsHud()

```

## GetRichTextItem

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  返回一个富文本控件实例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始，继承自rich_text.RichTextPanel控件的路径 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| object | RichTextItem 返回一个富文本控件实例 |

- 示例

```python
# we want get a rich-text-item
richTextPath = "/RichTextPanel"
richTextItem = uiNode.GetRichTextItem(richTextPath)

```

## GetScreenName

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获得本界面的名称

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| str | 返回本界面的名称。仅当该界面是调用PushScreen方法生成的时候才有返回值，返回值为注册UI时（调用RegisterUI）所使用的参数 uiScreenDef ，否则为 None |

- 示例

```python
# 我们需要获得本界面名称
screenName = uiNode.GetScreenName()

```

## GetSelf

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  获取零件界面自身

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| ScreenNode | 零件界面自身 |

## OnActive

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  UI生命周期函数，当UI重新回到栈顶时调用。

- 参数

  无

- 返回值

  无

- 备注

  - 不建议使用在OnDeactive函数中调用SetScreenVisible(False)，在OnActive函数中调用SetScreenVisible(True)的方式实现打开新界面时隐藏原界面，新界面关闭时自动显示原界面的功能，由于隐藏接口不会改动UI栈，多Mod容易形成冲突。推荐使用PushScreen，PopScreen接口实现。
    调用OpenPauseGui、OpenChatGui、OpenInventoryGui等原生UI函数弹出的UI被关闭时，不会调用此函数。

## OnDeactive

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  UI生命周期函数，当栈顶UI有其他UI入栈时调用。

- 参数

  无

- 返回值

  无

- 备注

  - 不建议使用在OnDeactive函数中调用SetScreenVisible(False)，在OnActive函数中调用SetScreenVisible(True)的方式实现打开新界面时隐藏原界面，新界面关闭时自动显示原界面的功能，由于隐藏接口不会改动UI栈，多Mod容易形成冲突。推荐使用PushScreen，PopScreen接口实现。
    调用OpenPauseGui、OpenChatGui、OpenInventoryGui等原生UI函数时不会调用此函数。

## RemoveChildControl

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  移除当前画布中的子控件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| childControl | BaseUIControl | 所要移除的子控件 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | True表示移除成功，False表示移除失败 |

- 示例

```python
# 移除"/panel/myChild"控件
childControl = uiNode.GetBaseUIControl("/panel/myChild")
if childControl:
    ret = uiNode.RemoveChildControl(childControl)
    print ret

```

## RemoveComponent

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  动态删除某一控件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始，被删除控件路径 |
| parentPath | str | 为从main节点开始，父节点的控件路径 |

- 返回值

  无

- 示例

```python
# we want to remove text2
text2Path = "/panel/text2"
parentPath = "/panel"
uiNode.RemoveComponent(text2Path, parentPath)

```

## SetBindWorldPosition

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置UI绑定的worldPosition

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| dimension | int | 维度id |
| position | `tuple(float,float,float)` | 坐标 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 |

- 示例

```python
uiNode.SetBindWorldPosition(0, (100,60,100))

```

## SetIsHud

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置本界面的输入模式

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHud | int | 设置1表示该界面不屏蔽游戏操作，设置0则屏蔽。 |

- 返回值

  无

- 备注

  - CreateUI的isHud参数设置为0或不传时，会在比当前UI的层级大1000的地方生成，生成后再调用SetIsHud接口无法再修改层级！

- 示例

```python
# 我们需要设置本界面为HUD操作模式
uiNode.SetIsHud(1)

```

## SetRemove

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  删除本界面节点

- 参数

  无

- 返回值

  无

- 示例

```python
# we want to remove this screen
uiNode.SetRemove()

```

## SetScreenVisible

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置是否显示本界面

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| visible | bool | False为隐藏该界面，True为显示该界面 |

- 返回值

  无

- 示例

```python
# 我们隐藏当前UI的界面
uiNode.SetScreenVisible(False)

```

## SetSelectControl

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置当前焦点所在的控件,当设置控件为文本输入框时会弹出系统小键盘

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始，所要选中控件的路径 |
| enable | bool | True为选中componentPath所代表的控件，False为取消选中 |

- 返回值

  无

- 示例

```python
path = "/text_edit_box0"
uiNode.SetSelectControl(path, True)

```

## SetStackGridCount

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置StackGrid控件的大小

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始，Grid控件的路径 |
| count | int | 设置StackGrid的内容数量 |

- 返回值

  无

- 示例

```python
# we want change stackgrid count
stackgridPath = "/stack_grid1"
uiNode.SetStackGridCount(stackgridPath, 3)

```

## SetUiEntity

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置PaperDoll控件需要显示的生物模型,PaperDoll控件的配置方式详见控件介绍PaperDoll

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始的控件路径 |
| entityIdentifier | str | 生物定义中设定的identifier |

- 返回值

  无

- 备注

  - 暂不支持的微软原版模型：minecraft:horse（马）、minecraft:donkey（驴）、minecraft:mule（骡子）、minecraft:skeleton_horse（骷髅马）、minecraft:zombie_horse（僵尸马）、minecraft:llama（羊驼）、minecraft:tropicalfish（热带鱼）、minecraft:slime（史莱姆）、minecraft:magma_cube（岩浆怪）、minecraft:ghast（恶魂）、minecraft:shulker（潜影贝）、minecraft:ender_dragon（末影龙）、minecraft:thrown_trident（三叉戟）、minecraft:ender_crystal（末影水晶）、minecraft:boat（船）、minecraft:tnt（TNT）

- 示例

```python
# we want to show an entity model
imagePath = "/paper_doll0"
uiNode.SetUiEntity(imagePath, 'minecraft:cat')

```

## SetUiModel

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置PaperDoll控件需要显示的模型,PaperDoll控件的配置方式详见控件介绍PaperDoll

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始的控件路径 |
| modelName | str | 骨骼模型的名称 |
| animateName | str | 动画名称，默认为'idle' |
| looped | bool | 是否循环播放动画，默认为True |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否设置成功 |

- 示例

```python
# we want to change model
imagePath = "/paper_doll0"
uiNode.SetUiModel(imagePath, 'saber', 'idle', True)

```

## SetUiModelScale

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  设置PaperDoll控件模型的缩放比例

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| componentPath | str | 为从main节点开始，PaperDoll控件路径 |
| scale | float | PaperDoll的缩放比例，默认为1.0 |

- 返回值

  无

- 备注

  - 当设置为原版生物模型时会导致偏移，建议开发者自行调整位置

- 示例

```python
imagePath = "/paper_doll0"
uiNode.SetUiModelScale(imagePath, 1.2)

```

## Update

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  客户端每帧调用，1秒有30帧

- 参数

  无

- 返回值

  无

## UpdateScreen

客户端

method in mod.client.ui.screenNode.ScreenNode

- 描述

  刷新界面，重新计算各个控件的相关数据

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| syncRefresh | bool | 是否需要同步刷新，默认值为True。置True为游戏在同一帧计算各个控件的相关数据，置False则在下一帧进行计算。若置True不建议在同一帧调用多次 |

- 返回值

  无

- 示例

```python
# 当前帧刷新界面
uiNode.UpdateScreen(True)
# 下一帧刷新界面
uiNode.UpdateScreen(False)

```
