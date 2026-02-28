# PermissionChangeCause

class in mod.common.minecraftEnum

- 描述

  玩家权限变更原因枚举

```python
class PermissionChangeCause(object):
	ProgrammingInterfaceCaused = 1 	#  API变更
	CommandCaused = 2 				#  指令变更（包含玩家输入/命令方块）
	UserInterfaceCaused = 3			#  房主变更（也即房主在设置给他人变更）
	CocosInterfaceCaused = 4		#  cocos发起变更

```
