# EntityTeleportCause

class in mod.common.minecraftEnum

- 描述

  传送理由枚举

```python
class EntityTeleportCause(object):
	Unkown = "0"		# 尚未具体分类，末影人自体传送目前归为此类
	Projectile = "1"	# 弹射物，类似末影珍珠
	Command = "3"		# op指令，类似tp指令，changedimension指令
	ChorusFruit = "2"	# 吃紫颂果传送
	Behavior = "4"		# 微软原生脚本组件
	Agent = "agent"		# 教育版指导传送
	Client = "client"	# 客户端发送的传送移动包
	GateWay = "gateway"	# 传送门
	Script = "script"	# python接口，类似SetPos，SetFootPos，ChangePlayerDimension，ChangeEntityDimension

```
