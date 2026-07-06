# Python AI Code Review Plan

## 1. 目标

基于仓库内已有的 `libs/tree-sitter` 与 `libs/tree-sitter-python`，实现一个面向 Python 项目的代码审查能力，用于识别 AI 生成代码中常见的结构性问题：

- AI 是否忘记已有代码：已有函数、类、工具模块已经能完成任务，新代码却绕开或重复实现。
- AI 是否重复发明轮子：新增逻辑与项目内现有符号、工具函数、服务封装高度相似。
- AI 是否把逻辑堆到一起：单个函数、类或文件承担过多职责，嵌套和分支复杂度明显偏高。
- AI 是否用 `try` 掩盖问题：大范围 `try`、裸 `except`、吞异常、只打印不处理、低置信 fallback。
- AI 是否制造隐式全局状态：模块级可变对象、`global`、单例缓存、导入时副作用、跨函数隐式写状态。
- AI 是否破坏项目依赖结构：跨层导入、反向依赖、循环依赖、从业务层直接依赖底层实现细节。
- AI 是否破坏命名语义：用过短、过泛、与返回语义不匹配的变量名，让代码含义丢失。
- AI 是否滥用或缺失注释/doc：大面积复杂代码没有说明用途，小范围代码又堆满重复注释导致脏乱。

输出不是替代人工 review 的最终判决，而是给 AI/开发者一份带证据、位置、置信度、修复方向的审查报告。

## 2. 非目标

- 不做完整 Python 类型检查，避免与 pyright/mypy 重叠。
- 不尝试执行用户代码，不通过 import 加载被审查项目。
- 不把所有启发式结论伪装成确定性错误；规则必须区分 `error`、`warning`、`risk`、`hint`。
- 不强依赖 Git。Git diff 可作为可选输入，但核心能力只依赖文件系统路径和 tree-sitter AST。

## 3. 与现有能力的关系

当前仓库已有 `src/project_analysis/project_analyzer.*`，并已接入：

- `libs/tree-sitter`
- `libs/tree-sitter-python`
- `src/tools/register_python_analysis.hpp`
- `minecraft_py` 的 `arch` / `imports` 子命令
- `src/common/path_utils.hpp`：UTF-8 ↔ `std::filesystem::path` 的统一封装（`mcdk::path::from_utf8` / `to_utf8` / `filename_to_utf8`），全库通用，`register_python_analysis.hpp` 已在用

新功能建议作为“审查层”增量实现，而不是塞进现有 Python2/Minecraft 项目分析器：

```text
src/python_review/
  py_parser.hpp/.cpp              # TSParser/TSLanguage 包装，复用 tree-sitter-python
  source_file.hpp/.cpp            # 文件读取、行列映射、源码切片
  ast_utils.hpp/.cpp              # AST 遍历、节点文本、父子关系工具
  symbol_index.hpp/.cpp           # 项目符号、函数签名、类、模块级变量
  import_graph.hpp/.cpp           # import/from-import 解析、依赖图、循环检测
  code_metrics.hpp/.cpp           # LOC、嵌套、分支、try 范围、职责指标
  comment_index.hpp/.cpp          # 注释、docstring、注释密度、注释邻接关系
  naming_index.hpp/.cpp           # 标识符拆词、缩写、命名语义候选
  review_rule.hpp                 # 统一 Rule / Finding 接口
  rule_registry.hpp/.cpp          # 内置规则注册、启停、参数合并
  custom_rule_loader.hpp/.cpp     # 外部 TOML/JSON 规则文件加载与校验
  rules/
    existing_code_rule.cpp        # 忘记已有代码
    duplicate_wheel_rule.cpp      # 重复发明轮子
    logic_blob_rule.cpp           # 逻辑堆积
    try_masking_rule.cpp          # try 掩盖问题
    implicit_global_rule.cpp      # 隐式全局状态
    dependency_rule.cpp           # 依赖结构破坏
    naming_rule.cpp               # 命名语义破坏
    comment_doc_rule.cpp          # 注释/doc 缺失或过载
  review_analyzer.hpp/.cpp        # 总入口，组织索引和规则运行
  review_formatter.hpp/.cpp       # Markdown/JSON 报告

src/tools/register_python_review.hpp
tools/mcdk-python-review/
  main.cpp                       # 独立 CLI：传入行为包路径即可诊断所有 Python 包
tests/python_review_test.cpp
```

如果后续希望合并目录，也可以把 `python_review` 迁入 `project_analysis`，但早期建议保持独立边界，避免 Minecraft/Python2 特化逻辑污染通用 Python review。

## 4. MCP 与 CLI 形态

这部分必须同时提供两种入口：

1. MCP 工具能力：让 AI 在协作过程中直接调用审查器。
2. 独立 CLI 程序：可以在命令行、CI 或本地脚本中直接诊断项目。

两者共享 `src/python_review/` 的核心分析器、规则注册表、配置加载和报告格式化逻辑，避免 MCP 与 CLI 行为分叉。

### 4.1 MCP 工具

新增 MCP 工具建议命名为 `python_review`，采用命令式入口，风格对齐当前 `minecraft_py`。默认输入是行为包路径，而不是单个 Python 文件：

```text
python_review(command="review <behavior_pack_path> [--scope <pkg-or-module-list>] [--changed <path1,path2>] [--rules <rule_file>] [--format markdown|json]")
python_review(command="index <behavior_pack_path> [--scope <pkg-or-module-list>] [--rules <rule_file>]")
python_review(command="list-rules [--rules <rule_file>]")
python_review(command="explain-rule <rule-id>")
python_review(command="help")
python_review(command="help-rules [--rules <rule_file>] [--rule <rule-id>] [--format markdown|json]")
```

参数语义：

- `behavior_pack_path`：**必填**，行为包根目录的**完整 UTF-8 路径**。构造 `std::filesystem::path` 必须走现有封装 `mcdk::path::from_utf8`（`src/common/path_utils.hpp`），**禁止直接用 `std::filesystem::u8path` 或 Win32 API**——否则 Windows 中文路径会按本地代码页（GBK）解析而乱码/找不到。遵循该文件既定约定：模块内部一律传 `path`，只有日志/协议边界才用 `to_utf8` 转回字符串。工具在此路径下自动递归发现所有 Python 包。
- `scope`：**选填**，具体包/模块清单（逗号分隔），用于**可控缩小范围**。不填时**自动视为全部**——递归审查该行为包下所有 Python 包。可写包名（`mod_a`）或包内模块（`mod_a/client`、`mod_a.client`）。这是**粗粒度**范围过滤（选哪些包/模块进入分析），与 `changed` 的细粒度文件 diff 互补。
- `changed`：可选，逗号分隔的新增/修改文件或目录，用于只审查本次改动（闭环里常传 AI 刚写的文件）；为空时审查 `scope` 圈定的全部范围。
- `baseline`：可选，用于传入一份旧索引 JSON；没有 baseline 时使用当前项目全量索引做“已有代码”对照。
- `config`：可选，规则阈值与项目分层配置文件。
- `rules`：可选，外部规则文件路径；在内置规则基础上追加、覆盖或禁用规则。
- `format`：`markdown` 默认，`json` 用于自动化测试和工具链消费。

MCP 工具必须提供帮助命令：

- `help`：输出工具用途、所有子命令、所有参数、默认值、示例。
- `list-rules`：列出加载后的有效规则，包括内置规则和 `--rules` 文件追加的规则。
- `explain-rule <rule-id>`：解释单条规则的目的、触发条件、默认阈值、可配置项和示例 finding。
- `help-rules`：输出规则系统完整帮助，包括规则文件 schema、可用 matcher、所有内置规则参数和默认值。
- `help-rules --rule <rule-id>`：只输出指定规则的参数、默认值和可覆盖字段。

行为包扫描规则：

- 从 `behavior_pack_path` 开始递归查找所有 `modMain.py`。
- 每个 `<behavior_pack_path>/<mod_name>/modMain.py` 视为一个独立 Python 包入口。
- 对每个包递归扫描 `.py` 文件，并建立包内模块名、import graph、符号索引。
- 行为包内可以同时存在多个 Python 包，报告需要按包分组，并在 Summary 中列出包数量、入口文件和解析健康度。
- 若没有发现 `modMain.py`，退化为普通 Python 目录扫描，但报告中标记 `package_discovery=fallback-directory`。

建议支持项目配置文件，推荐 `.mcdk-python-review.toml`：

```toml
allowed_dependencies = [
  ["service", "domain"],
  ["infra", "domain"],
]

ignore_paths = [
  "**/__pycache__/**",
  "**/.venv/**",
  "**/site-packages/**",
]

[rules]
enable_builtin = true
disabled = ["comment-doc.too-dense"]

[rules.severity_overrides]
"naming.semantic.too-short" = "warning"

[[layers]]
name = "domain"
patterns = ["src/domain/**"]

[[layers]]
name = "service"
patterns = ["src/service/**"]

[[layers]]
name = "infra"
patterns = ["src/infra/**"]

[thresholds]
function_loc = 80
nesting_depth = 4
branch_count = 12
try_body_loc = 40
similarity = 0.72
comment_min_density = 0.04
comment_max_density = 0.38
short_name_max_len = 2

[naming]
allowed_short_names = ["i", "j", "k", "x", "y", "z", "id", "db", "ui"]
banned_names = ["data", "obj", "tmp", "ret", "res"]

[[naming.semantic_expectations]]
call = "clientApi.GetEngineCompFactory"
preferred_names = ["compFactory", "engineCompFactory"]
bad_names = ["CF", "cf", "factory"]
```

配置文件用于项目级审查策略；`--rules <path>` 用于临时追加规则，例如某次 review 只想检查团队刚发现的一类坏味道。

### 4.2 独立 CLI 程序

新增可单独编译的命令行程序，建议 target 名称：

```text
mcdk-python-review
```

基本用法：

```text
mcdk-python-review <behavior_pack_path>
mcdk-python-review <behavior_pack_path> --scope mod_a,mod_b/client
mcdk-python-review <behavior_pack_path> --rules review-rules.toml
mcdk-python-review <behavior_pack_path> --format json --output report.json
mcdk-python-review <behavior_pack_path> --changed my_mod/modMain.py,my_mod/client
mcdk-python-review --list-rules
mcdk-python-review --explain-rule naming.semantic.bad-call-result-name
mcdk-python-review --help
mcdk-python-review --help-rules
mcdk-python-review --help-rules --rule naming.semantic.bad-call-result-name
```

CLI 行为要求：

- 第一个位置参数是行为包完整 UTF-8 路径（必填）。
- `--scope <pkg-or-module-list>` 选填，缩小到指定包/模块；不传时自动分析行为包下所有 Python 包，不要求用户逐个传 `modMain.py`。
- 默认输出 Markdown 到 stdout。
- `--output <path>` 写报告文件；不传则只打印。
- `--format markdown|json` 与 MCP 输出字段保持一致。
- `--scope <pkg-or-module-list>` 缩小到指定包/模块；不传视为全部。
- `--rules <path>` 支持加载外部规则文件。
- `--config <path>` 支持加载项目结构和阈值配置。
- `--fail-on warning|error` 可用于 CI；存在达到阈值的 finding 时返回非零退出码。
- `--ignore <glob>` 可追加忽略路径。
- `--help` 输出 CLI 参数、默认值和示例。
- `--help-rules` 输出规则系统帮助、规则文件 schema、内置规则默认值。
- `--list-rules` 输出加载后的有效规则列表。
- `--explain-rule <rule-id>` 输出单条规则说明。

CMake 接入建议：

```text
tools/mcdk-python-review/CMakeLists.txt
tools/mcdk-python-review/main.cpp
```

该 CLI 只链接核心审查库和已有 vendored tree-sitter，不依赖 MCP server runtime。

### 4.3 工具参数默认值

MCP 和 CLI 必须共享同一份默认值定义，并在 `help` / `help-rules` 输出中展示。建议默认值：

```text
behavior_pack_path            required (完整 UTF-8 路径)
scope                         "" (空 = 全部包/模块)
changed                       ""
config                        auto-detect .mcdk-python-review.toml/.json
rules                         none
format                        markdown
output                        stdout
fail_on                       none
ignore                        []
max_files                     5000
max_file_size_kb              512
max_total_size_mb             20
include_tests                 true
include_third_party           false
package_discovery             modMain, fallback-directory
report_context_lines          2
min_confidence                0.50
severity_threshold            hint
```

内置规则默认阈值也必须通过 `help-rules` 输出：

```text
function_loc                  80
class_public_methods          20
file_loc                      500
nesting_depth                 4
branch_count                  12
return_count                  8
try_body_loc                  40
max_try_depth                 2
similarity                    0.72
high_similarity               0.86
comment_min_density           0.04
comment_max_density           0.38
max_code_run_without_comment  80
short_name_max_len            2
generic_name_min_scope_loc    20
max_relative_import_level     2
```

### 4.4 自定义规则文件格式

规则系统必须有一套内置规则，同时允许通过参数传入规则文件路径：

```text
python_review(command="review D:/behavior_pack --rules D:/behavior_pack/review-rules.toml")
mcdk-python-review D:/behavior_pack --rules D:/behavior_pack/review-rules.toml
```

JSON 不建议作为主规则编写格式。它适合机器输出、测试断言和 schema 交换，但不适合长期人工维护规则：不能写注释，长 message/suggestion 难读，深层嵌套会产生大量引号和逗号，规则 diff 噪声也更大。

YAML 也不建议作为默认主格式。它很适合表达嵌套结构，但对审查规则而言有几个明显缺点：缩进错误不容易在 diff 中察觉，YAML 1.1/1.2 的隐式类型差异容易制造歧义，anchors/aliases/merge keys 会让规则来源变得不直观。即使采用受限 YAML，也需要额外解释和防御。

推荐格式：

- 规则文件主格式：TOML 1.0，扩展名 `.toml`。
- JSON 只作为兼容输入和机器输出格式，扩展名 `.json`。
- YAML 可作为未来可选导入格式，但不是默认推荐格式。
- 如果实现阶段不想立刻引入 TOML 解析依赖，可以先支持 JSONC 作为过渡格式，但计划目标应是 TOML 规则文件，而不是裸 JSON。

TOML 规则文件第一阶段不执行脚本，避免把审查工具变成任意代码执行入口。它必须支持深度自定义：

- `disable_builtin`：禁用指定内置规则。
- `override`：覆盖内置规则阈值、严重级别、置信度、说明文案、适用路径。
- `profiles`：定义多套规则组合，例如 `strict`、`ci`、`local`。
- `rule_defaults`：统一覆盖全局默认值。
- `path_overrides`：按目录/文件 glob 覆盖规则，例如测试目录降低 docstring 要求。
- `naming`：定义命名约定、缩写白名单、调用结果推荐变量名。
- `ast_patterns`：基于 tree-sitter 节点类型、调用名、赋值目标、作用域的声明式匹配。
- `dependencies`：补充或覆盖项目分层依赖约束。
- `comments`：定义注释/docstring 密度、复杂代码注释要求、过度注释判定。
- `message_templates`：自定义报告文案和修复建议。
- `tags`：为规则打标签，例如 `ai-generated`、`architecture`、`naming`、`ci-blocker`。

TOML 示例：

```toml
version = 1
profile = "strict"

# 可以禁用过吵的内置规则。
disable_builtin = ["comment-doc.too-dense"]

[rule_defaults]
min_confidence = 0.55
severity = "risk"
enabled = true

[override."logic-blob.large-function"]
severity = "warning"
include_paths = ["my_mod/**/*.py"]
exclude_paths = ["my_mod/generated/**"]

[override."logic-blob.large-function".thresholds]
function_loc = 100

[[path_overrides]]
paths = ["tests/**", "**/*_test.py"]

[path_overrides.override."comment-doc.missing-purpose"]
severity = "hint"

[path_overrides.override."naming.semantic.generic-name"]
enabled = false

[naming]
allowed_short_names = ["i", "j", "k", "x", "y", "z", "id", "db", "ui"]
banned_names = ["data", "obj", "tmp", "ret", "res"]

[[naming.semantic_expectations]]
id = "netease.comp-factory-name"
when_assigned_from_call = "clientApi.GetEngineCompFactory"
preferred_names = ["compFactory", "engineCompFactory"]
reject_names = ["CF", "cf", "factory"]
severity = "warning"
message = """
GetEngineCompFactory() 的返回值应保留 compFactory 语义，
CF 会严重破坏可读性。
"""

[[ast_patterns]]
id = "project.no-silent-create-component"
severity = "risk"
confidence = 0.80
tags = ["try", "component"]
message = "组件创建失败不应被 try 静默吞掉。"

[ast_patterns.match]
node = "call"
callee = "*.CreateComponent"
inside_try_without_raise = true

[comments]
min_density = 0.04
max_density = 0.38
complexity_requires_doc = 8
allow_header_blocks = true
```

TOML 解析策略：

- 采用 TOML 1.0 语义。
- rule id 这类含点号的 key 必须使用 quoted key，例如 `[override."logic-blob.large-function"]`。
- 长说明使用 TOML multiline basic string。
- 所有字段必须经过 schema 校验，未知字段输出 config warning。
- 内部归一化成统一 `RuleConfig` DTO，后续规则引擎不关心来源是 TOML 还是 JSON。

第一阶段的自定义规则只做声明式匹配，不支持用户传入 Python/Lua/JS 脚本。若未来需要脚本规则，应单独设计沙箱、超时、只读文件访问和禁用网络的执行模型。

规则文件加载顺序：

1. 加载内置默认值。
2. 加载 `.mcdk-python-review.toml` / `.json` 项目配置。
3. 加载 `--rules <path>` 外部规则文件。
4. 应用命令行参数覆盖。
5. 生成有效规则表，供 `list-rules` / `help-rules` 展示。

冲突处理：

- 后加载的配置覆盖先加载的配置。
- `enabled=false` 优先级高于 severity 覆盖。
- 外部规则 id 与内置规则 id 冲突时，默认视为覆盖；若要新增变体必须使用新 id。
- 无法识别的字段不应静默忽略，应在报告和帮助输出中列为 config warning。

### 4.5 规则帮助输出契约

`help-rules` 必须输出机器和人都能读懂的信息。Markdown 输出建议包含：

```text
# Python Review Rules Help

## Rule File Loading
- builtin defaults
- .mcdk-python-review.toml / .json
- --rules <path>
- command-line overrides

## Global Defaults
- format: markdown
- min_confidence: 0.50
- max_files: 5000

## Builtin Rules
### naming.semantic.bad-call-result-name
- default severity: warning
- default confidence: 0.75
- configurable fields:
  - enabled: true
  - severity: warning
  - allowed_short_names: [...]
  - semantic_expectations: []
- example:
  CF = clientApi.GetEngineCompFactory()
- suggestion:
  compFactory = clientApi.GetEngineCompFactory()

## Custom Rule Schema
- override
- path_overrides
- naming.semantic_expectations
- ast_patterns
- comments
- dependencies
```

JSON 输出必须包含稳定字段：

```json
{
  "global_defaults": {},
  "builtin_rules": [],
  "custom_rules": [],
  "schema": {},
  "config_warnings": []
}
```

## 5. 核心数据模型

### 5.1 行为包索引

审查器的顶层输入是行为包路径，先构建行为包索引：

- `behavior_pack_root`：规范化后的行为包根目录。
- `python_packages`：自动发现的 Python 包列表。
- `entry_file`：每个包的 `modMain.py`。
- `package_root`：`modMain.py` 所在目录。
- `package_name`：默认取包目录名。
- `files`：包内递归发现的 `.py` 文件。
- `discovery_mode`：`modMain` 或 `fallback-directory`。
- `ignored_paths`：因配置、缓存目录或第三方库规则被跳过的路径。

行为包内多个包必须在索引和报告中保持隔离：依赖图默认先按包内解析，跨包 import 单独标记为跨包依赖，不直接混成一个普通 Python 项目。

### 5.2 文件索引

每个 `.py` 文件解析为：

- 文件路径、模块名、包名。
- parse health：`ok`、`recovered`、`fallback`。
- import 列表：原始文本、导入模块、导入符号、行号、是否相对导入。
- 符号列表：函数、类、方法、模块变量、常量。
- 顶层副作用：顶层调用、顶层 I/O、注册调用、monkey patch 赋值。
- 注释/doc 摘要：文件级 docstring、类/函数 docstring、普通注释行、注释块、注释密度。
- 语法错误摘要：`ERROR` / `MISSING` 节点数量和位置。

### 5.3 函数/类摘要

每个函数或方法记录：

- 名称、限定名、行列范围、LOC。
- 参数列表、默认值是否可变。
- 返回点数量、分支数量、循环数量、异常处理数量、最大嵌套深度。
- 调用摘要：被调用函数名、属性链、构造调用。
- 读写摘要：局部变量、nonlocal/global、模块级变量写入。
- 命名摘要：局部变量名、赋值来源、缩写程度、是否命中项目语义命名规则。
- 注释摘要：docstring 是否存在、复杂块前是否有用途说明、注释/代码比例。
- 规范化 token 指纹：用于相似度和重复实现检测。

### 5.4 审查结果

统一 Finding：

```json
{
  "finding_key": "try.masking.broad-except-pass@src/foo.py::Foo.load",
  "rule_id": "try.masking.broad-except-pass",
  "severity": "warning",
  "tier": 1,
  "actionability": "should-fix",
  "confidence": 0.91,
  "file": "src/foo.py",
  "line": 42,
  "symbol": "Foo.load",
  "title": "broad except swallows the failure",
  "evidence": [
    "except Exception handler only logs/prints and continues",
    "try body spans 67 lines"
  ],
  "suggestion": "Catch a narrower exception, handle the error path explicitly, or re-raise after adding context."
}
```

字段约束（面向 AI 闭环，见 §8.1）：

- `finding_key`：`rule_id + 规范化位置 + 符号`，稳定且去重，保证同一份代码多次 review 输出一致，AI 不因报告抖动反复改。
- `tier`：规则精度层（见 §6.0）。
- `actionability`：`must-fix` / `should-fix`（Tier 1/2，AI 可直接据此修改）/ `advisory-verify`（Tier 3，**仅提示核实，禁止盲改**）。

### 5.5 引用解析状态与开放世界假设

审查器对项目外符号采用**开放世界假设**：解析不到不代表代码有错，只代表在工具视野之外（标准库、游戏内部库、netease/Minecraft ModSDK、第三方包、运行时注入）。每个被引用符号标注解析状态：

- `local`：本文件内定义。
- `project`：命中项目符号 / import 索引。
- `external-known`：标准库，或游戏 API 白名单（`clientApi` / `serverApi` / 组件工厂 / ModSDK 接口等）。
- `unresolved`：无法解析（第三方包、动态属性、game 内部库、反射注入）。

硬规则：

- **`external-known` 与 `unresolved` 引用绝不产生「未定义 / 缺失导入 / 未知符号」类 finding，不抛异常、不告警。** 解析失败只是**静默降低覆盖**，永不制造噪声。
- **需要解析**的规则（§6.1 忘记已有代码、§6.2 的项目候选替换、§6.6 依赖结构）对 `unresolved` 引用**跳过**，不当违规；项目外依赖不计入循环 / 分层判定。
- **不需要解析**的规则照常作用于「对外部符号的调用」：§6.4 `try` 滥用、§6.7 命名、§6.8 注释、§6.3 复杂度、§6.5 隐式全局。例如 `CF = clientApi.GetEngineCompFactory()` 的命名、`try` 罩住 game API 调用后的静默吞异常——都与 callee 能否解析无关，照查。
- 游戏 API 白名单（`external-known`）可选、可增量维护；有它能让命名 / 用法规则对 netease API 更聪明，但**没有它也不得退化为报错**，只是这些外部调用少一层用法检查而已。

## 6. 检查规则设计

### 6.0 规则精度分层与严重级别映射

这些规则的误报率不在一个数量级上，因此必须先确立统领原则：**精度决定默认 severity，severity 决定能否进入 `--fail-on`。** 不允许「高噪声启发式」与「确定性语法铁证」在报告里同权呈现——否则用户被 Tier 3 噪声淹没后会连 Tier 1 一起关掉。

| 层级 | 判定性质 | 代表规则 | 默认 severity | 可否 CI 阻断 |
|------|----------|----------|:---:|:---:|
| Tier 1 | 确定性语法铁证，AST 可直接判定，误报极低 | `try.masking.*`、`implicit-global.mutable-default`、`implicit-global.global-write`、`stub.placeholder`、`comment-doc.noisy-restatement` | warning（部分 error） | ✅ 允许 |
| Tier 2 | 度量/结构类，客观可算但有阈值悬崖与风格差异 | `logic-blob.*`、`dependency.cycle`、`dependency.layer-violation`、`comment-doc.missing-purpose`、`clone.intra-file` | warning | ⚠️ 默认否，可显式开启 |
| Tier 3 | 启发式/相似度/主观语义，token 级近似，能力天花板有限 | `existing-code.*`、`duplicate-wheel.*`、`naming.semantic.*` | risk / hint | ❌ 永不阻断 |

规则实现时，`severity` 默认值必须与其所在 Tier 一致；`--rules` 覆盖可调整单条 severity，但 Tier 3 规则被提升到 warning 时，`help-rules` 输出应标注「用户强制提级，精度自负」。

**度量类避免阈值悬崖**：Tier 2 的 `logic-blob` 等不应「79 行静默、81 行 warning」。建议分级映射，例如 `function_loc` 80→hint、150→warning、300→risk，让 severity 随程度连续变化，而不是单点跳变。

**信号收敛（compound scoring）**：代码质量本质是多信号共振，规则不应完全孤立判定。规则引擎合并 finding 时（见 §5.4、Phase 2）执行两条调整：

- **叠加增信**：同一符号命中多条规则时提升整体 confidence，并聚合为一个「high-risk symbol」条目上报，而非刷屏多条。例如某函数同时命中 `logic-blob.large-function` + `try.masking.broad-except` + `comment-doc.missing-purpose`，几乎必然有问题，应合并并提级展示。
- **孤证抑制**：Tier 3 的单条弱信号若在同一符号上无任何其他信号佐证，默认压到 `hint` 或直接抑制，避免相似度/命名规则单独刷屏。

`confidence` 是启发式估计而非统计概率，`help-rules` 必须明确标注，避免用户把 `0.80` 误读为「80% 正确」。

**游戏业务适配**：多数通用 Python linter 规则带有企业/PEP8 惯例假设，直接套进 Minecraft mod 业务会大量误报。每条规则都要区分「域安全内核」（不吃 mod 惯例，保持既定 Tier）与「企业惯例外壳」（在 mod 语境下降级或豁免）。下表为默认适配，可被 `--rules` 覆盖：

| 规则 | 域安全内核（保留 Tier） | 企业外壳（mod 语境降级/豁免） |
|------|------|------|
| §6.4 try 掩盖 | `except: pass` 真静默吞异常 → warning | 已 `logging` 记录并继续，在 mod 里常是有意容错（避免单系统崩溃拖垮客户端）→ 降 risk/hint |
| §6.5 隐式全局 | 可变默认参数、`global` 写模块变量 → Tier 1 | 模块级单例 / `registry` / `manager` / `clientApi` 缓存是 netease 建模惯用法 → 默认不报或 hint |
| §6.3 逻辑堆积 | 明显超阈值的巨型函数 | 事件分发 / UI 按钮路由天然长且多分支（`if event_id == ...` 链）→ handler/dispatch 模式函数用更高阈值，纯分发链不计入复杂度 |
| §6.7 命名 | `semantic_expectations` 驱动的语义丢失（如 `CF = GetEngineCompFactory()`） | camel/snake 风格一致性、布尔谓词前缀 → netease API 强制 camelCase/PascalCase，风格混用被 API 逼迫，风格类子规则默认 off、API 派生名豁免 |
| §6.8 注释 | `comment-doc.noisy-restatement`（注释复述相邻代码，纯 AI tell）→ Tier 1 | 注释密度下限、public 函数强制 docstring → 小体量游戏脚本天然少注释、`OnXxxEvent` 回调名自解释，默认 off/hint，生命周期回调豁免 |

底线（正向守住）：`try: pass` 真静默、可变默认参数、`global` 写、语义命名丢失、注释复述这五类**不吃「预留/惯例」借口**，是本工具对 mod 代码的核心价值，不因「游戏代码」而废——适配只收企业外壳，不砍域安全内核。

### 6.1 AI 忘记已有代码

目的：发现新代码绕过项目内已有封装、工具函数或服务入口。

信号：

- 新增/修改函数名与已有函数名 token 高相似，例如 `load_config_file` vs `read_config`。
- 新函数的调用序列、字符串常量、参数形状与已有函数相似。
- 新代码直接访问底层 API，而项目已有更高层封装，例如多处已有 `config.load()`，新代码直接 `open(... json.load(...))`。
- 新文件重新定义项目中已有的常量、路径拼接、协议字段、错误码。

实现方式：

- 建立全项目 symbol index。
- 对 changed scope 内的新/修改符号生成 normalized fingerprint。
- 与项目内其他符号做相似度比较：
  - 名称 token Jaccard。
  - 调用 multiset Jaccard。
  - 字符串/数字常量 overlap。
  - 参数名 overlap。
- 命中后报告“可能已有实现”，列出候选符号与证据。

定位（Tier 3，baseline 驱动）：

- 本规则真正要回答的是「本次**新增/修改**的代码，是否重复了改动**之前**就已存在的封装」。因此应以 `baseline`（见 §95）为对照基准：只把 changed scope 内的新符号，与 baseline 索引里的既有符号比对。
- 没有 baseline 时，无法区分两个相似符号里谁是「原有」、谁是「新造重复」，能力显著受限：此时仅在 changed 文件内对项目其余符号做**高阈值**比对（`high_similarity` 起报），每条最多列 top-3 候选。
- 默认输出 `risk`，**永不进入 `--fail-on`**；仅当相似度极高且候选位于明确工具模块时才提升为 `warning`。

### 6.2 AI 重复发明轮子

目的：发现项目已有工具或标准库封装可复用，新代码却自写低质量实现。

信号：

- 手写常见算法：flatten、chunk、retry、memoize、deep merge、safe get、path normalize。
- 手写解析：用 split/replace 解析 JSON、CSV、URL、路径、import path。
- 手写缓存/单例，而项目已有 cache/helper。
- 手写文件查找、递归遍历，而项目已有 file/project index 工具。

实现方式：

- 建立“轮子模式”规则库，基于 AST 模式而非纯字符串：
  - `for` + `append` + nested list -> flatten/chunk 类模式。
  - `split(":")` / `split(",")` + 固定下标 -> ad-hoc parser。
  - `os.walk` 大段逻辑 -> 文件扫描工具候选。
- 结合项目符号索引找可替代函数。
- 报告要包含“已有候选 API”，而不是只说重复。

文件内克隆单列：AI 复制粘贴产生的近似兄弟函数/分支，用同一份规范化指纹在**单文件/单包内**即可检出，精度远高于跨项目相似度，应拆为独立规则 `clone.intra-file`（Tier 2），不要混进跨项目「重复轮子」的 Tier 3。跨项目相似度（本节主体）保持 Tier 3。

### 6.3 AI 把逻辑堆到一起

目的：识别函数/类/文件职责过载。

信号：

- 函数 LOC 超阈值。
- 最大嵌套深度超阈值。
- 分支数、循环数、早返回数量过多。
- 单函数同时处理 I/O、解析、业务判断、格式化输出。
- 单文件出现多个互不相关的高层概念。
- 类中 public 方法过多，或同时持有配置、状态、I/O、格式化职责。

实现方式：

- 使用 tree-sitter 统计 `function_definition`、`class_definition`、`if_statement`、`for_statement`、`while_statement`、`try_statement`、`with_statement`、`return_statement`。
- 对函数调用前缀做职责粗分：
  - 文件/路径 I/O：`open`、`os.*`、`pathlib.*`。
  - 网络：`requests.*`、`urllib.*`。
  - 序列化：`json.*`、`yaml.*`、`pickle.*`。
  - 日志/输出：`print`、`logging.*`。
  - 项目内部业务模块调用。
- 职责种类超过阈值时提示拆分方向。

### 6.4 AI 用 try 掩盖问题

目的：识别异常处理被用来隐藏设计问题。

信号：

- 裸 `except:`。
- `except Exception` / `BaseException`。
- handler 只有 `pass`、`return None`、`return False`、`print`、`logging` 后继续。
- `try` body 范围过大。
- 嵌套 `try` 过多。
- `try` 包住 import 后静默 fallback。
- handler 中没有 re-raise，也没有上下文补充或明确降级路径。

实现方式：

- 遍历 `try_statement`、`except_clause`。
- 提取 except 类型、handler body 节点类型、try body LOC。
- 判断 handler 是否包含 `raise`。
- 判断 handler 是否只包含低信息操作：`pass`、`print`、`logging.*`、简单赋默认值。
- 对 import fallback 单独标记为 `risk`，需要人工确认是否为兼容策略。

### 6.5 AI 制造隐式全局状态

目的：发现隐藏、难测试、难复用的状态。

信号：

- 模块级可变对象：`[]`、`{}`、`set()`、`dict()`、自定义类实例。
- 函数内 `global` / `nonlocal`。
- 写入模块级变量。
- 默认可变参数。
- 单例缓存：`_instance`、`_cache`、`registry`、`manager`。
- 顶层执行 I/O、注册、启动线程、读取环境变量。
- monkey patch：给外部模块/类动态赋值。

实现方式：

- 区分 module scope、class scope、function scope。
- 记录模块级 assignment 目标与值类型。
- 在函数内查找 `global_statement`、`nonlocal_statement` 和对模块级目标的写入。
- 查找 `default_parameter` 中的 list/dict/set/call。
- 顶层调用按 allowlist/denylist 分级：常量构造低风险，I/O 和注册高风险。

### 6.6 AI 破坏项目依赖结构

目的：保证新增代码没有绕过项目边界。

信号：

- 新增 import 形成循环依赖。
- 低层模块反向依赖高层模块。
- 业务模块直接依赖工具内部实现或测试模块。
- 从 sibling/private 模块导入 `_internal`、`_impl`。
- 相对导入层级过深。
- `sys.path.append` / `importlib` 动态绕过正常依赖结构。

实现方式：

- 基于 import graph 计算强连通分量。
- 根据配置文件的 `layers` 和 `allowed_dependencies` 判断方向。
- 没有配置时使用启发式目录层级：
  - `tests` 不应被 `src` 导入。
  - `tools` / `scripts` 不应被核心库依赖。
  - `_private` / `_internal` 默认不建议跨包导入。
- 动态 import 作为 `risk` 报告，并输出具体字符串或表达式。

### 6.7 AI 破坏命名语义

目的：发现 AI 为了省事使用过短、过泛、误导性的变量名，导致代码含义丢失。

典型例子：

```python
CF = clientApi.GetEngineCompFactory()
```

`CF` 严重破坏含义。根据调用语义，推荐命名应为 `compFactory` 或 `engineCompFactory`。

信号：

- 变量名长度过短，且不在局部短名白名单中。
- 变量名全大写，但实际不是常量。
- 变量名过泛：`data`、`obj`、`tmp`、`ret`、`res`、`manager`、`handler`。
- 赋值来源是强语义调用，但变量名没有保留核心名词。
- 同一作用域内出现多个相似泛名，例如 `data1`、`data2`、`result2`。
- 布尔变量不是谓词命名，例如缺少 `is`、`has`、`can`、`should`。
- 集合变量名与值类型冲突，例如 list 却使用单数名，单个对象却使用复数名。
- 类名、函数名、变量名风格与项目局部约定明显不一致。

实现方式：

- 对所有 identifier 做拆词：camelCase、snake_case、PascalCase、全大写缩写。
- 为赋值表达式提取语义来源：
  - `x = clientApi.GetEngineCompFactory()` -> call source `clientApi.GetEngineCompFactory`。
  - `x = FooBar(...)` -> constructor source `FooBar`。
  - `x = module.create_user_profile(...)` -> call tokens `create/user/profile`。
- 基于调用名、构造名、属性链生成推荐 token。
- 与变量名 token 比较，低重叠时输出风险。
- 加载配置中的 `naming.semantic_expectations`，覆盖项目特定约定。

内置规则：

- `naming.semantic.too-short`：非白名单短名。
- `naming.semantic.bad-call-result-name`：赋值来源语义强但变量名丢失关键 token。
- `naming.semantic.generic-name`：泛名在较大作用域或复杂函数中出现。
- `naming.style.constant-confusion`：局部变量使用常量风格。
- `naming.boolean.non-predicate`：布尔语义变量没有谓词前缀。

命名规则要避免误伤：

- `i`、`j`、`x`、`y` 等短循环变量在很小作用域中允许。
- `id`、`db`、`ui` 等项目允许缩写由配置控制。
- 测试 fixture 中的局部样例变量可以降低严重级别。
- 对第三方 API 固定字段名不强行改名。

### 6.8 注释与 docstring 质量

目的：发现两个极端：

- 大面积复杂代码没有任何用途说明，后续维护者不知道为什么存在。
- 小范围简单代码塞入大量重复注释，反而让代码脏乱。

信号：

- 文件没有模块 docstring，且文件 LOC/复杂度超过阈值。
- public class/function 没有 docstring，且名称不足以解释用途。
- 高复杂函数没有用途说明、输入输出说明或关键分支说明。
- 大段业务逻辑连续超过阈值行数没有任何注释。
- 注释密度过低：复杂代码几乎没有注释。
- 注释密度过高：简单代码旁边逐行解释“做了什么”。
- 注释与代码重复：例如 `# set name` 后面是 `name = ...`。
- TODO/FIXME/HACK 大量堆积，没有 owner、原因或后续动作。
- 注释块离目标代码太远，或者连续注释块覆盖多个不相关逻辑。

实现方式：

- tree-sitter-python 会保留 `comment` 节点；同时识别模块、类、函数 body 首个 string 作为 docstring。
- 对文件、类、函数分别计算：
  - `code_lines`
  - `comment_lines`
  - `docstring_lines`
  - `comment_density`
  - `max_code_run_without_comment`
  - `complexity_score`
- 高复杂度且低注释密度时输出 `comment-doc.missing-purpose`。
- 低复杂度且高注释密度时输出 `comment-doc.too-dense`。
- 注释文本与相邻代码 token 高重叠时输出 `comment-doc.noisy-restatement`。
- TODO/FIXME/HACK 缺少上下文时输出 `comment-doc.unowned-todo`。

内置阈值建议：

- 复杂函数 `complexity_score >= 8` 且没有 docstring/用途注释 -> `warning`。
- 文件 `code_lines >= 200` 且注释密度 `< 0.04` -> `risk`。
- 函数 `code_lines <= 20` 且注释密度 `> 0.50` -> `hint` 或 `risk`。
- 连续无注释业务代码超过 `80` 行 -> `risk`。

注释规则要关注“解释为什么”，而不是鼓励机械注释。报告建议应优先提示补充用途、约束、边界条件、异常策略，而不是要求给每行代码加注释。

### 6.9 桩实现与占位残留

目的：发现 AI 留下的半成品。注意游戏业务里「空实现是有意预留」非常常见（空的事件/生命周期回调重写、按字符串反射注册的接口、随时可能接上的钩子），因此本规则只盯**明确表达“还没写完”**的信号，绝不盯“暂时没用到 / 未被调用”——后者是 PyCharm 式企业规则，不适用于 mod 业务，已从本计划移除。

信号：

- `raise NotImplementedError`。
- `# TODO: implement`、`# FIXME`、`# placeholder`、`# stub` 且无 owner/原因。
- 函数体只有 `...`（Ellipsis）却被正常调用。

实现方式：

- 检测 `function_definition` 是否处于**明确未完成态**：单 `raise NotImplementedError` / 单 `Ellipsis`。
- **空 `pass` body、只 `return None` 不单独报**——它们在 mod 里大量是有意预留的空回调重写（如空的 `OnDestroy`），仅当同时带 TODO/FIXME 注释时才升级。
- 抽象方法（`@abstractmethod`、`Protocol`、`ABC` 子类）、重写基类/注册回调的空实现一律豁免。
- 与 §6.8 `unowned-todo` 共享 TODO/占位识别逻辑，避免重复实现。

定位（Tier 1）：`raise NotImplementedError` 默认 `warning`；裸 TODO 默认 `hint`。

## 7. Tree-sitter 查询策略

优先使用 C API 手写遍历，避免一开始引入 query 文件管理复杂度。需要稳定后，可把常见规则迁移到 `.scm`：

```text
src/python_review/queries/
  imports.scm
  symbols.scm
  try_masking.scm
  globals.scm
  naming.scm
  comments.scm
```

第一阶段必须覆盖节点：

- `module`
- `import_statement`
- `import_from_statement`
- `function_definition`
- `class_definition`
- `assignment`
- `augmented_assignment`
- `global_statement`
- `nonlocal_statement`
- `try_statement`
- `except_clause`
- `call`
- `attribute`
- `return_statement`
- `raise_statement`
- `comment`
- `string`
- `identifier`
- `keyword_identifier`

所有节点提取都要容忍 `ERROR` / `MISSING`，并在 report 中输出 `parse_health`。

## 8. 报告格式

Markdown 默认输出：

```text
# Python AI Code Review

## Summary
- files_scanned: 42
- packages_scanned: 3
- package_discovery: modMain
- changed_files: 3
- findings: 11
- highest_severity: warning
- parse_health: 40 ok, 2 recovered

## Findings
### [warning] try.masking.broad-except-pass
src/foo.py:42 Foo.load
Evidence:
- catches Exception
- handler only logs and returns None
- try body spans 67 lines
Suggestion:
Catch a narrower exception or re-raise with context.

## Existing Code Candidates
- src/config/loader.py:18 read_config_file, similarity=0.84

## Dependency Graph Risks
- cycle: a.service -> b.store -> a.service
```

JSON 输出用于测试断言，字段稳定，不直接依赖中文文案。

### 8.1 面向 AI 闭环的消费契约

本诊断的**主要消费者是写完代码的 AI**，而非人类 reviewer：报告打回后由 AI 据此调整，形成 review → fix → re-review 闭环。这个定位改变几条核心约束：

1. **误报的代价被放大为“破坏”。** 人看到坏 finding 一眼略过；AI 被告知「修这个」往往会照做——为不存在的问题改代码、制造 churn，甚至按报告删掉「预留回调」。因此 §6.0 的游戏业务适配与降噪不是可选优化，而是**防止 AI 主动破坏正确代码**的必需项。原则：宁可漏报，不可让 AI 照着假阳性改。

2. **Finding 必须区分「可执行」与「仅供核实」**（见 §5.4 `actionability`）：
   - `must-fix` / `should-fix`（Tier 1/2 确定性问题）：AI 可直接据此修改。
   - `advisory-verify`（Tier 3 启发式/相似度/命名）：**只提示核实，禁止盲改**。措辞用「疑似…，请确认；确为重复则复用 X，若为有意保留则忽略」，而非「删除」「重命名」。AI 面对相似度/死代码类命令式指令会把工作代码改坏（Goodhart：为过检把 `CF`→`cf2`、为降 LOC 无意义拆函数、把 `except: pass` 换成 `except: log` 却仍不真处理）。

3. **闭环必须收敛。** 报告携带机器可读门控：
   - 驱动方只对 `must-fix`/`should-fix` 迭代；`advisory-verify` 不触发自动修复，仅供 AI 确认。
   - 终止条件：无 must/should-fix 剩余，**或**连续两轮 `finding_key` 集合不变——防止「修 A 引入 B」的震荡。
   - 稳定 `finding_key` + 确定排序 + 去重，保证同代码多次 review 输出一致。

4. **抑制机制防止反复打回有意模式。** 支持行内 `# mcdk-review: ignore <rule-id> [原因]` 与配置级抑制。被抑制 finding 不上报但计入 summary。没有它，闭环会对「有意预留的空回调」「被 API 逼的 camelCase」反复报、AI 反复「修」，永不收敛。

5. **JSON 是闭环主格式**：打回给 AI 用紧凑、确定性、字段稳定的 JSON（省 token、可 diff）；Markdown 仅供人阅读。

## 9. 实现阶段

### Phase 1：解析与索引

- 新增 `python_review` 目录。
- 所有路径构造走 `mcdk::path::from_utf8`（复用 `src/common/path_utils.hpp`），禁止裸 `u8path`/系统 API；内部传 `path`，边界用 `to_utf8`。
- 封装 tree-sitter parser。
- 输入行为包路径，自动发现所有 `modMain.py` 对应的 Python 包。
- 扫描每个 Python 包内 `.py` 文件，支持 ignore paths。
- 提取 imports、symbols、module globals、function metrics、naming metrics、comment/doc metrics。
- 增加 `tests/python_review_test.cpp` 的最小样例。

验收：

- 能扫描一个临时行为包，自动发现多个 Python 包。
- 行为包没有 `modMain.py` 时能退化为普通目录扫描并标记 fallback。
- 能输出稳定 JSON index。
- 对语法错误文件不崩溃，`parse_health` 正确降级。

### Phase 2：规则引擎

- 定义 `ReviewRule` 接口。
- 定义 `RuleRegistry`，统一注册内置规则和外部声明式规则。
- 定义 `CustomRuleLoader`，加载 `--rules` 指向的 TOML/JSON 规则文件。
- 定义 `RuleHelpFormatter`，输出规则帮助、参数默认值、规则 schema。
- 实现 finding 合并、排序、置信度，含 §6.0 的信号收敛（叠加增信 + 孤证抑制）。
- 按 §6.0 精度分层实现核心规则，Tier 1 铁证优先。
- 每条规则的默认 severity 必须与其 Tier 一致，Tier 3 永不进入 `--fail-on`。
- 增加每类规则的 fixture。

验收：

- 每类核心问题至少一个正例和一个反例。
- 同一问题不会重复刷屏，能合并相邻证据；同一符号多信号命中时聚合为 high-risk 条目并提级。
- Tier 3 孤立弱信号被抑制或降级为 hint。
- 对项目外 / 不可解析引用不产生任何「未定义 / 缺导入」类 finding（开放世界假设，§5.5）。
- 在至少一个真实 netease mod 上标定：`must-fix`/`should-fix` 的误报率低于约定阈值，否则该规则默认降级或保持 `advisory-verify`，不得直接进入闭环自动修复。
- 外部规则文件可以禁用内置规则、覆盖阈值、追加命名语义规则。
- `help-rules` 能输出所有内置规则参数、默认值、可覆盖字段和规则文件 schema。
- 配置文件未知字段会产生 config warning，而不是静默忽略。

### Phase 3：MCP 工具接入

- 新增 `src/tools/register_python_review.hpp`。
- 在 `src/app/server_runtime.cpp` 注册工具。
- CMake 链接现有 tree-sitter target，无新增外部依赖。
- 支持 `review`、`index`、`help`、`list-rules`、`explain-rule`、`help-rules` 子命令。
- `review <behavior_pack_path>` 默认分析行为包下所有 Python 包。

验收：

- `python_review(command="review <behavior_pack_path>")` 返回 Markdown。
- `--format json` 返回可解析 JSON。
- `--rules <path>` 能加载外部规则。
- `python_review(command="help")` 输出所有参数和默认值。
- `python_review(command="help-rules --rules <path>")` 输出合并后的有效规则说明。
- 空路径、非法路径、无 Python 文件时有清晰错误。

### Phase 4：独立 CLI 程序

- 新增 `tools/mcdk-python-review/main.cpp`。
- 新增 CMake target `mcdk-python-review`。
- CLI 直接调用 `src/python_review/` 核心库，不依赖 MCP。
- 支持行为包路径、`--scope`、`--rules`、`--config`、`--format`、`--output`、`--fail-on`、`--help`、`--help-rules`、`--list-rules`、`--explain-rule`。

验收：

- `mcdk-python-review <behavior_pack_path>` 可直接诊断行为包。
- 默认自动分析所有 Python 包。
- Markdown 输出到 stdout，JSON 输出可重定向或写文件。
- `--fail-on warning` 在 CI 场景下返回非零退出码。
- `mcdk-python-review --help-rules` 输出所有规则参数、默认值和自定义规则 schema。
- `mcdk-python-review --help-rules --rule <rule-id>` 只输出指定规则详情。

### Phase 5：项目结构配置

- 支持 `.mcdk-python-review.toml` / `.json`。
- 支持 layer/pattern/allowed_dependencies。
- 支持 threshold 覆盖。
- 支持 naming/comment 规则阈值和白名单。

验收：

- 能用配置检测跨层依赖。
- 未提供配置时仍有保守启发式结果。

### Phase 6：与现有 Python 分析能力协同

- 评估是否复用 `ProjectAnalyzer` 的模块解析逻辑。
- 若重复明显，将通用 parser/file/symbol 工具抽到共享目录。
- `minecraft_py` 保持 Python2/Minecraft 架构理解输出，`python_review` 保持行为包代码审查输出。

验收：

- 两个工具都能独立测试。
- 不因 review 功能改变已有 `minecraft_py` 输出。

## 10. 测试样例设计

新增 fixture：

```text
tests/fixtures/python_review/
  existing_code/
    src/config.py
    src/new_config_loader.py
  duplicate_wheel/
    src/utils.py
    src/generated.py
  logic_blob/
    src/checkout.py
  try_masking/
    src/loader.py
  implicit_global/
    src/state.py
  stub_placeholder/
    src/half_done.py
  dependency/
    src/domain/model.py
    src/service/order.py
    src/infra/db.py
  naming/
    my_mod/modMain.py
    my_mod/client.py
  comments/
    my_mod/large_no_comment.py
    my_mod/noisy_comments.py
  multi_package_behavior_pack/
    mod_a/modMain.py
    mod_a/client.py
    mod_b/modMain.py
    mod_b/server.py
  custom_rules/
    review-rules.toml
    my_mod/modMain.py
```

关键断言：

- `try_masking` 能抓到裸 `except`、`except Exception`、handler `pass`。
- `implicit_global` 能抓到模块级 mutable、`global`、默认可变参数。
- `stub_placeholder` 能抓到 `raise NotImplementedError`、带 TODO 的未完成函数；且**不**误报有意预留的空 `pass` 回调重写。
- `logic_blob` 能抓到 LOC、嵌套、分支超阈值。
- `dependency` 能抓到循环和配置层级违规。
- `existing_code` 和 `duplicate_wheel` 输出候选已有符号，而不是只输出泛泛建议。
- `naming` 能抓到 `CF = clientApi.GetEngineCompFactory()` 并推荐 `compFactory`。
- `comments` 能分别抓到大面积复杂代码无注释和小范围注释过载。
- `multi_package_behavior_pack` 能从一个行为包路径发现多个 `modMain.py` 包。
- `custom_rules` 能通过外部规则文件改变命名、注释或阈值规则。

## 11. 风险与边界

- “忘记已有代码”和“重复发明轮子”天然是启发式，必须输出候选和证据，不能直接判定。
- 仅靠 AST 无法理解完整业务语义，报告要鼓励人工确认。
- Python 动态 import、猴子补丁、运行时注册只能做风险提示。
- 如果项目大量生成代码，需要 ignore 配置，否则相似度和复杂度规则会产生噪声。
- 对 Python2 代码，tree-sitter-python 当前可解析部分旧语法，但审查规则应避免依赖 Python3-only 节点。
- 项目外引用（标准库、游戏内部库、ModSDK、第三方包）采用开放世界假设（§5.5）：解析不到不报错、不告警，只做不依赖解析的风格 / 用法 / `try` 滥用检查。资源解析能力越弱，越要保证「静默降覆盖」而非「误报刷屏」。
- 误报率是闭环生死线：合成 fixture 不足以证明可用，必须在真实 netease mod 上标定 FP 率，达标后才允许把规则升到 `must-fix`（见 Phase 2 验收）。

## 12. 最小可交付版本

MVP 只做以下内容即可进入可用状态：

1. 扫描项目并建立 Python AST 索引。
2. MCP 支持 `python_review(command="review <behavior_pack_path> --changed <paths>")`。
3. CLI 支持 `mcdk-python-review <behavior_pack_path>`，自动分析所有 Python 包。
4. 支持 `--rules <rule_file>` 加载外部声明式规则。
5. 支持 `help` / `help-rules` / `list-rules` / `explain-rule` 输出参数、默认值、规则 schema。
6. 输出确定性 JSON（闭环主格式，含 `finding_key` / `tier` / `actionability`）+ Markdown（供人阅读）；支持行内 `# mcdk-review: ignore` 抑制（见 §8.1）。
7. 实现高价值规则，Tier 1 铁证优先（见 §6.0）：
   - Tier 1：`try.masking.*`、`implicit-global.*`、`stub.placeholder`
   - Tier 2：`logic-blob.*`、`comment-doc.*`
   - Tier 3：`naming.semantic.*`（默认 hint）
8. 对“已有代码/重复轮子/依赖破坏”先输出基础启发式（Tier 3，永不阻断）：
   - 名称相似候选（有 baseline 时对照 baseline，无则 changed 文件内高阈值）。
   - 调用序列相似候选。
   - import cycle。

这样可以先解决 AI 代码 review 最容易漏掉、也最容易通过 AST 稳定发现的问题，再逐步提升相似度和架构理解能力。
