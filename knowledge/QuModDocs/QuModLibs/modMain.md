# 创建 QuMod 项目

QuModLibs 是适用于网易 MC MOD 开发的免费开源框架。您可以通过 Gitee 或其他发布渠道获取源码，并按项目结构接入到行为包脚本目录中。

- [Gitee 项目地址](https://gitee.com/bili_zero123/qu_mod_libs)

::: tip 署名说明
如需摘录 QuModLibs 部分源代码到其他同类项目中，请保留原作者署名。
:::

## 版本说明 <Badge type="tip" text="v1.4" />

`v1.4` 起，QuModLibs 的包体组织有几项变化：

- 已废弃 `MINI` 精简版。
- 部分不常用模块移至 `Optional/Modules` 可选部分。
- 移除了内置补全库，改为依赖系统补全库。
- 如需旧版完整包体，请查看 `release/1.4-full` 分支。

### 补全库安装

网易 MC ModSDK 的脚本运行时由游戏内置的 Python 2.7 解释器执行，不会使用开发者电脑上的用户解释器。补全库只服务于本地编辑器分析、补全和类型提示，不会改变实际运行环境。

由于日常开发更常使用现代 Python 3 编辑器环境，推荐优先安装 `mc-netease-sdk-nyrev`：

```bash
# 推荐：Python 3 本地开发环境
pip install mc-netease-sdk-nyrev

# 如仍使用 Python 2 环境做编辑器分析
pip install mc-netease-sdk
```

::: warning 运行时版本
安装 Python 3 补全库并不代表项目可以使用 Python 3 语法运行。实际脚本仍需保持 Python 2.7 兼容，除非您的目标环境明确提供了额外的运行时方案。
:::

## 项目结构

推荐将库目录放置在 `Scripts/QuModLibs` 下，但这不是强制要求。关键是确保您的脚本包能够正确相对导入 `QuModLibs`。

```text
├── 行为包
│   └── 脚本目录
│       └── QuModLibs
│           ├── __init__.py
│           └── ...
│       ├── __init__.py
│       ├── modMain.py
│       ├── Server.py
│       └── Client.py
```

## MOD 初始化

`modMain.py` 是推荐的初始化入口。通过 `EasyMod` 注册服务端与客户端脚本后，QuMod 会在对应端侧加载目标文件。

```python
# modMain.py
# -*- coding: utf-8 -*-
from .QuModLibs.QuMod import *

myMod = EasyMod()

# 将加载: 脚本目录/Server.py 和 脚本目录/Client.py
myMod.Server("Server")
myMod.Client("Client")
```

::: tip 注册说明
注册只负责在对应端侧线程下加载特定文件，并不要求所有业务文件都在 `modMain.py` 中注册。运行时 `import` 同样生效。
:::

## 动态注册脚本

动态注册适合简单项目，或只有少量端侧入口的项目。

```python
# modMain.py
from .QuModLibs.QuMod import *

myMod = EasyMod()

myMod.Server("Server")
myMod.Client("Client")

# 多级目录使用点号分隔
myMod.Server("Features.Example.Server")
myMod.Client("Features.Example.Client")
```

## 前置依赖检测注册 <Badge type="tip" text="技巧" />

当项目依赖其他前置 MOD 时，可以使用 `PRE_SERVER_LOADER_HOOK` / `PRE_CLIENT_LOADER_HOOK` 在加载器正式导入端侧模块之前做检测，并按检测结果决定注册哪个入口。

```python
# modMain.py
# -*- coding: utf-8 -*-
from .QuModLibs.QuMod import *

mod = EasyMod()

@PRE_SERVER_LOADER_HOOK
def SERVER_LOADER():
    if not SRAPI.hasXXXMod():
        return
    # 包含前置 MOD 时加载服务端业务。
    mod.Server("Server")

@PRE_CLIENT_LOADER_HOOK
def CLIENT_LOADER():
    if not CLAPI.hasXXXMod():
        # 客户端未包含前置 MOD 时注册异常提示模块。
        mod.Client("ErrClient")
        return
    # 包含前置 MOD 时加载客户端业务。
    mod.Client("Client")
```

::: tip 使用场景
示例中的 `SRAPI` / `CLAPI` 代表您自己的前置检测 API。该写法适合“有前置才加载真实业务；客户端缺前置时加载错误提示 UI”的项目。
:::

::: warning 注意事项
使用该技巧时，不要再在 `modMain.py` 顶层直接注册同一份 `Server` / `Client` 入口，否则会绕过前置检测。
:::

## Feature 开发范式

QuModLibs 推荐以 **Feature（业务特性）** 为边界组织代码。每个 Feature 独立承载一组完整业务能力，并通过 `import` 完成事件注册、RPC 声明、服务初始化等副作用加载。

这样做可以让顶层入口保持清晰，同时把业务初始化关系收敛在 Feature 内部。

### 范式一：端侧入口平铺

适合中小型 Feature，或逻辑边界清晰、模块层级不深的功能。

```text
Features/
└── ExampleFeature/
    ├── __init__.py
    ├── Server.py
    ├── Client.py
    └── Common.py
```

顶层入口只导入对应 Feature 的端侧入口：

```python
# Server.py
from .Features.ExampleFeature import Server

# Client.py
from .Features.ExampleFeature import Client
```

### 范式二：端侧目录聚合

适合大型 Feature，或内部还需要继续拆分多个子模块的功能。

```text
Features/
└── ExampleFeature/
    ├── __init__.py
    ├── ServerSide/
    │   ├── __init__.py
    │   ├── xxx.py
    │   └── xxx.py
    ├── ClientSide/
    │   ├── __init__.py
    │   ├── xxx.py
    │   └── xxx.py
    └── Common/
        ├── __init__.py
        ├── xxx.py
        └── xxx.py
```

顶层入口导入端侧聚合目录即可：

```python
# Server.py
from .Features.ExampleFeature import ServerSide

# Client.py
from .Features.ExampleFeature import ClientSide
```

该范式强调“入口聚合，向下管理”：对外只暴露稳定的端侧入口，对内由各级 `__init__.py` 维护子模块加载关系。

## IMP 即初始化

QuMod 推崇 `import` 即初始化的理念。一个模块被导入时，可以完成必要的初始化工作，例如：

- 注册事件监听。
- 声明 `@AllowCall` RPC 函数。
- 启动 `BaseService.Init` 服务。
- 注册 UI、实体组件或资源配置。

```python
# Features/ExampleFeature/Server.py
from ...QuModLibs.Server import *

@Listen("ServerItemTryUseEvent")
def ServerItemTryUseEvent(args={}):
    ...

@AllowCall
def ExampleServerAPI():
    ...
```

只要顶层 `Server.py` 导入该 Feature，以上监听和 RPC 声明就会完成加载。

::: warning 加载顺序
`import` 即初始化很方便，但也意味着导入顺序会影响副作用执行时机。大型项目建议使用 Feature 聚合入口统一管理子模块导入顺序。
:::

## 静态初始化逻辑 <Badge type="tip" text="^1.3.0" />

如果需要绕过动态注册脚本，也可以自行定制服务端/客户端初始化调用逻辑。

```python
# modMain.py
from .QuModLibs.QuMod import *

def SERVER_INIT():
    import Server

REG_SERVER_INIT_CALL(SERVER_INIT)

def CLIENT_INIT():
    clientApi.RegisterSystem(...)

REG_CLIENT_INIT_CALL(CLIENT_INIT)

def SERVER_CLIENT():
    if IN.IsServerUser:
        myMod.Client("Client")

REG_CLIENT_INIT_CALL(SERVER_CLIENT)
```

## Tools 与可选模块

`QuModLibs/Tools` 提供项目优化、模块裁剪、自动化处理等工具；部分不常用模块在 `v1.4` 后被放入 `Optional/Modules`，可按需复制或集成。
