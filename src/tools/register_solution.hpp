#pragma once
// register_solution.hpp — 解决方案层在 minecraft_docs 上的接入 helper
//
// 仅当编译期启用 MCDK_WITH_SOLUTIONS（solutions 子模块存在）时才有内容；
// 否则整文件为空，主工程照常编译（向下兼容无子模块权限的克隆者）。
//
// 运行时是否真正提供功能，还取决于 exe 相邻是否存在 mcdk_solutions_cache.bin
// —— 该判定由 main.cpp 加载缓存后以「索引指针是否非空」体现。

#ifdef MCDK_WITH_SOLUTIONS

#include <mcdk_solutions/solution_index.hpp>

#include <mcp_message.h>

#include <sstream>
#include <string>
#include <vector>

namespace mcdk::solution_tool {

using mcdk::solutions::Solution;
using mcdk::solutions::SolutionIndex;

// 从一个搜索结果 JSON 里取出各结果的正文与 file 字段，供触发匹配抽取「结果标识符」。
inline std::vector<std::string> collect_result_texts(const mcp::json& result) {
    std::vector<std::string> texts;
    if (!result.contains("content") || !result["content"].is_array()) return texts;
    for (const auto& item : result["content"]) {
        if (!item.is_object()) continue;
        if (item.value("type", std::string()) == "text")
            texts.push_back(item.value("text", std::string()));
        if (item.contains("file") && item["file"].is_string())
            texts.push_back(item["file"].get<std::string>());
    }
    return texts;
}

// 在搜索结果 JSON 末尾追加「相关解决方案」指针块（若有命中）。
inline void append_pointer_block(mcp::json& result, const SolutionIndex& index,
                                 const std::string& query, const std::string& scope) {
    if (!result.contains("content") || !result["content"].is_array()) return;
    auto result_texts = collect_result_texts(result);
    auto matches = mcdk::solutions::match_solutions(index, query, scope, result_texts, 3);
    if (matches.empty()) return;

    std::ostringstream out;
    out << "相关解决方案（动手写代码前建议先读完整范式，避免盲猜用法）:\n";
    for (const auto& m : matches) {
        const auto& s = index.solutions[m.solution_index];
        out << "  - " << (s.title.empty() ? s.id : s.title)
            << "  →  minecraft_docs(command=\"solution " << s.id << "\")\n";
    }
    out << "说明: 解决方案是经维护、可运行的接口组合范式；先读完再写代码。";
    result["content"].push_back({{"type", "text"}, {"text", out.str()}});
}

// 渲染 `solution <id>` 正文。
inline mcp::json solution_result(const SolutionIndex& index, const std::string& id) {
    auto wrap = [](const std::string& text) {
        return mcp::json{{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
    };

    const Solution* s = mcdk::solutions::find_solution(index, id);
    if (!s) {
        std::ostringstream out;
        out << "未找到解决方案: '" << id << "'。\n可用解决方案:\n";
        for (const auto& sol : index.solutions)
            out << "  - " << sol.id << "  (" << sol.title << ")\n";
        if (index.solutions.empty())
            out << "  (当前没有已编译的解决方案)\n";
        return wrap(out.str());
    }

    std::ostringstream out;
    out << "# 解决方案: " << s->title << "\n";
    out << "id: " << s->id
        << "  |  level: " << (s->level.empty() ? "api" : s->level)
        << "  |  status: " << (s->status.empty() ? "draft" : s->status);
    if (!s->applies.edition.empty()) out << "  |  edition: " << s->applies.edition;
    if (!s->applies.version.empty()) out << "  |  version: " << s->applies.version;
    out << "\n";
    if (!s->summary.empty()) out << s->summary << "\n";

    if (!s->requires_deps.empty()) {
        out << "\n前置解决方案（requires，建议先掌握）:\n";
        for (const auto& r : s->requires_deps)
            out << "  - " << r << "  →  minecraft_docs(command=\"solution " << r << "\")\n";
    }
    if (!s->related.empty()) {
        out << "相关: ";
        for (std::size_t i = 0; i < s->related.size(); ++i) {
            if (i) out << ", ";
            out << s->related[i];
        }
        out << "\n";
    }

    out << "\n";
    if (s->entry_body.empty())
        out << "(该解决方案正文为空，或未随缓存打包)";
    else
        out << s->entry_body;

    if (s->status == "draft")
        out << "\n\n注意: 本解决方案标记为 draft（可能未完全实测）；套用时请结合实际验证。";

    return wrap(out.str());
}

}  // namespace mcdk::solution_tool

#endif  // MCDK_WITH_SOLUTIONS
