# RayFilterType

class in mod.common.minecraftEnum

- 描述

  射线检测类型

```python
class RayFilterType(object):
	OnlyEntities = 1 << 0 #仅检测实体
	OnlyBlocks = 1 << 1 #仅检测方块
	BothEntitiesAndBlock = OnlyEntities | OnlyBlocks #检测方块和实体

```
