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

    // 输出格式："markdown"（人读）| "json"（闭环主格式，plan §8.1）。
    std::string format = "markdown";
};

class ReviewAnalyzer {
public:
    ReviewReport review(const std::filesystem::path& behavior_pack_root,
                        const ReviewOptions& options) const;

    // 便捷：review + 渲染。
    std::string review_formatted(const std::filesystem::path& behavior_pack_root,
                                 const ReviewOptions& options) const;
};

std::string render_markdown(const ReviewReport& report);
std::string render_json(const ReviewReport& report);

} // namespace mcdk::python_review
