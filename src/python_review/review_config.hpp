#pragma once
// Python AI Code Review — 可调配置（阈值 / 规则开关 / 输出密度）。
//
// 本头只依赖标准库（纯结构 + 默认值），任何 TU 都可安全包含而不引入 toml++。
// 实际的 TOML 解析/自动发现在 review_config.cpp（那里才 include toml++）。
//
// 配置来源优先级（resolve_config）：
//   1) 显式 --config <path>（CLI / MCP 均可）
//   2) 行为包根下的 mcdk-review.toml 或 .mcdk-review.toml（自动发现）
//   3) 内置默认值（下方字段初值）
#include <filesystem>
#include <string>
#include <vector>

namespace mcdk::python_review {

struct ReviewConfig {
    // ── 阈值（[thresholds]）───────────────────────────────────────────────
    int file_code_lines = 1000; // 单文件「代码行」屎山阈值（与 file_units 同时满足才报）
    int file_units       = 25; // 单文件 def+class 数阈值
    int func_loc         = 50; // 单函数行跨度：偏长起报
    int func_loc_high    = 100;// 单函数行跨度：过长（warning）
    int try_small        = 5;  // try 体 <= 此值：窄防御，低级（hint）
    int try_large        = 20; // try 体 >= 此值：吞掉大量业务，高级（warning）
    int dup_min_stmts    = 5;  // 跨模块重复：单函数最少语句数（过滤 trivial）
    int dup_min_fp_len   = 10; // 跨模块重复：指纹最短长度
    int dup_min_count    = 2;  // 跨模块重复：最少重复处数
    int shallow_min_body = 3;  // 假实现：函数体至少 N 条业务语句才判（排除 1 行补全库桩）
    int max_params       = 5;  // 参数过多：非 self 参数 > 此值触发

    // ── 规则开关（[rules]）：false 可整族禁用 ──────────────────────────────
    bool rule_try_masking       = true; // try.masking.bare-except / broad-except-swallow
    bool rule_mutable_default   = true; // implicit-global.mutable-default
    bool rule_global_write      = true; // implicit-global.global-write
    bool rule_stub_placeholder  = true; // stub.placeholder（NotImplementedError / ...）
    bool rule_shallow_impl      = true; // stub.shallow-impl（假实现）
    bool rule_large_function    = true; // logic-blob.large-function
    bool rule_large_file        = true; // logic-blob.large-file
    bool rule_duplicate_function= true; // duplicate-function.cross-module
    bool rule_unowned_todo      = true; // comment-doc.unowned-todo
    bool rule_too_many_params   = true; // signature.too-many-params
    bool rule_encoding_declaration = true; // encoding.missing-utf8-declaration（Py2 PEP263）
    bool rule_unicode_default_encoding = true; // encoding.unicode-default-encoding（ModSDK 魔改 Py2）
    bool rule_restricted_module_import = true; // platform.restricted-module-import
    bool rule_dynamic_code_execution = true; // platform.dynamic-code-execution
    bool rule_reflective_security_bypass = true; // platform.reflective-security-bypass

    // ── 输出密度（[output]）：控 MCP 召回体积，避免撑爆上下文 ───────────────
    int max_findings_per_rule = 20; // 每条规则最多展示 N 处定位，超出仅计数；0 = 不限

    // ── 进阶（[advanced]）─────────────────────────────────────────────────
    // 「总由调用方填充」的参数名白名单：这些可变默认参数即便逃逸也豁免（使用方显式声明，
    // 非代码内置枚举）。可变默认参数规则本身以「是否逃逸出函数」判定，一般无需配置。
    std::vector<std::string> extra_caller_supplied_params;
};

// 解析配置：显式路径优先，否则在 bp_root 下自动发现，否则用默认值。
// warnings 追加加载失败/回退提示；source_desc 输出实际来源描述（用于报告展示）。
ReviewConfig resolve_config(const std::filesystem::path& behavior_pack_root,
                            const std::string& explicit_path,
                            std::vector<std::string>& warnings,
                            std::string& source_desc);

// 带注释的默认 TOML 模板（供 `review --dump-config` 打印，兼作配置说明文档）。
std::string default_toml_template();

} // namespace mcdk::python_review
