# 原生UI

# 索引

包括原版HUD界面的相关接口

______________________________________________________________________

| 接口 | | 描述 |
| --- | --- | --- |
| ChangeSneakState | 客户端 | 切换潜行状态 |
| ClickInteractGUI | 客户端 | 模拟点击交互按钮，交互按钮指的在喂食、钓鱼、交易等交互场景出现的按钮 |
| GetOriginAreaOffset | 客户端 | 获取指定原生UI的offset,包括左上角和右下角 |
| GetScreenSize | 客户端 | 获取游戏分辨率 |
| GetScreenViewInfo | 客户端 | 获取游戏视角信息。首先获得当前分辨率下UI放大倍数，计算方式可参考《我的世界》界面适配方法。则当前游戏视角的宽度的计算方式为：若当前分辨率的宽度能被该放大倍数整除，则等于当前分辨率，若不能，则等于当前分辨率加放大倍数再减去当前分辨率对放大倍数求余的结果，当前游戏视角的高度计算方法类似。例：以分辨率为1792，828的手机计算，画布是分辨率的3倍，所以x = 1792 + 3 - 1 = 1794；y = 828，该接口返回的结果为(1794.0, 828.0, 0.0, 0.0) |
| GetWalkState | 客户端 | 获取玩家行走/潜行/跑步状态 |
| HideAirSupplyGUI | 客户端 | 隐藏玩家氧气值界面 |
| HideArmorGui | 客户端 | 隐藏hud界面的护甲值显示 |
| HideChangePersonGui | 客户端 | 隐藏切换人称的按钮。隐藏后点击相应位置不会响应 |
| HideChatGUI | 客户端 | 隐藏聊天按钮原生UI。该接口在开启新版聊天时不生效 |
| HideCrossHairGUI | 客户端 | 隐藏hud界面的十字准心显示 |
| HideEmoteGUI | 客户端 | 设置是否开启表情功能，默认PC端关闭，手机端开启，且该接口只能在手机端使用。该接口在开启新版聊天时不生效 |
| HideExpGui | 客户端 | 非创造者模式下隐藏经验条显示 |
| HideFoldGUI | 客户端 | 隐藏下拉按钮原生UI。 |
| HideHealthGui | 客户端 | 隐藏hud界面的血量显示 |
| HideHorseHealthGui | 客户端 | 隐藏hud界面的坐骑的血量显示 |
| HideHudGUI | 客户端 | 隐藏HUD游戏界面的游戏原生UI。与原版F1按钮效果一致，只隐藏显示，但点击跳跃键等位置依然会响应 |
| HideHungerGui | 客户端 | 隐藏hud界面的饥饿值显示 |
| HideInteractGui | 客户端 | 隐藏交互按钮。隐藏后点击相应位置不会响应 |
| HideJumpGui | 客户端 | 隐藏游戏中右下角的跳跃按钮。隐藏后点击相应位置不会响应 |
| HideMoveGui | 客户端 | 隐藏游戏中左下角的移动按钮。隐藏后点击相应位置不会响应 |
| HideNeteaseStoreGui | 客户端 | 隐藏游戏中的网易商店按钮。隐藏后点击相应位置不会响应 |
| HidePauseGUI | 客户端 | 隐藏暂停按钮原生UI。 |
| HideSlotBarGui | 客户端 | 隐藏游戏中底部中间的物品栏界面 |
| HideSneakGui | 客户端 | 隐藏游戏中左下角方向键的中心处潜行按钮。隐藏后点击相应位置不会响应 |
| HideSwimGui | 客户端 | 隐藏游戏中的浮潜按钮。隐藏后点击相应位置不会响应。 |
| HideVoiceGUI | 客户端 | 隐藏语音按钮原生UI。该接口在开启新版聊天时不生效 |
| HideWalkGui | 客户端 | 隐藏游戏中跑/走按钮。隐藏后点击相应位置不会响应 |
| OpenChatGui | 客户端 | 打开原版聊天栏 |
| OpenEmoteGui | 客户端 | 打开表情界面 |
| OpenFoldGui | 客户端 | 打开原版下拉界面 |
| OpenInventoryGui | 客户端 | 打开原版背包界面，并支持选中某个分页(支持自定义分页名称) |
| OpenNeteaseStoreGui | 客户端 | 打开游戏中的网易商店购买商品界面 |
| OpenPauseGui | 客户端 | 打开原版暂停界面 |
| OpenReportGui | 客户端 | 打开原版举报界面 |
| PlayHudHeartBlinkAnim | 客户端 | 播放原版受伤时血量变化的动效 |
| SetCrossHair | 客户端 | 设置是否使用“准星瞄准” |
| SetEmoteSwitch | 客户端 | 设置是否开启表情功能，默认PC端关闭，手机端开启，且该接口只能在手机端使用，在原生UI初始化前调用设置 |
| SetHudChatStackPosition | 客户端 | 设置HUD界面左上小聊天窗口位置 |
| SetHudChatStackVisible | 客户端 | 设置HUD界面左上小聊天窗口可见性 |
| SetResponse | 客户端 | 设置原生UI是否响应 |
| SimulateJump | 客户端 | 模拟跳跃 |

## ChangeSneakState

客户端

method in mod.client.extraClientApi

- 描述

  切换潜行状态

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.ChangeSneakState()

```

## ClickInteractGUI

客户端

method in mod.client.extraClientApi

- 描述

  模拟点击交互按钮，交互按钮指的在喂食、钓鱼、交易等交互场景出现的按钮

- 参数

  无

- 返回值

  无

- 备注

  - 交互行为

| 条件 | 交互按钮 |
| --- | --- |
| 玩家站在坐骑附近 | 【骑乘】 |
| 玩家手持打火石 | 【点燃】 |
| 玩家给羊驼、驴、骡连接箱子 | 【连接箱子】 |
| 玩家和村民交易 | 【交易】 |
| 玩家拿着栓绳对实体 | 【拴住】/【解开】 |
| 玩家手持钓竿钓鱼 | 【钓鱼】 |
| 玩家站在矿车附近 | 【乘坐】 |
| 玩家在船附近 | 【上船】/【下船】 |
| 玩家手持装备，或站在盔甲架附近 | 【装备】 |
| 玩家站在动物附近 | 【喂养】 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.ClickInteractGUI()

```

## GetOriginAreaOffset

客户端

method in mod.client.extraClientApi

- 描述

  获取指定原生UI的offset,包括左上角和右下角

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| areaEnum | str | 原生UI枚举(原生UI枚举) |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float,float,float)` | (xMin, yMin, xMax, yMax)原生UI的Area |

- 备注

  - 部分情况下获取的area值返回(0,0,0,0)，如开启了聊天插件后，聊天按钮不再支持获取，默认返回(0,0,0,0)
  - 以屏幕左上角为原点，由于原版UI使用基类画布，因此坐标为基类画布下的坐标，在适配时需要注意异形屏的情况。

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.GetOriginAreaOffset(clientApi.GetMinecraftEnum().OriginGUIName.ChatBtn)

```

## GetScreenSize

客户端

method in mod.client.component.gameCompClient.GameComponentClient

- 描述

  获取游戏分辨率

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float)` | 宽高（像素） |

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
width, height = comp.GetScreenSize()

```

## GetScreenViewInfo

客户端

method in mod.client.component.gameCompClient.GameComponentClient

- 描述

  获取游戏视角信息。首先获得当前分辨率下UI放大倍数，计算方式可参考《我的世界》界面适配方法。则当前游戏视角的宽度的计算方式为：若当前分辨率的宽度能被该放大倍数整除，则等于当前分辨率，若不能，则等于当前分辨率加放大倍数再减去当前分辨率对放大倍数求余的结果，当前游戏视角的高度计算方法类似。例：以分辨率为1792，828的手机计算，画布是分辨率的3倍，所以x = 1792 + 3 - 1 = 1794；y = 828，该接口返回的结果为(1794.0, 828.0, 0.0, 0.0)

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(float,float,float,float)` | 依次为宽、高、x偏移、y偏移 |

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
width, height, offsetX, offsetY= comp.GetScreenViewInfo()

```

## GetWalkState

客户端

method in mod.client.extraClientApi

- 描述

  获取玩家行走/潜行/跑步状态

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| WalkState | 行走/潜行/跑步状态 (行走状态) |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.GetWalkState()

```

## HideAirSupplyGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏玩家氧气值界面

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideAirSupplyGUI(True)

```

## HideArmorGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏hud界面的护甲值显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideArmorGui(True)

```

## HideChangePersonGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏切换人称的按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideChangePersonGui(True)

```

## HideChatGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏聊天按钮原生UI。该接口在开启新版聊天时不生效

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 备注

  - 该接口只对原版聊天的聊天按钮生效，如需隐藏新版聊天拓展按钮，建议使用官方聊天拓展的Disable接口，关闭新版聊天拓展功能
    如需在关闭新版聊天拓展功能后仍隐藏原版聊天的聊天按钮，需要在ClientLoadAddonsFinishServerEvent后的事件调用该接口，如OnLocalPlayerStopLoading、UiInitFinished事件

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideChatGUI(True)

```

## HideCrossHairGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏hud界面的十字准心显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideCrossHairGUI(True)

```

## HideEmoteGUI

客户端

method in mod.client.extraClientApi

- 描述

  设置是否开启表情功能，默认PC端关闭，手机端开启，且该接口只能在手机端使用。该接口在开启新版聊天时不生效

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideEmoteGUI(True)

```

## HideExpGui

客户端

method in mod.client.extraClientApi

- 描述

  非创造者模式下隐藏经验条显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideExpGui(True)

```

## HideFoldGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏下拉按钮原生UI。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideFoldGUI(True)

```

## HideHealthGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏hud界面的血量显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideHealthGui(True)

```

## HideHorseHealthGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏hud界面的坐骑的血量显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideHorseHealthGui(True)

```

## HideHudGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏HUD游戏界面的游戏原生UI。与原版F1按钮效果一致，只隐藏显示，但点击跳跃键等位置依然会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | True为隐藏原生HUD，False为恢复显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideHudGUI(True)

```

## HideHungerGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏hud界面的饥饿值显示

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideHungerGui(True)

```

## HideInteractGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏交互按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideInteractGui(True)

```

## HideJumpGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中右下角的跳跃按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideJumpGui(True)

```

## HideMoveGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中左下角的移动按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideMoveGui(True)

```

## HideNeteaseStoreGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中的网易商店按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 备注

  - 由于目前商店本身的实现，短期内无法实现纳入UIStack中管理的流程中
  - 如果需要额外操作商店界面，可考虑使用GetUI获取UINode后进行后续操作.下述UINode的namespace均为"Minecraft"

| uiKey | 解释 |
| --- | --- |
| newUIShopAsking | 余额不足界面 |
| newUIShopConfirm | 购买确认界面 |
| newUIShopFlying | 小图标飞入物品栏界面 |
| newUIShopWaiting | 购买等待界面 |
| newUIShopSingleItem | 物品购买界面 |
| newUIShopMain | 商店主界面 |
| newUIShopRecoModel1 | 推荐商品1 |
| newUIShopRecoModel2 | 推荐商品2 |
| newUIShopDesk | 商店入口按钮界面 |

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideNeteaseStoreGui(True)

```

## HidePauseGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏暂停按钮原生UI。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HidePauseGUI(True)

```

## HideSlotBarGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中底部中间的物品栏界面

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 备注

  - 受限于MC主界面结构控制，不能实现在保留经验条的情况下隐藏slotbar

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideSlotBarGui(True)

```

## HideSneakGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中左下角方向键的中心处潜行按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideSneakGui(True)

```

## HideSwimGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中的浮潜按钮。隐藏后点击相应位置不会响应。

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideSwimGui(True)

```

## HideVoiceGUI

客户端

method in mod.client.extraClientApi

- 描述

  隐藏语音按钮原生UI。该接口在开启新版聊天时不生效

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideVoiceGUI(True)

```

## HideWalkGui

客户端

method in mod.client.extraClientApi

- 描述

  隐藏游戏中跑/走按钮。隐藏后点击相应位置不会响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isHide | bool | 是否隐藏，True为隐藏，False为显示 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.HideWalkGui(True)

```

## OpenChatGui

客户端

method in mod.client.extraClientApi

- 描述

  打开原版聊天栏

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isForce | bool | 是否强制打开，默认False，当已经打开is_showing_menu为true的ui时，无法打开 |

- 返回值

  无

- 备注

  - 当已经打开is_showing_menu为true的ui时，需要设置isForce为True才能打开
  - 该接口同时会触发PlayerChatButtonClickClientEvent事件

- 示例

```python
import mod.client.extraClientApi as clientApi
#请不要在监听PlayerChatButtonClickClientEvent事件的回调函数中执行此接口,会导致聊天界面无法关闭
clientApi.OpenChatGui()

```

## OpenEmoteGui

客户端

method in mod.client.extraClientApi

- 描述

  打开表情界面

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenEmoteGui()

```

## OpenFoldGui

客户端

method in mod.client.extraClientApi

- 描述

  打开原版下拉界面

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenFoldGui()

```

## OpenInventoryGui

客户端

method in mod.client.extraClientApi

- 描述

  打开原版背包界面，并支持选中某个分页(支持自定义分页名称)

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| categoryName | str | 分页名称(原版分页名称枚举) |
| isForce | bool | 是否强制打开，默认False，当已经打开is_showing_menu为true的ui时，无法打开 |

- 返回值

  无

- 备注

  - 当已经打开is_showing_menu为true的ui时，需要设置isForce为True才能打开

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenInventoryGui(clientApi.GetMinecraftEnum().InventoryType.NATURE)

```

## OpenNeteaseStoreGui

客户端

method in mod.client.extraClientApi

- 描述

  打开游戏中的网易商店购买商品界面

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| categoryName | str | 商品分类名称 |
| itemName | str | 商品名称 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenNeteaseStoreGui("商品", "测试商品1")

```

## OpenPauseGui

客户端

method in mod.client.extraClientApi

- 描述

  打开原版暂停界面

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isForce | bool | 是否强制打开，默认False，当已经打开is_showing_menu为true的ui时，无法打开 |

- 返回值

  无

- 备注

  - 当已经打开is_showing_menu为true的ui时，需要设置isForce为True才能打开

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenPauseGui()

```

## OpenReportGui

客户端

method in mod.client.extraClientApi

- 描述

  打开原版举报界面

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.OpenReportGui()

```

## PlayHudHeartBlinkAnim

客户端

method in mod.client.extraClientApi

- 描述

  播放原版受伤时血量变化的动效

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.PlayHudHeartBlinkAnim()

```

## SetCrossHair

客户端

method in mod.client.extraClientApi

- 描述

  设置是否使用“准星瞄准”

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| visible | bool | 十字模式下True为开启“准星瞄准”，False为关闭，摇杆模式下True为点击互动、False为瞄准十字线 |

- 返回值

  无

- 示例

```python
# we want to hide cross hair
import mod.client.extraClientApi as clientApi
clientApi.SetCrossHair(False)

```

## SetEmoteSwitch

客户端

method in mod.client.component.gameCompClient.GameComponentClient

- 描述

  设置是否开启表情功能，默认PC端关闭，手机端开启，且该接口只能在手机端使用，在原生UI初始化前调用设置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| flag | bool | 是否开启表情 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置成功返回true，设置失败返回false |

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateGame(levelId)
res = comp.SetEmoteSwitch(False)

```

## SetHudChatStackPosition

客户端

method in mod.client.extraClientApi

- 描述

  设置HUD界面左上小聊天窗口位置

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| pos | `tuple(float,float)` | 该界面的目标坐标，第一项为横轴，第二项为纵轴，(0,0)点在Hud界面左上角，玩家纸娃娃下方 |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.SetHudChatStackPosition((100, 0))

```

## SetHudChatStackVisible

客户端

method in mod.client.extraClientApi

- 描述

  设置HUD界面左上小聊天窗口可见性

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| visible | bool | 是否可见 |

- 返回值

  无

- 备注

  - 需要在UiInitFinished事件触发后调用

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.SetHudChatStackVisible(True)

```

## SetResponse

客户端

method in mod.client.extraClientApi

- 描述

  设置原生UI是否响应

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| response | bool | 是否响应原生UI，True为响应原生UI，即点击屏幕时可以敲击方块/攻击实体，False为不响应原生UI |

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.SetResponse(False)

```

## SimulateJump

客户端

method in mod.client.extraClientApi

- 描述

  模拟跳跃

- 参数

  无

- 返回值

  无

- 示例

```python
import mod.client.extraClientApi as clientApi
clientApi.SimulateJump()

```
