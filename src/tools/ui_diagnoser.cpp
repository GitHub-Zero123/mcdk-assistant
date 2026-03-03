#include "tools/ui_diagnoser.h"
#include <regex>
#include <sstream>
#include <set>
#include <nlohmann/json.hpp>

namespace mcdk {

using json = nlohmann::json;

// ── 结构化检查：递归遍历 controls 数组，每层独立检查 key 重复 ──
static void check_controls_keys(const json& controls_arr, const std::string& parent_path,
                                 std::vector<UiDiagnosis>& results) {
    if (!controls_arr.is_array()) return;

    std::set<std::string> keys_in_this_level;
    for (const auto& item : controls_arr) {
        if (!item.is_object()) continue;
        for (auto it = item.begin(); it != item.end(); ++it) {
            // 提取 key（去掉 @继承 部分）
            std::string full_key = it.key();
            std::string key = full_key;
            auto at_pos = key.find('@');
            if (at_pos != std::string::npos) key = key.substr(0, at_pos);

            if (!keys_in_this_level.insert(key).second) {
                results.push_back({0, "warning",
                    "controls 中控件 key \"" + key + "\" 重复（位于 " + parent_path + "），可能导致覆盖"});
            }

            // 递归检查子控件的 controls
            if (it.value().is_object() && it.value().contains("controls")) {
                check_controls_keys(it.value()["controls"],
                                    parent_path + "/" + key, results);
            }
        }
    }
}

// ── 结构化检查：从解析后的 JSON 进行 ──
static void check_structured(const json& root, std::vector<UiDiagnosis>& results) {
    if (!root.is_object()) return;

    // 检查 namespace
    if (!root.contains("namespace")) {
        results.push_back({0, "error", "缺少 \"namespace\" 定义，JSON UI 文件必须声明命名空间"});
    }

    // 遍历所有顶层控件定义
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key() == "namespace") continue;
        if (!it.value().is_object()) continue;

        std::string ctrl_name = it.key();
        auto at_pos = ctrl_name.find('@');
        if (at_pos != std::string::npos) ctrl_name = ctrl_name.substr(0, at_pos);

        // 检查该控件的 controls 子数组
        if (it.value().contains("controls")) {
            check_controls_keys(it.value()["controls"], ctrl_name, results);
        }
    }
}

// ── 基于正则的行级检查（不依赖 JSON 解析成功） ──
static void check_regex(const std::string& content, std::vector<UiDiagnosis>& results) {
    std::istringstream iss(content);
    std::string line;
    int lineno = 0;

    static const std::regex re_size_bad_space(R"("size"\s*:\s*\[.*\d+\s*%\s+[+\-]\s*\d+)");
    static const std::regex re_binding_name(R"re("binding_name"\s*:\s*"([^"]*)")re");

    while (std::getline(iss, line)) {
        ++lineno;

        // 跳过注释行
        auto pos = line.find_first_not_of(" \t");
        if (pos != std::string::npos && line.substr(pos, 2) == "//") continue;

        // 检查 size 格式错误："100% + 0px" 应为 "100%+0px"
        if (std::regex_search(line, re_size_bad_space)) {
            results.push_back({lineno, "warning", "size 值中 % 与 +/- 之间不应有空格，正确格式如 \"100%+0px\""});
        }

        // 检查 binding_name 是否以 # 开头
        std::smatch m;
        if (std::regex_search(line, m, re_binding_name)) {
            std::string bname = m[1].str();
            if (!bname.empty() && bname[0] != '#' && bname[0] != '$') {
                results.push_back({lineno, "error", "binding_name \"" + bname + "\" 应以 # 开头（数据绑定名）或 $ 开头（变量）"});
            }
        }
    }
}

std::vector<UiDiagnosis> diagnose_ui(const std::string& content) {
    std::vector<UiDiagnosis> results;

    // 始终执行行级正则检查（size 格式、binding_name 格式）
    check_regex(content, results);

    // 尝试 JSON 解析，成功则进行结构化检查（controls key 重复、namespace 缺失）
    try {
        json root = json::parse(content, nullptr, true, true); // ignore_comments=true
        check_structured(root, results);
    } catch (const json::parse_error&) {
        // JSON 解析失败，回退到基于正则的 namespace 检查
        if (content.find("\"namespace\"") == std::string::npos) {
            results.push_back({0, "error", "缺少 \"namespace\" 定义，JSON UI 文件必须声明命名空间"});
        }
        // key 重复检查跳过（无法可靠地用正则判断层级）
    }

    return results;
}

} // namespace mcdk
