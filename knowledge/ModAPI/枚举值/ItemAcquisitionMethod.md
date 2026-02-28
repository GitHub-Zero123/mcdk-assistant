# ItemAcquisitionMethod

class in mod.common.minecraftEnum

- 描述

  获得物品的方法枚举值

```python
class ItemAcquisitionMethod(object):
    Unknown = -1                # 获得方法未知。
    MethodNone = 0              # 无获得方法。
    PickedUp = 1                # 通过捡起道具的方式获得。服务端和客户端均触发。
    Crafted = 2                 # 通过工具合成的方式获得，工具包括工作台、制图台、砂轮、织布机和切石机。从客户端触发。
    TakenFromChest = 3          # 通过从箱子中拿取的方式获得。从客户端触发。
    TakenFromEnderchest = 4     # 通过从末影箱中拿取的方式获得。目前从末影箱子拿取物品时只返回TakenFromChest的值。
    Bought = 5                  # 通过与村民交易的方式获得。目前与村民交易只返回Trading的值。
    Anvil = 6                   # 通过铁砧的方式获得。从客户端触发。
    Smelted = 7                 # 通过烧炼的方式获得，包括熔炉、烟熏炉及高炉。从客户端触发。
    Brewed = 8                  # 通过酿造的方式获得。只要从酿造台取下道具都会触发。从客户端触发。
    Filled = 9                  # 通过装满空瓶、空桶或炼药锅，又或是从其中倒出内容物的方式获得，服务端和客户端均触发。
                                # 注意，对象为炼药锅时仅从服务端触发事件。
    Trading = 10                # 通过交易的方式获得。从客户端触发。
    Fishing = 11                # 通过钓鱼的方式获得。服务端和客户端均触发。
    Container = 13              # 通过容器的方式获得，目前只支持锻造台。从客户端触发。
    Feeding = 14                # 被喂食。从服务端触发。

```
