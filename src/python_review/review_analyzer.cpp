#include "python_review/review_analyzer.hpp"
#include "python_review/review_config.hpp"

#include "common/path_utils.hpp"

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-python.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// 说明：本层的 tree-sitter 封装/文件读取与 project_analysis/project_analyzer.cpp 有小幅重叠。
// 计划 Phase 6 会把 py_parser/source_file 抽到共享层；当前切片保持自包含，避免中途重构通用分析器。
namespace mcdk::python_review {
namespace fs = std::filesystem;

namespace {

// ── 基础工具 ──────────────────────────────────────────────────────────────

bool is_python_file(const fs::path& path) {
    return path.extension() == ".py";
}

// QuModLibs：唯一静态三方库，默认跳过（plan §5.5；与 project_analyzer.cpp:171 一致）。
bool should_skip_dir(const fs::path& dir, bool include_third_party) {
    auto name = dir.filename().string();
    if (!include_third_party && name == "QuModLibs") return true;
    return name == "__pycache__" || name == ".git" || name == ".svn" ||
           name == ".idea" || name == ".vs";
}

// 读文件并去除 UTF-8 BOM。整块读取（避免 istreambuf_iterator 逐字符的性能陷阱）。
std::string read_text_file(const fs::path& file_path) {
    std::ifstream ifs(file_path, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) return {};
    std::streamoff size = ifs.tellg();
    if (size <= 0) return {};
    std::string text(static_cast<size_t>(size), '\0');
    ifs.seekg(0);
    ifs.read(text.data(), size);
    text.resize(static_cast<size_t>(ifs.gcount()));
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3);
    }
    return text;
}

std::string to_utf8(const fs::path& p) { return mcdk::path::to_utf8(p); }

// 相对行为包根的 UTF-8 路径（generic '/' 分隔，稳定用于 finding_key）。
std::string rel_utf8(const fs::path& file, const fs::path& root) {
    std::error_code ec;
    fs::path rel = fs::relative(file, root, ec);
    if (ec || rel.empty()) rel = file.lexically_relative(root);
    if (rel.empty()) rel = file;
    return to_utf8(rel);
}

std::string node_text(TSNode node, const std::string& source) {
    if (ts_node_is_null(node)) return {};
    auto start = ts_node_start_byte(node);
    auto end = ts_node_end_byte(node);
    if (start > source.size() || end > source.size() || start > end) return {};
    return source.substr(start, end - start);
}

// 节点类型的零分配视图（ts_node_type 返回静态 const char*，包裹为 string_view 免堆分配）。
inline std::string_view nt(TSNode node) { return ts_node_type(node); }

int node_start_row(TSNode node) {
    return static_cast<int>(ts_node_start_point(node).row) + 1; // 1-based
}

bool subtree_has_type(TSNode node, std::string_view type) {
    if (ts_node_is_null(node)) return false;
    if (type == ts_node_type(node)) return true;
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) {
        if (subtree_has_type(ts_node_child(node, i), type)) return true;
    }
    return false;
}

bool contains_ci(const std::string& hay, std::string_view needle) {
    return hay.find(needle) != std::string::npos;
}

// ── 包发现（modMain 优先，退化为目录扫描）──────────────────────────────────

struct DiscoveredPackage {
    fs::path              root_dir;
    fs::path              entry_file; // modMain.py，可空（fallback）
    std::string           name;
    std::vector<fs::path> files;
};

std::vector<DiscoveredPackage> discover_packages(const fs::path& root, bool include_third_party,
                                                 std::string& discovery_mode,
                                                 std::vector<std::string>& ignored_dirs) {
    std::vector<DiscoveredPackage> pkgs;
    std::error_code ec;
    if (!fs::is_directory(root, ec)) return pkgs;

    // 1) 找所有 modMain.py。
    std::vector<fs::path> mod_mains;
    {
        fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec), end;
        while (!ec && it != end) {
            if (it->is_directory(ec) && should_skip_dir(it->path(), include_third_party)) {
                ignored_dirs.push_back(to_utf8(it->path()));
                it.disable_recursion_pending();
            } else if (it->is_regular_file(ec) && it->path().filename() == "modMain.py") {
                mod_mains.push_back(it->path());
            }
            it.increment(ec);
        }
    }

    auto collect_files = [&](const fs::path& dir) {
        std::vector<fs::path> files;
        std::error_code inner;
        fs::recursive_directory_iterator fit(dir, fs::directory_options::skip_permission_denied, inner), fend;
        while (!inner && fit != fend) {
            if (fit->is_directory(inner) && should_skip_dir(fit->path(), include_third_party)) {
                fit.disable_recursion_pending();
            } else if (fit->is_regular_file(inner) && is_python_file(fit->path())) {
                files.push_back(fit->path());
            }
            fit.increment(inner);
        }
        std::sort(files.begin(), files.end());
        return files;
    };

    if (!mod_mains.empty()) {
        discovery_mode = "modMain";
        std::sort(mod_mains.begin(), mod_mains.end());
        for (const auto& mm : mod_mains) {
            DiscoveredPackage pkg;
            pkg.entry_file = mm;
            pkg.root_dir = mm.parent_path();
            pkg.name = pkg.root_dir.filename().string();
            pkg.files = collect_files(pkg.root_dir);
            pkgs.push_back(std::move(pkg));
        }
    } else {
        // 退化：整个目录当一个包。
        discovery_mode = "fallback-directory";
        DiscoveredPackage pkg;
        pkg.root_dir = root;
        pkg.name = root.filename().string();
        pkg.files = collect_files(root);
        if (!pkg.files.empty()) pkgs.push_back(std::move(pkg));
    }
    return pkgs;
}

// scope 过滤：空 = 全部。匹配包名或包内相对模块前缀。
bool package_in_scope(const std::string& pkg_name, const std::vector<std::string>& scope) {
    if (scope.empty()) return true;
    for (const auto& s : scope) {
        // 取 scope 的第一段（"KID_ULTRA_X/Combat" -> "KID_ULTRA_X"）比对包名。
        std::string head = s;
        auto pos = head.find_first_of("/.");
        if (pos != std::string::npos) head = head.substr(0, pos);
        if (head == pkg_name) return true;
    }
    return false;
}

bool file_in_scope(const std::string& rel_from_pkg, const std::string& pkg_name,
                   const std::vector<std::string>& scope) {
    if (scope.empty()) return true;
    // 归一化分隔符为 '/'。
    std::string rel = rel_from_pkg;
    std::replace(rel.begin(), rel.end(), '\\', '/');
    for (auto s : scope) {
        std::replace(s.begin(), s.end(), '\\', '/');
        std::replace(s.begin(), s.end(), '.', '/');
        // 允许 scope 带包名前缀。
        std::string with_pkg = pkg_name + "/" + rel;
        if (s == pkg_name) return true;
        if (rel.rfind(s, 0) == 0) return true;       // rel 以 scope 开头
        if (with_pkg.rfind(s, 0) == 0) return true;  // 带包名前缀匹配
    }
    return false;
}

// ── 规则：try 掩盖（plan §6.4，Tier 1 旗舰）────────────────────────────────

// 函数结构指纹（用于跨模块重复检测）：归一化掉标识符/字面量，只留控制流 + 调用名。
struct FuncSig {
    std::string fingerprint;
    std::string file;   // UTF-8 相对路径
    std::string symbol; // 限定名
    int         line = 0;
    int         stmts = 0; // body 直接语句数，用于过滤 trivial
};

struct FileContext {
    const std::string* source;
    std::string        rel_path; // UTF-8，相对行为包根
    std::vector<Finding>* out;
    std::vector<FuncSig>* sigs = nullptr; // 跨文件累积（可空）
    const ReviewConfig* cfg = nullptr;    // 可调阈值/开关（analyze_file 注入）
};

// 递归构建结构指纹：只 append 控制流关键字与调用方法名，忽略变量名/字面量，
// 使复制粘贴（改了变量名）的函数得到相同指纹。
void fingerprint_walk(TSNode node, const std::string& source, std::string& fp) {
    std::string_view t = ts_node_type(node);
    // 不下钻嵌套函数：它们各自单独取指纹，否则父函数会重复遍历嵌套体（O(节点×嵌套深度)）。
    if (t == "function_definition") return;
    if (t == "if_statement") fp += "I";
    else if (t == "for_statement") fp += "F";
    else if (t == "while_statement") fp += "W";
    else if (t == "try_statement") fp += "T";
    else if (t == "with_statement") fp += "H";
    else if (t == "return_statement") fp += "R";
    else if (t == "raise_statement") fp += "X";
    else if (t == "assignment") fp += "=";
    else if (t == "augmented_assignment") fp += "+";
    else if (t == "call") {
        std::string callee = node_text(ts_node_child_by_field_name(node, "function", 8), source);
        auto pos = callee.rfind('.');
        fp += "c(" + (pos == std::string::npos ? callee : callee.substr(pos + 1)) + ")";
    }
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) fingerprint_walk(ts_node_child(node, i), source, fp);
}

// 指纹是否含「真实逻辑」：有控制流或 ≥2 次调用。纯赋值序列（构造器/数据holder）不算，
// 避免把结构雷同但语义无关的样板函数（如一堆 self.x=... 的 __init__）聚成假重复。
bool fp_has_logic(const std::string& fp) {
    if (fp.find_first_of("IFWTH") != std::string::npos) return true;
    size_t calls = 0, pos = 0;
    while ((pos = fp.find("c(", pos)) != std::string::npos) { ++calls; pos += 2; }
    return calls >= 2;
}

bool is_dunder(const std::string& name) {
    return name.size() >= 4 && name.rfind("__", 0) == 0 &&
           name.compare(name.size() - 2, 2, "__") == 0;
}

// 分类 except handler body：是否 re-raise、是否只有日志/低信息、是否有实质处理。
struct HandlerClass {
    bool has_raise = false;
    bool has_log = false;   // 含 print/logging 调用
    bool has_other = false; // 有实质处理语句（非 pass/return/log/注释）
    int  stmt_count = 0;
};

HandlerClass classify_handler(TSNode body, const std::string& source) {
    HandlerClass hc;
    if (ts_node_is_null(body)) return hc;
    hc.has_raise = subtree_has_type(body, "raise_statement");

    uint32_t n = ts_node_named_child_count(body);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode st = ts_node_named_child(body, i);
        std::string_view t = ts_node_type(st);
        if (t == "comment") continue;
        hc.stmt_count++;

        if (t == "pass_statement" || t == "return_statement" ||
            t == "continue_statement" || t == "break_statement") {
            continue; // 低信息：吞掉/跳过
        }
        if (t == "raise_statement") continue; // 已计入 has_raise
        if (t == "expression_statement") {
            TSNode inner = ts_node_named_child(st, 0);
            std::string_view it = ts_node_is_null(inner) ? std::string_view{} : nt(inner);
            if (it == "string") continue; // 文档串
            if (it == "call") {
                TSNode fn = ts_node_child_by_field_name(inner, "function", 8);
                std::string callee = node_text(fn, source);
                if (contains_ci(callee, "print") || contains_ci(callee, "log") ||
                    contains_ci(callee, "Log") || contains_ci(callee, "Print")) {
                    hc.has_log = true;
                    continue;
                }
                hc.has_other = true;
                continue;
            }
            hc.has_other = true;
            continue;
        }
        // if/for/while/with/assignment 等 → 视为有实质处理。
        hc.has_other = true;
    }
    return hc;
}

// try_statement 的 try body（第一个 block 命名子节点）。
TSNode try_body_block(TSNode try_stmt) {
    uint32_t n = ts_node_named_child_count(try_stmt);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode c = ts_node_named_child(try_stmt, i);
        if (std::string(ts_node_type(c)) == "block") return c;
    }
    return {};
}

int block_loc(TSNode block) {
    if (ts_node_is_null(block)) return 0;
    int start = static_cast<int>(ts_node_start_point(block).row);
    int end = static_cast<int>(ts_node_end_point(block).row);
    return std::max(1, end - start + 1);
}

void make_finding(const FileContext& ctx, const std::string& rule_id, Severity sev,
                  double confidence, int line, const std::string& symbol,
                  const std::string& title, std::vector<std::string> evidence,
                  const std::string& suggestion,
                  int tier = 1, Actionability act = Actionability::ShouldFix) {
    Finding f;
    f.rule_id = rule_id;
    f.severity = sev;
    f.tier = tier;
    f.actionability = act;
    f.confidence = confidence;
    f.file = ctx.rel_path;
    f.line = line;
    f.symbol = symbol;
    f.title = title;
    f.evidence = std::move(evidence);
    f.suggestion = suggestion;
    f.finding_key = rule_id + "@" + ctx.rel_path + ":" + std::to_string(line) +
                    (symbol.empty() ? "" : "::" + symbol);
    ctx.out->push_back(std::move(f));
}

// try 体规模阈值来自配置（try_small / try_large）：小 try 多为窄防御（低风险），
// 大 try 吞掉大量业务失败（高风险）。
void check_try_masking(TSNode try_stmt, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
    const int kTrySmall = ctx.cfg->try_small;
    const int kTryLarge = ctx.cfg->try_large;
    int try_loc = block_loc(try_body_block(try_stmt));

    uint32_t n = ts_node_named_child_count(try_stmt);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode ec = ts_node_named_child(try_stmt, i);
        if (std::string(ts_node_type(ec)) != "except_clause") continue;

        // 解析 except_clause：类型表达式 + handler block。
        bool typed = false;
        std::string type_text;
        TSNode handler = {};
        uint32_t en = ts_node_named_child_count(ec);
        for (uint32_t j = 0; j < en; ++j) {
            TSNode c = ts_node_named_child(ec, j);
            std::string ct = ts_node_type(c);
            if (ct == "block") {
                handler = c;
            } else if (ct != "comment") {
                typed = true;
                if (type_text.empty()) type_text = node_text(c, source);
            }
        }

        HandlerClass hc = classify_handler(handler, source);
        if (hc.has_raise) continue; // 捕获后重新抛出，非掩盖。

        int line = node_start_row(ec);

        // handler 性质：silent（纯 pass/return，真吞）/ logged（仅日志）/ fallback（回退值等兜底逻辑）。
        enum HandlerKind { Silent, Logged, Fallback };
        HandlerKind kind = hc.has_other ? Fallback : (hc.has_log ? Logged : Silent);
        std::string handler_ev =
            kind == Silent   ? "handler 只有 pass/return，静默吞掉异常"
          : kind == Logged   ? "handler 仅记录日志后继续，未重新抛出"
                             : "handler 用回退值/其他逻辑兜底，但捕获过宽";

        // 严重级别由 try 体规模驱动（用户反馈）：小 try（<=5 行）多为窄防御（如 PC/服务器 API
        // 差异），低级；大 try（>=20 行）往往是为解决一个问题吞掉了大量业务逻辑的失败，高级。
        // 一律 advisory-verify —— 未必是错误（可能是有意的环境/兼容防御），禁止 AI 盲目收窄/重抛。
        Severity sev = try_loc <= kTrySmall ? Severity::Hint
                     : try_loc >= kTryLarge ? Severity::Warning
                                            : Severity::Risk;
        double conf = try_loc >= kTryLarge ? 0.80 : (try_loc <= kTrySmall ? 0.55 : 0.65);
        std::string size_ev =
            try_loc <= kTrySmall ? "try 体仅 " + std::to_string(try_loc) + " 行（窄防御，低风险）"
          : try_loc >= kTryLarge ? "try 体达 " + std::to_string(try_loc) + " 行（疑似吞掉大量业务失败，高风险）"
                                 : "try 体 " + std::to_string(try_loc) + " 行";
        const std::string suggestion =
            "确认是否为有意的环境/兼容防御（如 PC 独占 API 在服务器不存在）：若是则忽略；"
            "try 体越大越可能吞掉真实业务错误，此时应收窄异常类型或补上下文后重抛。";

        if (!typed) {
            make_finding(ctx, "try.masking.bare-except", sev, conf, line, symbol,
                         "裸 except 吞掉异常（确认是否有意防御）",
                         {"裸 except，未指定异常类型", size_ev, handler_ev}, suggestion,
                         1, Actionability::AdvisoryVerify);
            continue;
        }

        // 过宽捕获：except Exception / BaseException 且吞掉（无实质兜底）。有真实兜底则不报。
        bool broad = contains_ci(type_text, "BaseException") || contains_ci(type_text, "Exception");
        if (broad && kind != Fallback) {
            make_finding(ctx, "try.masking.broad-except-swallow", sev, conf, line, symbol,
                         "过宽 except 吞掉异常（确认是否有意防御）",
                         {"except " + type_text + " 捕获过宽", size_ev, handler_ev}, suggestion,
                         1, Actionability::AdvisoryVerify);
        }
    }
}

std::string join_scope(const std::vector<std::string>& s) {
    std::string q;
    for (size_t i = 0; i < s.size(); ++i) { if (i) q += "."; q += s[i]; }
    return q;
}

// ── 规则：可变默认参数（plan §6.5，Tier 1）────────────────────────────────
// 关键：只报「默认对象在体内被真正修改」的（真 bug）。游戏业务里大量 def(args={})
// 只读默认值，无害且是惯用法——盲报会淹没真信号并让 AI 大改（plan §6.0 域适配）。
bool is_mutating_method(const std::string& m) {
    static const std::unordered_set<std::string> s = {
        "append", "extend", "insert", "remove", "pop", "clear", "sort", "reverse",
        "update", "setdefault", "add", "discard", "popitem"};
    return s.count(m) > 0;
}

// 只认「累积型」修改：append/extend 等增长方法，或 += 累加。
// 不认 pname[k]=v / pname.attr=v —— 那是往（通常由调用方/引擎提供的）dict/对象里塞字段，
// 是 mod 事件回调惯用法（如 onHurt(args={}): args["damage"]=0），并非默认对象跨调用累积的 bug。
bool param_mutated(TSNode node, const std::string& pname, const std::string& source, bool top) {
    std::string_view t = ts_node_type(node);
    if (!top && t == "function_definition") return false; // 嵌套函数会遮蔽同名参数，不下钻
    if (t == "augmented_assignment") { // pname += ...
        TSNode left = ts_node_child_by_field_name(node, "left", 4);
        if (!ts_node_is_null(left) && nt(left) == "identifier" &&
            node_text(left, source) == pname)
            return true;
    } else if (t == "call") { // pname.append(...) 等增长型方法
        TSNode fn = ts_node_child_by_field_name(node, "function", 8);
        if (!ts_node_is_null(fn) && nt(fn) == "attribute") {
            TSNode obj = ts_node_child_by_field_name(fn, "object", 6);
            TSNode attr = ts_node_child_by_field_name(fn, "attribute", 9);
            if (!ts_node_is_null(obj) && nt(obj) == "identifier" &&
                node_text(obj, source) == pname && is_mutating_method(node_text(attr, source)))
                return true;
        }
    }
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i)
        if (param_mutated(ts_node_child(node, i), pname, source, false)) return true;
    return false;
}

// 约定的「调用方/引擎提供」参数名：这些默认值几乎总被真实入参覆盖，默认对象不会跨调用共享。
bool is_caller_supplied_param(const std::string& name) {
    static const std::unordered_set<std::string> s = {
        "args", "data", "event", "evt", "eventData", "extraArgs", "customArgs",
        "kwargs", "params", "payload", "context", "ctx"};
    return s.count(name) > 0;
}

void check_mutable_defaults(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
    TSNode params = ts_node_child_by_field_name(fn, "parameters", 10);
    TSNode body = ts_node_child_by_field_name(fn, "body", 4);
    if (ts_node_is_null(params) || ts_node_is_null(body)) return;
    uint32_t n = ts_node_named_child_count(params);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode p = ts_node_named_child(params, i);
        std::string pt = ts_node_type(p);
        if (pt != "default_parameter" && pt != "typed_default_parameter") continue;
        TSNode val = ts_node_child_by_field_name(p, "value", 5);
        if (ts_node_is_null(val)) continue;
        std::string vt = ts_node_type(val);
        bool mutable_lit = (vt == "list" || vt == "dictionary" || vt == "set");
        bool mutable_call = false;
        if (vt == "call") {
            std::string callee = node_text(ts_node_child_by_field_name(val, "function", 8), source);
            mutable_call = (callee == "list" || callee == "dict" || callee == "set");
        }
        if (!mutable_lit && !mutable_call) continue;
        std::string pname = node_text(ts_node_child_by_field_name(p, "name", 4), source);
        bool caller_supplied = is_caller_supplied_param(pname);
        if (!caller_supplied && ctx.cfg) // 项目自定义的调用方参数名（配置追加）
            for (const auto& extra : ctx.cfg->extra_caller_supplied_params)
                if (extra == pname) { caller_supplied = true; break; }
        if (caller_supplied) continue;                         // 引擎/调用方提供，默认值不共享
        if (!param_mutated(body, pname, source, true)) continue; // 只读默认值不报

        // 定位 risk + advisory-verify：即便被累积型修改，若默认值实际总被入参覆盖也无害，
        // 故只提示核实、不列 must-fix（呼应「通常不是 bug」，plan §8.1）。
        make_finding(ctx, "implicit-global.mutable-default", Severity::Risk, 0.62,
                     node_start_row(p), symbol, "可变默认参数被累积型修改，疑似跨调用共享状态",
                     {"参数 " + pname + " 默认值为可变对象 " + node_text(val, source),
                      "且在体内被 append/+= 等累积修改",
                      "若该参数实际总由调用方传入则无害；仅当会以默认值反复调用时才是 bug"},
                     "确认是否会以默认值反复调用；若是，改用 None 哨兵：def(" + pname +
                     "=None) 后 if " + pname + " is None: " + pname + " = ...",
                     1, Actionability::AdvisoryVerify);
    }
}

// ── 规则：桩实现（plan §6.9，Tier 1）——只盯明确未完成态 ───────────────────
bool decorators_have_abstract(TSNode fn, const std::string& source) {
    TSNode parent = ts_node_parent(fn);
    if (ts_node_is_null(parent) || std::string(ts_node_type(parent)) != "decorated_definition")
        return false;
    uint32_t n = ts_node_named_child_count(parent);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode c = ts_node_named_child(parent, i);
        if (std::string(ts_node_type(c)) == "decorator" && contains_ci(node_text(c, source), "abstract"))
            return true;
    }
    return false;
}

// 接口/抽象基类命名：这类里的 raise NotImplementedError 是有意抽象声明，非 AI 遗漏。
bool looks_like_interface(const std::string& cls) {
    if (cls.size() >= 2 && cls[0] == 'I' && std::isupper(static_cast<unsigned char>(cls[1]))) return true;
    if (cls.rfind("Base", 0) == 0 || cls.rfind("Abstract", 0) == 0) return true;
    auto ends_with = [&](const char* suf) {
        std::string s(suf);
        return cls.size() >= s.size() && cls.compare(cls.size() - s.size(), s.size(), s) == 0;
    };
    return ends_with("Interface") || ends_with("Base") || ends_with("ABC");
}

// 从限定名取直接外层类名（"ISerializable.serialize" -> "ISerializable"）。
std::string enclosing_class(const std::string& symbol) {
    auto pos = symbol.rfind('.');
    if (pos == std::string::npos) return "";
    std::string before = symbol.substr(0, pos);
    auto p2 = before.rfind('.');
    return p2 == std::string::npos ? before : before.substr(p2 + 1);
}

// 限定名的末段（函数自身名）："A.B.foo" -> "foo"。
std::string symbol_leaf(const std::string& symbol) {
    auto pos = symbol.rfind('.');
    return pos == std::string::npos ? symbol : symbol.substr(pos + 1);
}

// 返回值是否为「平凡固定值」：标量字面量，或**空**集合。非空集合（如 [Obj(), ...]、
// (0,-0.2,0)）是在返回真实数据，不算 stub，排除以降误报。
bool is_constant_return(TSNode val) {
    std::string_view t = nt(val);
    // 注意：不含 none —— `return None`（含隐式）多为 mod 里合法的空回调重写（同 §6.9 决策）。
    if (t == "true" || t == "false" || t == "integer" || t == "float" ||
        t == "string" || t == "concatenated_string")
        return true;
    if (t == "list" || t == "dictionary" || t == "set" || t == "tuple")
        return ts_node_named_child_count(val) == 0; // 仅空集合算 stub
    return false;
}

// 非 self/cls 的参数个数（用于判定「接了输入却不用」）。
int non_self_param_count(TSNode fn, const std::string& source) {
    TSNode params = ts_node_child_by_field_name(fn, "parameters", 10);
    if (ts_node_is_null(params)) return 0;
    int cnt = 0;
    uint32_t n = ts_node_named_child_count(params);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode p = ts_node_named_child(params, i);
        std::string_view pt = nt(p);
        std::string name;
        if (pt == "identifier") name = node_text(p, source);
        else if (pt == "default_parameter" || pt == "typed_parameter" || pt == "typed_default_parameter")
            name = node_text(ts_node_child_by_field_name(p, "name", 4), source);
        else continue; // *args/**kwargs 等跳过
        if (!name.empty() && name != "self" && name != "cls") cnt++;
    }
    return cnt;
}

void check_stub(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
    if (decorators_have_abstract(fn, source)) return;              // 抽象方法豁免
    if (looks_like_interface(enclosing_class(symbol))) return;     // 接口/抽象基类豁免（plan §6.9）
    TSNode body = ts_node_child_by_field_name(fn, "body", 4);
    if (ts_node_is_null(body)) return;

    std::vector<TSNode> stmts;
    uint32_t n = ts_node_named_child_count(body);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode s = ts_node_named_child(body, i);
        std::string t = ts_node_type(s);
        if (t == "comment") continue;
        if (stmts.empty() && t == "expression_statement") { // 跳过 docstring
            TSNode inner = ts_node_named_child(s, 0);
            if (!ts_node_is_null(inner) && std::string(ts_node_type(inner)) == "string") continue;
        }
        stmts.push_back(s);
    }
    if (stmts.size() != 1) return; // 空 pass / return None / 多语句 —— 不报（域适配，plan §6.9）

    TSNode only = stmts[0];
    std::string t = ts_node_type(only);
    if (t == "raise_statement" && contains_ci(node_text(only, source), "NotImplementedError")) {
        make_finding(ctx, "stub.placeholder", Severity::Warning, 0.93, node_start_row(fn), symbol,
                     "未实现的桩函数", {"函数体只有 raise NotImplementedError"},
                     "补全实现；若为接口占位，用 @abstractmethod 明确声明。");
    } else if (t == "expression_statement") {
        TSNode inner = ts_node_named_child(only, 0);
        if (!ts_node_is_null(inner) && std::string(ts_node_type(inner)) == "ellipsis") {
            make_finding(ctx, "stub.placeholder", Severity::Hint, 0.85, node_start_row(fn), symbol,
                         "占位省略号函数体", {"函数体只有 ...（Ellipsis）"},
                         "补全实现；若为类型存根/接口占位可忽略。",
                         1, Actionability::AdvisoryVerify);
        }
    }
    // 注意：单条 `return <固定值>` 不再作为 shallow-impl 上报——py2 无 .pyi，开发者常裸写
    // 补全库（def f(self,x): return True 触发类型推导），1 行固定返回过于普遍。真正的假实现
    // 判定移交 check_shallow_impl：要求「体内≥N行业务 + 完全不用参数 + 只返回固定值」。
}

// ── 规则：假实现 stub.shallow-impl（抓 AI 偷懒，plan §6.9 延伸）──────────────
// 场景判定（用户反馈）：前提是函数体已有 ≥shallow_min_body 条业务语句（排除 py2 裸写补全
// 库的 1 行 return 固定值），却完全不引用任何入参、且所有 return 都是固定值——写了一堆
// "门面"逻辑但与参数无关，是 AI 假实现的典型形态。仍为 advisory-verify（纯 AST 无法确证
// 语义，只能提示核实；语义偷懒的根本天花板需跑测试/LLM）。

// 全下划线名（_ / __）是 Python 约定的"故意忽略"参数，不能算作"接了参数却不用"。
bool is_ignored_param_name(const std::string& n) {
    return !n.empty() && n.find_first_not_of('_') == std::string::npos;
}

// 收集函数「有意义的」参数名（排除 self/cls 与全下划线占位）。
void collect_param_names(TSNode fn, const std::string& source, std::unordered_set<std::string>& out) {
    TSNode params = ts_node_child_by_field_name(fn, "parameters", 10);
    if (ts_node_is_null(params)) return;
    uint32_t n = ts_node_named_child_count(params);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode p = ts_node_named_child(params, i);
        std::string_view pt = nt(p);
        std::string name;
        if (pt == "identifier") name = node_text(p, source);
        else if (pt == "default_parameter" || pt == "typed_parameter" || pt == "typed_default_parameter")
            name = node_text(ts_node_child_by_field_name(p, "name", 4), source);
        else continue; // *args/**kwargs 等
        if (!name.empty() && name != "self" && name != "cls" && !is_ignored_param_name(name))
            out.insert(name);
    }
}

// body 直接/嵌套的"业务语句"计数（跳过 docstring/注释/pass；不进嵌套函数/类作用域）。
int count_logic_stmts(TSNode node, bool top) {
    std::string_view t = nt(node);
    if (!top && (t == "function_definition" || t == "class_definition")) return 0;
    int c = 0;
    if (t == "block") {
        uint32_t n = ts_node_named_child_count(node);
        for (uint32_t i = 0; i < n; ++i) {
            TSNode s = ts_node_named_child(node, i);
            std::string_view st = nt(s);
            if (st == "comment" || st == "pass_statement") continue;
            if (st == "expression_statement") {
                TSNode inner = ts_node_named_child(s, 0);
                if (!ts_node_is_null(inner) && nt(inner) == "string") continue; // docstring
            }
            ++c;
        }
    }
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) c += count_logic_stmts(ts_node_child(node, i), false);
    return c;
}

// body 是否引用了任一参数（作为值读取）。obj.attr 只看 obj，foo(name=v) 只看 v，
// 避免把属性名/关键字实参名误判为"用了参数"。不进嵌套作用域。
bool body_uses_any_param(TSNode node, const std::string& source,
                         const std::unordered_set<std::string>& params, bool top) {
    std::string_view t = nt(node);
    if (!top && (t == "function_definition" || t == "class_definition")) return false;
    if (t == "identifier") return params.count(node_text(node, source)) > 0;
    if (t == "attribute") {
        TSNode obj = ts_node_child_by_field_name(node, "object", 6);
        return !ts_node_is_null(obj) && body_uses_any_param(obj, source, params, false);
    }
    if (t == "keyword_argument") {
        TSNode val = ts_node_child_by_field_name(node, "value", 5);
        return !ts_node_is_null(val) && body_uses_any_param(val, source, params, false);
    }
    uint32_t n = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < n; ++i)
        if (body_uses_any_param(ts_node_named_child(node, i), source, params, false)) return true;
    return false;
}

// 收集本函数作用域（不含嵌套函数）里的所有 return 语句。
void collect_returns(TSNode node, std::vector<TSNode>& out, bool top) {
    std::string_view t = nt(node);
    if (!top && (t == "function_definition" || t == "class_definition")) return;
    if (t == "return_statement") out.push_back(node);
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) collect_returns(ts_node_child(node, i), out, false);
}

void check_shallow_impl(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
    if (decorators_have_abstract(fn, source)) return;
    if (looks_like_interface(enclosing_class(symbol))) return;
    if (is_dunder(symbol_leaf(symbol))) return;
    TSNode body = ts_node_child_by_field_name(fn, "body", 4);
    if (ts_node_is_null(body)) return;

    std::unordered_set<std::string> params;
    collect_param_names(fn, source, params);
    if (params.empty()) return; // 没接参数，无从谈"接了不用"

    // 前提：函数体已有足够业务语句（排除 1 行 return 固定值的补全库桩）。
    if (count_logic_stmts(body, true) < ctx.cfg->shallow_min_body) return;

    // 所有 return 都是固定值，且至少有一个显式 return（无 return→隐式 None，不属"返回固定值"）。
    std::vector<TSNode> rets;
    collect_returns(body, rets, true);
    if (rets.empty()) return;
    std::string rv;
    for (size_t i = 0; i < rets.size(); ++i) {
        TSNode val = ts_node_named_child(rets[i], 0);
        if (ts_node_is_null(val) || !is_constant_return(val)) return; // 有非常量返回→非假实现
        if (i == 0) rv = node_text(val, source);
    }

    // 核心信号：完全不引用任何参数。
    if (body_uses_any_param(body, source, params, true)) return;

    if (rv.size() > 40) rv = rv.substr(0, 40) + "…";
    make_finding(ctx, "stub.shallow-impl", Severity::Risk, 0.6, node_start_row(fn), symbol,
                 "疑似假实现：有多行逻辑却不用任何参数、只返回固定值",
                 {"函数体有 ≥" + std::to_string(ctx.cfg->shallow_min_body) + " 条业务语句，但未引用任何参数",
                  "所有 return 都是固定值（如 return " + rv + "），与入参无关"},
                 "确认是否为真实现：若应据参数计算，请补全逻辑；若为类型补全/占位可忽略。",
                 2, Actionability::AdvisoryVerify);
}

// ── 规则：参数过多 signature.too-many-params（硬堆参数、未封装）─────────────
// 非 self/cls 参数超过 max_params（默认 5）多半是偷懒硬加参数、职责过载。
// __init__/__new__ 天然聚合状态，豁免；引擎要求的固定回调签名可整族关掉或调阈值。
void check_too_many_params(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    std::string leaf = symbol_leaf(symbol);
    if (leaf == "__init__" || leaf == "__new__") return;
    const std::string& source = *ctx.source;
    int cnt = non_self_param_count(fn, source);
    int limit = ctx.cfg->max_params;
    if (cnt <= limit) return;
    Severity sev = cnt >= limit + 3 ? Severity::Warning : Severity::Hint;
    make_finding(ctx, "signature.too-many-params", sev, 0.6, node_start_row(fn), symbol,
                 "函数参数过多（疑似硬堆参数、未封装）",
                 {"非 self 参数 " + std::to_string(cnt) + " 个 > " + std::to_string(limit),
                  "参数过多通常意味着职责过载，或本应聚合为配置对象/数据类"},
                 "把相关参数聚合为一个对象/字典，或按职责拆分函数；引擎要求的固定签名可忽略或调 max_params。",
                 2, Actionability::AdvisoryVerify);
}

// ── 规则：global 重绑定模块状态（plan §6.5，Tier 1）───────────────────────
void collect_identifiers(TSNode node, const std::string& source, std::unordered_set<std::string>& out) {
    if (ts_node_is_null(node)) return;
    std::string_view t = ts_node_type(node);
    if (t == "identifier") { out.insert(node_text(node, source)); return; }
    if (t == "attribute" || t == "subscript") return; // self.x / x[i] 不是对模块名的重绑定
    uint32_t n = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < n; ++i) collect_identifiers(ts_node_named_child(node, i), source, out);
}

void collect_globals_and_assigns(TSNode node, const std::string& source, bool top,
                                 std::vector<std::string>& globals,
                                 std::unordered_set<std::string>& assigns) {
    std::string_view t = ts_node_type(node);
    if (!top && t == "function_definition") return; // 嵌套函数有自己的作用域，不下钻
    if (t == "global_statement") {
        uint32_t n = ts_node_named_child_count(node);
        for (uint32_t i = 0; i < n; ++i) {
            TSNode c = ts_node_named_child(node, i);
            if (nt(c) == "identifier") globals.push_back(node_text(c, source));
        }
    } else if (t == "assignment" || t == "augmented_assignment") {
        collect_identifiers(ts_node_child_by_field_name(node, "left", 4), source, assigns);
    }
    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i)
        collect_globals_and_assigns(ts_node_child(node, i), source, false, globals, assigns);
}

void check_global_write(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
    TSNode body = ts_node_child_by_field_name(fn, "body", 4);
    if (ts_node_is_null(body)) return;
    std::vector<std::string> globals;
    std::unordered_set<std::string> assigns;
    collect_globals_and_assigns(body, source, true, globals, assigns);
    std::vector<std::string> written;
    for (const auto& g : globals) if (assigns.count(g)) written.push_back(g);
    if (written.empty()) return;
    std::sort(written.begin(), written.end());
    written.erase(std::unique(written.begin(), written.end()), written.end());
    std::string names;
    for (size_t i = 0; i < written.size(); ++i) { if (i) names += ", "; names += written[i]; }
    make_finding(ctx, "implicit-global.global-write", Severity::Warning, 0.85,
                 node_start_row(fn), symbol, "函数通过 global 重绑定模块级状态",
                 {"global 声明并写入: " + names, "隐式全局状态难测试、难复用"},
                 "把状态作为参数/返回值传递或封装到对象，避免函数直接重绑定模块级变量。");
}

// ── 规则：无 owner 的 TODO/FIXME（plan §6.8/§6.9，hint）───────────────────
void check_todo(TSNode node, const FileContext& ctx, const std::string& symbol) {
    std::string txt = node_text(node, *ctx.source);
    std::string upper = txt;
    for (auto& c : upper) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    const char* hit = nullptr;
    for (const char* k : {"TODO", "FIXME", "HACK", "XXX"})
        if (upper.find(k) != std::string::npos) { hit = k; break; }
    if (!hit) return;
    if (txt.find('(') != std::string::npos) return; // 疑似带 owner，如 TODO(alice) —— 不报
    std::string shown = txt.size() > 80 ? txt.substr(0, 80) + "…" : txt;
    make_finding(ctx, "comment-doc.unowned-todo", Severity::Hint, 0.70, node_start_row(node),
                 symbol, std::string(hit) + " 无 owner/原因", {shown},
                 "为 TODO 标注负责人与原因，或转为 issue 跟踪，避免长期腐烂。",
                 2, Actionability::AdvisoryVerify);
}

// ── 规则：单文件逻辑堆积 / 屎山（plan §6.3，Tier 2）───────────────────────
// 核心目的：拦住 AI 把大量逻辑堆进一个巨型文件。多重判定——必须同时「行数大」且
// 「def/class 多」——纯数据/配置文件（如 StaticDefine.py 1938 行但 u=1）天然大体量、
// 极少定义，不会命中。阈值 file_code_lines / file_units 可配（默认 1000 / 25）。
struct FileMetrics {
    int def_count = 0;
    int class_count = 0;
};

int count_code_lines(const std::string& src) {
    int n = 0;
    std::istringstream iss(src);
    std::string line;
    while (std::getline(iss, line)) {
        size_t a = line.find_first_not_of(" \t\r");
        if (a == std::string::npos) continue; // 空行
        if (line[a] == '#') continue;         // 纯注释行
        ++n;
    }
    return n;
}

// ── 规则：单函数过长（plan §6.3，Tier 2）─────────────────────────────────
// 「代码内容过多」的函数级版本。行跨度分级：偏长→risk，过长→warning。advisory-verify
//（有些事件分发/UI 回调天然长，不强制拆）。阈值 func_loc / func_loc_high 可配。
void check_large_function(TSNode fn, const FileContext& ctx, const std::string& symbol) {
    const int kFuncLoc = ctx.cfg->func_loc;
    const int kFuncLocHigh = ctx.cfg->func_loc_high;
    int start = static_cast<int>(ts_node_start_point(fn).row);
    int end = static_cast<int>(ts_node_end_point(fn).row);
    int loc = end - start + 1;
    if (loc < kFuncLoc) return;
    Severity sev = loc >= kFuncLocHigh ? Severity::Warning : Severity::Risk;
    make_finding(ctx, "logic-blob.large-function", sev, loc >= kFuncLocHigh ? 0.80 : 0.66,
                 node_start_row(fn), symbol, "单函数过长（逻辑堆积）",
                 {"函数约 " + std::to_string(loc) + " 行 ≥ " + std::to_string(kFuncLoc),
                  loc >= kFuncLocHigh ? "远超阈值，强烈建议拆分" : "偏长，建议按步骤/职责拆分"},
                 "把独立的步骤/职责抽成子函数，降低单函数认知复杂度；事件分发类长函数可酌情忽略。",
                 2, Actionability::AdvisoryVerify);
}

void check_large_file(const FileContext& ctx, int code_lines, const FileMetrics& fm) {
    const int kFileCodeLines = ctx.cfg->file_code_lines;
    const int kFileUnits = ctx.cfg->file_units;
    int units = fm.def_count + fm.class_count;
    if (code_lines < kFileCodeLines || units < kFileUnits) return;
    make_finding(ctx, "logic-blob.large-file", Severity::Warning, 0.82, 1, /*symbol=*/"",
                 "单文件过大且逻辑单元过多（职责堆积，疑似屎山）",
                 {"代码行 " + std::to_string(code_lines) + " ≥ " + std::to_string(kFileCodeLines),
                  "def+class 共 " + std::to_string(units) + " 个 ≥ " + std::to_string(kFileUnits),
                  "多重判定：纯数据/配置文件（大体量但极少 def/class）不会命中"},
                 "按职责把不相关的类/函数群拆分到独立模块，降低单文件认知负担；"
                 "若确为数据/生成文件可忽略。",
                 2, Actionability::AdvisoryVerify);
}

// ── AST 遍历：维护 def/class 限定名上下文 ─────────────────────────────────

void walk(TSNode node, const FileContext& ctx, std::vector<std::string>& scope_stack,
          FileMetrics& fm) {
    const std::string& source = *ctx.source;
    std::string_view type = ts_node_type(node);

    bool pushed = false;
    if (type == "function_definition" || type == "class_definition") {
        TSNode name = ts_node_child_by_field_name(node, "name", 4);
        std::string sym = node_text(name, source);
        if (!sym.empty()) { scope_stack.push_back(sym); pushed = true; }
        if (type == "function_definition") {
            fm.def_count++;
            std::string qual = join_scope(scope_stack);
            const ReviewConfig& c = *ctx.cfg;
            if (c.rule_mutable_default)  check_mutable_defaults(node, ctx, qual);
            if (c.rule_stub_placeholder) check_stub(node, ctx, qual);
            if (c.rule_shallow_impl)     check_shallow_impl(node, ctx, qual);
            if (c.rule_global_write)     check_global_write(node, ctx, qual);
            if (c.rule_large_function)   check_large_function(node, ctx, qual);
            if (c.rule_too_many_params)  check_too_many_params(node, ctx, qual);
            if (c.rule_duplicate_function && ctx.sigs && !is_dunder(sym)) { // 跳过 __init__ 等样板 dunder
                TSNode body = ts_node_child_by_field_name(node, "body", 4);
                std::string fp;
                fingerprint_walk(body, source, fp);
                if (fp_has_logic(fp)) { // 只收含真实逻辑的函数
                    ctx.sigs->push_back({fp, ctx.rel_path, qual, node_start_row(node),
                                         ts_node_is_null(body) ? 0
                                             : static_cast<int>(ts_node_named_child_count(body))});
                }
            }
        } else {
            fm.class_count++;
        }
    } else if (type == "try_statement") {
        if (ctx.cfg->rule_try_masking) check_try_masking(node, ctx, join_scope(scope_stack));
    } else if (type == "comment") {
        if (ctx.cfg->rule_unowned_todo) check_todo(node, ctx, join_scope(scope_stack));
    }

    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) {
        walk(ts_node_child(node, i), ctx, scope_stack, fm);
    }
    if (pushed) scope_stack.pop_back();
}

inline double ms_since(std::chrono::steady_clock::time_point a) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - a).count();
}

// 单文件解析 + 运行规则，返回 parse health。parser 复用；无共享可变状态，可并行调用。
ParseHealth analyze_file(const fs::path& file, const fs::path& bp_root,
                         std::vector<Finding>& out, std::vector<FuncSig>& sigs,
                         TSParser* parser, const ReviewConfig& cfg) {
    std::string source = read_text_file(file);
    if (source.empty()) {
        // 空文件（如空 __init__.py）是合法空模块，不算解析失败；仅文件非空却读不出才 fallback。
        std::error_code ec;
        auto sz = fs::file_size(file, ec);
        return (!ec && sz == 0) ? ParseHealth::Ok : ParseHealth::Fallback;
    }

    TSTree* tree = ts_parser_parse_string(parser, nullptr, source.c_str(),
                                          static_cast<uint32_t>(source.size()));
    TSNode root = ts_tree_root_node(tree);
    bool has_error = ts_node_has_error(root);

    FileContext ctx;
    ctx.source = &source;
    ctx.rel_path = rel_utf8(file, bp_root);
    ctx.out = &out;
    ctx.sigs = &sigs;
    ctx.cfg = &cfg;

    // 有 ERROR 节点时 tree-sitter 仍可局部遍历：recovered，规则照跑。
    std::vector<std::string> scope_stack;
    FileMetrics fm;
    walk(root, ctx, scope_stack, fm);
    if (cfg.rule_large_file) check_large_file(ctx, count_code_lines(source), fm);

    ts_tree_delete(tree);
    return has_error ? ParseHealth::Recovered : ParseHealth::Ok;
}

// 跨模块重复函数聚类（plan §6.2，Tier 3）：相同结构指纹、非 trivial、跨 ≥2 文件、≥N 处。
// 阈值 dup_min_stmts / dup_min_fp_len / dup_min_count 可配。
void cluster_duplicate_functions(const std::vector<FuncSig>& sigs, std::vector<Finding>& out,
                                 const ReviewConfig& cfg) {
    const int kMinStmts = cfg.dup_min_stmts;             // 过滤 trivial（getter/空壳）
    const size_t kMinFpLen = static_cast<size_t>(cfg.dup_min_fp_len); // 指纹过短易巧合
    const int kMinCount = cfg.dup_min_count;             // 最少重复处数

    std::unordered_map<std::string, std::vector<const FuncSig*>> clusters;
    for (const auto& s : sigs) {
        if (s.stmts < kMinStmts || s.fingerprint.size() < kMinFpLen) continue;
        clusters[s.fingerprint].push_back(&s);
    }
    for (auto& kv : clusters) {
        auto& members = kv.second;
        if (static_cast<int>(members.size()) < kMinCount) continue;
        std::unordered_set<std::string> files;
        for (auto* m : members) files.insert(m->file);
        if (files.size() < 2) continue; // 必须散落在不同模块/文件

        std::sort(members.begin(), members.end(), [](const FuncSig* a, const FuncSig* b) {
            if (a->file != b->file) return a->file < b->file;
            return a->line < b->line;
        });
        const FuncSig* first = members.front();
        Finding f;
        f.rule_id = "duplicate-function.cross-module";
        f.severity = Severity::Warning;
        f.tier = 3;
        f.actionability = Actionability::AdvisoryVerify;
        f.confidence = 0.60;
        f.file = first->file;
        f.line = first->line;
        f.symbol = first->symbol;
        f.title = "多个结构相同的函数散落在不同模块（疑似重复造轮子）";
        f.evidence.push_back("相同结构指纹出现 " + std::to_string(members.size()) +
                             " 处，跨 " + std::to_string(files.size()) + " 个文件");
        size_t shown = 0;
        for (auto* m : members) {
            if (shown++ >= 8) { f.evidence.push_back("…其余略"); break; }
            f.evidence.push_back(m->file + ":" + std::to_string(m->line) + "  " + m->symbol);
        }
        f.suggestion = "确认是否为同类逻辑重复：若是，抽取为共享工具函数/基类方法，消除散落副本。";
        f.finding_key = "duplicate-function.cross-module@" + first->file + ":" +
                        std::to_string(first->line);
        out.push_back(std::move(f));
    }
}

} // namespace

// ── 公开接口 ──────────────────────────────────────────────────────────────

ReviewReport ReviewAnalyzer::review(const fs::path& behavior_pack_root,
                                    const ReviewOptions& options) const {
    ReviewReport report;
    report.behavior_pack_root = to_utf8(behavior_pack_root);

    std::error_code ec;
    if (!fs::exists(behavior_pack_root, ec)) {
        report.config_warnings.push_back("路径不存在: " + report.behavior_pack_root);
        report.discovery_mode = "none";
        return report;
    }

    // 解析可调配置（显式路径 > 行为包根自动发现 > 默认值）。
    ReviewConfig cfg = resolve_config(behavior_pack_root, options.config_path,
                                      report.config_warnings, report.config_source);
    if (options.max_findings_per_rule >= 0) // --max-per-rule 快捷覆盖
        cfg.max_findings_per_rule = options.max_findings_per_rule;
    report.max_findings_per_rule = cfg.max_findings_per_rule;

    auto pkgs = discover_packages(behavior_pack_root, options.include_third_party,
                                  report.discovery_mode, report.ignored_dirs);

    // ① 顺序阶段（廉价）：建包报告 + 收集待扫描文件（in-scope），保留确定性顺序。
    struct ScanItem { size_t pkg_idx; fs::path file; };
    std::vector<ScanItem> items;
    for (const auto& pkg : pkgs) {
        if (!package_in_scope(pkg.name, options.scope)) continue;
        PackageReport pr;
        pr.name = pkg.name;
        pr.entry_file = pkg.entry_file.empty() ? "" : to_utf8(pkg.entry_file);
        pr.root_dir = to_utf8(pkg.root_dir);
        size_t idx = report.packages.size();
        report.packages.push_back(std::move(pr));
        for (const auto& file : pkg.files) {
            if (!file_in_scope(rel_utf8(file, pkg.root_dir), pkg.name, options.scope)) continue;
            items.push_back({idx, file});
        }
    }

    // ② 并行阶段：每文件独立解析+跑规则，结果按 index 落位（无共享写，确定性）。
    struct ScanResult {
        std::vector<Finding> findings;
        std::vector<FuncSig> sigs;
        ParseHealth          health = ParseHealth::Ok;
    };
    std::vector<ScanResult> results(items.size());
    auto t_scan = std::chrono::steady_clock::now();

    unsigned hw = std::thread::hardware_concurrency();
    unsigned nthreads = std::min<unsigned>(std::max(1u, hw), 8u);
    if (items.size() < 4) nthreads = 1; // 小任务不值得开线程
    std::atomic<size_t> next{0};
    auto worker = [&]() {
        TSParser* parser = ts_parser_new(); // 每线程独立 parser（parser 非线程安全）
        ts_parser_set_language(parser, tree_sitter_python());
        for (size_t i = next.fetch_add(1); i < items.size(); i = next.fetch_add(1)) {
            results[i].health = analyze_file(items[i].file, behavior_pack_root,
                                             results[i].findings, results[i].sigs, parser, cfg);
        }
        ts_parser_delete(parser);
    };
    if (nthreads <= 1) {
        worker();
    } else {
        std::vector<std::thread> pool;
        for (unsigned t = 0; t < nthreads; ++t) pool.emplace_back(worker);
        for (auto& th : pool) th.join();
    }

    // ③ 顺序合并（按 index，确定性）。
    std::vector<FuncSig> all_sigs;
    for (size_t i = 0; i < items.size(); ++i) {
        auto& r = results[i];
        PackageReport& pr = report.packages[items[i].pkg_idx];
        pr.files_scanned++;
        report.files_scanned++;
        switch (r.health) {
            case ParseHealth::Ok:        pr.parse_ok++; report.parse_ok++; break;
            case ParseHealth::Recovered: pr.parse_recovered++; report.parse_recovered++; break;
            case ParseHealth::Fallback:  pr.parse_failed++; report.parse_failed++; break;
        }
        for (auto& f : r.findings) report.findings.push_back(std::move(f));
        for (auto& s : r.sigs) all_sigs.push_back(std::move(s));
    }
    double scan_ms = ms_since(t_scan);

    // 全局跨模块重复函数检测（需所有函数指纹到齐后再聚类）。
    if (cfg.rule_duplicate_function) cluster_duplicate_functions(all_sigs, report.findings, cfg);

    if (std::getenv("MCDK_REVIEW_TIMING")) {
        std::cerr << "[timing] files=" << report.files_scanned << " threads=" << nthreads
                  << " scan=" << (long)scan_ms << "ms sigs=" << all_sigs.size() << "\n";
    }

    // 确定性排序（plan §8.1：稳定 finding_key + 确定排序）。
    std::sort(report.findings.begin(), report.findings.end(),
              [](const Finding& a, const Finding& b) {
                  if (a.file != b.file) return a.file < b.file;
                  if (a.line != b.line) return a.line < b.line;
                  return a.finding_key < b.finding_key;
              });
    return report;
}

std::string ReviewAnalyzer::review_formatted(const fs::path& behavior_pack_root,
                                             const ReviewOptions& options) const {
    ReviewReport report = review(behavior_pack_root, options);
    if (options.format == "json") return render_json(report);
    if (options.format == "summary") return render_summary(report);
    return render_markdown(report);
}

// ── 渲染 ──────────────────────────────────────────────────────────────────

namespace {
std::string highest_severity(const std::vector<Finding>& fs) {
    Severity hi = Severity::Hint;
    bool any = false;
    for (const auto& f : fs) { if (!any || static_cast<int>(f.severity) > static_cast<int>(hi)) { hi = f.severity; any = true; } }
    return any ? to_string(hi) : "none";
}

struct SevCounts { int hint = 0, risk = 0, warning = 0, error = 0; };
SevCounts count_severities(const std::vector<Finding>& fs) {
    SevCounts c;
    for (const auto& f : fs) switch (f.severity) {
        case Severity::Hint:    c.hint++; break;
        case Severity::Risk:    c.risk++; break;
        case Severity::Warning: c.warning++; break;
        case Severity::Error:   c.error++; break;
    }
    return c;
}

// 按 (rule_id, severity) 分组：同规则同级别的 findings 共享 title/tier/actionability/suggestion，
// 只输出一次（去重降密度）。按 severity 分是因为同一规则可含不同级别（如 too-many-params 6~7
// 为 hint、≥8 为 warning；try.* 由 try 体大小分级），若只按 rule_id 分组头部级别会失真。
// 组按 severity 降序、rule_id 升序；组内保持 findings 的 file/line 有序（report.findings 已排序）。
struct RuleGroup {
    std::string                 rule_id;
    Severity                    severity;
    int                         tier;
    Actionability               act;
    std::string                 title;
    std::string                 suggestion;
    std::vector<const Finding*> items;
};
std::vector<RuleGroup> group_by_rule(const std::vector<Finding>& fs) {
    std::unordered_map<std::string, size_t> idx;
    std::vector<RuleGroup> groups;
    for (const auto& f : fs) {
        std::string key = f.rule_id + "\x01" + std::to_string(static_cast<int>(f.severity));
        auto it = idx.find(key);
        if (it == idx.end()) {
            idx.emplace(key, groups.size());
            groups.push_back({f.rule_id, f.severity, f.tier, f.actionability, f.title, f.suggestion, {}});
        }
        groups[idx[key]].items.push_back(&f);
    }
    std::sort(groups.begin(), groups.end(), [](const RuleGroup& a, const RuleGroup& b) {
        if (a.severity != b.severity) return static_cast<int>(a.severity) > static_cast<int>(b.severity);
        return a.rule_id < b.rule_id;
    });
    return groups;
}

// 压平换行并截断，保证"每处一行"的密度。
std::string oneline(std::string s, size_t max_len) {
    for (auto& ch : s) if (ch == '\n' || ch == '\r' || ch == '\t') ch = ' ';
    if (s.size() > max_len) s = s.substr(0, max_len) + "…";
    return s;
}
} // namespace

std::string render_markdown(const ReviewReport& report) {
    std::ostringstream o;
    SevCounts sc = count_severities(report.findings);
    o << "# Python AI Code Review\n\n";
    o << "behavior_pack: " << report.behavior_pack_root << "\n";
    o << "packages=" << report.packages.size() << " · files=" << report.files_scanned
      << " · parse " << report.parse_ok << " ok/" << report.parse_recovered << " rec/"
      << report.parse_failed << " fail\n";
    o << "findings=" << report.findings.size() << "  (warning=" << sc.warning
      << " risk=" << sc.risk << " hint=" << sc.hint;
    if (sc.error) o << " error=" << sc.error;
    o << ")  highest=" << highest_severity(report.findings) << "\n";
    o << "config: " << report.config_source << "\n";
    if (report.max_findings_per_rule > 0)
        o << "note: 每规则最多列 " << report.max_findings_per_rule
          << " 处（超出仅计数；改 output.max_findings_per_rule 或用 --format json 看全部）\n";
    if (!report.ignored_dirs.empty())
        o << "ignored_third_party: " << report.ignored_dirs.size() << " dir(s)\n";
    o << "\n";

    if (!report.config_warnings.empty()) {
        o << "## Config Warnings\n";
        for (const auto& w : report.config_warnings) o << "- " << w << "\n";
        o << "\n";
    }

    if (report.findings.empty()) {
        o << "## Findings\n_(no findings)_\n";
        return o.str();
    }

    // 按规则分组：title/建议每组只印一次，每处定位压成单行，超出上限只计数——控 MCP 召回体积。
    o << "## Findings（按规则分组）\n\n";
    const int cap = report.max_findings_per_rule; // 0 = 不限
    for (const auto& g : group_by_rule(report.findings)) {
        o << "### [" << to_string(g.severity) << "] " << g.rule_id << "  ×" << g.items.size()
          << "  (tier " << g.tier << " · " << to_string(g.act) << ")\n";
        o << g.title << "\n";
        int shown = 0;
        for (const auto* f : g.items) {
            if (cap > 0 && shown >= cap) break;
            o << "- " << f->file << ":" << f->line;
            if (!f->symbol.empty()) o << "  " << f->symbol;
            if (!f->evidence.empty()) o << "  · " << oneline(f->evidence.front(), 90);
            o << "\n";
            ++shown;
        }
        if (cap > 0 && static_cast<int>(g.items.size()) > cap)
            o << "- …其余 " << (static_cast<int>(g.items.size()) - cap) << " 处（同规则，--format json 看全部）\n";
        if (!g.suggestion.empty()) o << "→ " << g.suggestion << "\n";
        o << "\n";
    }
    return o.str();
}

std::string render_summary(const ReviewReport& report) {
    std::ostringstream o;
    SevCounts sc = count_severities(report.findings);
    o << "# Python AI Code Review — Summary\n";
    o << "behavior_pack: " << report.behavior_pack_root << "\n";
    o << "files=" << report.files_scanned << " · parse " << report.parse_ok << " ok/"
      << report.parse_recovered << " rec/" << report.parse_failed << " fail · config: "
      << report.config_source << "\n";
    o << "findings=" << report.findings.size() << " · highest=" << highest_severity(report.findings) << "\n";
    o << "severity: warning=" << sc.warning << " risk=" << sc.risk << " hint=" << sc.hint;
    if (sc.error) o << " error=" << sc.error;
    o << "\n";
    if (!report.config_warnings.empty()) {
        o << "config_warnings:\n";
        for (const auto& w : report.config_warnings) o << "  - " << w << "\n";
    }
    if (report.findings.empty()) { o << "\n(no findings)\n"; return o.str(); }

    o << "\nby rule (severity desc):\n";
    for (const auto& g : group_by_rule(report.findings))
        o << "  [" << to_string(g.severity) << "] " << g.rule_id << "  ×" << g.items.size() << "\n";

    std::unordered_map<std::string, int> fc;
    for (const auto& f : report.findings) fc[f.file]++;
    std::vector<std::pair<std::string, int>> files(fc.begin(), fc.end());
    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    o << "\ntop files:\n";
    size_t n = std::min<size_t>(files.size(), 8);
    for (size_t i = 0; i < n; ++i) o << "  " << files[i].second << "  " << files[i].first << "\n";
    if (files.size() > n) o << "  …其余 " << (files.size() - n) << " 文件\n";
    o << "\n(用 --format markdown 看定位与建议；--format json 取机读全量)\n";
    return o.str();
}

std::string render_json(const ReviewReport& report) {
    using nlohmann::json;
    json j;
    j["behavior_pack_root"] = report.behavior_pack_root;
    j["package_discovery"] = report.discovery_mode;
    j["config_source"] = report.config_source;
    j["files_scanned"] = report.files_scanned;
    j["parse_health"] = {
        {"ok", report.parse_ok},
        {"recovered", report.parse_recovered},
        {"failed", report.parse_failed},
    };
    SevCounts sc = count_severities(report.findings);
    j["severity_counts"] = {{"warning", sc.warning}, {"risk", sc.risk},
                            {"hint", sc.hint}, {"error", sc.error}};
    j["findings_total"] = static_cast<int>(report.findings.size());
    j["highest_severity"] = highest_severity(report.findings);
    j["max_findings_per_rule"] = report.max_findings_per_rule;

    j["packages"] = json::array();
    for (const auto& p : report.packages) {
        j["packages"].push_back({
            {"name", p.name},
            {"entry_file", p.entry_file},
            {"files_scanned", p.files_scanned},
            {"parse_ok", p.parse_ok},
            {"parse_recovered", p.parse_recovered},
            {"parse_failed", p.parse_failed},
        });
    }

    // 按规则分组：rule_id/severity/tier/actionability/title/suggestion 每组一份（去重降体积），
    // 每处 finding 只留定位 + 证据；超出 max_findings_per_rule 只计 truncated（非静默）。
    const int cap = report.max_findings_per_rule; // 0 = 不限
    j["rules"] = json::array();
    for (const auto& g : group_by_rule(report.findings)) {
        json rj;
        rj["rule_id"] = g.rule_id;
        rj["severity"] = to_string(g.severity);
        rj["tier"] = g.tier;
        rj["actionability"] = to_string(g.act);
        rj["title"] = g.title;
        rj["suggestion"] = g.suggestion;
        rj["count"] = static_cast<int>(g.items.size());
        json arr = json::array();
        int shown = 0;
        for (const auto* f : g.items) {
            if (cap > 0 && shown >= cap) break;
            arr.push_back({
                {"finding_key", f->finding_key},
                {"file", f->file},
                {"line", f->line},
                {"symbol", f->symbol},
                {"confidence", f->confidence},
                {"evidence", f->evidence},
            });
            ++shown;
        }
        rj["shown"] = shown;
        if (cap > 0 && static_cast<int>(g.items.size()) > cap)
            rj["truncated"] = static_cast<int>(g.items.size()) - cap;
        rj["findings"] = std::move(arr);
        j["rules"].push_back(std::move(rj));
    }

    j["ignored_third_party"] = report.ignored_dirs;
    j["config_warnings"] = report.config_warnings;
    return j.dump(2);
}

} // namespace mcdk::python_review
