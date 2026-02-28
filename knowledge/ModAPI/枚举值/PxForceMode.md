# PxForceMode

class in mod.common.minecraftEnum

- 描述

  添加力的模式

```python
class PxForceMode(object):
    eFORCE = 0              # 力，单位为质量 × 距离 / 时间²
    eIMPULSE = 1            # 冲量，单位为质量 × 距离 / 时间
    eVELOCITY_CHANGE = 2    # 速度变化，单位为距离 / 时间，直接改变速度，与质量无关
    eACCELERATION = 3       # 加速度，单位为距离 / 时间²，直接施加加速度，与质量无关

```
