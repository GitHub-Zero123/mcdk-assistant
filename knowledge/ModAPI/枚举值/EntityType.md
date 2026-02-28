# EntityType

class in mod.common.minecraftEnum

- 描述

  实体类型枚举

- 备注

  - 当实体行为包内使用[runtime_identifier (opens new window)](https://dev.mc.163.com/dev/mcmanual/mc-dev/mconline/15-%E7%8E%A9%E6%B3%95%E7%BB%84%E4%BB%B6%E6%95%99%E7%A8%8B/7-%E8%87%AA%E5%AE%9A%E4%B9%89%E5%AE%9E%E4%BD%93/4-%E6%8E%A2%E7%B4%A2%E5%AE%9E%E4%BD%93%E7%9A%84%E8%A1%8C%E4%B8%BA%E6%96%87%E4%BB%B6.html#%E7%90%86%E8%A7%A3%E5%AE%9E%E4%BD%93%E8%A1%8C%E4%B8%BA%E7%9A%84%E6%8F%8F%E8%BF%B0%E4%BF%A1%E6%81%AF)字段继承某种实体的特征时，此时实体所属分类与runtime_indentifier所指向的实体相同。
  - 自定义实体未使用runtime_identifier字段时该实体默认只属于Mob类。
  - 在控制台打印枚举值为位运算结果，例如，EntityType.Zombie 输出为 199456。

```python
class EntityType(object):
	Undefined = 1									# 未定义类型
	TypeMask = 0x000000ff							# 类型过滤
	Mob = 0x00000100								# 生物
	PathfinderMob = 0x00000200 | Mob				# 可寻路生物
	Monster = 0x00000800 | PathfinderMob			# 敌对怪物
	Animal = 0x00001000 | PathfinderMob				# 动物
	TamableAnimal = 0x00004000 | Animal				# 可驯服动物
	Ambient = 0x00008000 | Mob						# 环境
	UndeadMob = 0x00010000 | Monster				# 亡灵生物
	ZombieMonster = 0x00020000 | UndeadMob			# 僵尸生物
	Arthropod = 0x00040000 | Monster				# 节肢生物
	Minecart = 0x00080000							# 矿车
	SkeletonMonster = 0x00100000 | UndeadMob		# 骷髅生物
	EquineAnimal = 0x00200000 | TamableAnimal		# 马类生物
	Projectile = 0x00400000							# 抛射物
	AbstractArrow = 0x00800000						# 抽象箭矢
	WaterAnimal = 0x00002000 | PathfinderMob		# 水生生物
	VillagerBase = 0x01000000 | PathfinderMob		# 村民生物
	Chicken = 10 | Animal							# 鸡
	Cow = 11 | Animal								# 牛
	Pig = 12 | Animal								# 猪
	Sheep = 13 | Animal								# 羊
	Wolf = 14 | TamableAnimal						# 狼
	Villager = 15 | VillagerBase					# 村民
	MushroomCow = 16 | Animal						# 哞菇
	Squid = 17 | WaterAnimal						# 鱿鱼
	Rabbit = 18 | Animal							# 兔子
	Bat = 19 | Ambient								# 蝙蝠
	IronGolem = 20 | PathfinderMob					# 铁傀儡
	SnowGolem = 21 | PathfinderMob					# 雪傀儡
	Ocelot = 22 | TamableAnimal						# 豹猫
	Horse = 23 | EquineAnimal						# 马
	PolarBear = 28 | Animal							# 北极熊
	Llama = 29 | Animal								# 羊驼
	Parrot = 30 | TamableAnimal						# 鹦鹉
	Dolphin = 31 | WaterAnimal						# 海豚
	Donkey = 24 | EquineAnimal						# 驴
	Mule = 25 | EquineAnimal						# 骡
	SkeletonHorse = 26 | EquineAnimal | UndeadMob	# 骷髅马
	ZombieHorse = 27 | EquineAnimal | UndeadMob		# 僵尸马
	Zombie = 32 | ZombieMonster						# 僵尸
	Creeper = 33 | Monster							# 苦力怕
	Skeleton = 34 | SkeletonMonster					# 骷髅
	Spider = 35 | Arthropod							# 蜘蛛
	PigZombie = 36 | UndeadMob						# 僵尸猪灵
	Slime = 37 | Monster							# 史莱姆
	EnderMan = 38 | Monster							# 末影人
	Silverfish = 39 | Arthropod						# 蠹虫
	CaveSpider = 40 | Arthropod						# 洞穴蜘蛛
	Ghast = 41 | Monster							# 恶魂
	LavaSlime = 42 | Monster						# 岩浆怪
	Blaze = 43 | Monster							# 烈焰人
	ZombieVillager = 44 | ZombieMonster				# 僵尸村民
	Witch = 45 | Monster							# 女巫
	Stray = 46 | SkeletonMonster					# 流浪者
	Husk = 47 | ZombieMonster						# 尸壳
	WitherSkeleton = 48 | SkeletonMonster			# 凋灵骷髅
	Guardian = 49 | Monster							# 守卫者
	ElderGuardian = 50 | Monster					# 远古守卫者
	Npc = 51 | Mob									# NPC
	WitherBoss = 52 | UndeadMob						# 凋灵
	Dragon = 53 | Monster							# 末影龙
	Shulker = 54 | Monster							# 潜影贝
	Endermite = 55 | Arthropod						# 末影螨
	Agent = 56 | Mob								# 吉祥物
	Vindicator = 57 | Monster						# 卫道士
	Phantom = 58 | UndeadMob						# 幻翼
	IllagerBeast = 59 | Monster						# 劫掠兽
	ArmorStand = 61 | Mob							# 盔甲架
	TripodCamera = 62 | Mob							# 三脚架摄像机
	Player = 63 | Mob								# 玩家
	ItemEntity = 64									# 物品
	PrimedTnt = 65									# TNT
	FallingBlock = 66								# 下落的方块
	MovingBlock = 67								# 移动的方块
	ExperiencePotion = 68 | Projectile				# 附魔之瓶
	Experience = 69									# 经验球
	EyeOfEnder = 70									# 末影之眼
	EnderCrystal = 71								# 末影水晶
	FireworksRocket = 72							# 烟花火箭
	Trident = 73 | Projectile | AbstractArrow		# 三叉戟
	Turtle = 74 | Animal							# 海龟
	Cat = 75 | TamableAnimal						# 猫
	ShulkerBullet = 76 | Projectile					# 潜影贝导弹
	FishingHook = 77								# 浮漂
	Chalkboard = 78									# 黑板
	DragonFireball = 79 | Projectile				# 末影龙火球
	Arrow = 80 | Projectile | AbstractArrow			# 箭
	Snowball = 81 | Projectile						# 雪球
	ThrownEgg = 82 | Projectile						# 鸡蛋
	Painting = 83									# 画
	LargeFireball = 85 | Projectile					# 火球
	ThrownPotion = 86 | Projectile					# 喷溅药水
	Enderpearl = 87 | Projectile					# 末影珍珠
	LeashKnot = 88									# 栓绳结
	WitherSkull = 89 | Projectile					# 黑色凋灵骷髅头
	BoatRideable = 90								# 可乘骑船
	WitherSkullDangerous = 91 | Projectile			# 蓝色凋灵骷髅头
	LightningBolt = 93								# 闪电
	SmallFireball = 94 | Projectile					# 小火球
	AreaEffectCloud = 95							# 区域效果云
	LingeringPotion = 101 | Projectile				# 滞留药水
	LlamaSpit = 102 | Projectile					# 羊驼唾沫
	EvocationFang = 103 | Projectile				# 唤魔者尖牙
	EvocationIllager = 104 | Monster				# 唤魔者
	Vex = 105 | Monster								# 恼鬼
	MinecartRideable = 84 | Minecart				# 可乘骑矿车
	MinecartHopper = 96 | Minecart					# 漏斗矿车
	MinecartTNT = 97 | Minecart						# TNT矿车
	MinecartChest = 98 | Minecart					# 运输矿车
	MinecartFurnace = 99 | Minecart					# 动力矿车
	MinecartCommandBlock = 100 | Minecart			# 命令方块矿车
	IceBomb = 106 | Projectile						# 冰弹
	Balloon = 107									# 气球
	Pufferfish = 108 | WaterAnimal					# 河豚
	Salmon = 109 | WaterAnimal						# 鲑鱼
	Drowned = 110 | ZombieMonster					# 溺尸
	Tropicalfish = 111 | WaterAnimal				# 热带鱼
	Fish = 112 | WaterAnimal						# 鱼
	Panda = 113 | Animal							# 熊猫
	Pillager = 114 | Monster						# 掠夺者
	VillagerV2 = 115 | VillagerBase					# 村民
	ZombieVillagerV2 = 116 | ZombieMonster			# 僵尸村民
	Shield = 117									# 盾牌
	WanderingTrader = 118 | PathfinderMob			# 流浪商人
	Lectern = 119									# 讲台
	ElderGuardianGhost = 120 | Monster				# 远古守卫者恶魂
	Fox = 121 | Animal								# 狐狸
	Bee = 122 | Mob									# 蜜蜂
	Piglin = 123 | Mob								# 猪灵
	Hoglin = 124 | Animal							# 疣猪兽
	Strider = 125 | Animal							# 炽足兽
	Zoglin = 126 | UndeadMob						# 僵尸疣猪兽
	PiglinBrute = 127 | Mob							# 猪灵蛮兵
	Goat = 128 | Animal								# 山羊
	GlowSquid = 129 | WaterAnimal					# 发光鱿鱼
	Axolotl = 130 | Animal							# 美西螈
	Warden = 131 | Monster							# 监守者
	Frog = 132 | Animal								# 青蛙
	Tadpole = 133 | WaterAnimal						# 蝌蚪
	Allay = 134 | Mob								# 悦灵
	ChestBoatRideable = 136 | BoatRideable			# 可乘骑运输船
	TraderLlama = 137 | Llama						# 行商羊驼
	Camel = 138 | Animal							# 骆驼
	Sniffer = 139 | Animal							# 嗅探兽
	Breeze = 140 | Monster							# 旋风人
	BreezeWindChargeProjectile = 141 | Projectile	# 旋风人风弹
	Armadillo = 142 | Animal						# 犰狳
	WindChargeProjectile = 143 | Projectile			# 风弹
	Bogged = 144 | SkeletonMonster					# 沼骸
	OminousItemSpawner = 145						# 不祥之物生成器
	Creaking = 146 | Monster						# 嘎枝
	CustomProjectile = 254 | Projectile				# 自定义抛射物
	EntityExtension = 255							# 实体扩展
	MAX_ENTITY_ID = 256								# 最大实体ID

```
