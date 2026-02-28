# UI

# 索引

| 事件 | | 描述 |
| --- | --- | --- |
| AchievementButtonMovedClientEvent | 客户端 | 触发时机：使用自定义成就系统的时，拖动成就入口结束时触发 |
| ClientChestCloseEvent | 客户端 | 关闭箱子界面时触发，包括小箱子，合并后大箱子和末影箱(实体容器不属于方块，不会触发该事件) |
| ClientChestOpenEvent | 客户端 | 打开箱子界面时触发，包括小箱子，合并后大箱子和末影箱(实体容器不属于方块，不会触发该事件) |
| ClientPlayerInventoryCloseEvent | 客户端 | 关闭物品背包界面时触发 |
| ClientPlayerInventoryOpenEvent | 客户端 | 打开物品背包界面时触发 |
| CloseNeteaseShopEvent | 客户端 | 关闭商城界面时触发，包括脚本商城和Apollo插件商城 |
| GridComponentSizeChangedClientEvent | 客户端 | UI grid控件里格子重新排列时触发 |
| HudButtonChangedClientEvent | 客户端 | 当原生HUD按钮位置或大小发生改变时触发，例如玩家使用了自定义控件功能会触发，可在该事件中修改mod按钮的位置防止重叠 |
| OnItemSlotButtonClickedEvent | 客户端 | 点击快捷栏、背包栏、盔甲栏、副手栏的物品槽时触发 |
| PlayerChatButtonClickClientEvent | 客户端 | 玩家点击聊天按钮或回车键触发呼出聊天窗口时客户端抛出的事件 |
| PlayerInventoryOpenScriptServerEvent | 服务端 | 某个客户端打开物品背包界面时触发 |
| PopScreenAfterClientEvent | 客户端 | screen移除触发 |
| PopScreenEvent | 客户端 | screen移除触发 |
| PushScreenEvent | 客户端 | screen创建触发 |
| ScreenSizeChangedClientEvent | 客户端 | 改变屏幕大小时会触发的事件 |
| UiInitFinished | 客户端 | UI初始化框架完成,此时可以创建UI |
| UrgeShipEvent | 服务端 | 玩家点击商城催促发货按钮时触发该事件 |

# UI

## AchievementButtonMovedClientEvent

客户端

- 描述

  触发时机：使用自定义成就系统的时，拖动成就入口结束时触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| oldPosition | `tuple(float,float)` | 移动前该控件相对父节点的坐标信息，第一项为横轴，第二项为纵轴 |
| newPosition | `tuple(float,float)` | 移动后该控件相对父节点的坐标信息，第一项为横轴，第二项为纵轴 |

- 返回值

  无

## ClientChestCloseEvent

客户端

- 描述

  关闭箱子界面时触发，包括小箱子，合并后大箱子和末影箱(实体容器不属于方块，不会触发该事件)

- 参数

  无

- 返回值

  无

## ClientChestOpenEvent

客户端

- 描述

  打开箱子界面时触发，包括小箱子，合并后大箱子和末影箱(实体容器不属于方块，不会触发该事件)

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| playerId | str | 玩家实体id |
| x | int | 箱子位置x值 |
| y | int | 箱子位置y值 |
| z | int | 箱子位置z值 |

- 返回值

  无

在零件中直接声明一个同名函数，即可完成监听，详情参考零件事件

## ClientPlayerInventoryCloseEvent

客户端

- 描述

  关闭物品背包界面时触发

- 参数

  无

- 返回值

  无

## ClientPlayerInventoryOpenEvent

客户端

- 描述

  打开物品背包界面时触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| isCreative | bool | 是否是创造模式背包界面 |
| cancel | bool | 取消打开物品背包界面 |

- 返回值

  无

## CloseNeteaseShopEvent

客户端

- 描述

  关闭商城界面时触发，包括脚本商城和Apollo插件商城

- 参数

  无

- 返回值

  无

## GridComponentSizeChangedClientEvent

客户端

- 描述

  UI grid控件里格子重新排列时触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| path | str | grid网格所在的路径（从UI根节点算起） |

- 返回值

  无

## HudButtonChangedClientEvent

客户端

- 描述

  当原生HUD按钮位置或大小发生改变时触发，例如玩家使用了自定义控件功能会触发，可在该事件中修改mod按钮的位置防止重叠

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| changedList | `tuple(dict)` | 修改后的按钮列表 |

- 返回值

  无

- 备注

  - 修改后的按钮列表中，每个按钮的字段如下：

| 字段名 | 类型 | 说明 |
| --- | --- | --- |
| areaEnum | str | HUD原生UI枚举值 |
| beforeSize | `tuple(float,float,float,float)` | (xMin, yMin, xMax, yMax)修改前原生UI的Area |
| afterSize | `tuple(float,float,float,float)` | (xMin, yMin, xMax, yMax)修改后原生UI的Area |

## OnItemSlotButtonClickedEvent

客户端

- 描述

  点击快捷栏、背包栏、盔甲栏、副手栏的物品槽时触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| slotIndex | int | 点击的物品槽的编号，编号对应位置详见\[[物品栏](https://zh.minecraft.wiki/w/%E7%89%A9%E5%93%81%E6%A0%8F)\] |

- 返回值

  无

## PlayerChatButtonClickClientEvent

客户端

- 描述

  玩家点击聊天按钮或回车键触发呼出聊天窗口时客户端抛出的事件

- 参数

  无

- 返回值

  无

## PlayerInventoryOpenScriptServerEvent

服务端

- 描述

  某个客户端打开物品背包界面时触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| playerId | str | 客户端对应的玩家entity的唯一ID |
| isCreative | bool | 是否是创造模式背包界面 |

- 返回值

  无

- 备注

  - 可以监听此事件判定客户端是否打开了创造背包

在零件中直接声明一个同名函数，即可完成监听，详情参考零件事件

## PopScreenAfterClientEvent

客户端

- 描述

  screen移除触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| screenName | str | UI名字 |
| screenDef | str | 包含命名空间的UI名字，格式为namespace.screenName |

- 返回值

  无

- 备注

  - 与PopScreenEvent不同，PopScreenAfterClientEvent触发时机是在完全把UI弹出后，返回的参数是弹出后最顶层UI的

## PopScreenEvent

客户端

- 描述

  screen移除触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| screenName | str | UI名字 |
| screenDef | str | 包含命名空间的UI名字，格式为namespace.screenName |

- 返回值

  无

- 备注

  - screenName为正在弹出的Screen名，如果需要获取下一个Screen可使用PopScreenAfterClientEvent

## PushScreenEvent

客户端

- 描述

  screen创建触发

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| screenName | str | UI名字 |
| screenDef | str | 包含命名空间的UI名字，格式为namespace.screenName |

- 返回值

  无

## ScreenSizeChangedClientEvent

客户端

- 描述

  改变屏幕大小时会触发的事件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| beforeX | float | 屏幕大小改变前的宽度 |
| beforeY | float | 屏幕大小改变前的高度 |
| afterX | float | 屏幕大小改变后的宽度 |
| afterY | float | 屏幕大小改变后的高度 |

- 返回值

  无

- 备注

  - 该事件仅支持PC

## UiInitFinished

客户端

- 描述

  UI初始化框架完成,此时可以创建UI

- 参数

  无

- 返回值

  无

- 备注

  - 切换维度后会重新初始化UI并触发该事件

## UrgeShipEvent

服务端

- 描述

  玩家点击商城催促发货按钮时触发该事件

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| playerId | str | 玩家id |

- 返回值

  无

在零件中直接声明一个同名函数，即可完成监听，详情参考零件事件
