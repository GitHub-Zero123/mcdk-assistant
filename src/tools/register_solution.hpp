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
    // 略放宽候选数，给「解决方案引用」与「提示/踩坑」各留位；仍受库内相关度闸门约束。
    auto matches = mcdk::solutions::match_solutions(index, query, scope, result_texts, 6);
    if (matches.empty()) return;

    // 按 kind 分桶：普通方案=只给引用+跳转命令；tip（踩坑/小技巧）=summary 直接内联、不跳转。
    std::vector<const Solution*> sols, tips;
    for (const auto& m : matches) {
        const Solution& s = index.solutions[m.solution_index];
        if (mcdk::solutions::is_tip_kind(s.kind)) tips.push_back(&s);
        else sols.push_back(&s);
    }
    if (sols.size() > 3) sols.resize(3);   // 引用最多 3 条
    if (tips.size() > 2) tips.resize(2);   // 内联提示最多 2 条
    if (sols.empty() && tips.empty()) return;

    std::ostringstream out;
    if (!sols.empty()) {
        out << "相关解决方案（仅引用；按相关度排序，挑与当前任务最贴合的一篇用下面命令读全文，勿逐条跳读）:\n";
        for (const Solution* s : sols) {
            out << "  - " << (s->title.empty() ? s->id : s->title) << "\n";
            if (!s->summary.empty())
                out << "      简介: " << s->summary << "\n";
            out << "      读全文: minecraft_docs(command=\"solution " << s->id << "\")\n";
        }
    }
    if (!tips.empty()) {
        if (!sols.empty()) out << "\n";
        out << "提示 / 踩坑（可直接采纳，无需跳转读全文）:\n";
        for (const Solution* s : tips) {
            out << "  - " << (s->title.empty() ? s->id : s->title);
            if (!s->summary.empty()) out << "：" << s->summary;
            out << "\n";
        }
    }
    if (!sols.empty())
        out << "说明: 解决方案栏仅为引用（标题+简介）；正文经维护、可运行，先读最贴合的一篇再动手，避免盲猜用法。";
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
    if (s->entry_body.empty()) {
        if (mcdk::solutions::is_tip_kind(s->kind))
            out << "（提示/踩坑类：内容即上方简介，无独立正文，触发时会直接内联）";
        else
            out << "(该解决方案正文为空，或未随缓存打包)";
    } else {
        out << s->entry_body;
    }

    if (s->status == "draft")
        out << "\n\n注意: 本解决方案标记为 draft（可能未完全实测）；套用时请结合实际验证。";

    return wrap(out.str());
}

}  // namespace mcdk::solution_tool

#endif  // MCDK_WITH_SOLUTIONS
