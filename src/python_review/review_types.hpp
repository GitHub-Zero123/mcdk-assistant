#pragma once
// Python AI Code Review — 统一数据模型（见 plans/python-ai-code-review-plan.md §5、§6.0、§8.1）
//
// 该层只依赖标准库；tree-sitter / 文件系统封装在 review_analyzer.* 内部。
#include <string>
#include <vector>

namespace mcdk::python_review {

// 严重级别四分法（plan §2）：error/warning/risk/hint。
enum class Severity { Hint, Risk, Warning, Error };

inline const char* to_string(Severity s) {
    switch (s) {
        case Severity::Hint:    return "hint";
        case Severity::Risk:    return "risk";
        case Severity::Warning: return "warning";
        case Severity::Error:   return "error";
    }
    return "warning";
}

// 面向 AI 闭环的可执行性（plan §5.4 / §8.1）。
// must-fix / should-fix：Tier 1/2 确定性问题，AI 可直接据此修改。
// advisory-verify：Tier 3 启发式，仅提示核实，禁止盲改。
enum class Actionability { MustFix, ShouldFix, AdvisoryVerify };

inline const char* to_string(Actionability a) {
    switch (a) {
        case Actionability::MustFix:        return "must-fix";
        case Actionability::ShouldFix:      return "should-fix";
        case Actionability::AdvisoryVerify: return "advisory-verify";
    }
    return "should-fix";
}

// 引用/文件的解析健康度（plan §7：容忍 ERROR/MISSING，输出 parse_health）。
enum class ParseHealth { Ok, Recovered, Fallback };

inline const char* to_string(ParseHealth h) {
    switch (h) {
        case ParseHealth::Ok:        return "ok";
        case ParseHealth::Recovered: return "recovered";
        case ParseHealth::Fallback:  return "fallback";
    }
    return "ok";
}

// 统一 Finding（plan §5.4）。finding_key 稳定去重，保证闭环里同代码多次 review 输出一致。
struct Finding {
    std::string              finding_key;   // rule_id@相对路径:line[::symbol]
    std::string              rule_id;
    Severity                 severity = Severity::Warning;
    int                      tier = 1;      // 规则精度层，见 plan §6.0
    Actionability            actionability = Actionability::ShouldFix;
    double                   confidence = 0.9;
    std::string              file;          // UTF-8，相对行为包根
    int                      line = 0;
    std::string              symbol;        // 所在函数/类限定名，可空
    std::string              title;
    std::vector<std::string> evidence;
    std::string              suggestion;
};

// 单个 Python 包（modMain 入口）的摘要。
struct PackageReport {
    std::string name;
    std::string entry_file;    // UTF-8
    std::string root_dir;      // UTF-8
    int         files_scanned = 0;
    int         parse_ok = 0;
    int         parse_recovered = 0;
    int         parse_failed = 0;
};

struct ReviewReport {
    std::string                behavior_pack_root; // UTF-8
    std::string                discovery_mode;     // "modMain" | "fallback-directory"
    std::vector<PackageReport> packages;
    std::vector<Finding>       findings;
    std::vector<std::string>   ignored_dirs;       // 被跳过的三方/缓存目录（UTF-8）
    std::vector<std::string>   config_warnings;
    int                        files_scanned = 0;
    int                        parse_ok = 0;
    int                        parse_recovered = 0;
    int                        parse_failed = 0;
};

} // namespace mcdk::python_review
