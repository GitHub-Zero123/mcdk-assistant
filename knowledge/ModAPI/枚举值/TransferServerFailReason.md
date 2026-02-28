# TransferServerFailReason

class in mod.common.minecraftEnum

- 描述

  转服判定失败的错误码

```python
class TransferServerFailReason(object):
	TypeNotExist = 10		# 目标类型的服务器不存在
	VersionNotExist = 11	# 目标类型的服务器的版本与玩家客户端的版本不符
	ServerIsFull = 12		# 目标类型的服务器均已经满员了
	VersionNotFix = 13		# 目标ID的服务器的版本与玩家客户端版本不符
	TargetIsFull = 14		# 目标ID的服务器已经满员了
	TargetNotVaild = 15		# 目标ID的服务器不存在或者已经与控制服断开连接
	ApiInputFail = 16		# 目标玩家不在线

```
