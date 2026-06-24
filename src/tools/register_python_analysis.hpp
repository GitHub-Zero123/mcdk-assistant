#pragma once

#include "common/path_utils.hpp"
#include "project_analysis/project_analyzer.hpp"

#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <algorithm>
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

inline void register_python_analysis_tools(mcp::server& srv) {
    auto analyzer = std::make_shared<project_analysis::ProjectAnalyzer>();

    auto full_tool = mcp::tool_builder("scan_py2_mod_architecture")
        .with_description(
            "适合在需要理解未知 Python2 Addon（MOD）项目整体结构时调用，用于生成全局总览。"
            "扫描指定行为包，自动识别其下所有 modMain.py 对应的 mod 包，并生成适合 AI 阅读的架构说明。"
            "输出重点包括：入口模块、模块分层、核心依赖链、关键子系统、模糊风险点。"
            "可先用本工具确认项目整体结构，再按需要对重点目录或文件使用 scan_py2_import_chain 深挖；也可直接对已知目标使用 scan_py2_import_chain。"
            "若项目包含 QuModLibs 且目标是理解业务逻辑，通常可保持 ignore_third_party_analysis=true，以减少框架噪音。")
        .with_string_param("behavior_pack_path", "行为包根目录绝对路径", true)
        .with_number_param("depth", "分析深度（0-8，越高越展开间接依赖与模块摘要）", false)
        .with_boolean_param("include_symbols", "是否输出类/函数等符号摘要", false)
        .with_boolean_param("include_unresolved", "是否展示未解析外部引用", false)
        .with_boolean_param("ignore_third_party_analysis", "是否忽略 QuModLibs 深度分析，仅保留其调用摘要。理解业务逻辑时通常可保持 true", false)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .with_open_world_hint(true)
        .build();

    srv.register_tool(
        full_tool,
        [analyzer](const mcp::json& params, const std::string&) -> mcp::json {
            auto path_text = params.value("behavior_pack_path", "");
            if (path_text.empty()) {
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "behavior_pack_path 不能为空");
            }
            auto report = analyzer->analyze_behavior_pack(
                mcdk::path::from_utf8(path_text),
                read_py_analysis_options(params));
            return {{"content", mcp::json::array({{{"type", "text"}, {"text", report}}})}};
        });

    auto ref_tool = mcp::tool_builder("scan_py2_import_chain")
        .with_description(
            "适合对具体子系统、目录或文件做定点分析；可以在已有全局总览后使用，也可以直接对明确目标使用。"
            "分析指定 Python 文件或目录在其所属项目中的引用关系，自动向上寻找 modMain.py 与 manifest.json 确定最优作用域，并避免无限上溯。"
            "输出重点包括：谁直接引用它、谁间接依赖它、它依赖了哪些内部模块、它在整体架构中的位置。"
            "适合定位注册点、入口点、某个功能为什么会生效，以及某个目录在项目中的职责。")
        .with_string_param("target_path", "目标 Python 文件或文件夹绝对路径", true)
        .with_number_param("depth", "间接引用深度（0-8）", false)
        .with_boolean_param("include_indirect", "是否计算项目内间接引用", false)
        .with_boolean_param("include_unresolved", "是否展示未解析外部引用", false)
        .with_boolean_param("include_call_hints", "是否保留模糊调用说明字段", false)
        .with_boolean_param("ignore_third_party_analysis", "是否忽略 QuModLibs 深度分析，仅保留其调用摘要。分析业务子系统时通常可保持 true", false)
        .with_number_param("max_scope_upward_levels", "向上解析最大层数，默认 12", false)
        .with_read_only_hint(true)
        .with_idempotent_hint(true)
        .with_open_world_hint(true)
        .build();

    srv.register_tool(
        ref_tool,
        [analyzer](const mcp::json& params, const std::string&) -> mcp::json {
            auto path_text = params.value("target_path", "");
            if (path_text.empty()) {
                throw mcp::mcp_exception(mcp::error_code::invalid_params, "target_path 不能为空");
            }
            auto report = analyzer->analyze_target_references(
                mcdk::path::from_utf8(path_text),
                read_py_analysis_options(params));
            return {{"content", mcp::json::array({{{"type", "text"}, {"text", report}}})}};
        });
}

} // namespace mcdk
