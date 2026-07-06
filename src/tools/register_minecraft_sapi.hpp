#pragma once

#include "sapi/sapi_index.hpp"
#include "tools/command_parser.hpp"

#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace mcdk {
namespace minecraft_sapi_detail {

constexpr const char* kToolName = "minecraft_sapi";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

inline std::string help_text() {
    return R"(minecraft_sapi - Minecraft Bedrock Script API 符号库
用法: minecraft_sapi(command="<子命令> [参数...] [--选项 值]")

注意：先识别项目框架，不要仅凭“网易版/国际版”判断。
若项目含 @minecraft/server、script 模块、scripts/main.js/ts，继续用本工具。
若项目含 modMain.py、extraServerApi、extraClientApi，优先查 minecraft_docs 的 Py ModSDK。

强制触发：
- 遇到 SAPI 线索时先调用本工具；不要先按普通仓库流程读结构后凭经验设计。
- SAPI 线索包括：@minecraft/server、manifest script 模块、scripts/main.ts/js、world.afterEvents、system.run、用户提到 SAPI/Script API/国际版脚本。
- 只要任务涉及 SAPI/Script API/@minecraft/server 开发、事件名、组件名、manifest 依赖版本或 API 签名，先查本工具，不要只凭记忆写。
- 新建或改造 SAPI TypeScript 项目时，先读 ts-setup，再写 manifest/package/tsconfig。
- 写具体 API 前，先 search，再 symbol 精确确认签名、成员和版本线索。

推荐工作流：
1. 先用 search 模糊找 API，一次只搜一个目标。
2. 再用 symbol 精确看签名、文档、成员、关联类型。
3. 命中模块后用 module 分页看清单。
4. 想知道谁用了某类型，用 refs 反查。

子命令：
  help
      显示本说明。

  ts-setup
      查看原版 SAPI TypeScript/npm 项目配置教程。新建 SAPI 项目建议先读。

  search <英文查询> [--offset <n>] [--limit <n>] [--refs <0-3>]
      英文模糊搜索，支持大小写、驼峰、少量拼写错误。
      例：search "spawn entity" --limit 5 --refs 0
          search "scoreboard objective" --refs 1
      不要把多个独立目标塞在一起，如 Player Dimension ItemStack。

  symbol <符号名> [--refs <0-3>] [--member-offset <n>] [--members <n>]
      精确查符号/成员。类和接口默认给成员摘要。
      例：symbol Player --members 16 --refs 0
          symbol Dimension.spawnEntity --refs 1
          symbol @minecraft/server.Player --member-offset 16 --members 16

  refs <符号名> [--offset <n>] [--limit <n>]
      反查哪些签名引用了该类型。
      例：refs SecretString --limit 20

  module <包名> [--offset <n>] [--limit <n>] [--detail true] [--refs <0-3>]
      分页列出包内符号。默认紧凑输出，避免大模块刷屏。
      例：module @minecraft/server --limit 40
          module @minecraft/server --offset 40 --limit 40
          module @minecraft/server --detail true --limit 5 --refs 1

引用深度：
  --refs 0  只返回命中项，最省上下文。
  --refs 1  返回直接引用类型，默认值。
  --refs 2  再展开一层间接引用，可能较长。
  输出中的 direct 表示直接引用，depth 2 表示间接引用。

原版 SAPI 最小入口：
  SAPI 脚本属于行为包。行为包 manifest.json 里通常需要 data 模块、
  script 模块和 @minecraft/server 依赖。script 模块 entry 指向 JS 入口：

  {
    "format_version": 2,
    "header": { "name": "My Pack", "uuid": "...", "version": [1,0,0],
      "min_engine_version": [1,20,30] },
    "modules": [
      { "type": "data", "uuid": "...", "version": [1,0,0] },
      { "type": "script", "uuid": "...", "version": [1,0,0],
        "entry": "scripts/main.js" }
    ],
    "dependencies": [
      { "module_name": "@minecraft/server", "version": "2.3.0" }
    ]
  }
  2.3.0 是当前网易实况可用模板版本；按实况游戏可用版本调整。

运行入口 scripts/main.js：
  import { world, system } from "@minecraft/server";

  system.run(() => {
    world.sendMessage("SAPI loaded");
  });

JS / TS 关系：
  - 游戏实际加载 JavaScript，manifest 的 entry 指向 .js 文件。
  - 主流 SAPI 项目使用 TypeScript 开发，因为类型提示更好、错误更早暴露。
  - 新建项目默认按 TS 编写；除非用户项目本来就是 JS，或只做极小验证，不要主动裸写 JS。
  - TypeScript 不能被游戏直接加载，必须由 tsc/打包工具先编译成 JS。
  - TS 项目常见写法是编辑 scripts/main.ts，构建后输出 scripts/main.js。
  - 正式项目推荐让 main.ts/js 作为加载器入口，导入其他模块完成事件注册和初始化。
  - import 其他 TS 文件时，源码中通常按编译后路径写 .js 后缀，例如：
    import Utils from "./Utils.js";

事件注册：
  import { world } from "@minecraft/server";

  world.afterEvents.playerSpawn.subscribe((ev) => {
    ev.player.sendMessage("Welcome");
  });

常用模式：
  - world.afterEvents：事件已经发生后处理，适合大多数玩法逻辑。
  - world.beforeEvents：事件发生前拦截/取消，限制更多，谨慎使用。
  - system.run(fn)：下一 tick 执行；system.runInterval(fn, ticks) 定时执行。
  - 世界刚加载时不要假设所有对象都已就绪，复杂初始化可延后若干 tick。
  - TypeScript 项目最终仍需编译成 JS，manifest 的 entry 指向编译后的 JS。

Py/JS 通信经验：
  - Py ModSDK 可执行 scriptevent 命令，JS 侧监听 ScriptEvent 可实现 Py -> JS 实时同步。
  - scriptevent 事件名建议带项目名/命名空间隔离，避免跨 Mod 冲突。
  - 对于 JS -> Py 通信，上述方案同理可用。

新手任务检索提示：
  生成实体：search "spawn entity"
  发送消息：search "send message"
  玩家事件：search "player spawn event"
  定时逻辑：symbol system --members 40 --refs 0
  记分板：search "scoreboard objective"
  物品：search "item stack"
  方块：search "block permutation")";
}

inline mcp::json help_result() {
    return text_result(help_text());
}

inline std::string ts_setup_text() {
    return R"(minecraft_sapi ts-setup - 原版 SAPI TypeScript 项目配置

核心规则：
  - Bedrock 运行时加载 JavaScript，不直接加载 TypeScript。
  - TypeScript 必须由 tsc/打包工具先编译成 JS。
  - manifest.json 的 script entry 必须指向编译后的 .js 文件。
  - 新建 SAPI 项目默认按 TS 编写；除非现有项目本来是 JS，不要主动裸写 JS。
  - API 包版本需和目标游戏/manifest dependency 对齐，不要盲目使用 latest。
  - 网易 MC 最新版目前对应国际版 1.21.120，可先按当前实况模板写，再按游戏可用版本调整。
  - 版本号优先看目标文档/本地 d.ts 顶部 Manifest Details；搜索结果若标出最小版本，以该 API 的要求为准。

推荐结构：
  project/
    package.json
    tsconfig.json
    src/main.ts
    B/manifest.json             # 行为包；目录名可自定，以 manifest 内容区分
    B/scripts/main.js           # 构建产物
    R/manifest.json             # 资源包；需要资源时才有

manifest.json 关键片段：
  {
    "format_version": 2,
    "modules": [
      {
        "type": "script",
        "language": "javascript",
        "uuid": "...",
        "version": [1, 0, 0],
        "entry": "scripts/main.js"
      }
    ],
    "dependencies": [
      { "module_name": "@minecraft/server", "version": "2.3.0" }
    ]
  }

SAPI 依赖版本：
  - @minecraft/server 模板版本：2.3.0（当前网易实况可用版本，具有时效性）。
  - 若资料检索结果/官方文档显示某 API 需要更高版本，manifest 和 npm 包需同步调整。
  - 若游戏实际不支持该版本，降到该游戏可用的 @minecraft/server 版本，并重新查询 API 是否存在。

package.json 最小思路：
  TypeScript 模板版本按 2026-06-26 npm latest 给出，具有时效性；真实项目可按锁文件/团队规范更新。
  {
    "private": true,
    "type": "module",
    "scripts": {
      "build": "tsc -p tsconfig.json",
      "watch": "tsc -p tsconfig.json --watch"
    },
    "devDependencies": {
      "typescript": "^6.0.3"
    },
    "dependencies": {
      "@minecraft/server": "2.3.0"
    }
  }

tsconfig.json 最小思路：
  {
    "compilerOptions": {
      "target": "ES2020",
      "module": "ES2020",
      "moduleResolution": "Bundler",
      "strict": true,
      "rootDir": "src",
      "outDir": "B/scripts",
      "skipLibCheck": true,
      "forceConsistentCasingInFileNames": true
    },
    "include": ["src/**/*.ts"]
  }

src/main.ts 示例：
  import { world, system } from "@minecraft/server";
  import "./features/playerJoin.js";
  import "./features/timers.js";

  system.run(() => {
    world.sendMessage("SAPI loaded");
  });

正式项目入口：
  - 推荐 main.ts 只做加载器/总入口，按功能导入其他模块初始化事件和系统。
  - 相比在一个入口里逐个堆注册逻辑，模块化导入更方便维护、测试和增量修改。
  - 示例：import "./features/playerJoin.js"; 对应 src/features/playerJoin.ts。

本地模块导入：
  - TS 项目输出为 ESM JS，源代码中导入本地文件时通常写编译后的 .js 后缀。
  - 例：import Utils from "./Utils.js";  // 对应 src/Utils.ts

构建/测试流程：
  1. npm install
  2. npm run build
  3. 若无游戏控制工具，不要反复要求进游戏验证；把运行验证交给用户。
  4. 修改 TS 后重新 build；游戏内通常需要 /reload 或重进世界加载新 JS

开发约束：
  - 不要把 main.ts 写进 manifest entry。
  - 不要因为运行时是 JS 就放弃 TS。
  - 不确定 API 名称时用 search/symbol 查，不要编造 @minecraft/server 成员。
  - 若项目已有构建链路，延续原项目配置，不要强行替换成这份最小模板。)";
}

inline mcp::json ts_setup_result() {
    return text_result(ts_setup_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + help_text());
}

inline int clamped_refs(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "refs", 1), 0, 3);
}

inline int clamped_refs(const ParsedCommand& pc, int def) {
    return std::clamp(flag_int(pc, "refs", def), 0, 3);
}

inline int clamped_top(const ParsedCommand& pc, int def = 8) {
    return std::clamp(flag_int(pc, "top", def), 1, 50);
}

inline int clamped_limit(const ParsedCommand& pc, int def = 30) {
    int value = has_flag(pc, "limit") ? flag_int(pc, "limit", def) : flag_int(pc, "top", def);
    return std::clamp(value, 1, 200);
}

inline int clamped_offset(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "offset", 0), 0, 1000000);
}

inline int clamped_members(const ParsedCommand& pc, int def = 24) {
    return std::clamp(flag_int(pc, "members", def), 0, 200);
}

inline int clamped_member_offset(const ParsedCommand& pc) {
    return std::clamp(flag_int(pc, "member-offset", flag_int(pc, "member_offset", 0)), 0, 1000000);
}

inline bool flag_bool(const ParsedCommand& pc, const std::string& key, bool def = false) {
    if (!has_flag(pc, key)) return def;
    std::string value = command_parser_detail::to_lower_ascii(flag_str(pc, key));
    if (value.empty() || value == "1" || value == "true" || value == "yes" || value == "on")
        return true;
    if (value == "0" || value == "false" || value == "no" || value == "off")
        return false;
    return def;
}

inline std::string sapi_version_note() {
    return "version context: symbol source=bundled SAPI d.ts; project availability depends on its manifest/package/local d.ts. "
           "Verify with the current project version or tsc before relying on version-sensitive APIs.";
}

inline void append_version_note(std::ostringstream& out) {
    out << "version note: " << sapi_version_note() << "\n";
}

inline std::string symbol_header(const sapi::SapiSymbol& symbol) {
    std::ostringstream out;
    out << symbol.fqname << "\n"
        << "kind: " << symbol.kind << "\n"
        << "module: " << symbol.module << "\n"
        << "source: " << symbol.source_file << ":" << symbol.line_start << "\n";
    if (!symbol.signature.empty())
        out << "signature: " << symbol.signature << "\n";
    if (!symbol.doc.empty())
        out << "doc:\n" << symbol.doc << "\n";
    return out.str();
}

inline void append_refs(std::ostringstream& out,
                        const sapi::SapiIndex& index,
                        const sapi::SapiSearchResult& result) {
    if (result.referenced_symbols.empty()) return;
    out << "referenced types:\n";
    for (size_t i = 0; i < result.referenced_symbols.size(); ++i) {
        int id = result.referenced_symbols[i];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& ref = index.symbols[id];
        int depth = i < result.referenced_depths.size() ? result.referenced_depths[i] : 1;
        out << "- ";
        if (depth <= 1) out << "direct ";
        else out << "depth " << depth << " ";
        out << ref.fqname << " (" << ref.kind << ")";
        if (!ref.signature.empty()) out << " - " << ref.signature;
        out << "\n";
    }
}

// 取文档里第一句人类可读的描述，跳过 @remarks/@beta/@throws 这类纯 JSDoc 标签行。
inline std::string doc_summary_line(const std::string& doc) {
    std::istringstream iss(doc);
    std::string line;
    while (std::getline(iss, line)) {
        size_t b = line.find_first_not_of(" \t\r");
        if (b == std::string::npos) continue;
        size_t e = line.find_last_not_of(" \t\r");
        std::string t = line.substr(b, e - b + 1);
        if (t.empty() || t[0] == '@') continue;
        if (t.size() > 140) {
            t.resize(137);
            t += "...";
        }
        return t;
    }
    return {};
}

inline void append_member_summary(std::ostringstream& out,
                                  const sapi::SapiSymbol& symbol,
                                  int max_members,
                                  int member_offset = 0) {
    if (max_members <= 0 || symbol.members.empty()) return;
    int total = static_cast<int>(symbol.members.size());
    if (member_offset >= total) {
        out << "members: " << total << ". member-offset " << member_offset
            << " is out of range.\n";
        return;
    }
    int end = std::min(total, member_offset + max_members);
    out << "members (" << member_offset << "-" << end << "/" << total << "):\n";
    for (int i = member_offset; i < end; ++i) {
        const auto& member = symbol.members[i];
        out << "- " << member.kind << " " << member.name;
        if (member.kind == "enum-value") {
            if (!member.value.empty()) out << " = " << member.value;
        } else if (!member.signature.empty()) {
            out << ": " << member.signature;
        }
        out << "\n";
        std::string doc_line = doc_summary_line(member.doc);
        if (!doc_line.empty()) out << "    " << doc_line << "\n";
    }
    if (end < total)
        out << "Use symbol " << symbol.fqname << " --members " << total
            << " or --member-offset " << end << " --members " << max_members
            << " to list more members.\n";
}

inline std::string format_result(const sapi::SapiIndex& index,
                                 const sapi::SapiSearchResult& result,
                                 int ordinal,
                                 int max_members = 24,
                                 int member_offset = 0) {
    std::ostringstream out;
    if (result.symbol_id < 0 || result.symbol_id >= static_cast<int>(index.symbols.size()))
        return {};

    const auto& symbol = index.symbols[result.symbol_id];
    out << ordinal << ". ";
    if (result.member_index >= 0 && result.member_index < static_cast<int>(symbol.members.size())) {
        const auto& member = symbol.members[result.member_index];
        if (!result.inherited_from.empty()) {
            out << result.inherited_from << " inherited from " << member.fqname << "\n";
            if (!result.inheritance_path.empty()) {
                out << "inheritance path: ";
                for (size_t i = 0; i < result.inheritance_path.size(); ++i) {
                    if (i > 0) out << " -> ";
                    out << result.inheritance_path[i];
                }
                out << "\n";
            }
        } else {
            out << member.fqname << "\n";
        }
        out << "resolved symbol: " << member.fqname << "\n"
            << "kind: " << member.kind << "\n"
            << "owner: " << symbol.fqname << "\n"
            << "module: " << symbol.module << "\n"
            << "source: " << symbol.source_file << ":" << member.line_start << "\n"
            << "score: " << result.score << " (" << result.match_kind << ")\n";
        if (!member.signature.empty())
            out << "signature: " << member.signature << "\n";
        if (!member.doc.empty())
            out << "doc:\n" << member.doc << "\n";
    } else {
        out << symbol.fqname << "\n"
            << "kind: " << symbol.kind << "\n"
            << "module: " << symbol.module << "\n"
            << "source: " << symbol.source_file << ":" << symbol.line_start << "\n"
            << "score: " << result.score << " (" << result.match_kind << ")\n";
        if (!symbol.signature.empty())
            out << "signature: " << symbol.signature << "\n";
        if (!symbol.doc.empty())
            out << "doc:\n" << symbol.doc << "\n";
        append_member_summary(out, symbol, max_members, member_offset);
    }
    append_refs(out, index, result);
    return out.str();
}

inline mcp::json format_results(const sapi::SapiIndex& index,
                                const std::vector<sapi::SapiSearchResult>& results,
                                const std::string& empty_message,
                                int max_members = 24,
                                int member_offset = 0,
                                int ordinal_base = 0,
                                const std::string& prefix = {}) {
    if (results.empty()) return text_result(empty_message);
    std::ostringstream out;
    if (!prefix.empty()) out << prefix << "\n\n";
    append_version_note(out);
    out << "\n";
    for (size_t i = 0; i < results.size(); ++i) {
        if (i > 0) out << "\n---\n";
        out << format_result(index, results[i], ordinal_base + static_cast<int>(i + 1),
                             max_members, member_offset);
    }
    return text_result(out.str());
}

inline const sapi::SapiModule* find_module_exact(const sapi::SapiIndex& index,
                                                 const std::string& module_name) {
    std::string query = command_parser_detail::to_lower_ascii(module_name);
    for (const auto& module : index.modules) {
        if (command_parser_detail::to_lower_ascii(module.name) == query) return &module;
    }
    return nullptr;
}

inline std::string module_suggestions(const sapi::SapiIndex& index,
                                      const std::string& module_name,
                                      int limit = 8) {
    std::string needle = command_parser_detail::to_lower_ascii(module_name);
    std::ostringstream out;
    int count = 0;
    for (const auto& module : index.modules) {
        std::string hay = command_parser_detail::to_lower_ascii(module.name);
        if (hay.find(needle) == std::string::npos && needle.find(hay) == std::string::npos)
            continue;
        if (count == 0) out << "Did you mean:\n";
        out << "- " << module.name << " (" << module.symbol_ids.size() << " symbols)\n";
        if (++count >= limit) break;
    }
    return out.str();
}

inline std::string first_doc_line(const std::string& doc) {
    size_t pos = doc.find('\n');
    std::string line = pos == std::string::npos ? doc : doc.substr(0, pos);
    if (line.size() > 140) {
        line.resize(137);
        line += "...";
    }
    return line;
}

// 模块级 @packageDocumentation 以 kind=module 的合成符号入库，这里按模块名取回它的文档
// （含 Manifest Details 版本块），供 module 命令顶部展示。
inline std::string module_package_doc(const sapi::SapiIndex& index, const std::string& module_name) {
    for (const auto& symbol : index.symbols) {
        if (symbol.kind == "module" && symbol.fqname == module_name)
            return symbol.doc;
    }
    return {};
}

inline mcp::json format_module_compact(const sapi::SapiIndex& index,
                                       const sapi::SapiModule& module,
                                       int offset,
                                       int limit) {
    int total = static_cast<int>(module.symbol_ids.size());
    if (offset >= total) {
        std::ostringstream empty;
        empty << "Module " << module.name << " has " << total
              << " symbols. Offset " << offset << " is out of range.";
        return text_result(empty.str());
    }

    int end = std::min(total, offset + limit);
    std::ostringstream out;
    out << "module: " << module.name << "\n"
        << "source: " << module.source_file << "\n"
        << "symbols: " << total << "\n"
        << "range: [" << offset << ", " << end << ")\n";
    if (offset == 0) {
        std::string pkg_doc = module_package_doc(index, module.name);
        if (!pkg_doc.empty()) out << "package doc:\n" << pkg_doc << "\n";
    }
    out << "\n";

    for (int ordinal = offset; ordinal < end; ++ordinal) {
        int id = module.symbol_ids[ordinal];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& symbol = index.symbols[id];
        out << (ordinal + 1) << ". " << symbol.kind << " " << symbol.name;
        if (!symbol.members.empty()) out << " members=" << symbol.members.size();
        out << " line=" << symbol.line_start << "\n";
        if (!symbol.signature.empty()) out << "   " << symbol.signature << "\n";
        if (!symbol.doc.empty()) out << "   doc: " << first_doc_line(symbol.doc) << "\n";
    }

    if (end < total) {
        out << "\nNext page: module " << module.name
            << " --offset " << end << " --limit " << limit << "\n";
    }
    out << "Use --detail true with a small --limit when full signatures/docs/refs are needed.";
    return text_result(out.str());
}

inline mcp::json dispatch_search(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("search requires a query.");
    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 8);
    int requested = std::clamp(offset + limit, 1, 300);
    auto results = sapi::search_sapi_symbols(index, pc.positional, requested, clamped_refs(pc));
    if (offset >= static_cast<int>(results.size()))
        return text_result("No more SAPI symbols matched the query at offset " + std::to_string(offset) + ".");
    int end = std::min(static_cast<int>(results.size()), offset + limit);
    std::vector<sapi::SapiSearchResult> page(results.begin() + offset, results.begin() + end);
    std::string prefix;
    if (!page.empty() && page.front().score < 300.0) {
        prefix = "Low-confidence fuzzy results. Treat these as discovery hints, not exact API matches.";
    }
    return format_results(index, page, "No SAPI symbol matched the query.",
                          clamped_members(pc, 12), clamped_member_offset(pc), offset, prefix);
}

inline mcp::json dispatch_symbol(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("symbol requires a symbol name.");
    auto results = sapi::lookup_sapi_symbol(index, pc.positional, clamped_refs(pc));
    if (results.empty()) {
        results = sapi::search_sapi_symbols(index, pc.positional, clamped_top(pc, 8), clamped_refs(pc));
        results.erase(std::remove_if(results.begin(), results.end(), [](const sapi::SapiSearchResult& result) {
            return result.score < 300.0;
        }), results.end());
    }
    if (results.empty()) {
        return text_result("No exact SAPI symbol matched the name: " + pc.positional +
                           "\nUse search " + pc.positional + " when fuzzy discovery is intended.");
    }
    return format_results(index, results, "No SAPI symbol matched the name.",
                          clamped_members(pc, 32), clamped_member_offset(pc));
}

inline std::vector<std::string> reverse_ref_names(const sapi::SapiIndex& index,
                                                  const ParsedCommand& pc,
                                                  std::unordered_set<int>& target_ids) {
    std::vector<std::string> names;
    auto targets = sapi::lookup_sapi_symbol(index, pc.positional, 0);
    for (const auto& target : targets) {
        if (target.symbol_id < 0 || target.symbol_id >= static_cast<int>(index.symbols.size()))
            continue;
        target_ids.insert(target.symbol_id);
        const auto& symbol = index.symbols[target.symbol_id];
        names.push_back(symbol.name);
        names.push_back(symbol.fqname);
        size_t dot = symbol.fqname.rfind('.');
        if (dot != std::string::npos) names.push_back(symbol.fqname.substr(dot + 1));
    }
    if (names.empty()) names.push_back(pc.positional);

    std::vector<std::string> out;
    std::unordered_set<std::string> seen;
    for (auto& name : names) {
        name = command_parser_detail::to_lower_ascii(name);
        if (!name.empty() && seen.insert(name).second) out.push_back(std::move(name));
    }
    return out;
}

inline bool text_references_any(const std::string& text, const std::vector<std::string>& names) {
    std::unordered_set<std::string> wanted;
    for (const auto& name : names) {
        size_t dot = name.rfind('.');
        std::string simple = dot == std::string::npos ? name : name.substr(dot + 1);
        if (!simple.empty()) wanted.insert(command_parser_detail::to_lower_ascii(simple));
    }
    if (wanted.empty()) return false;

    for (size_t pos = 0; pos < text.size();) {
        unsigned char ch = static_cast<unsigned char>(text[pos]);
        if (!(std::isalpha(ch) || text[pos] == '_' || text[pos] == '$')) {
            ++pos;
            continue;
        }
        size_t begin = pos++;
        while (pos < text.size()) {
            unsigned char next = static_cast<unsigned char>(text[pos]);
            if (!(std::isalnum(next) || text[pos] == '_' || text[pos] == '$')) break;
            ++pos;
        }
        std::string token = text.substr(begin, pos - begin);
        if (!std::isupper(static_cast<unsigned char>(token[0]))) continue;
        if (wanted.count(command_parser_detail::to_lower_ascii(token))) return true;
    }
    return false;
}

inline mcp::json dispatch_reverse_refs(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("refs requires a symbol name, for example SecretString.");

    std::unordered_set<int> target_ids;
    auto names = reverse_ref_names(index, pc, target_ids);
    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 30);
    std::vector<sapi::SapiSearchResult> matches;

    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        if (target_ids.count(i)) continue;
        const auto& symbol = index.symbols[i];
        if (text_references_any(symbol.signature, names)) {
            matches.push_back({i, -1, 100.0, "used-by", {}});
        }
        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            if (text_references_any(symbol.members[m].signature, names)) {
                matches.push_back({i, m, 100.0, "used-by", {}});
            }
        }
    }

    if (matches.empty())
        return text_result("No reverse references found for: " + pc.positional);

    int total = static_cast<int>(matches.size());
    if (offset >= total) {
        std::ostringstream out;
        out << "Reverse refs for " << pc.positional << ": " << total
            << " matches. Offset " << offset << " is out of range.";
        return text_result(out.str());
    }

    int end = std::min(total, offset + limit);
    std::ostringstream out;
    out << "reverse refs: " << pc.positional << "\n"
        << "matches: " << total << "\n"
        << "range: [" << offset << ", " << end << ")\n\n";
    for (int i = offset; i < end; ++i) {
        const auto& match = matches[i];
        const auto& symbol = index.symbols[match.symbol_id];
        out << (i + 1) << ". ";
        if (match.member_index >= 0 && match.member_index < static_cast<int>(symbol.members.size())) {
            const auto& member = symbol.members[match.member_index];
            out << member.fqname << "\n"
                << "   kind: " << member.kind << "\n"
                << "   signature: " << member.signature << "\n";
        } else {
            out << symbol.fqname << "\n"
                << "   kind: " << symbol.kind << "\n"
                << "   signature: " << symbol.signature << "\n";
        }
    }
    if (end < total)
        out << "\nNext page: refs " << pc.positional << " --offset " << end
            << " --limit " << limit;
    return text_result(out.str());
}

inline mcp::json dispatch_module(const sapi::SapiIndex& index, const ParsedCommand& pc) {
    if (pc.positional.empty())
        return error_with_help("module requires a module name, for example @minecraft/server.");

    std::string module_name = pc.positional;
    const sapi::SapiModule* module = find_module_exact(index, module_name);
    if (!module) {
        std::string hint = module_suggestions(index, module_name);
        return text_result("No SAPI module matched the name: " + module_name +
                           (hint.empty() ? "" : "\n\n" + hint));
    }

    int offset = clamped_offset(pc);
    int limit = clamped_limit(pc, 30);
    bool detail = flag_bool(pc, "detail", false) || flag_bool(pc, "full", false);
    if (!detail)
        return format_module_compact(index, *module, offset, limit);

    int refs = clamped_refs(pc, 0);
    std::vector<sapi::SapiSearchResult> results;
    int total = static_cast<int>(module->symbol_ids.size());
    int end = std::min(total, offset + limit);
    for (int ordinal = offset; ordinal < end; ++ordinal) {
        int id = module->symbol_ids[ordinal];
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        const auto& symbol = index.symbols[id];
        sapi::SapiSearchResult result;
        result.symbol_id = id;
        result.member_index = -1;
        result.score = 100.0;
        result.match_kind = "module";
        if (refs > 0) {
            auto expanded = sapi::lookup_sapi_symbol(index, symbol.fqname, refs);
            if (!expanded.empty()) result.referenced_symbols = std::move(expanded.front().referenced_symbols);
        }
        results.push_back(std::move(result));
    }
    std::string prefix;
    if (offset == 0) {
        std::string pkg_doc = module_package_doc(index, module->name);
        if (!pkg_doc.empty()) prefix = "package doc:\n" + pkg_doc;
    }
    return format_results(index, results, "No SAPI module matched the name.",
                          clamped_members(pc, 12), 0, 0, prefix);
}

inline mcp::json dispatch_minecraft_sapi(const sapi::SapiIndex& index, const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = command_parser_detail::to_lower_ascii(pc.sub);

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "ts-setup" || sub == "npm-help" || sub == "typescript-setup")
        return ts_setup_result();
    if (sub == "search" || sub == "find")
        return dispatch_search(index, pc);
    if (sub == "symbol" || sub == "sym" || sub == "member")
        return dispatch_symbol(index, pc);
    if (sub == "refs" || sub == "used-by" || sub == "usages")
        return dispatch_reverse_refs(index, pc);
    if (sub == "module" || sub == "mod")
        return dispatch_module(index, pc);

    return error_with_help("Unknown minecraft_sapi subcommand: '" + pc.sub + "'.");
}

} // namespace minecraft_sapi_detail

inline void register_minecraft_sapi_tools(mcp::server& srv,
                                          std::shared_ptr<sapi::SapiIndex> sapi_index) {
    auto tool = mcp::tool_builder(minecraft_sapi_detail::kToolName)
        .with_description(
            "Minecraft Bedrock SAPI/Script API symbol database. "
            "遇到 SAPI 线索(@minecraft/server/script 模块/scripts/main.ts)时先查本工具再读写工程；"
            "新建 TS 项目先用 ts-setup，API 用 search 再 symbol。"
            "网易 Py ModSDK 项目改查 minecraft_docs。")
        .with_string_param("command",
            "命令，如 'search \"spawn entity\" --refs 1', 'symbol Player --refs 1', "
            "'ts-setup'，或 'module @minecraft/server --limit 30'；首次用 'help'学习用法。", true)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .build();

    srv.register_tool(
        tool,
        [sapi_index](const mcp::json& params, const std::string&) -> mcp::json {
            return minecraft_sapi_detail::dispatch_minecraft_sapi(
                *sapi_index,
                params.value("command", ""));
        });
}

} // namespace mcdk
