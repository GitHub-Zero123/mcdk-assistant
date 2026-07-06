#pragma once
// python_review 不是独立 MCP 工具，而是 minecraft_py 单工具下的 `review` 子命令
// （项目方向：所有 tool 合并为单工具+命令式以省 token）。本头只提供子命令处理逻辑，
// 由 register_python_analysis.hpp 的 minecraft_py 分发器调用。独立 CLI 见 tools/mcdk-python-review。
#include "common/path_utils.hpp"
#include "python_review/review_analyzer.hpp"
#include "tools/command_parser.hpp"

#include <string>
#include <vector>

namespace mcdk {
namespace python_review_cmd {

inline std::string review_help_text() {
    return R"(review <behavior_pack_path> [--scope <a,b/c>] [--format markdown|json] [--include-third-party]
    对 Python MOD 行为包做结构性代码审查（面向 AI 写完/改完代码后自查、按报告回改）。
    默认只报确定性问题：掩盖异常的 try、被修改并累积的可变默认参数、global 重绑定、
    未实现桩、无 owner 的 TODO。QuModLibs 与游戏 API 不误报（开放世界）。
    --scope: 缩小到指定包/模块（逗号分隔），不填=全部；如 --scope KID_ULTRA_X/Combat
    --format: markdown（默认，人读）| json（闭环主格式，含 finding_key/tier/actionability）
    --include-third-party: 连 QuModLibs 等三方库一并审查（默认排除）)";
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
    std::string path_text = flag_str(pc, "path");
    if (path_text.empty()) path_text = pc.positional;
    if (path_text.empty())
        return "缺少行为包根目录路径。\n\n" + review_help_text();

    python_review::ReviewOptions options;
    if (has_flag(pc, "scope")) options.scope = split_csv(flag_str(pc, "scope"));
    options.format = flag_str(pc, "format", "markdown");
    if (options.format != "markdown" && options.format != "json")
        return "未知 --format: " + options.format + "（应为 markdown|json）\n\n" + review_help_text();
    options.include_third_party =
        has_flag(pc, "include-third-party") || has_flag(pc, "include_third_party");

    python_review::ReviewAnalyzer analyzer;
    ok = true;
    return analyzer.review_formatted(mcdk::path::from_utf8(path_text), options);
}

} // namespace python_review_cmd
} // namespace mcdk
