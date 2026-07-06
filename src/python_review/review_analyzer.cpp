#include "python_review/review_analyzer.hpp"

#include "common/path_utils.hpp"

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-python.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
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

// 读文件并去除 UTF-8 BOM。
std::string read_text_file(const fs::path& file_path) {
    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs.is_open()) return {};
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
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

struct FileContext {
    const std::string* source;
    std::string        rel_path; // UTF-8，相对行为包根
    std::vector<Finding>* out;
};

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
        std::string t = ts_node_type(st);
        if (t == "comment") continue;
        hc.stmt_count++;

        if (t == "pass_statement" || t == "return_statement" ||
            t == "continue_statement" || t == "break_statement") {
            continue; // 低信息：吞掉/跳过
        }
        if (t == "raise_statement") continue; // 已计入 has_raise
        if (t == "expression_statement") {
            TSNode inner = ts_node_named_child(st, 0);
            std::string it = ts_node_is_null(inner) ? "" : ts_node_type(inner);
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

void check_try_masking(TSNode try_stmt, const FileContext& ctx, const std::string& symbol) {
    const std::string& source = *ctx.source;
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
        std::string loc_ev = "try 体约 " + std::to_string(try_loc) + " 行";

        // handler 性质：silent（纯 pass/return，真吞）/ logged（仅日志）/ fallback（回退值等兜底逻辑）。
        enum HandlerKind { Silent, Logged, Fallback };
        HandlerKind kind = hc.has_other ? Fallback : (hc.has_log ? Logged : Silent);
        std::string handler_ev =
            kind == Silent   ? "handler 只有 pass/return，静默吞掉异常"
          : kind == Logged   ? "handler 仅记录日志后继续，未重新抛出"
                             : "handler 用回退值/其他逻辑兜底，但捕获过宽";
        const std::string suggestion =
            "捕获更窄的异常类型；显式处理错误路径，或补充上下文后重新 raise。";

        if (!typed) {
            // 裸 except 分类过宽（会吞 KeyboardInterrupt/SystemExit），即便有兜底也报。
            // silent → warning；logged/fallback → risk（mod 容错/防御惯例，plan §6.0 适配）。
            Severity sev = (kind == Silent) ? Severity::Warning : Severity::Risk;
            double conf = (kind == Silent) ? 0.92 : (kind == Logged ? 0.80 : 0.72);
            make_finding(ctx, "try.masking.bare-except", sev, conf, line, symbol,
                         "裸 except 吞掉了失败",
                         {"裸 except，未指定异常类型", loc_ev, handler_ev}, suggestion);
            continue;
        }

        // 过宽捕获：except Exception / BaseException 且吞掉（无实质兜底）。有真实兜底则不报。
        bool broad = contains_ci(type_text, "BaseException") || contains_ci(type_text, "Exception");
        if (broad && kind != Fallback) {
            Severity sev = (kind == Logged) ? Severity::Risk : Severity::Warning;
            double conf = (kind == Logged) ? 0.78 : 0.88;
            make_finding(ctx, "try.masking.broad-except-swallow", sev, conf, line, symbol,
                         "过宽的 except 吞掉了失败",
                         {"except " + type_text + " 捕获过宽", loc_ev, handler_ev}, suggestion);
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
    std::string t = ts_node_type(node);
    if (!top && t == "function_definition") return false; // 嵌套函数会遮蔽同名参数，不下钻
    if (t == "augmented_assignment") { // pname += ...
        TSNode left = ts_node_child_by_field_name(node, "left", 4);
        if (!ts_node_is_null(left) && std::string(ts_node_type(left)) == "identifier" &&
            node_text(left, source) == pname)
            return true;
    } else if (t == "call") { // pname.append(...) 等增长型方法
        TSNode fn = ts_node_child_by_field_name(node, "function", 8);
        if (!ts_node_is_null(fn) && std::string(ts_node_type(fn)) == "attribute") {
            TSNode obj = ts_node_child_by_field_name(fn, "object", 6);
            TSNode attr = ts_node_child_by_field_name(fn, "attribute", 9);
            if (!ts_node_is_null(obj) && std::string(ts_node_type(obj)) == "identifier" &&
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
        if (is_caller_supplied_param(pname)) continue;         // 引擎/调用方提供，默认值不共享
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
}

// ── 规则：global 重绑定模块状态（plan §6.5，Tier 1）───────────────────────
void collect_identifiers(TSNode node, const std::string& source, std::unordered_set<std::string>& out) {
    if (ts_node_is_null(node)) return;
    std::string t = ts_node_type(node);
    if (t == "identifier") { out.insert(node_text(node, source)); return; }
    if (t == "attribute" || t == "subscript") return; // self.x / x[i] 不是对模块名的重绑定
    uint32_t n = ts_node_named_child_count(node);
    for (uint32_t i = 0; i < n; ++i) collect_identifiers(ts_node_named_child(node, i), source, out);
}

void collect_globals_and_assigns(TSNode node, const std::string& source, bool top,
                                 std::vector<std::string>& globals,
                                 std::unordered_set<std::string>& assigns) {
    std::string t = ts_node_type(node);
    if (!top && t == "function_definition") return; // 嵌套函数有自己的作用域，不下钻
    if (t == "global_statement") {
        uint32_t n = ts_node_named_child_count(node);
        for (uint32_t i = 0; i < n; ++i) {
            TSNode c = ts_node_named_child(node, i);
            if (std::string(ts_node_type(c)) == "identifier") globals.push_back(node_text(c, source));
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

// ── AST 遍历：维护 def/class 限定名上下文 ─────────────────────────────────

void walk(TSNode node, const FileContext& ctx, std::vector<std::string>& scope_stack) {
    const std::string& source = *ctx.source;
    std::string type = ts_node_type(node);

    bool pushed = false;
    if (type == "function_definition" || type == "class_definition") {
        TSNode name = ts_node_child_by_field_name(node, "name", 4);
        std::string sym = node_text(name, source);
        if (!sym.empty()) { scope_stack.push_back(sym); pushed = true; }
        if (type == "function_definition") {
            std::string qual = join_scope(scope_stack);
            check_mutable_defaults(node, ctx, qual);
            check_stub(node, ctx, qual);
            check_global_write(node, ctx, qual);
        }
    } else if (type == "try_statement") {
        check_try_masking(node, ctx, join_scope(scope_stack));
    } else if (type == "comment") {
        check_todo(node, ctx, join_scope(scope_stack));
    }

    uint32_t n = ts_node_child_count(node);
    for (uint32_t i = 0; i < n; ++i) {
        walk(ts_node_child(node, i), ctx, scope_stack);
    }
    if (pushed) scope_stack.pop_back();
}

// 单文件解析 + 运行规则，返回 parse health。
ParseHealth analyze_file(const fs::path& file, const fs::path& bp_root,
                         std::vector<Finding>& out) {
    std::string source = read_text_file(file);
    if (source.empty()) {
        // 空文件（如空 __init__.py）是合法空模块，不算解析失败；仅文件非空却读不出才 fallback。
        std::error_code ec;
        auto sz = fs::file_size(file, ec);
        return (!ec && sz == 0) ? ParseHealth::Ok : ParseHealth::Fallback;
    }

    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_python());
    TSTree* tree = ts_parser_parse_string(parser, nullptr, source.c_str(),
                                          static_cast<uint32_t>(source.size()));
    TSNode root = ts_tree_root_node(tree);

    bool has_error = ts_node_has_error(root);

    FileContext ctx;
    ctx.source = &source;
    ctx.rel_path = rel_utf8(file, bp_root);
    ctx.out = &out;

    // 语法完全崩坏（fallback）时，跳过依赖 AST 的规则，避免在半解析树上误报（plan §7 门槛）。
    ParseHealth health = ParseHealth::Ok;
    if (!has_error) {
        std::vector<std::string> scope_stack;
        walk(root, ctx, scope_stack);
        health = ParseHealth::Ok;
    } else {
        // 有 ERROR 节点但仍可局部遍历：recovered，规则照跑（tree-sitter 局部恢复）。
        std::vector<std::string> scope_stack;
        walk(root, ctx, scope_stack);
        health = ParseHealth::Recovered;
    }

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return health;
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

    auto pkgs = discover_packages(behavior_pack_root, options.include_third_party,
                                  report.discovery_mode, report.ignored_dirs);

    for (const auto& pkg : pkgs) {
        if (!package_in_scope(pkg.name, options.scope)) continue;

        PackageReport pr;
        pr.name = pkg.name;
        pr.entry_file = pkg.entry_file.empty() ? "" : to_utf8(pkg.entry_file);
        pr.root_dir = to_utf8(pkg.root_dir);

        for (const auto& file : pkg.files) {
            std::string rel_from_pkg = rel_utf8(file, pkg.root_dir);
            if (!file_in_scope(rel_from_pkg, pkg.name, options.scope)) continue;

            std::vector<Finding> file_findings;
            ParseHealth h = analyze_file(file, behavior_pack_root, file_findings);

            pr.files_scanned++;
            report.files_scanned++;
            switch (h) {
                case ParseHealth::Ok:        pr.parse_ok++; report.parse_ok++; break;
                case ParseHealth::Recovered: pr.parse_recovered++; report.parse_recovered++; break;
                case ParseHealth::Fallback:  pr.parse_failed++; report.parse_failed++; break;
            }
            for (auto& f : file_findings) report.findings.push_back(std::move(f));
        }
        report.packages.push_back(std::move(pr));
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
    return options.format == "json" ? render_json(report) : render_markdown(report);
}

// ── 渲染 ──────────────────────────────────────────────────────────────────

namespace {
std::string highest_severity(const std::vector<Finding>& fs) {
    Severity hi = Severity::Hint;
    bool any = false;
    for (const auto& f : fs) { if (!any || static_cast<int>(f.severity) > static_cast<int>(hi)) { hi = f.severity; any = true; } }
    return any ? to_string(hi) : "none";
}
} // namespace

std::string render_markdown(const ReviewReport& report) {
    std::ostringstream o;
    o << "# Python AI Code Review\n\n";
    o << "## Summary\n";
    o << "- behavior_pack: " << report.behavior_pack_root << "\n";
    o << "- packages_scanned: " << report.packages.size() << "\n";
    o << "- package_discovery: " << report.discovery_mode << "\n";
    o << "- files_scanned: " << report.files_scanned << "\n";
    o << "- parse_health: " << report.parse_ok << " ok, " << report.parse_recovered
      << " recovered, " << report.parse_failed << " failed\n";
    o << "- findings: " << report.findings.size() << "\n";
    o << "- highest_severity: " << highest_severity(report.findings) << "\n";
    if (!report.ignored_dirs.empty())
        o << "- ignored_third_party: " << report.ignored_dirs.size() << " dir(s)\n";
    o << "\n";

    if (!report.config_warnings.empty()) {
        o << "## Config Warnings\n";
        for (const auto& w : report.config_warnings) o << "- " << w << "\n";
        o << "\n";
    }

    o << "## Findings\n";
    if (report.findings.empty()) {
        o << "_(no findings)_\n";
        return o.str();
    }
    for (const auto& f : report.findings) {
        o << "### [" << to_string(f.severity) << "] " << f.rule_id
          << "  (" << to_string(f.actionability) << ", tier " << f.tier << ")\n";
        o << f.file << ":" << f.line;
        if (!f.symbol.empty()) o << "  " << f.symbol;
        o << "\n" << f.title << "\n";
        if (!f.evidence.empty()) {
            o << "Evidence:\n";
            for (const auto& e : f.evidence) o << "- " << e << "\n";
        }
        if (!f.suggestion.empty()) o << "Suggestion:\n" << f.suggestion << "\n";
        o << "\n";
    }
    return o.str();
}

std::string render_json(const ReviewReport& report) {
    using nlohmann::json;
    json j;
    j["behavior_pack_root"] = report.behavior_pack_root;
    j["package_discovery"] = report.discovery_mode;
    j["files_scanned"] = report.files_scanned;
    j["parse_health"] = {
        {"ok", report.parse_ok},
        {"recovered", report.parse_recovered},
        {"failed", report.parse_failed},
    };
    j["highest_severity"] = highest_severity(report.findings);

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

    j["findings"] = json::array();
    for (const auto& f : report.findings) {
        j["findings"].push_back({
            {"finding_key", f.finding_key},
            {"rule_id", f.rule_id},
            {"severity", to_string(f.severity)},
            {"tier", f.tier},
            {"actionability", to_string(f.actionability)},
            {"confidence", f.confidence},
            {"file", f.file},
            {"line", f.line},
            {"symbol", f.symbol},
            {"title", f.title},
            {"evidence", f.evidence},
            {"suggestion", f.suggestion},
        });
    }

    j["ignored_third_party"] = report.ignored_dirs;
    j["config_warnings"] = report.config_warnings;
    return j.dump(2);
}

} // namespace mcdk::python_review
