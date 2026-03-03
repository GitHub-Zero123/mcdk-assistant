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

// 有效的控件类型列表
static const std::set<std::string> VALID_TYPES = {
    "panel", "stack_panel", "grid", "label", "image", "button",
    "toggle", "slider", "slider_box", "dropdown", "edit_box",
    "scroll_view", "scrollbar_box", "scrollbar_track",
    "input_panel", "screen", "custom", "tab",
    "selection_wheel", "factory", "scroll_track"
};

// ── 递归检查控件属性 ──
static void check_ctrl_props(const json& node, const std::string& path,
                              const json& root, std::vector<UiDiagnosis>& results) {
    if (!node.is_object()) return;

    // 检查 type 有效性
    if (node.contains("type") && node["type"].is_string()) {
        std::string t = node["type"].get<std::string>();
        if (VALID_TYPES.find(t) == VALID_TYPES.end()) {
            results.push_back({0, "warning",
                "控件 \"" + path + "\" 的 type \"" + t + "\" 不是已知类型"});
        }
    }

    // 检查 size 格式（应为 [x, y] 且每项为字符串或数字）
    if (node.contains("size") && node["size"].is_array()) {
        const auto& sz = node["size"];
        if (sz.size() != 2) {
            results.push_back({0, "warning",
                "控件 \"" + path + "\" 的 size 数组应恰好 2 个元素，当前 " +
                std::to_string(sz.size()) + " 个"});
        }
        // 检查大固定像素值（>100 的纯数字建议使用百分比）
        for (size_t i = 0; i < sz.size() && i < 2; ++i) {
            if (sz[i].is_number()) {
                double val = sz[i].get<double>();
                if (val > 100.0) {
                    results.push_back({0, "suggestion",
                        "控件 \"" + path + "\" 的 size[" + std::to_string(i) +
                        "] 使用固定像素 " + std::to_string((int)val) +
                        "，建议使用百分比以适配不同分辨率（如 \"50%+0px\"）"});
                }
            }
        }
    }

    // 检查 anchor_from/anchor_to 应成对出现
    bool has_from = node.contains("anchor_from");
    bool has_to = node.contains("anchor_to");
    if (has_from != has_to) {
        results.push_back({0, "warning",
            "控件 \"" + path + "\" 的 anchor_from 和 anchor_to 应成对设置"});
    }

    // 检查 button 类型控件的子控件图层遮挡问题
    std::string type_str;
    if (node.contains("type") && node["type"].is_string())
        type_str = node["type"].get<std::string>();
    if (type_str == "button" && node.contains("controls") && node["controls"].is_array()) {
        int max_image_layer = -1;
        int min_label_layer = 9999;
        std::string label_name, image_name;
        for (const auto& item : node["controls"]) {
            if (!item.is_object()) continue;
            for (auto ct = item.begin(); ct != item.end(); ++ct) {
                if (!ct.value().is_object()) continue;
                std::string child_type;
                if (ct.value().contains("type") && ct.value()["type"].is_string())
                    child_type = ct.value()["type"].get<std::string>();
                int child_layer = ct.value().value("layer", 0);
                if (child_type == "image" && child_layer > max_image_layer) {
                    max_image_layer = child_layer;
                    image_name = ct.key();
                }
                if (child_type == "label" && child_layer < min_label_layer) {
                    min_label_layer = child_layer;
                    label_name = ct.key();
                }
            }
        }
        if (max_image_layer >= 0 && min_label_layer < 9999 &&
            min_label_layer <= max_image_layer) {
            results.push_back({0, "warning",
                "控件 \"" + path + "\" 的 label 子控件 \"" + label_name +
                "\"(L:" + std::to_string(min_label_layer) + ") 图层 ≤ image 子控件 \"" +
                image_name + "\"(L:" + std::to_string(max_image_layer) +
                ")，文字可能被图片遮挡。label 的 layer 应大于 image"});
        }
    }

    // 检查 stack_panel 子控件不应使用 anchor/offset
    if (type_str == "stack_panel" && node.contains("controls") && node["controls"].is_array()) {
        for (const auto& item : node["controls"]) {
            if (!item.is_object()) continue;
            for (auto ct = item.begin(); ct != item.end(); ++ct) {
                if (!ct.value().is_object()) continue;
                std::string cname = ct.key();
                auto ap = cname.find('@');
                if (ap != std::string::npos) cname = cname.substr(0, ap);
                if (ct.value().contains("anchor_from") || ct.value().contains("offset")) {
                    results.push_back({0, "suggestion",
                        "stack_panel \"" + path + "\" 的子控件 \"" + cname +
                        "\" 设置了 anchor/offset，在栈布局中不生效"});
                }
            }
        }
    }

    // 递归子控件
    if (node.contains("controls") && node["controls"].is_array()) {
        for (const auto& item : node["controls"]) {
            if (!item.is_object()) continue;
            for (auto ct = item.begin(); ct != item.end(); ++ct) {
                std::string child_name = ct.key();
                auto at_pos = child_name.find('@');
                if (at_pos != std::string::npos) child_name = child_name.substr(0, at_pos);
                check_ctrl_props(ct.value(), path + "/" + child_name, root, results);
            }
        }
    }
}

// ── 收集所有顶层 key（去掉 @ 后缀）用于引用检查 ──
static void collect_top_keys(const json& root, std::set<std::string>& keys) {
    std::string ns;
    if (root.contains("namespace") && root["namespace"].is_string())
        ns = root["namespace"].get<std::string>();
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key() == "namespace") continue;
        std::string k = it.key();
        auto at_pos = k.find('@');
        std::string bare = (at_pos != std::string::npos) ? k.substr(0, at_pos) : k;
        keys.insert(bare);
        if (!ns.empty())
            keys.insert(ns + "." + bare);
    }
}

// ── 递归检查 @ 引用是否指向已知控件 ──
static void check_references(const json& node, const std::string& path,
                               const std::set<std::string>& known_keys,
                               std::vector<UiDiagnosis>& results) {
    if (!node.is_object()) return;

    if (node.contains("controls") && node["controls"].is_array()) {
        for (const auto& item : node["controls"]) {
            if (!item.is_object()) continue;
            for (auto ct = item.begin(); ct != item.end(); ++ct) {
                std::string full_key = ct.key();
                auto at_pos = full_key.find('@');
                std::string bare = (at_pos != std::string::npos) ? full_key.substr(0, at_pos) : full_key;
                // 检查 @ 引用
                if (at_pos != std::string::npos) {
                    std::string ref = full_key.substr(at_pos + 1);
                    // 只检查同文件内的引用（不含 . 的是同 namespace 引用）
                    // 含 . 的是跨 namespace 引用，我们检查当前文件内已收集的 ns.name
                    if (known_keys.find(ref) == known_keys.end()) {
                        // 可能是外部引用（common.button 等），不报 error 只做提示
                        // 但如果引用格式明显不含 . 且不在 known_keys 里，报 warning
                        if (ref.find('.') == std::string::npos) {
                            results.push_back({0, "warning",
                                "控件 \"" + path + "/" + bare +
                                "\" 引用 \"@" + ref + "\" 未在当前文件中找到定义"});
                        }
                    }
                }
                // 递归
                check_references(ct.value(), path + "/" + bare, known_keys, results);
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

    // 收集顶层 key 用于引用检查
    std::set<std::string> known_keys;
    collect_top_keys(root, known_keys);

    // 遍历所有顶层控件定义
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key() == "namespace") continue;
        if (!it.value().is_object()) continue;

        std::string ctrl_name = it.key();
        auto at_pos = ctrl_name.find('@');
        if (at_pos != std::string::npos) ctrl_name = ctrl_name.substr(0, at_pos);

        // 检查该控件的 controls 子数组 key 重复
        if (it.value().contains("controls")) {
            check_controls_keys(it.value()["controls"], ctrl_name, results);
        }

        // 检查控件属性
        check_ctrl_props(it.value(), ctrl_name, root, results);

        // 检查 @ 引用
        check_references(it.value(), ctrl_name, known_keys, results);
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
