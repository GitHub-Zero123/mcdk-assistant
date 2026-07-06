#pragma once
// python_review 不是独立 MCP 工具，而是 minecraft_py 单工具下的 `review` 子命令
// （项目方向：所有 tool 合并为单工具+命令式以省 token）。本头只提供子命令处理逻辑，
// 由 register_python_analysis.hpp 的 minecraft_py 分发器调用。独立 CLI 见 tools/mcdk-python-review。
#include "common/path_utils.hpp"
#include "python_review/review_analyzer.hpp"
#include "python_review/review_config.hpp"
#include "tools/command_parser.hpp"

#include <string>
#include <vector>

namespace mcdk {
namespace python_review_cmd {

inline std::string review_help_text() {
    return R"(review <behavior_pack_path> [--scope <a,b/c>] [--format summary|markdown|json]
       [--include-third-party] [--config <toml>] [--max-per-rule <n>]
review --dump-config    打印可编辑的默认配置模板（TOML，兼作阈值/开关说明文档）
    对 Python MOD 行为包做结构性代码审查（面向 AI 写完/改完代码后自查、按报告回改）。
    规则：掩盖异常的 try、可变默认参数被累积、global 重绑定、未实现桩、假实现（有多行逻辑却
    不用任何参数、只返回固定值）、单函数过长、单文件屎山、跨模块重复函数、参数过多、无 owner TODO。
    QuModLibs 与游戏 API 不误报（开放世界）。
    --scope: 缩小到指定包/模块（逗号分隔），不填=全部；如 --scope KID_ULTRA_X/Combat
    --format: summary（极简概览，最省上下文）| markdown（默认，人读，按规则分组）
             | json（闭环机读，按规则分组去重）
    --include-third-party: 连 QuModLibs 等三方库一并审查（默认排除）
    --config: 显式 TOML 配置；不填则自动发现行为包根下 mcdk-review.toml（可调所有阈值/规则开关）
    --max-per-rule: 每规则最多列 N 处定位（0=不限），控 MCP 召回体积、避免撑爆上下文)";
}

inline std::vector<std::string> split_csv(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == ',') { if (!cur.empty()) out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

// 处理 `review ...` 子命令，返回报告文本（或错误提示 + 用法）。ok 输出是否成功。
inline std::string run_review(const ParsedCommand& pc, bool& ok) {
    ok = false;

    // --dump-config：打印默认配置模板（兼作说明文档），无需路径。
    if (has_flag(pc, "dump-config") || has_flag(pc, "dump_config")) {
        ok = true;
        return python_review::default_toml_template();
    }

    std::string path_text = flag_str(pc, "path");
    if (path_text.empty()) path_text = pc.positional;
    if (path_text.empty())
        return "缺少行为包根目录路径。\n\n" + review_help_text();

    python_review::ReviewOptions options;
    if (has_flag(pc, "scope")) options.scope = split_csv(flag_str(pc, "scope"));
    options.format = flag_str(pc, "format", "markdown");
    if (options.format != "markdown" && options.format != "json" && options.format != "summary")
        return "未知 --format: " + options.format + "（应为 summary|markdown|json）\n\n" + review_help_text();
    options.include_third_party =
        has_flag(pc, "include-third-party") || has_flag(pc, "include_third_party");
    if (has_flag(pc, "config")) options.config_path = flag_str(pc, "config");
    if (has_flag(pc, "max-per-rule"))
        options.max_findings_per_rule = flag_int(pc, "max-per-rule", -1);
    else if (has_flag(pc, "max_per_rule"))
        options.max_findings_per_rule = flag_int(pc, "max_per_rule", -1);

    python_review::ReviewAnalyzer analyzer;
    ok = true;
    return analyzer.review_formatted(mcdk::path::from_utf8(path_text), options);
}

} // namespace python_review_cmd
} // namespace mcdk
