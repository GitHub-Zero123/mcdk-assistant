# Python AI Code Review · `minecraft_py review`

> 给 AI 写的代码,立一面镜子。

一套面向 **「AI 写完 / 改完 Python2 MOD 代码后,自查、按报告回改」** 的结构性审查工具。
它诞生于一个朴素而顽固的事实:**AI 总会偷懒**——能糊弄的地方就糊弄,能堆一坨的地方就堆一坨。
于是有了这条闭环:**AI 落笔 → 工具体检 → 报告打回 → AI 修正**,如此往复,直到代码不再是一座屎山。

- 设计文档:[../../plans/python-ai-code-review-plan.md](../../plans/python-ai-code-review-plan.md)
- 解析引擎:`review_analyzer.{hpp,cpp}`(tree-sitter 解析 + 规则 + 渲染,多线程并行)
- 配置层:`review_config.{hpp,cpp}`(toml++ 读取 `mcdk-review.toml`)
- 数据模型:`review_types.hpp`

> **一句坦白**:本工具只是审查的**第一层**。纯 AST 看得见**结构性的偷懒**——堆屎山、吞异常、
> 抄代码、留空桩、踩 Py2 语法坑;却看不穿**语义上的偷懒**——那些命名工整、缩进漂亮、
> 读起来像模像样,实则空转或跑偏的代码。后者是静态分析的天花板,要靠闭环再续两层:
> **跑测试验证**与 **LLM 语义评审**。所以——必要,但远不充分。心里有数,方能用好。

---

## 用法

### MCP:单工具 `minecraft_py` 的 `review` 子命令

```
minecraft_py(command="review <行为包完整绝对路径> [选项]")
minecraft_py(command="review --dump-config")   # 打印默认配置模板(亦即一份自解释的文档)
```

### 独立 CLI

```
mcdk-python-review <行为包完整绝对路径> [选项]
mcdk-python-review --dump-config
```

### 选项一览

| 选项 | 说明 |
|---|---|
| `--scope <a,b/c>` | 收敛到指定包 / 模块(逗号分隔),不填即全量。如 `--scope KID_ULTRA_X/Combat` |
| `--format <summary\|markdown\|json>` | 输出格式,默认 `markdown` |
| `--include-third-party` | 连 QuModLibs 等三方库一并审查(默认不碰) |
| `--config <toml>` | 显式指定配置;不给则在行为包根自动寻找 `mcdk-review.toml` |
| `--max-per-rule <n>` | 每条规则最多列 N 处(0 = 不限),为召回瘦身,免得撑爆上下文 |
| `--dump-config` | 打印默认配置模板后退出 |
| `--output <file>`(仅 CLI) | 报告写入文件;不给则打到 stdout |

> 路径一律要**完整绝对路径**(UTF-8,支持中文),不认相对路径,也不猜工作目录。

---

## 严重级别与可执行性:两把尺子

一条发现,量它两次。**一把量"有多严重",一把量"该不该动手"**——因为这份报告的读者是 AI,
分清「必须改」与「先核实、别乱改」,比单纯喊"有问题"重要得多。

- **严重级别**(由轻到重):`hint` → `risk` → `warning` → `error`
- **可执行性**:
  - `must-fix` —— 板上钉钉的硬错误,照着改即可。
  - `should-fix` —— 确定性问题,建议修。
  - `advisory-verify` —— 启发式判断或合法惯用法,**只提请核实,严禁盲目照改**。

---

## 诊断范围:十三条规则

每一条都为「抓真问题、不扰真代码」而校准过——宁可漏报几分,不肯把游戏业务里天经地义的写法
误判成罪状。下表是它们各自的脾性:

| rule_id | 级别 | 可执行性 | tier | 触发条件(默认) | 分寸 / 豁免 |
|---|---|---|---|---|---|
| `encoding.missing-utf8-declaration` | warning | **must-fix** | 1 | 文件带非 ASCII 字节,首两行却无 PEP263 编码声明,也无 UTF-8 BOM —— Py2 一 import 就 `SyntaxError` | **唯有含非 ASCII 才查**;空文件 / 纯空白 / 纯 ASCII 天然免检;BOM 即声明;识得 `# -*- coding: utf-8 -*-`、`# coding=utf-8`、shebang 后第二行、`# vim: set fileencoding=utf-8 :` 等多种版式 |
| `encoding.unicode-default-encoding` | warning | **should-fix** | 1 | 直接调用 `unicode(...)` 且恰好只有 1 个实参,即未显式指定编码 | 网易 ModSDK 魔改 CPython 默认使用 UTF-8,原生 Linux Py2 默认使用 ASCII,裸转换行为不可移植;改为 `unicode(value, "utf-8")`;`unicode()`、两个及以上实参、`obj.unicode(value)` 不报 |
| `try.masking.bare-except` | hint\|risk\|warning | advisory-verify | 1 | 裸 `except:` 未 re-raise;轻重看 try 体大小:≤5 行→hint、≥20 行→warning、居中→risk | 未必是错——可能是 PC / 服务器环境差异下的有意防御,只请核实 |
| `try.masking.broad-except-swallow` | hint\|risk\|warning | advisory-verify | 1 | `except Exception/BaseException` 且吞掉(无实质兜底),分级同上 | 若有真实回退兜底,则不问 |
| `implicit-global.mutable-default` | risk | advisory-verify | 1 | 可变默认对象(`list`/`dict`/`set`)**「边攒边漏」两条件同时成立**:①函数内**就地修改**它(`append`/`+=`/`x[k]=v`/`x.attr=v`) 且 ②它**逃逸出函数**(被 `return`,或写入 `self`/对象属性/外部容器) | **纯结构信号,不枚举回调名**:只存不改(构造器 `self.x=x`)、只传不改(工厂 `return Cls(x=x)`)、回调填充(`args[k]=v` 不逃逸)、`args.append()`(只改自身不逃逸)全部不报;`x.attr`/`x[i]` 读成员不算暴露。仅配置 `extra_caller_supplied_params` 可显式豁免 |
| `implicit-global.global-write` | risk | advisory-verify | 1 | 函数 `global x` 且写 `x`,且 `x` 为**公开名**(无下划线前缀) | **下划线私有全局径直放行**——缓存 / 单例 / 计数器是模块封装的正道;并顺手建议"若仅本模块自用,加个下划线即可" |
| `stub.placeholder` | warning / hint | should-fix / advisory | 1 | 函数体仅 `raise NotImplementedError`(warning)或 `...`(hint) | 接口 / 抽象命名类(`^I[A-Z]`、`Base*`/`Abstract*`、`*Interface`/`*ABC`)与 `@abstractmethod` 豁免;空 `pass`/`return None` 不问 |
| `stub.shallow-impl` | risk | advisory-verify | 2 | 函数体 ≥3 条业务语句,**却所有 `return` 皆固定值、且完全不碰任何参数** —— 疑似假实现 | 放过 1 行 `return 固定值`(Py2 无 .pyi,裸写补全库触发类型推导);排除 `return None`/非空集合;`_`/`__` 占位参、dunder、接口类豁免 |
| `signature.too-many-params` | hint / warning | advisory-verify | 2 | 非 `self`/`cls` 参数 > 5;6~7 个→hint、≥8 个→warning | `__init__`/`__new__` 豁免(构造器天然聚合);`*args`/`**kwargs` 不计;引擎固定签名可调阈值或整族关闭 |
| `logic-blob.large-function` | risk / warning | advisory-verify | 2 | 单函数行跨度 ≥50→risk、≥100→warning | 事件分发 / UI 回调类的长函数,可酌情放过 |
| `logic-blob.large-file` | warning | advisory-verify | 2 | 单文件代码行 ≥1000 **且** `def+class` ≥25(双重判定) | 纯数据 / 配置文件(体量大却极少定义)自然不触发 |
| `duplicate-function.cross-module` | warning | advisory-verify | 3 | 相同结构指纹(抹去标识符 / 字面量,只留控制流 + 调用名)、≥5 语句、指纹长 ≥10、散落 ≥2 文件、≥2 处 | 跳过 dunder;要求确有逻辑(控制流或 ≥2 次调用),免得把样板 `__init__` 错聚成"重复" |
| `comment-doc.unowned-todo` | hint | advisory-verify | 2 | 无主的 `TODO`/`FIXME`/`HACK`/`XXX` | 带括号者(疑似署名,如 `TODO(alice)`)不问 |

> **tier** 是精度分层:**1** 确定性词法 / 句法 · **2** 度量阈值 · **3** 启发式 / 相似度。级别越低,越是"证据确凿"。

### 降噪的底线:开放世界

这套规则刻意不做企业 linter 那一套"事无巨细的挑刺",而是守住几条底线,让报告只说要紧话:

- **QuModLibs** 等静态三方库默认不看(`--include-third-party` 可开)。
- **项目外与游戏引擎的 API,不解析、不报错**——只查结构与用法,绝不因"找不到定义"就发难。
- 空 `__init__.py`、空文件,不算解析失败。
- 输出**确定性排序** + `finding_key` 稳定去重——同一份代码反复审,结果始终如一,闭环方能收敛。

---

## 配置 `mcdk-review.toml`:一切皆可调

**取用次第**:`--config <path>` > 行为包根下自动发现的 `mcdk-review.toml` / `.mcdk-review.toml` > 内置默认值。
解析出错或文件缺失,则告警并回退默认(报告的 `config_warnings` 与 `config_source` 会如实记下来龙去脉);
任何键都可省略,省即取默认;越界的值会被夹回合理区间,不至于把整仓刷屏。

`review --dump-config` 便可取到下面这份带注释的模板:

```toml
[thresholds]
file_code_lines  = 1000  # 单文件"代码行"屎山阈值(需与 file_units 同时满足才报)
file_units       = 25    # 单文件 def+class 总数阈值(多重判定,避免误伤纯数据/配置文件)
func_loc         = 50    # 单函数行跨度:偏长起报(risk)
func_loc_high    = 100   # 单函数行跨度:过长(warning)
try_small        = 5     # try 体 <= 此值:窄防御,低级(hint)
try_large        = 20    # try 体 >= 此值:疑似吞掉大量业务,高级(warning)
dup_min_stmts    = 5     # 跨模块重复:单函数最少语句数(过滤 getter/空壳)
dup_min_fp_len   = 10    # 跨模块重复:结构指纹最短长度(过短易巧合)
dup_min_count    = 2     # 跨模块重复:最少重复处数
shallow_min_body = 3     # 假实现:函数体至少 N 条业务语句才判(排除 py2 裸写补全库的 1 行 return 固定值)
max_params       = 5     # 参数过多:非 self 参数超过此值触发(__init__/__new__ 豁免)

[rules]                  # 整族开关:false 可禁用对应规则
try_masking          = true
mutable_default      = true
global_write         = true
stub_placeholder     = true
shallow_impl         = true
large_function       = true
large_file           = true
duplicate_function   = true
unowned_todo         = true
too_many_params      = true
encoding_declaration = true
unicode_default_encoding = true

[output]
max_findings_per_rule = 20  # 每规则最多展示 N 处定位,超出仅计数;0 = 不限(控 MCP 召回体积)

[advanced]
# "总由调用方填充"的参数名白名单:这些可变默认参数(def f(x={}))即便逃逸也不报。
# 可变默认参数规则以"是否逃逸出函数(被 return / 写入外部)"判定,一般无需配置。
extra_caller_supplied_params = []
```

### 默认值速查

| 键 | 默认 | 含义 |
|---|---|---|
| `thresholds.file_code_lines` | `1000` | 单文件代码行屎山阈值 |
| `thresholds.file_units` | `25` | 单文件 def+class 数阈值 |
| `thresholds.func_loc` / `func_loc_high` | `50` / `100` | 单函数偏长 / 过长行数 |
| `thresholds.try_small` / `try_large` | `5` / `20` | try 体小 / 大的分级线 |
| `thresholds.dup_min_stmts` / `dup_min_fp_len` / `dup_min_count` | `5` / `10` / `2` | 重复检测的三道门槛 |
| `thresholds.shallow_min_body` | `3` | 假实现前提:最少业务语句数 |
| `thresholds.max_params` | `5` | 参数过多阈值 |
| `rules.*` | `true` | 十一族规则开关(默认全开) |
| `output.max_findings_per_rule` | `20` | 每规则展示上限(0 = 不限) |
| `advanced.extra_caller_supplied_params` | `[]` | "总由调用方填充"参数名:即便逃逸也豁免(一般无需配置) |

---

## 输出:密度是一种体贴

报告最终要喂回 AI 的上下文,冗长即是负担。于是三种 `--format` 各司其职,而降密度的关键手法只有一招:
**按 (rule_id, severity) 分组**——同规则同级别的标题与建议只落一次笔,每处定位压成一行,
超出 `max_findings_per_rule` 的只计数不铺陈(且明说"…其余 N 处",绝不悄悄截断)。

| 格式 | 体量(参考:188 文件 / 119 findings) | 适用 |
|---|---|---|
| `summary` | ~1.3 KB | 极简概览:计数、级别分布、各规则条数、热点文件 |
| `markdown`(默认) | ~19 KB | 给人读:分组 + 逐处定位 + 每规则一句建议 |
| `json` | ~75 KB | 给机器读:按规则分组去重,含 `finding_key`/`confidence`/证据 |

> 闭环里的心法:配上 `--scope`,只审刚动过的包 —— 报告更小,靶心更准。

---

## 性能

- 多线程并行:每文件独立解析 + 独立跑规则,每线程各持一个 `TSParser`,按 index 确定性归并;线程数取 `min(核数, 8)`。
- 参考实测:188 文件全量 scan ~300 ms;`--scope` 单包 ~100 ms。
- `MCDK_REVIEW_TIMING=1` 会打印分段耗时(files / threads / scan ms / sigs)。
- 字节级规则(如编码检查)一律短路:有声明先返回、逢首个非 ASCII 即停,绝不对全文件暴力匹配——无可测的性能回退。
