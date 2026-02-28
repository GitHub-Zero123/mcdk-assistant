# AttrType

class in mod.common.minecraftEnum

- 描述

  描述属性枚举值，用于设置与获取实体的引擎属性的当前值与最大值

- 备注

  - ABSORPTION: 伤害吸收效果的量化值，详见wiki文档：[伤害吸收 (opens new window)](https://zh.minecraft.wiki/w/%E4%BC%A4%E5%AE%B3%E5%90%B8%E6%94%B6)
  - 各类属性值一般通过entity的json配置，如`minecraft:knockback_resistance : {"value": 100, "max": 100}`
  - 当json文件中未配置时，引擎会针对不同属性进行不同初始值、不同最大值的设置

```python
class AttrType(object):
	HEALTH = 0              # 生命值, 原版的值范围为`[0,20]`
	SPEED = 1               # 移速, 原版的值范围为[0,+∞]
	DAMAGE = 2              # 攻击力, 原版的值范围为[1,+∞], 默认最大值为1
	UNDERWATER_SPEED = 3    # 水里的移速, 原版的值范围为[0,+∞]
	HUNGER = 4              # 饥饿值, 原版的值范围为`[0,20]`
	SATURATION = 5          # 饱和值, 原版的值范围为`[0,20]`
	ABSORPTION = 6          # 伤害吸收生命值，详见备注, 原版的值范围为`[0,16]`
	LAVA_SPEED = 7          # 岩浆里的移速, 原版的值范围为[0,+∞]
	LUCK = 8                # 幸运值，原版的值范围为[-1024,1024]
	FOLLOW_RANGE = 9		# 跟随方块数(一般指怪的仇恨范围), 原版值范围为`[1,2024]`，默认值为16
	KNOCKBACK_RESISTANCE = 10	# 击退抵抗，原版值范围为[1,+∞]，默认最大值为1
	JUMP_STRENGTH = 11		# 跳跃力(指骑乘后跳跃可跳跃的高度)，原版值范围为[0,+∞]
	ARMOR = 12				# 护甲值，取决于身上穿戴的护甲总防御量和接口增加的额外护甲值。客户端无法获取接口增加的护甲值，建议开发者自行同步

```
