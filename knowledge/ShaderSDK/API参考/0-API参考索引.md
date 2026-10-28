# API 参考索引

本索引按 CSS 的核心概念组织各页内容。下图表示概念之间的关系，不表示引擎内部的实际执行顺序。

```
Root Type ─┬─ Properties
           └─ EntryPoint → StandardSurfaceOutput → 有效渲染环境

```

## 着色接口

| 页面 | 查询内容 |
| --- | --- |
| [入口函数](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/1-%E5%85%A5%E5%8F%A3%E5%87%BD%E6%95%B0.html) | 固定函数签名、输入参数、读写规则和限制 |
| [标准表面输出](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/2-%E6%A0%87%E5%87%86%E8%A1%A8%E9%9D%A2%E8%BE%93%E5%87%BA.html) | Albedo、Alpha 与 PBR 字段的类型、默认值和生效环境 |
| [属性](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/3-%E5%B1%9E%E6%80%A7.html) | 四种 Property 类型、默认值语法、BuiltIn 和 Exposed |

## 引擎能力

| 页面 | 查询内容 |
| --- | --- |
| [保留内置 Uniform](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/4-%E5%86%85%E7%BD%AE%E5%B1%9E%E6%80%A7.html) | 时间、相机、维度、天气、太阳/月亮等引擎只读数据 |
| [内置函数](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/5-%E5%86%85%E7%BD%AE%E5%87%BD%E6%95%B0.html) | 游戏状态、类型、地形层、维度、天气和月相判断 |

## 根类型与材质

| 页面 | 查询内容 |
| --- | --- |
| [根类型与材质映射](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/6-%E6%A0%B9%E7%B1%BB%E5%9E%8B%E4%B8%8E%E6%9D%90%E8%B4%A8%E6%98%A0%E5%B0%84.html) | 11 个有效根类型以及花式、ForwardPBR、Prepass 材质映射 |

## 快速查询

| 想查询的问题 | 对应页面 |
| --- | --- |
| CSS 入口能接收哪些参数？ | [入口函数](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/1-%E5%85%A5%E5%8F%A3%E5%87%BD%E6%95%B0.html) |
| 哪些输出字段在花式渲染器中有效？ | [标准表面输出](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/2-%E6%A0%87%E5%87%86%E8%A1%A8%E9%9D%A2%E8%BE%93%E5%87%BA.html) |
| 如何声明标量、颜色或纹理？ | [属性](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/3-%E5%B1%9E%E6%80%A7.html) |
| 如何读取时间、相机或天气？ | [保留内置 Uniform](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/4-%E5%86%85%E7%BD%AE%E5%B1%9E%E6%80%A7.html) |
| 如何判断实体类型、地形层或维度？ | [内置函数](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/5-%E5%86%85%E7%BD%AE%E5%87%BD%E6%95%B0.html) |
| 某个目标对应哪些具体材质？ | [根类型与材质映射](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/6-%E6%A0%B9%E7%B1%BB%E5%9E%8B%E4%B8%8E%E6%9D%90%E8%B4%A8%E6%98%A0%E5%B0%84.html) |

## 环境标记

| 标记 | 含义 |
| --- | --- |
| 全部 | 花式渲染器与灵动视效均生效 |
| 仅灵动视效 | 只在 Vibrant Visuals PBR 路径中生效 |
| Upgradeable: Yes | 规格承诺后续 CSS 与游戏版本继续支持 |
| Upgradeable: No | 规格不保证后续版本继续支持；不表示当前已弃用 |

来源：[4-ShaderSDK](https://mc.163.com/dev/mcmanual/mc-dev/mcdocs/4-ShaderSDK/API%E5%8F%82%E8%80%83/0-API%E5%8F%82%E8%80%83%E7%B4%A2%E5%BC%95.html?catalog=1)
