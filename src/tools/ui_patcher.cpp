#include "tools/ui_patcher.h"
#include <fstream>
#include <sstream>

namespace mcdk {

using json = nlohmann::json;

// ── 辅助：按路径在 JSON 树中定位控件 ──
// path 格式: "main/panel/workPanel" (第一段=顶层 key, 后续=controls 中的 key)
// 返回指向目标节点的指针，失败返回 nullptr 并设置 error
static json* resolve_path(json& root, const std::string& path, std::string& error) {
    if (path.empty()) {
        error = "path 不能为空";
        return nullptr;
    }

    // 分割路径
    std::vector<std::string> segments;
    {
        std::istringstream ss(path);
        std::string seg;
        while (std::getline(ss, seg, '/'))
            if (!seg.empty()) segments.push_back(seg);
    }
    if (segments.empty()) {
        error = "path 不能为空";
        return nullptr;
    }

    // 第一段: 顶层控件
    json* current = nullptr;
    for (auto it = root.begin(); it != root.end(); ++it) {
        std::string k = it.key();
        auto at = k.find('@');
        std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
        if (bare == segments[0]) {
            current = &it.value();
            break;
        }
    }
    if (!current) {
        error = "未找到顶层控件: " + segments[0];
        return nullptr;
    }

    // 后续段: 在 controls 中查找
    for (size_t i = 1; i < segments.size(); ++i) {
        const std::string& seg = segments[i];
        bool found = false;
        if (current->is_object() && current->contains("controls") &&
            (*current)["controls"].is_array()) {
            for (auto& item : (*current)["controls"]) {
                if (!item.is_object()) continue;
                for (auto jt = item.begin(); jt != item.end(); ++jt) {
                    std::string k = jt.key();
                    auto at = k.find('@');
                    std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
                    if (bare == seg) {
                        current = &jt.value();
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
        if (!found) {
            error = "路径 \"" + path + "\" 中未找到控件: " + seg;
            return nullptr;
        }
    }

    return current;
}

// ── 辅助：在 controls 数组中查找指定 key 的项 ──
// 返回 controls 数组中包含该 key 的 object 的迭代器位置
static int find_ctrl_index(json& controls_arr, const std::string& key) {
    for (size_t i = 0; i < controls_arr.size(); ++i) {
        if (!controls_arr[i].is_object()) continue;
        for (auto it = controls_arr[i].begin(); it != controls_arr[i].end(); ++it) {
            std::string k = it.key();
            auto at = k.find('@');
            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
            if (bare == key) return (int)i;
        }
    }
    return -1;
}

// ── 执行单个 patch 操作 ──
static bool apply_one_patch(json& root, const json& patch, std::string& error) {
    std::string op = patch.value("op", "");

    // ── set_prop: 设置属性 ──
    if (op == "set_prop") {
        std::string path = patch.value("path", "");
        json* node = resolve_path(root, path, error);
        if (!node) return false;
        if (!node->is_object()) {
            error = "目标不是对象: " + path;
            return false;
        }
        if (!patch.contains("props") || !patch["props"].is_object()) {
            error = "set_prop 需要 props 对象";
            return false;
        }
        for (auto it = patch["props"].begin(); it != patch["props"].end(); ++it) {
            (*node)[it.key()] = it.value();
        }
        return true;
    }

    // ── remove_prop: 删除属性 ──
    if (op == "remove_prop") {
        std::string path = patch.value("path", "");
        json* node = resolve_path(root, path, error);
        if (!node) return false;
        if (!node->is_object()) {
            error = "目标不是对象: " + path;
            return false;
        }
        if (!patch.contains("keys") || !patch["keys"].is_array()) {
            error = "remove_prop 需要 keys 数组";
            return false;
        }
        for (const auto& k : patch["keys"]) {
            if (k.is_string()) node->erase(k.get<std::string>());
        }
        return true;
    }

    // ── add_ctrl: 添加子控件 ──
    if (op == "add_ctrl") {
        std::string path = patch.value("path", "");
        json* node = resolve_path(root, path, error);
        if (!node) return false;
        if (!node->is_object()) {
            error = "目标不是对象: " + path;
            return false;
        }
        std::string key = patch.value("key", "");
        if (key.empty()) {
            error = "add_ctrl 需要 key";
            return false;
        }
        if (!patch.contains("value")) {
            error = "add_ctrl 需要 value";
            return false;
        }

        // 确保 controls 数组存在
        if (!node->contains("controls"))
            (*node)["controls"] = json::array();
        auto& controls = (*node)["controls"];

        // 构建新控件项 { "key": value }
        json new_item = json::object();
        new_item[key] = patch["value"];

        // 确定插入位置
        std::string after = patch.value("after", "");
        std::string before = patch.value("before", "");
        if (!after.empty()) {
            int idx = find_ctrl_index(controls, after);
            if (idx >= 0) {
                controls.insert(controls.begin() + idx + 1, new_item);
            } else {
                controls.push_back(new_item); // after 未找到则追加到末尾
            }
        } else if (!before.empty()) {
            int idx = find_ctrl_index(controls, before);
            if (idx >= 0) {
                controls.insert(controls.begin() + idx, new_item);
            } else {
                controls.insert(controls.begin(), new_item); // before 未找到则插入到开头
            }
        } else {
            controls.push_back(new_item); // 默认追加到末尾
        }
        return true;
    }

    // ── remove_ctrl: 删除子控件 ──
    if (op == "remove_ctrl") {
        std::string path = patch.value("path", "");
        json* node = resolve_path(root, path, error);
        if (!node) return false;
        std::string key = patch.value("key", "");
        if (key.empty()) {
            error = "remove_ctrl 需要 key";
            return false;
        }
        if (!node->contains("controls") || !(*node)["controls"].is_array()) {
            error = "目标控件没有 controls 数组";
            return false;
        }
        auto& controls = (*node)["controls"];
        int idx = find_ctrl_index(controls, key);
        if (idx < 0) {
            error = "controls 中未找到: " + key;
            return false;
        }
        controls.erase(controls.begin() + idx);
        return true;
    }

    // ── replace_ctrl: 替换子控件 ──
    if (op == "replace_ctrl") {
        std::string path = patch.value("path", "");
        json* node = resolve_path(root, path, error);
        if (!node) return false;
        std::string key = patch.value("key", "");
        if (key.empty()) {
            error = "replace_ctrl 需要 key";
            return false;
        }
        if (!patch.contains("value")) {
            error = "replace_ctrl 需要 value";
            return false;
        }
        if (!node->contains("controls") || !(*node)["controls"].is_array()) {
            error = "目标控件没有 controls 数组";
            return false;
        }
        auto& controls = (*node)["controls"];
        int idx = find_ctrl_index(controls, key);
        if (idx < 0) {
            error = "controls 中未找到: " + key;
            return false;
        }
        // 保留原始 key（可能含 @继承）
        std::string original_key;
        for (auto it = controls[idx].begin(); it != controls[idx].end(); ++it) {
            std::string k = it.key();
            auto at = k.find('@');
            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
            if (bare == key) {
                original_key = it.key();
                break;
            }
        }
        controls[idx] = json::object();
        controls[idx][original_key.empty() ? key : original_key] = patch["value"];
        return true;
    }

    // ── add_top: 添加顶层控件 ──
    if (op == "add_top") {
        std::string key = patch.value("key", "");
        if (key.empty()) {
            error = "add_top 需要 key";
            return false;
        }
        if (!patch.contains("value")) {
            error = "add_top 需要 value";
            return false;
        }
        root[key] = patch["value"];
        return true;
    }

    // ── remove_top: 删除顶层控件 ──
    if (op == "remove_top") {
        std::string key = patch.value("key", "");
        if (key.empty()) {
            error = "remove_top 需要 key";
            return false;
        }
        // 查找含 @ 的完整 key
        std::string full_key;
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string k = it.key();
            auto at = k.find('@');
            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
            if (bare == key) {
                full_key = it.key();
                break;
            }
        }
        if (full_key.empty()) {
            error = "未找到顶层控件: " + key;
            return false;
        }
        root.erase(full_key);
        return true;
    }

    error = "未知操作: " + op + "。支持: set_prop, remove_prop, add_ctrl, remove_ctrl, replace_ctrl, add_top, remove_top";
    return false;
}

// ── 主入口 ──
PatchResult apply_ui_patches(const std::string& file_path,
                              const json& patches,
                              bool backup) {
    PatchResult result{true, 0, -1, ""};

    // 读取文件
    std::string content;
    {
        std::ifstream ifs(file_path, std::ios::binary);
        if (!ifs.is_open()) {
            result.success = false;
            result.error = "无法打开文件: " + file_path;
            return result;
        }
        content.assign((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    }

    // 解析 JSON（支持注释）
    json root;
    try {
        root = json::parse(content, nullptr, true, true);
    } catch (const json::parse_error& e) {
        result.success = false;
        result.error = std::string("JSON 解析失败: ") + e.what();
        return result;
    }

    if (!patches.is_array()) {
        result.success = false;
        result.error = "patches 必须是数组";
        return result;
    }

    // 逐个执行 patch
    for (size_t i = 0; i < patches.size(); ++i) {
        std::string err;
        if (!apply_one_patch(root, patches[i], err)) {
            result.success = false;
            result.failed_index = (int)i;
            result.error = "Patch #" + std::to_string(i) + " 失败: " + err;
            return result; // 原子性：失败则不写入
        }
        result.applied_count++;
    }

    // 创建备份（用原始内容写入 .bak）
    if (backup) {
        std::string bak_path = file_path + ".bak";
        std::ofstream bak(bak_path, std::ios::binary | std::ios::trunc);
        if (bak.is_open()) bak << content;
    }

    // 写回文件（缩进 2 格）
    {
        std::ofstream ofs(file_path, std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            result.success = false;
            result.error = "无法写入文件: " + file_path;
            return result;
        }
        ofs << root.dump(2);
    }

    return result;
}

} // namespace mcdk
