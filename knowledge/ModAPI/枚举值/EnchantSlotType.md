# EnchantSlotType

class in mod.common.minecraftEnum

- 描述

  附魔槽位枚举值

```python
class EnchantSlotType(object):
	NONE = 0					# 非法
	ALL = 4294967295			# 全部
	G_ARMOR = 15				# 全部盔甲
	ARMOR_HEAD = 1				# 头盔
	ARMOR_TORSO = 2				# 胸甲
	ARMOR_FEET = 4				# 靴子
	ARMOR_LEGS = 8				# 护腿

	# ungrouped weapons
	SWORD = 16					# 剑
	BOW = 32					# 弓
	SPEAR = 32768				# 三叉戟
	CROSSBOW = 65536			# 弩
	HEAVY_WEAPON = 4194304		# 重锤

	# tool group
	G_TOOL = 131520				# 剪刀、打火石、盾
	SHEARS = 128				# 剪刀
	FLINTSTEEL = 256			# 打火石
	SHIELD = 131072				# 盾

	# digging tool group
	G_DIGGING = 3648			# 锄、斧、镐、锹
	HOE = 64					# 锄
	AXE = 512					# 斧
	PICKAXE = 1024				# 镐
	SHOVEL = 2048				# 锹

	# ungrouped tools
	FISHING_ROD = 4096			# 钓竿
	CARROT_STICK = 8192			# 胡萝卜钓竿
	ELYTRA = 16384				# 鞘翅
	COSMETIC_HEAD = 262144		# 骷髅模型/凋零骷髅模型/模型/僵尸模型/龙的模型/爬行者的模型/雕刻南瓜
	COMPASS = 524288			# 指南针
	MUSHROOM_STICK = 1048576	# 诡异菌钓竿
	BRUSH = 2097152				# 刷子

```
