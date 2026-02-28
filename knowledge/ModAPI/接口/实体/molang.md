# molang

## EvalMolangExpression

服务端客户端

### 服务端接口

method in mod.server.component.queryVariableCompServer.QueryVariableComponentServer

- 描述

  在实体上下文上执行molang表达式

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| expression | str | molang表达式 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| dict | 执行结果在"value"的键中，如果有执行错误，通过"error"的键返回错误信息 |

- 备注

  - molang最初是为渲染设计的，所以在服务端上大部分接口是无法获取到数据的。

- 示例

```python
import mod.server.extraServerApi as serverApi
comp = serverApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.EvalMolangExpression('query.can_fly')

```

### 客户端接口

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  在实体上下文上执行molang表达式

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| expression | str | molang表达式 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| dict | 执行结果在"value"的键中，如果有执行错误，通过"error"的键返回错误信息 |

- 备注

  - ```

    本接口执行时没有渲染上下文，某些molang无法通过该种方式获取到正确的值，如query.actor_count【获取最近一帧渲染实体数量】 会永远返回0。
    ```

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.EvalMolangExpression('query.can_fly')

```

## Get

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  获取某一个实体计算节点的值，如果不存在返回注册时的默认值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| variableName | str | 节点名称，必须以"query.mod."开头 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float | 节点的值 |

- 备注

  - 自定义的"query.mod.xxxx"类型的molang变量需要先调用Register接口进行注册，才能正常设置/读取

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.Get('query.mod.state')

```

## GetAllProperties

服务端

method in mod.server.component.queryVariableCompServer.QueryVariableComponentServer

- 描述

  获取实体属性列表

- 参数

  无

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| `tuple(str)` | 所有已注册properties，每个str为properties的名称，如果有错误，返回None |

- 备注

  - 可配合query.property，使用EvalMolangExpression获取实体属性的值。在客户端只能获取到client_sync为true的属性

- 示例

```python
import mod.server.extraServerApi as serverApi
comp = serverApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.GetAllProperties()

```

## GetMolangValue

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  获取实体molang变量的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| molangName | str | molang变量名称，如query.can_fly |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| float或long | 节点的值，不存在返回None |

- 备注

  - 因为没有渲染上下文，某些molang无法通过该种方式获取到正确的值，如query.is_first_person、variable.is_first_person等。
  - 当molangName传入query.get_name或者query.owner_identifier等需要返回字符串的变量值时，返回其hash64，类型是个long，可以用于与GetStringHash64获取的值比较。query.get_name：返回生物的名字；query.owner_identifier：返回其owner的identifier
  - 该接口无法进行molang运算，例如无法计算query.average_frame_time(20)
  - 该接口对客户端实体(CreateClientEntityByTypeStr)使用时，涉及到服务端逻辑的molang无法返回正确的值，比如query.health、query.in_water、query.can_fly等等。

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.GetMolangValue('query.can_fly')

```

## GetStringHash64

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  返回字符串变量的hash64

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| key | str | 需要计算的字符串变量,如“steve" |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| long | 结果值，如果传入不是字符串则返回None |

- 备注

  - 可以配合GetMolangValue使用，可以获取query.get_name或者query.owner_identifier等返回hash64的是不是等于某个值

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(levelId)
result = comp.GetStringHash64('steve')

```

## Register

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  注册实体计算节点

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| variableName | str | 节点名称，必须以"query.mod."开头 |
| defalutValue | float | 默认值 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 注册是否成功 |

- 备注

  - 注意节点的值使用单精度浮点数存储，如果用来设置整数，那么值大于16777216（或小于-16777216）时就会有误差

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(levelId)
result = comp.Register('query.mod.state', 0.0)

```

## Set

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  设置某一个实体计算节点的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| variableName | str | 节点名称，必须以"query.mod."开头 |
| value | float | 计算节点的值 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 设置是否成功 |

- 备注

  - 注意节点的值使用单精度浮点数存储，如果用来设置整数，那么值大于16777216（或小于-16777216）时就会有误差
  - 自定义的"query.mod.xxxx"类型的molang变量需要先调用Register接口进行注册，才能正常设置/读取

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.Set('query.mod.state', 1.0)

```

## SetPropertyValue

服务端

method in mod.server.component.queryVariableCompServer.QueryVariableComponentServer

- 描述

  设置实体属性的值

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| propertyName | str | 属性名，如minecraft:has_nectar |
| value | str | 属性值支持int/float/bool/enum(str)，支持molang表达式 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 是否执行成功 |

- 备注

  - molang表达式需无副作用， 例如不能设置molang变量的值，例如"variable.moo = 1;return variable.moo;"。query仅支持query.has_property、query.property

- 示例

```python
import mod.server.extraServerApi as serverApi
comp = serverApi.GetEngineCompFactory().CreateQueryVariable(entityId)
result = comp.SetPropertyValue('minecraft:has_nectar', True)

```

## UnRegister

客户端

method in mod.client.component.queryVariableCompClient.QueryVariableComponentClient

- 描述

  注销实体计算节点

- 参数

| 参数名 | 数据类型 | 说明 |
| --- | --- | --- |
| variableName | str | 节点名称，必须以"query.mod."开头 |

- 返回值

| 数据类型 | 说明 |
| --- | --- |
| bool | 注销是否成功 |

- 示例

```python
import mod.client.extraClientApi as clientApi
comp = clientApi.GetEngineCompFactory().CreateQueryVariable(levelId)
result = comp.UnRegister('query.mod.state')

```
