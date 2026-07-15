#pragma once

#include "common/path_utils.hpp"
#include "project_analysis/project_analyzer.hpp"
#include "tools/command_parser.hpp"
#include "tools/register_python_review.hpp" // review 子命令（合并进 minecraft_py 单工具）

#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <string>

namespace mcdk {

inline project_analysis::AnalysisOptions read_py_analysis_options(const mcp::json& params) {
    project_analysis::AnalysisOptions options;
    options.depth = params.contains("depth") && params["depth"].is_number()
        ? params["depth"].get<int>() : 2;
    options.depth = std::max(0, std::min(options.depth, 8));
    options.include_symbols = params.value("include_symbols", true);
    options.include_unresolved = params.value("include_unresolved", true);
    options.include_indirect = params.value("include_indirect", true);
    options.include_call_hints = params.value("include_call_hints", true);
    options.ignore_third_party_analysis = params.value("ignore_third_party_analysis", true);
    options.max_scope_upward_levels = params.contains("max_scope_upward_levels") && params["max_scope_upward_levels"].is_number()
        ? params["max_scope_upward_levels"].get<int>() : 12;
    options.max_scope_upward_levels = std::max(1, std::min(options.max_scope_upward_levels, 32));
    return options;
}

namespace minecraft_py_detail {

constexpr const char* kToolName = "minecraft_py";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

inline std::string py_help_text() {
    return R"(minecraft_py — Python2 Addon/MOD 项目分析工具统一入口（命令式用法）
用法: minecraft_py(command="<子命令> [路径] [--选项 值]")
命令名前可加 '/'，例如 /help、/arch，与 help/arch 等价。
重要: 所有路径均须为完整绝对路径（UTF-8，支持中文）；不支持相对路径/工作目录推断。
重要: 路径或参数含空格时，必须用 "..." 或 '...' 包裹成整体。
重要: Windows 路径建议写成 D:/path/to/file；反斜杠 \ 会按命令行转义处理。

【子命令】
  help
      显示本帮助。

  arch <behavior_pack_path> [--depth <0-8>] [--include-symbols true|false]
       [--include-unresolved true|false] [--ignore-third-party true|false]
      生成 Python2 行为包全局架构总览。
      会自动识别 modMain.py 对应的 mod 包，并输出入口模块、模块分层、
      核心依赖链、关键子系统和模糊风险点。

  imports <target_path> [--depth <0-8>] [--include-indirect true|false]
          [--include-unresolved true|false] [--include-call-hints true|false]
          [--ignore-third-party true|false] [--max-scope-upward-levels <n>]
      对具体 Python 文件或目录做引用链分析。
      可用于定位注册点、入口点、某个功能为什么会生效，以及目录职责。

)" + python_review_cmd::review_help_text() + R"(

【使用边界】
  - 这是低优先级的项目结构辅助工具，不是通用 Python 代码阅读或符号搜索入口。
  - arch/imports：不要仅因任务涉及 .py 文件就调用；已知具体文件、函数或报错位置时，优先直接读取/搜索相关源码。
  - 只有在需要跨文件追踪注册点、入口链、目录职责或未知行为包整体结构时，再按需调用 arch/imports。
  - 不要默认全局扫描；整体结构不明时才用 arch，且优先控制 depth，必要时可加 --include-symbols false 降低噪音。
  - review：AI 写完/改完一段 Python MOD 代码后自查用；配合 --scope 只审查刚改动的包/模块，按报告回改。
  - 分析业务逻辑时，ignore-third-party 默认开启，用于减少 QuModLibs 等框架噪音。

示例:
  arch "D:/mc/addons/MyAddon/behavior_pack" --depth 3
  imports "D:/mc/addons/MyAddon/behavior_pack/my_mod/client" --depth 2
  review "D:/mc/addons/MyAddon/behavior_pack" --scope my_mod/client --format json)";
}

inline mcp::json help_result() {
    return text_result(py_help_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + py_help_text());
}

inline std::string flag_any(const ParsedCommand& pc,
                            const std::initializer_list<const char*>& keys,
                            const std::string& def = "") {
    for (const char* key : keys) {
        auto it = pc.flags.find(key);
        if (it != pc.flags.end()) return it->second;
    }
    return def;
}

inline bool has_flag_any(const ParsedCommand& pc,
                         const std::initializer_list<const char*>& keys) {
    for (const char* key : keys) {
        if (has_flag(pc, key)) return true;
    }
    return false;
}

inline int flag_int_any(const ParsedCommand& pc,
                        const std::initializer_list<const char*>& keys,
                        int def) {
    for (const char* key : keys) {
        if (has_flag(pc, key)) return flag_int(pc, key, def);
    }
    return def;
}

inline bool flag_bool_any(const ParsedCommand& pc,
                          const std::initializer_list<const char*>& keys,
                          bool def) {
    for (const char* key : keys) {
        auto it = pc.flags.find(key);
        if (it == pc.flags.end()) continue;
        if (it->second.empty()) return true;

        std::string value = command_parser_detail::to_lower_ascii(it->second);
        if (value == "1" || value == "true" || value == "yes" || value == "on")
            return true;
        if (value == "0" || value == "false" || value == "no" || value == "off")
            return false;
        return def;
    }
    return def;
}

inline void put_common_options(mcp::json& params, const ParsedCommand& pc) {
    if (has_flag(pc, "depth")) params["depth"] = flag_int(pc, "depth", 2);

    if (has_flag_any(pc, {"include-symbols", "include_symbols"}))
        params["include_symbols"] = flag_bool_any(pc, {"include-symbols", "include_symbols"}, true);
    if (has_flag_any(pc, {"include-unresolved", "include_unresolved"}))
        params["include_unresolved"] = flag_bool_any(pc, {"include-unresolved", "include_unresolved"}, true);
    if (has_flag_any(pc, {"ignore-third-party", "ignore_third_party", "ignore-third-party-analysis", "ignore_third_party_analysis"}))
        params["ignore_third_party_analysis"] = flag_bool_any(pc, {"ignore-third-party", "ignore_third_party", "ignore-third-party-analysis", "ignore_third_party_analysis"}, true);
}

inline mcp::json dispatch_architecture(project_analysis::ProjectAnalyzer& analyzer,
                                       const ParsedCommand& pc) {
    std::string path_text = flag_any(pc, {"behavior-pack-path", "behavior_pack_path", "behavior-pack", "behavior_pack", "bp", "path"});
    if (path_text.empty()) path_text = pc.positional;
    if (path_text.empty()) return error_with_help("缺少行为包根目录路径。");

    mcp::json params = {{"behavior_pack_path", path_text}};
    put_common_options(params, pc);

    auto report = analyzer.analyze_behavior_pack(
        mcdk::path::from_utf8(path_text),
        read_py_analysis_options(params));
    return text_result(report);
}

inline mcp::json dispatch_imports(project_analysis::ProjectAnalyzer& analyzer,
                                  const ParsedCommand& pc) {
    std::string path_text = flag_any(pc, {"target-path", "target_path", "target", "path"});
    if (path_text.empty()) path_text = pc.positional;
    if (path_text.empty()) return error_with_help("缺少目标 Python 文件或目录路径。");

    mcp::json params = {{"target_path", path_text}};
    put_common_options(params, pc);
    if (has_flag_any(pc, {"include-indirect", "include_indirect"}))
        params["include_indirect"] = flag_bool_any(pc, {"include-indirect", "include_indirect"}, true);
    if (has_flag_any(pc, {"include-call-hints", "include_call_hints"}))
        params["include_call_hints"] = flag_bool_any(pc, {"include-call-hints", "include_call_hints"}, true);
    if (has_flag_any(pc, {"max-scope-upward-levels", "max_scope_upward_levels"}))
        params["max_scope_upward_levels"] = flag_int_any(pc, {"max-scope-upward-levels", "max_scope_upward_levels"}, 12);

    auto report = analyzer.analyze_target_references(
        mcdk::path::from_utf8(path_text),
        read_py_analysis_options(params));
    return text_result(report);
}

inline mcp::json dispatch_minecraft_py(project_analysis::ProjectAnalyzer& analyzer,
                                       const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = command_parser_detail::to_lower_ascii(pc.sub);

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "arch" || sub == "architecture" || sub == "scan" || sub == "overview")
        return dispatch_architecture(analyzer, pc);
    if (sub == "imports" || sub == "import" || sub == "import-chain" || sub == "refs" || sub == "references")
        return dispatch_imports(analyzer, pc);
    if (sub == "review" || sub == "check" || sub == "audit") {
        bool ok = false;
        return text_result(python_review_cmd::run_review(pc, ok));
    }

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_py_detail

inline void register_python_analysis_tools(mcp::server& srv) {
    auto analyzer = std::make_shared<project_analysis::ProjectAnalyzer>();

    auto tool = mcp::tool_builder(minecraft_py_detail::kToolName)
        .with_description(
            "Python MOD 辅助工具（命令式，单工具多子命令）：\n"
            "1) 理解 Minecraft Python2 Addon/MOD 的跨文件入口、注册链、目录职责或未知行为包结构"
            "（arch、imports）；\n"
            "2) 对 Python MOD 代码做结构性审查，供 AI 写完/改完代码后自查、按报告回改"
            "（review：掩盖异常的 try、被改并累积的可变默认参数、global 重绑定、未实现桩、"
            "假实现、裸 unicode 默认编码、平台受限模块、动态执行及反射绕过、单函数/单文件过大、"
            "跨模块重复函数、参数过多等；"
            "QuModLibs 与游戏 API 不误报；"
            "阈值/规则可用 mcdk-review.toml 配置；--format summary 与 --max-per-rule 控召回体积）。\n"
            "不要仅因涉及 .py 文件就调用 arch/imports；已知目标位置时优先直接读取/搜索源码。"
            "可传 command=\"help\" 查看全部子命令与边界。")
        .with_string_param("command",
            "命令语句，如 'arch <behavior_pack_path>'、'imports <target_path> --depth 2'、"
            "或 'review <behavior_pack_path> --scope pkg/module --format json'；"
            "按需使用，避免默认全局扫描；路径须为完整绝对路径（UTF-8，支持中文），不支持相对路径；"
            "路径含空格时用引号包裹；Windows 路径建议使用 /，不要直接写反斜杠。", true)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .with_open_world_hint(true)
        .build();

    srv.register_tool(
        tool,
        [analyzer](const mcp::json& params, const std::string&) -> mcp::json {
            return minecraft_py_detail::dispatch_minecraft_py(
                *analyzer,
                params.value("command", ""));
        });
}

} // namespace mcdk
