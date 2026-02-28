# ActorDamageCause

class in mod.common.minecraftEnum

- 描述

  描述实体伤害来源枚举值，及实体被伤害的原因。

```python
class ActorDamageCause(object):
    NONE = "none"  							# 其他
    Override = "override"  					# 非正常方式（如脚本直接设置血量为0），这种方式的伤害不会被盔甲与buff吸收
    Contact = "contact"  					# 接触伤害（如仙人掌）
    EntityAttack = "entity_attack" 			# 生物攻击
    Projectile = "projectile"  				# 抛射物攻击
    Suffocation = "suffocation" 		 	# 窒息（密封空间）
    Fall = "fall"  							# 掉落
    Fire = "fire"  							# 着火
    FireTick = "fire_tick"  				# 连续着火（生物着火、在火中）
    Lava = "lava"  							# 熔岩
    Drowning = "drowning"  					# 溺水
    BlockExplosion = "block_explosion"  	# 方块爆炸
    EntityExplosion = "entity_explosion"  	# 生物爆炸
    Void = "void"  							# 虚空
    Suicide = "self_destruct"  				# 自杀（kill命令）兼容旧版
    SelfDestruct = "self_destruct"          # 自杀（kill命令）
    Magic = "magic"  						# 尖牙对生物造成的伤害、守卫者对生物造成的魔法伤害和药水伤害等
    Wither = "wither"  						# 凋零效果
    Starve = "starve"  						# 饥饿
    Anvil = "anvil"  						# 下落的铁砧
    Thorns = "thorns"  						# 荆棘反弹伤害
    FallingBlock = "falling_block"  		# 下落的方块（除了铁砧）
    Piston = "piston"  						# 活塞
    FlyIntoWall = "fly_into_wall"  			# 滑翔（撞墙）
    Magma = "magma"  						# 岩浆（如站在岩浆方块上）
    Fireworks = "fireworks"  				# 烟花
    Lightning = "lightning"  				# 闪电
    Freezing = "freezing"					# 冰冻
    Stalactite = "stalactite"				# 被钟乳石砸到
    Stalagmite = "stalagmite"				# 掉落到石笋上
    RamAttack = "ram_attack"				# 山羊冲撞
    Custom = "custom"						# 自定义
    SonicBoom = "sonic_boom"				# 音波尖啸(监守者的远程攻击)
    Campfire = "campfire"					# 营火
    SoulCampfire = "soul_campfire"			# 灵魂营火(营火的灵魂火变种)
    MaceSmash = "mace_smash"				# 重锤粉碎攻击

```
