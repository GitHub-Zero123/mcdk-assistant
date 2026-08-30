# `src` 技术实现说明

本文面向维护 `src/` 的开发者，说明运行时入口、检索链路、缓存格式和主要模块边界。安装、配置和用户侧工具用法见仓库根目录的 [README](../README.md)。

## 1. 总体结构

核心运行链路如下：

```text
main.cpp
  |-- detect_runtime_paths()             定位 exe、dicts、knowledge 和缓存
  |-- SearchService                      加载 manifest，或从资料构建索引
  |-- load_sapi_cache()                  可选；失败时不注册 SAPI 工具
  |-- PluginManager / SolutionIndex      编译开关和运行时缓存同时满足时启用
  |-- make_server_config()
  |-- register_tools()
  `-- stdio_server.run() / server.start()

MCP request
  `-- command-style tool router
        |-- SearchService / SapiIndex     只读检索
        |-- ProjectAnalyzer / Review      Python2 项目分析
        `-- UI / NBT / model / ...        完整版的本地资源操作
```

目录职责：

| 路径 | 职责 |
| --- | --- |
| `app/` | 运行时路径发现、MCP 配置、工具注册和 Server 专用 HTTP 端点 |
| `common/` | 控制台编码、UTF-8 与平台路径转换 |
| `search/` | 文档切片、分词、BM25、二进制缓存和插件内存索引 |
| `sapi/` | SAPI `.d.ts` 的离线构建、缓存加载、符号检索 |
| `project_analysis/` | Python2 行为包入口、导入图和引用链分析 |
| `python_review/` | tree-sitter 驱动的结构性审查、配置和报告渲染 |
| `tools/` | MCP schema、命令解析、子命令分发和具体资源操作 |
| `plugins/` | 可选 pocketPy VM、插件工具/Hook 和插件内存检索 |
| `server/` | Server 版请求计数和持久化 |
| `web_server/` | 独立浏览器资料库服务，不经过 MCP |

## 2. 构建目标与编译边界

目标差异主要由 `MCDK_LITE`、`MCDK_SERVER`、`MCDK_WITH_PLUGINS` 和 `MCDK_WITH_SOLUTIONS` 控制：

| 目标 | 入口 | 传输/用途 | 主要差异 |
| --- | --- | --- | --- |
| `mcdk-assistant` | `main.cpp` | stdio 或 HTTP/SSE | 完整工具集，包含本地资源读写能力 |
| `mcdk-asst-lite` | `main.cpp` | stdio 或 HTTP/SSE | 只保留资料、SAPI、Python 分析等只读能力 |
| `mcdk-asst-server` | `main.cpp` | HTTP/SSE | `MCDK_LITE`；监听 `0.0.0.0`，增加调用统计，不注册 Python 分析工具 |
| `mcdk-web-server` | `web_server/main.cpp` | HTTP | cache-only 资料 API 和静态前端，默认端口 `18767` |
| `mcdk-index-compiler` | `tools/mcdk-index-compiler/` | 离线工具 | 强制重建通用索引，并构建 SAPI/解决方案缓存 |

两个边界不能混淆：

1. `sapi/sapi_build.cpp` 只属于索引编译器和 SAPI 测试。它引用 `tree-sitter-typescript`；若并入运行时目标，会把约 1.3 MB 的 TypeScript 语法表重新链接进每个可执行文件。
2. `web_server/main.cpp` 有独立入口和依赖面，不属于 MCP 目标的 `src/*.cpp` 集合。

工具是否存在不仅取决于编译宏。`minecraft_sapi` 只有在 `mcdk_sapi_index_cache.bin` 成功加载后才注册；解决方案层也要求对应子模块在编译期存在且运行时缓存可用。

## 3. 启动与资源定位

`app/runtime_paths.cpp` 始终以可执行文件所在目录为基准，不依赖当前工作目录：

```text
<exe-dir>/dicts/
<exe-dir>/knowledge/
<exe-dir>/mcdk_index_cache.bin
<exe-dir>/mcdk_sapi_index_cache.bin
```

`dicts/` 是必需资源。通用资料索引有三种启动路径：

- 缓存命中：只读取缓存头和 section 表，具体 section 在首次查询时加载。
- 缓存未命中但存在 `knowledge/`：扫描源资料、构建索引并写缓存。
- 不存在 `knowledge/` 但缓存存在：进入 cache-only 模式，跳过资料指纹校验。

本地 LITE/完整版在资料和缓存都缺失时退出。Server 版允许继续启动为降级服务，因此部署侧仍应通过日志或健康检查确认文档数量，而不能只检查端口是否监听。

stdio 模式必须保持 `stdout` 只承载 JSON-RPC。`main.cpp` 在任何普通输出前解析 `--stdio`，诊断信息统一写入 `stderr`。新增日志时不得绕过该约束。

## 4. 工具注册与命令路由

`app/server_runtime.cpp` 是工具暴露面的唯一汇总点。当前主要入口采用“一个 MCP Tool + 多个命令”的方式：

- `minecraft_docs`：资料搜索、文件读取、原版资源和参考速查。
- `minecraft_sapi`：SAPI 搜索、精确符号、模块枚举。
- `minecraft_py`：`arch`、`imports` 和 `review`。
- 完整版额外注册 `minecraft_ui`、`minecraft_pixelart`、`minecraft_model`、`minecraft_animation` 和 `minecraft_nbt`。

`tools/command_parser.hpp` 负责把 `command` 拆成子命令、位置参数和 flags，各 `register_minecraft_*.hpp` 负责 schema、帮助文本、参数边界和实际分发。旧的细粒度注册函数部分仍以注释或兼容实现保留，不代表它们仍暴露在 `tools/list` 中。

新增工具时应先决定它属于：

- 所有 MCP 变体都可用的只读能力；
- `#ifndef MCDK_LITE` 下的本地写入能力；
- `#ifdef MCDK_SERVER` 下的部署能力；
- 编译期可选模块。

随后在 `register_tools()` 集中注册，不要从其他静态初始化路径隐式注册。

## 5. 检索索引

### 5.1 资料切片与分词

`SearchService` 将资料分为 7 个文档分类和 2 个 GameAssets 分类：

```text
API, Event, Enum, Wiki, QuMod, NeteaseGuide, BedrockDev
GameAssets/BP, GameAssets/RP
```

Markdown 在二至四级标题处切片，每个 `DocFragment` 保存正文、相对路径和行号范围。GameAssets 以文件为基本条目，另存原始相对路径及其小写形式，以支持路径和内容联合检索。

中文分类使用 cppjieba 和停用词，Wiki、BedrockDev、GameAssets 使用 ASCII 单词分词。构建期并行读取 GameAssets，并行处理英文资料；jieba 构建阶段保持串行。缓存写完后会释放仅供构建使用的 `tokenized_docs`。

### 5.2 BM25

`search/bm25.hpp` 保存：

- 每篇文档的 token 数；
- 平均文档长度；
- 每个词的 IDF；
- `term -> [(doc_id, tf)]` 倒排表。

参数固定为 `K1 = 1.5`、`B = 0.75`。查询只为倒排表命中的文档稀疏累积分数，返回数量受限时使用 `partial_sort`。对极高频、低信息量词会限制 posting 扫描量和候选累计量；这是明确的性能换召回率策略，调整阈值时必须做搜索准确率回归，而不能只测耗时。

## 6. v7 通用缓存：section seek、分块解析与压缩

实现位于 `search/index_cache.hpp`。这里的“随机读取”是 **section 级 seek**：读取器能直接跳到某个资料分类，但单个 section 内没有块偏移表，仍按块顺序解码。因此当前格式不支持 section 内任意块的随机访问。

### 6.1 文件布局

```text
+----------------------+-----------------------------------------------+
| Header               | magic[8] = "MCDKIDX\0"                       |
|                      | version u32 = 7                               |
|                      | fingerprint = length u32 + bytes              |
|                      | section_count u32 = 9                         |
+----------------------+-----------------------------------------------+
| Section table        | 9 x SectionInfo                               |
|                      | kind u32                                      |
|                      | offset u64       encoded data start           |
|                      | length u64       encoded byte length          |
|                      | item_count u64                                |
|                      | codec u32        RAW=0, ZSTD=1                 |
|                      | level u32        diagnostic only              |
|                      | raw_length u64   decoded byte length          |
+----------------------+-----------------------------------------------+
| Section data         | section 1 encoded blocks                      |
|                      | section 2 encoded blocks                      |
|                      | ...                                           |
+----------------------+-----------------------------------------------+
```

分类 section 的逻辑数据依次为 fragments 和 BM25 状态；GameAssets section 在它们之前多一组 `rel_paths`。字符串统一编码为 `length u32 + bytes`。`tokenized_docs` 不写入缓存，因为运行时搜索只需要已构建的 BM25 状态。

数值当前直接按宿主机内存表示写入，没有 endian 标记或字节序转换。Windows x64 和常见 Linux x64 都是 little-endian，但该格式不能宣称可跨任意字节序平台复用。

### 6.2 两阶段写入和 section seek

`IndexCache::save()` 使用 `wb+` 打开文件：

1. 写 header，并为 9 个 `SectionInfo` 写入占位值。
2. 顺序序列化每个分类，记录其 `offset`。
3. `BlockWriter` 累积、压缩并写出该 section，记录编码后 `length` 和解码后 `raw_length`。
4. 所有 section 完成后，seek 回 section 表起点，回填真实元数据。
5. flush 并关闭文件。

64 位定位在 Windows 使用 `_fseeki64/_ftelli64`，其他平台使用 `fseeko/ftello`，避免大缓存被 32 位文件偏移截断。

当前实现直接覆盖目标文件，没有“临时文件 + 原子 rename”。构建中断可能留下半写缓存；有源资料时下次启动通常会拒绝并重建，纯 cache-only 分发则会加载失败。发布流程应先完整生成并验证缓存，再替换分发文件。

### 6.3 分块压缩策略

写入参数为：

```text
codec           = ZSTD
compression     = zstd level 12
logical block   = 4 MiB
block layout    = [comp_size u32][raw_size u32][zstd frame bytes]
```

每个逻辑块是独立 zstd frame，而不是把整个 section 压成一个 frame。这样做的主要目的不是块级随机访问，而是限制解码工作缓冲：一个 `BlockReader` 通常只保留当前块的约 4 MiB 原始数据和对应压缩缓冲，不必先把大型 section 整体解压。

这不等于进程只增加 4 MiB 内存：已经反序列化的 fragments、字符串、IDF 和倒排表仍会常驻；不同 section 也可能被并发首次加载，每个读取器都有自己的缓冲。

压缩等级写入 section 表仅用于诊断。zstd frame 自描述，解码不依赖 `level`，所以只调整压缩等级不必提升缓存 `VERSION`。改变逻辑序列化结构、字段含义或块头布局必须提升版本；增加新 codec 则必须分配新值并实现显式兼容处理。

### 6.4 读取与游标解析

启动阶段的 `load_manifest()` 只读取 header 和 section 表，不解压正文。首次查询某分类时：

```text
SearchService::ensure_*_loaded()
  `-- std::call_once(section slot)
        `-- IndexCache::load_category/load_game_assets
              |-- reopen cache and verify current file size
              |-- seek(section.offset)
              |-- BlockReader::refill()
              |     `-- read block header + zstd frame + decompressDCtx
              |-- parse fields through cur_/end_
              `-- restore BM25 and publish loaded flag
```

`BlockReader` 为一个 section 复用同一个 `ZSTD_DCtx`，避免每块重复创建解码上下文。字段完整位于当前块时，`u32/u64/f64` 使用编译期定长 `memcpy`，字符串直接从 `cur_` 构造；只有字段跨块时才进入通用 `read()` 循环。这条快路径很重要，因为大型 GameAssets section 会执行大量小字段读取。

`std::call_once` 保证同一 section 只成功发布一次，`atomic_bool` 以 release/acquire 标记可见性。加载函数抛出异常时该 `once_flag` 不会被永久置为完成，后续调用仍可重试。不同 section 可以并发加载。

### 6.5 校验与风险边界

manifest 阶段会检查：

- magic、版本、资料指纹和固定 section 数；
- section kind 范围及重复 kind；
- `offset + length` 不超出文件，且 section 不覆盖 header/table；
- codec 已知；
- RAW 的 `raw_length == length`；
- 单 section 解码后不超过 4 GiB。

块和逻辑解析阶段还会检查：

- `raw_size` 在 `(0, 4 MiB]`，单块 `comp_size` 非零且不大于 section 的编码长度；
- 所有块累计产生的原始字节不超过 `raw_length`；
- zstd 实际输出大小必须等于 `raw_size`；
- 字符串长度、各类 count 和 posting 数不超过剩余逻辑字节能容纳的上界；
- 最终必须恰好消费 `raw_length`；
- 实际条目数必须与 `item_count` 一致。

仍需明确以下边界：

- 当前没有缓存级校验和或签名，资料指纹也不是完整性校验；缓存应视为可信的本地构建产物。
- section 表没有检查各 section 之间是否重叠或是否按 offset 排序。
- `BlockReader` 没有累计物理读取字节并校验最终位置等于 `offset + length`；伪造的块长度可能让它读入相邻 section，尽管逻辑 `raw_length` 和反序列化约束仍会限制输出。
- header 中的 fingerprint 通过通用 `fread_string()` 读取，分配前没有按文件剩余长度限制其 `u32` 长度；不要把外部不可信文件直接作为缓存加载。
- 4 GiB 是单 section 上限，不是整个进程的内存上限。
- 指纹只组合若干高价值资料目录的 mtime，不扫描每个文件内容；离线编译器用 `force_rebuild_cache=true` 避免依赖该启发式判断。

## 7. SAPI 索引

SAPI 分成构建侧和运行时侧：

- `sapi_build.cpp`：用 tree-sitter-typescript 扫描 `.d.ts`，提取模块、导入、声明、成员、JSDoc、枚举值和类型引用，然后写 `mcdk_sapi_index_cache.bin`。
- `sapi_index.cpp`：运行时加载缓存，做符号搜索、精确查找、继承成员解析和引用展开，不解析 `.d.ts`。
- `sapi_internal.hpp`：两侧共享的格式常量、字符串处理和二进制 IO helper，不依赖 tree-sitter。

这个 TU 边界是体积优化的一部分，不应为了复用方便把 AST 解析函数移回 `sapi_index.cpp`。缓存缺失或损坏时，运行时选择不注册 `minecraft_sapi`，不会现场回退重建。

## 8. Python 项目分析与审查

`project_analysis/` 和 `python_review/` 都使用 tree-sitter-python，但目标不同：

- `ProjectAnalyzer` 建立行为包的模块、导入、入口和反向引用关系，输出面向 Agent 的架构或局部引用报告。
- `ReviewAnalyzer` 按文件解析并运行结构规则，支持 TOML 阈值、规则开关、scope 和多种输出格式。

审查器每个工作线程持有独立 `TSParser`，因为 parser 本身不应跨线程共享；结果按输入索引确定性归并。更完整的规则说明见 [python_review/README.md](python_review/README.md)。

## 9. 并发与生命周期

- MCP HTTP 配置的线程池大小为 `max(hardware_concurrency, 6)`。
- 通用索引的同一 section 由 `call_once` 串行初始化，完成后按不可变数据读取；不同 section 可并发初始化。
- cppjieba 查询分词器通过 `call_once` 延迟创建。修改分词器状态或升级依赖时，需要重新确认并发只读调用是否仍安全。
- 插件 VM 调用由 `vm_mutex_` 串行化；插件内存索引和日志分别有独立 mutex。
- Server 统计的已有计数器走 relaxed atomic；map 扩容、快照和落盘由锁保护。后台线程每 30 分钟落盘，析构时 join 并再次 flush。

不要让 MCP handler 捕获短生命周期对象的引用。当前 `main.cpp` 保证 `SearchService`、SAPI 索引和插件管理器的生命周期覆盖 server 主循环。

## 10. Web 服务

`web_server/main.cpp` 只接受 cache-only 索引，不读取原始 `knowledge/`。它提供健康检查、元信息、分页搜索、资源搜索和文档读取 API，并挂载构建后的 `web/` 静态目录。

服务端对查询长度、分页窗口、路径格式和文档大小设置上限。文档路径拒绝绝对路径、反斜杠和 `..` 段；修改路径解析时必须保持该目录穿越防线。它默认绑定 `127.0.0.1`，使用 `0.0.0.0` 对外服务时应由反向代理提供 TLS、访问控制和限流。

## 11. 修改检查表

### 修改缓存格式

1. 判断是“写入参数变化”还是“读取语义变化”。仅 zstd level 变化不升版本，字段或布局变化必须升 `IndexCache::VERSION`。
2. 同时修改写入、读取、manifest 校验和损坏输入上界。
3. 用索引编译器重建缓存，不能拿旧文件验证新 reader。
4. 覆盖正常加载、cache-only、首次懒加载、跨块字符串、截断文件和错误版本。
5. 比较旧/新索引的 fragments、IDF 和倒排表语义，而不只比较搜索示例。

### 新增资料分类

1. 扩展 `SectionKind`、section count、分类映射和 `SearchService` 存储。
2. 更新 `save_cache()` 的固定顺序和所有聚合搜索/路径读取分支。
3. 因 section 表语义改变，提升缓存版本。
4. 更新 Web scope 和 `minecraft_docs` 命令帮助。

### 新增或修改 MCP 工具

1. 在对应 `register_minecraft_*.hpp` 完成参数校验和错误返回。
2. 在 `app/server_runtime.cpp` 明确编译变体边界。
3. stdio 下所有诊断写 `stderr`。
4. 本地写入工具不得进入 `MCDK_LITE`。
5. 增加直接单元测试或 stdio JSON-RPC 冒烟测试。

## 12. 验证入口

CMake 测试目标在 `tests/CMakeLists.txt`，主要覆盖：

- `search_test`：通用搜索和缓存；
- `sapi_index_test`：SAPI build -> save -> load -> lookup；
- `python_analysis_test`、`python_review_test`；
- `nbt_test` 和可选插件 smoke test。

`tests/` 下的 `*_stdio_probe.py` 从协议边界启动真实可执行文件，适合验证工具注册、命令分发以及 stdout 是否被日志污染。`stdio_concurrency_repro.py` 用于并发请求回归。具体构建目录和配置应以本机 CMake preset/生成器为准，不要在文档或测试里写死某个开发者的绝对路径。
