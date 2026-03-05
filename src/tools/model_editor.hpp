#pragma once
// model_editor.hpp — 基岩版 JSON 模型（geometry.json）核心编辑逻辑
// 支持: 解析/骨骼增删改/cube增删改/镜像/模板创建
// 依赖: nlohmann/json（已有）
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace mcdk {

using json = nlohmann::json;

// ── 辅助函数 ────────────────────────────────────────────

// 加载 geometry.json 文件（支持带注释的宽松JSON）
inline json model_load_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open())
        throw std::runtime_error("无法打开文件: " + path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    // nlohmann/json 解析（allow_exceptions=true）
    return json::parse(content, nullptr, true, true); // allow_comments
}

// 从字符串解析（直接传JSON文本）
inline json model_parse_content(const std::string& content) {
    return json::parse(content, nullptr, true, true);
}

// 保存 geometry.json 文件（4空格缩进，UTF-8）
inline void model_save_file(const std::string& path, const json& geo) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open())
        throw std::runtime_error("无法写入文件: " + path);
    ofs << geo.dump(4);
}

// 从 file_path 或 json_content 参数解析模型
inline json model_resolve(const json& params) {
    std::string fpath   = params.value("file_path", "");
    std::string content = params.value("json_content", "");
    if (!fpath.empty())   return model_load_file(fpath);
    if (!content.empty()) return model_parse_content(content);
    throw std::runtime_error("必须提供 file_path 或 json_content 参数");
}

// 格式化 [x,y,z] 数组为字符串
inline std::string fmt_vec3(const json& arr) {
    if (!arr.is_array() || arr.size() < 3) return "[?,?,?]";
    std::ostringstream ss;
    ss << "[" << arr[0].dump() << "," << arr[1].dump() << "," << arr[2].dump() << "]";
    return ss.str();
}

// 格式化 [u,v] 为字符串
inline std::string fmt_uv(const json& uv) {
    if (!uv.is_array()) return "{per-face}";
    if (uv.size() >= 2) {
        std::ostringstream ss;
        ss << "[" << uv[0].dump() << "," << uv[1].dump() << "]";
        return ss.str();
    }
    return uv.dump();
}

// 根据 identifier 找到 geometry 数组中对应的元素引用
// 如果 identifier 为空，返回第一个
inline json& find_geo(json& root, const std::string& identifier = "") {
    if (!root.contains("minecraft:geometry") || !root["minecraft:geometry"].is_array())
        throw std::runtime_error("无效的 geometry.json：缺少 minecraft:geometry 数组");

    auto& arr = root["minecraft:geometry"];
    if (arr.empty())
        throw std::runtime_error("minecraft:geometry 数组为空");

    if (identifier.empty())
        return arr[0];

    for (auto& geo : arr) {
        if (geo.contains("description") &&
            geo["description"].value("identifier", "") == identifier)
            return geo;
    }
    throw std::runtime_error("未找到 geometry identifier: " + identifier);
}

// 在 geometry 中找到指定名称的 bone 引用
inline json& find_bone(json& geo, const std::string& bone_name) {
    if (!geo.contains("bones") || !geo["bones"].is_array())
        throw std::runtime_error("geometry 中没有 bones 数组");

    for (auto& bone : geo["bones"]) {
        if (bone.value("name", "") == bone_name)
            return bone;
    }
    throw std::runtime_error("未找到骨骼: " + bone_name);
}

// 检查骨骼是否存在（不抛出）
inline bool bone_exists(const json& geo, const std::string& bone_name) {
    if (!geo.contains("bones") || !geo["bones"].is_array()) return false;
    for (const auto& bone : geo["bones"]) {
        if (bone.value("name", "") == bone_name) return true;
    }
    return false;
}

// 收集某骨骼的所有子骨骼名称（直接子 + 间接子，递归）
inline std::vector<std::string> collect_children(const json& geo, const std::string& parent_name) {
    std::vector<std::string> result;
    if (!geo.contains("bones")) return result;

    // BFS
    std::vector<std::string> queue = {parent_name};
    while (!queue.empty()) {
        std::string cur = queue.back(); queue.pop_back();
        for (const auto& bone : geo["bones"]) {
            if (bone.value("parent", "") == cur) {
                std::string name = bone.value("name", "");
                result.push_back(name);
                queue.push_back(name);
            }
        }
    }
    return result;
}

// ── model_parse_summary：生成骨骼摘要文本 ───────────────

inline std::string model_parse_summary(const json& root, const std::string& identifier = "") {
    // 先拷贝一份 const 版
    json root_copy = root;
    json& geo = find_geo(root_copy, identifier);

    const auto& desc = geo.value("description", json::object());
    std::string id   = desc.value("identifier", "(unknown)");
    int tw = desc.value("texture_width",  64);
    int th = desc.value("texture_height", 64);

    std::ostringstream ss;
    ss << "=== " << id << " | 贴图:" << tw << "x" << th;

    // 统计
    int bone_count = 0, cube_count = 0;
    if (geo.contains("bones") && geo["bones"].is_array()) {
        bone_count = static_cast<int>(geo["bones"].size());
        for (const auto& b : geo["bones"])
            if (b.contains("cubes") && b["cubes"].is_array())
                cube_count += static_cast<int>(b["cubes"].size());
    }
    ss << " | 骨骼:" << bone_count << " | cubes共:" << cube_count << " ===\n";
    ss << "骨骼列表（可用于 model_op 的 bone_name 参数）:\n";

    if (geo.contains("bones") && geo["bones"].is_array()) {
        int idx = 0;
        for (const auto& bone : geo["bones"]) {
            std::string name    = bone.value("name", "?");
            std::string parent  = bone.value("parent", "-");
            std::string pivot   = bone.contains("pivot") ? fmt_vec3(bone["pivot"]) : "[0,0,0]";
            std::string rot     = bone.contains("rotation") ? fmt_vec3(bone["rotation"]) : "[0,0,0]";
            int cubes_n = bone.contains("cubes") && bone["cubes"].is_array()
                          ? static_cast<int>(bone["cubes"].size()) : 0;
            bool mirror = bone.value("mirror", false);
            std::string binding = bone.value("binding", "");

            ss << "  [" << idx << "] " << name
               << "  parent=" << parent
               << "  pivot=" << pivot
               << "  rot=" << rot
               << "  cubes=" << cubes_n;
            if (mirror) ss << "  mirror=true";
            if (!binding.empty()) ss << "  binding=" << binding;
            ss << "\n";
            ++idx;
        }
    } else {
        ss << "  (无骨骼)\n";
    }

    // 如果文件有多个 geometry，列出所有 identifier
    if (root_copy.contains("minecraft:geometry") && root_copy["minecraft:geometry"].size() > 1) {
        ss << "\n文件包含多个 geometry（可通过 geo_identifier 参数指定）:\n";
        for (const auto& g : root_copy["minecraft:geometry"]) {
            ss << "  - " << g.value("description", json::object()).value("identifier", "?") << "\n";
        }
    }

    return ss.str();
}

// ── model_get_bone_detail：获取单骨骼详细 JSON 文本 ──────

inline std::string model_get_bone_detail(const json& root,
                                         const std::string& bone_name,
                                         const std::string& identifier = "") {
    json root_copy = root;
    json& geo  = find_geo(root_copy, identifier);
    json& bone = find_bone(geo, bone_name);

    std::ostringstream ss;
    ss << "=== bone: " << bone_name << " ===\n";
    ss << bone.dump(4);
    return ss.str();
}

// ── model_op 实现：各操作 ────────────────────────────────

// add_bone
inline std::string op_add_bone(json& geo,
                                const std::string& bone_name,
                                const std::string& parent,
                                const json& pivot,
                                const json& rotation) {
    if (bone_exists(geo, bone_name))
        throw std::runtime_error("骨骼已存在: " + bone_name);

    json bone;
    bone["name"] = bone_name;
    if (!parent.empty()) bone["parent"] = parent;
    bone["pivot"] = pivot.is_null() ? json::array({0,0,0}) : pivot;
    if (!rotation.is_null() && rotation.is_array())
        bone["rotation"] = rotation;

    if (!geo.contains("bones") || geo["bones"].is_null())
        geo["bones"] = json::array();
    geo["bones"].push_back(bone);

    return "OK: added bone '" + bone_name + "'" +
           (parent.empty() ? "" : " (parent=" + parent + ")") +
           "  pivot=" + fmt_vec3(bone["pivot"]);
}

// remove_bone（cascade=true 时同时删除子骨骼）
inline std::string op_remove_bone(json& geo,
                                   const std::string& bone_name,
                                   bool cascade) {
    if (!bone_exists(geo, bone_name))
        throw std::runtime_error("骨骼不存在: " + bone_name);

    std::vector<std::string> to_remove = {bone_name};
    if (cascade) {
        auto children = collect_children(geo, bone_name);
        to_remove.insert(to_remove.end(), children.begin(), children.end());
    }

    auto& bones = geo["bones"];
    int removed = 0;
    bones.erase(std::remove_if(bones.begin(), bones.end(),
        [&](const json& b) {
            std::string n = b.value("name", "");
            bool del = std::find(to_remove.begin(), to_remove.end(), n) != to_remove.end();
            if (del) ++removed;
            return del;
        }), bones.end());

    std::ostringstream ss;
    ss << "OK: removed " << removed << " bone(s): ";
    for (size_t i = 0; i < to_remove.size(); ++i) {
        if (i) ss << ", ";
        ss << to_remove[i];
    }
    return ss.str();
}

// rename_bone（同时更新所有子骨骼的 parent 引用）
inline std::string op_rename_bone(json& geo,
                                   const std::string& bone_name,
                                   const std::string& new_name) {
    if (!bone_exists(geo, bone_name))
        throw std::runtime_error("骨骼不存在: " + bone_name);
    if (bone_exists(geo, new_name))
        throw std::runtime_error("目标名称已存在: " + new_name);

    for (auto& bone : geo["bones"]) {
        if (bone.value("name", "") == bone_name)
            bone["name"] = new_name;
        if (bone.value("parent", "") == bone_name)
            bone["parent"] = new_name;
    }
    return "OK: renamed '" + bone_name + "' -> '" + new_name + "'";
}

// set_bone_transform（修改 pivot 和/或 rotation）
inline std::string op_set_bone_transform(json& geo,
                                          const std::string& bone_name,
                                          const json& pivot,
                                          const json& rotation) {
    json& bone = find_bone(geo, bone_name);
    std::ostringstream ss;
    ss << "OK: set_bone_transform '" << bone_name << "'";

    if (!pivot.is_null() && pivot.is_array()) {
        bone["pivot"] = pivot;
        ss << "  pivot=" << fmt_vec3(pivot);
    }
    if (!rotation.is_null() && rotation.is_array()) {
        bone["rotation"] = rotation;
        ss << "  rotation=" << fmt_vec3(rotation);
    }
    return ss.str();
}

// add_cube
inline std::string op_add_cube(json& geo,
                                const std::string& bone_name,
                                const json& origin,
                                const json& size,
                                const json& uv,
                                const json& inflate,
                                const json& rotation,
                                const json& pivot) {
    json& bone = find_bone(geo, bone_name);
    if (!bone.contains("cubes") || bone["cubes"].is_null())
        bone["cubes"] = json::array();

    json cube;
    cube["origin"] = origin;
    cube["size"]   = size;
    cube["uv"]     = uv;
    if (!inflate.is_null() && inflate.is_number())
        cube["inflate"] = inflate;
    if (!rotation.is_null() && rotation.is_array())
        cube["rotation"] = rotation;
    if (!pivot.is_null() && pivot.is_array())
        cube["pivot"] = pivot;

    int idx = static_cast<int>(bone["cubes"].size());
    bone["cubes"].push_back(cube);

    std::ostringstream ss;
    ss << "OK: added cube to bone '" << bone_name << "' (index=" << idx << ")\n"
       << "  origin=" << fmt_vec3(origin)
       << "  size=" << fmt_vec3(size)
       << "  uv=" << fmt_uv(uv);
    if (!inflate.is_null()) ss << "  inflate=" << inflate.dump();
    return ss.str();
}

// remove_cube（按索引）
inline std::string op_remove_cube(json& geo,
                                   const std::string& bone_name,
                                   int cube_index) {
    json& bone = find_bone(geo, bone_name);
    if (!bone.contains("cubes") || !bone["cubes"].is_array())
        throw std::runtime_error("骨骼 '" + bone_name + "' 没有 cubes");

    auto& cubes = bone["cubes"];
    int n = static_cast<int>(cubes.size());
    if (cube_index < 0 || cube_index >= n)
        throw std::runtime_error("cube_index 越界: " + std::to_string(cube_index) +
                                  " (共" + std::to_string(n) + "个)");
    cubes.erase(cubes.begin() + cube_index);
    return "OK: removed cube[" + std::to_string(cube_index) + "] from bone '" + bone_name + "'";
}

// edit_cube（修改指定索引 cube 的字段，null 表示不修改）
inline std::string op_edit_cube(json& geo,
                                 const std::string& bone_name,
                                 int cube_index,
                                 const json& origin,
                                 const json& size,
                                 const json& uv,
                                 const json& inflate,
                                 const json& rotation,
                                 const json& pivot) {
    json& bone = find_bone(geo, bone_name);
    if (!bone.contains("cubes") || !bone["cubes"].is_array())
        throw std::runtime_error("骨骼 '" + bone_name + "' 没有 cubes");

    auto& cubes = bone["cubes"];
    int n = static_cast<int>(cubes.size());
    if (cube_index < 0 || cube_index >= n)
        throw std::runtime_error("cube_index 越界: " + std::to_string(cube_index));

    auto& cube = cubes[cube_index];
    std::ostringstream ss;
    ss << "OK: edit cube[" << cube_index << "] of bone '" << bone_name << "'";

    if (!origin.is_null() && origin.is_array())    { cube["origin"]   = origin;   ss << "  origin="  << fmt_vec3(origin); }
    if (!size.is_null()   && size.is_array())       { cube["size"]     = size;     ss << "  size="    << fmt_vec3(size); }
    if (!uv.is_null())                              { cube["uv"]       = uv;       ss << "  uv="      << fmt_uv(uv); }
    if (!inflate.is_null() && inflate.is_number())  { cube["inflate"]  = inflate;  ss << "  inflate=" << inflate.dump(); }
    if (!rotation.is_null() && rotation.is_array()) { cube["rotation"] = rotation; ss << "  rotation="<< fmt_vec3(rotation); }
    if (!pivot.is_null()   && pivot.is_array())     { cube["pivot"]    = pivot;    ss << "  pivot="   << fmt_vec3(pivot); }

    return ss.str();
}

// mirror_bone（X轴镜像骨骼 pivot 和所有 cube 的 origin，生成新骨骼）
inline std::string op_mirror_bone(json& geo,
                                   const std::string& bone_name,
                                   const std::string& new_name,
                                   bool mirror_uv) {
    if (!bone_exists(geo, bone_name))
        throw std::runtime_error("骨骼不存在: " + bone_name);
    if (bone_exists(geo, new_name))
        throw std::runtime_error("目标名称已存在: " + new_name);

    // 深拷贝
    json src_bone;
    for (const auto& b : geo["bones"]) {
        if (b.value("name", "") == bone_name) { src_bone = b; break; }
    }

    json new_bone = src_bone;
    new_bone["name"] = new_name;

    // 镜像 pivot.x
    if (new_bone.contains("pivot") && new_bone["pivot"].is_array() && new_bone["pivot"].size() >= 1) {
        float px = new_bone["pivot"][0].get<float>();
        new_bone["pivot"][0] = -px;
    }

    // 镜像每个 cube
    if (new_bone.contains("cubes") && new_bone["cubes"].is_array()) {
        for (auto& cube : new_bone["cubes"]) {
            // 镜像 origin.x: new_ox = -origin.x - size.x
            if (cube.contains("origin") && cube.contains("size") &&
                cube["origin"].is_array() && cube["size"].is_array() &&
                cube["origin"].size() >= 1 && cube["size"].size() >= 1) {
                float ox = cube["origin"][0].get<float>();
                float sx = cube["size"][0].get<float>();
                cube["origin"][0] = -ox - sx;
            }
            // 镜像 cube 自身 pivot（如果有）
            if (cube.contains("pivot") && cube["pivot"].is_array() && cube["pivot"].size() >= 1) {
                float cpx = cube["pivot"][0].get<float>();
                cube["pivot"][0] = -cpx;
            }
            // 设置 mirror UV
            if (mirror_uv) {
                cube["mirror"] = true;
            }
        }
    }

    geo["bones"].push_back(new_bone);
    return "OK: mirrored bone '" + bone_name + "' -> '" + new_name + "' (X-axis)"
           + (mirror_uv ? "  mirror_uv=true" : "");
}

// set_texture_size
inline std::string op_set_texture_size(json& geo, int width, int height) {
    if (!geo.contains("description")) geo["description"] = json::object();
    geo["description"]["texture_width"]  = width;
    geo["description"]["texture_height"] = height;
    return "OK: texture_size set to " + std::to_string(width) + "x" + std::to_string(height);
}

// add_locator
inline std::string op_add_locator(json& geo,
                                   const std::string& bone_name,
                                   const std::string& locator_name,
                                   const json& position) {
    json& bone = find_bone(geo, bone_name);
    if (!bone.contains("locators") || bone["locators"].is_null())
        bone["locators"] = json::object();
    bone["locators"][locator_name] = position;
    return "OK: added locator '" + locator_name + "' to bone '" + bone_name + "'"
           + "  pos=" + fmt_vec3(position);
}

// ── model_mirror_symmetry：完整骨骼对称复制 ──────────────

inline std::string model_mirror_symmetry(json& root,
                                          const std::string& identifier,
                                          const std::string& source_bone,
                                          const std::string& target_bone,
                                          bool mirror_uv) {
    json& geo = find_geo(root, identifier);
    return op_mirror_bone(geo, source_bone, target_bone, mirror_uv);
}

// ── model_create_from_template：从模板创建新模型 ──────────

inline json model_create_template(const std::string& identifier,
                                   int texture_width,
                                   int texture_height,
                                   const std::string& template_type) {
    json root;
    root["format_version"] = "1.12.0";

    json desc;
    desc["identifier"]          = identifier;
    desc["visible_bounds_width"]  = 1;
    desc["visible_bounds_height"] = 2;
    desc["visible_bounds_offset"] = json::array({0, 1, 0});
    desc["texture_width"]  = texture_width;
    desc["texture_height"] = texture_height;

    json geo;
    geo["description"] = desc;
    geo["bones"] = json::array();

    auto add_bone = [&](const std::string& name, const std::string& parent,
                        std::vector<double> pivot,
                        std::vector<double> rot = {}) {
        json bone;
        bone["name"] = name;
        if (!parent.empty()) bone["parent"] = parent;
        bone["pivot"] = pivot;
        if (rot.size() == 3) bone["rotation"] = rot;
        geo["bones"].push_back(bone);
    };

    if (template_type == "blank") {
        add_bone("root", "", {0,0,0});

    } else if (template_type == "humanoid") {
        // 标准人形骨骼（参考 geometry.humanoid.custom）
        add_bone("root",      "",       {0,0,0});
        add_bone("waist",     "root",   {0,12,0});
        add_bone("body",      "waist",  {0,24,0});
        add_bone("head",      "body",   {0,24,0});
        add_bone("hat",       "head",   {0,24,0});
        add_bone("cape",      "body",   {0,24,3});
        add_bone("leftArm",   "body",   {5,22,0});
        add_bone("leftSleeve","leftArm",{5,22,0});
        add_bone("leftItem",  "leftArm",{6,15,1});
        add_bone("rightArm",  "body",   {-5,22,0});
        add_bone("rightSleeve","rightArm",{-5,22,0});
        add_bone("rightItem", "rightArm",{-6,15,1});
        add_bone("leftLeg",   "root",   {1.9f,12,0});
        add_bone("leftPants", "leftLeg",{1.9f,12,0});
        add_bone("rightLeg",  "root",   {-1.9f,12,0});
        add_bone("rightPants","rightLeg",{-1.9f,12,0});
        add_bone("jacket",    "body",   {0,24,0});

    } else if (template_type == "sword") {
        add_bone("root",   "",     {0,0,0});
        add_bone("handle", "root", {0,0,0});
        add_bone("guard",  "root", {0,4,0});
        add_bone("blade",  "root", {0,8,0});

    } else if (template_type == "item_flat") {
        add_bone("root", "",     {0,0,0});
        add_bone("item", "root", {0,0,0});

    } else if (template_type == "quadruped") {
        add_bone("root",  "",     {0,0,0});
        add_bone("body",  "root", {0,12,0});
        add_bone("head",  "body", {0,14,8});
        add_bone("tail",  "body", {0,12,-8});
        add_bone("leg0",  "body", {-3,6,7});   // 右前
        add_bone("leg1",  "body", {3,6,7});    // 左前
        add_bone("leg2",  "body", {-3,6,-5});  // 右后
        add_bone("leg3",  "body", {3,6,-5});   // 左后

    } else {
        throw std::runtime_error("未知模板类型: " + template_type +
                                 "  有效值: blank/humanoid/sword/item_flat/quadruped");
    }

    root["minecraft:geometry"] = json::array({geo});
    return root;
}

} // namespace mcdk
