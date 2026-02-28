# UiBaseLayer

class in mod.common.minecraftEnum

- 描述

  自定义UI界面的层次宏定义，用于在多个插件之间协调UI界面的遮挡关系

```python
class UiBaseLayer(object):
	Desk = 0				# 主界面常驻
	DeskFloat = 15000		# 主界面浮动提示（浮动提示信息）
	PopUpLv1 = 25000		# 一级弹出界面
	PopUpLv2 = 45000		# 二级弹出界面
	PopUpModal = 60000		# 模态弹出界面（弹出提示）
	PopUpFloat = 75000		# 模态弹出之上的浮动提示（大喇叭）

```
