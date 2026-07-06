#pragma once
// Python AI Code Review — 审查入口（见 plans/python-ai-code-review-plan.md）。
//
// 用法：给定行为包根目录（完整 UTF-8 路径），自动发现所有 modMain 包，
// 逐文件解析并运行内置规则，产出 Finding 报告。
//
// 复用约定：路径构造一律走 mcdk::path::from_utf8（common/path_utils.hpp），
// 内部传 std::filesystem::path，只有边界转 UTF-8。tree-sitter 解析在 .cpp 内部。
#include "python_review/review_types.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace mcdk::python_review {

struct ReviewOptions {
    // 选填：具体包/模块清单，缩小范围；空 = 全部包/模块（plan §4.1 scope）。
    // 匹配包名（"KID_ULTRA_X"）或包内模块前缀（"KID_ULTRA_X/Combat"、"KID_ULTRA_X.Combat"）。
    std::vector<std::string> scope;

    // QuModLibs 等静态三方库默认排除（plan §5.5 开放世界；与 minecraft_py 一致）。
    bool include_third_party = false;

    // 输出格式："summary"（极简概览）| "markdown"（默认，人读）| "json"（闭环主格式，plan §8.1）。
    std::string format = "markdown";

    // 选填：显式配置文件路径（UTF-8）。空则在行为包根自动发现 mcdk-review.toml。
    // 配置可调所有阈值、按规则开关、输出密度上限（见 review_config.hpp）。
    std::string config_path;

    // 选填：每规则展示上限的快捷覆盖（免写配置文件）；<0 表示沿用配置值，0 = 不限。
    int max_findings_per_rule = -1;
};

class ReviewAnalyzer {
public:
    ReviewReport review(const std::filesystem::path& behavior_pack_root,
                        const ReviewOptions& options) const;

    // 便捷：review + 渲染。
    std::string review_formatted(const std::filesystem::path& behavior_pack_root,
                                 const ReviewOptions& options) const;
};

std::string render_markdown(const ReviewReport& report); // 按规则分组、每规则截断（省上下文）
std::string render_summary(const ReviewReport& report);  // 极简概览：仅计数/分布/热点文件
std::string render_json(const ReviewReport& report);     // 机读；按规则分组去重 title/建议

} // namespace mcdk::python_review
