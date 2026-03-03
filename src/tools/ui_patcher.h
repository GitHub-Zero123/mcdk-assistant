#pragma once
// ui_patcher.h — JSON UI 增量补丁工具
// 支持对大型 JSON UI 文件进行精确的增量修改，避免重写整个文件
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace mcdk {

struct PatchResult {
    bool success;
    int applied_count;      // 成功应用的 patch 数量
    int failed_index;       // 第一个失败的 patch 索引（-1 表示全部成功）
    std::string error;      // 错误信息
};

/// 对 JSON UI 文件应用一组补丁操作
/// patches 格式: JSON 数组，每项为一个操作:
///   { "op": "set_prop",    "path": "main/panel", "props": { "visible": false } }
///   { "op": "remove_prop", "path": "main/panel", "keys": ["visible", "layer"] }
///   { "op": "add_ctrl",    "path": "main/panel", "key": "newBtn", "value": {...}, "after": "topPanel" }
///   { "op": "remove_ctrl", "path": "main/panel", "key": "oldPanel" }
///   { "op": "replace_ctrl","path": "main/panel", "key": "topPanel", "value": {...} }
///   { "op": "merge_ctrl",  "path": "main/panel", "key": "topPanel", "props": {...}, "new_key": "..." }
///   { "op": "add_top",     "key": "my_component", "value": {...} }
///   { "op": "remove_top",  "key": "my_component" }
PatchResult apply_ui_patches(const std::string& file_path,
                              const nlohmann::json& patches,
                              bool backup = true);

} // namespace mcdk
