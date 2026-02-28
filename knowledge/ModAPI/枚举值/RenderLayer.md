# RenderLayer

class in mod.common.minecraftEnum

- 描述

  方块渲染时的材质类型

- 备注

  - 目前自定义方块只支持使用部分材质，具体见 自定义方块json组件
    由于联机大厅和apollo存在部分材质缺少定义，所以枚举值在联机大厅和apollo环境下，整体-2
    如：Blend = 5 变成 Blend = 3 ; Opaque = 6 变成 Opaque = 4，依此类推

```python
class RenderLayer(object):
	DOUBLE_SIDED = 0 # 双面
	BLEND_WATER = 1 # 半透明水面
	ALPHATEST_MICRO_BLOCK = 2 # 全透明微缩方块
	RAY_TRACED_WATER = 3 # 原版光线追踪水面
	DEFERRED_WATER = 4 # 原版延迟渲染水面
	BLEND = 5 # 半透明
	OPAQUE = 6 # 不透明
	LIGHT_SOURCE = 7 # 原版发光方块
	OPTIONAL_ALPHATEST = 8 # 局部全透明
	ALPHATEST = 9 # 全透明
	SEASONS_OPAQUE = 10 # 原版用于渲染不透明树叶
	SEASONS_OPTIONAL_ALPHATEST = 11 # 原版用于渲染局部全透明方块
	ALPHATEST_SINGLE_SIDE = 12 # 单面全透明
	ENDPORTAL = 13 # 原版末地传送门
	BARRIER = 14 # 原版屏障
	LIGHT = 15 # 原版光源
	STRUCTURE_VOID = 16

```
