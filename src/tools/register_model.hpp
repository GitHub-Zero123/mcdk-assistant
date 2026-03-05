#pragma once
// register_model.hpp — 基岩版 JSON 模型 MCP 工具注册
// 工具列表:
//   get_model_reference       — 语法速查手册（无参数）
//   model_parse               — 解析模型结构，输出骨骼节点列表
//   model_get_bone            — 获取单骨骼完整 JSON 详情
//   model_op                  — 核心增删改操作（file_path 必选，写回文件）
//   model_mirror_symmetry     — X轴对称骨骼复制（写回文件）
//   model_create_from_template — 从模板创建新模型（写入 save_path）
#include "tools/model_editor.hpp"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>

namespace mcdk {

// ── 辅助：标准返回格式 ────────────────────────────────────
static inline mcp::json model_ok(const std::string& text) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", text}}})}};
}
static inline mcp::json model_err(const std::string& text) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", "[ERROR] " + text}}})}};
}

// ── get_model_reference 手册文本 ──────────────────────────
static const char* MODEL_REFERENCE_TEXT = R"(
=== 基岩版 geometry.json 全栈速查手册 ===

一、文件格式与版本
  format_version: "1.12.0"  — 推荐（网易版最佳兼容）
  format_version: "1.16.0"  — 支持 per-face UV 的 uv_rotation 字段
  根结构:
    {
      "format_version": "1.12.0",
      "minecraft:geometry": [ <geo_object>, ... ]
    }

二、description 字段
  identifier        string  — 几何体ID，格式: "geometry.xxx.yyy"（小写+点+下划线）
                              动画文件、render_controller、client_entity 通过此ID引用
  texture_width     int     — 默认 64，贴图的UV坐标参考宽度
  texture_height    int     — 默认 64，贴图的UV坐标参考高度
  visible_bounds_width   float  — 可见区域宽度（模型空间单位）
  visible_bounds_height  float  — 可见区域高度
  visible_bounds_offset  [x,y,z] — 可见区域中心偏移

三、bones 骨骼系统（核心）
  name      string   — 骨骼名（动画文件通过此名引用）
  parent    string   — 父骨骼名（缺省则为根骨骼）
  pivot     [x,y,z]  — 旋转轴心（模型空间坐标，Y轴向上）
  rotation  [x,y,z]  — 初始旋转（度，X→Y→Z顺序），动画叠加在此基础上
  mirror    bool     — 对骨骼所有cube做UV X轴镜像
  inflate   float    — 所有cube全向膨胀量（可被单个cube的inflate覆盖）
  binding   molang   — 绑定到父骨骼骨架（物品用），如: "c.item_slot == 'main_hand'"
  render_group_id int — 渲染分组（半透明排序用）

四、cubes 立方体
  origin  [x,y,z] — cube最小角坐标（最小x/y/z）
  size    [x,y,z] — cube在xyz方向的延伸量
  inflate float   — 该cube全向膨胀（覆盖骨骼的inflate）
  mirror  bool    — 覆盖骨骼的mirror设置
  rotation [x,y,z] — cube自身旋转（绕自身pivot）
  pivot   [x,y,z] — cube自身旋转轴心（缺省=cube几何中心）

  UV 映射（二选一）:
  ① 简单UV（推荐）:
    "uv": [u, v]  — 贴图左上角坐标（像素）
    UV展开顺序（标准立方体展开）:
      X面(east/west): size.z × size.y
      Y面(up/down):   size.x × size.z
      Z面(south/north): size.x × size.y
    总贴图占用宽: 2*(size.x + size.z)，高: size.z + size.y

  ② per-face UV（精确控制每面）:
    "uv": {
      "north": {"uv": [u,v], "uv_size": [w,h], "uv_rotation": 0|90|180|270},
      "south": {...}, "east": {...}, "west": {...}, "up": {...}, "down": {...}
    }
    uv_size: 映射的贴图像素尺寸（缺省使用cube尺寸）
    uv_rotation: 面UV顺时针旋转度数（1.16.0+）

五、locators 定位点（骨骼上的附着点）
  格式:
    "locators": {
      "my_point": [x, y, z]              // 简单位置
      "my_point": {"offset": [x,y,z]}   // 扩展格式
    }
  用途: 特效挂接（particle_effect中的locator）、装备附着

六、坐标系说明
  - Y轴向上，1单位 = 1/16 Minecraft格子
  - origin = cube的最小角（左-下-前）
  - pivot = 旋转中心，子骨骼继承父骨骼变换
  - 动画中的rotation/position是相对于骨骼pivot的偏移量

七、骨骼层级与动画的关系
  动画文件 (animations.json) 通过 bone_name 引用骨骼:
    "bones": {
      "head": {
        "rotation": ["Math.sin(q.life_time*150)*15", 0, 0]
      }
    }
  pivot 决定旋转中心，父骨骼变换会传递给所有子骨骼

八、常见骨骼命名约定（人形）
  root / waist / body / head / hat / cape / jacket
  leftArm / leftSleeve / leftItem
  rightArm / rightSleeve / rightItem
  leftLeg / leftPants / rightLeg / rightPants
  特殊: rightItem 常含 locator "lead_hold"（皮带绳挂点）

九、网易版注意事项
  - format_version 使用 "1.12.0"（最高兼容）
  - geometry 文件放置: RP/models/entity/xxx.geo.json
  - render_controller 中通过 "geometry": "geometry.xxx" 引用
  - ModelComp API 可在运行时操作骨骼: SetBonePos/SetBoneRotation

十、model_op 可用操作（op 参数值）
  骨骼操作: add_bone / remove_bone / rename_bone / set_bone_transform
  cube操作: add_cube / remove_cube / edit_cube
  其他:     mirror_bone / set_texture_size / add_locator
)";

// ── 注册函数 ──────────────────────────────────────────────
inline void register_model_tools(mcp::server& srv) {

    // ════════════════════════════════════════════════════════
    // 1. get_model_reference — 语法速查手册
    // ════════════════════════════════════════════════════════
    srv.register_tool(
        mcp::tool_builder("get_model_reference")
            .with_description("获取基岩版 geometry.json 完整语法速查手册，包含骨骼系统、cube、UV、locator、动画关系、网易版注意事项")
            .with_read_only_hint(true).with_idempotent_hint(true).build(),
        [](const mcp::json&, const std::string&) -> mcp::json {
            return model_ok(MODEL_REFERENCE_TEXT);
        });

    // ════════════════════════════════════════════════════════
    // 2. model_parse — 解析模型结构
    // ════════════════════════════════════════════════════════
    srv.register_tool(
        mcp::tool_builder("model_parse")
            .with_description("解析 geometry.json，输出骨骼节点列表（name/parent/pivot/rotation/cubes数量）。"
                              "支持 file_path（绝对路径）或 json_content（直接传JSON字符串）")
            .with_string_param("file_path",     "geometry.json 的绝对路径（与 json_content 二选一）", false)
            .with_string_param("json_content",  "直接传入 geometry.json 文本内容（与 file_path 二选一）", false)
            .with_string_param("geo_identifier","指定解析哪个 geometry（如 'geometry.humanoid.custom'），缺省解析第一个", false)
            .with_read_only_hint(true).build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                auto root = model_resolve(p);
                std::string id = p.value("geo_identifier", "");
                return model_ok(model_parse_summary(root, id));
            } catch (const std::exception& e) { return model_err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 3. model_get_bone — 获取单骨骼完整 JSON 详情
    // ════════════════════════════════════════════════════════
    srv.register_tool(
        mcp::tool_builder("model_get_bone")
            .with_description("获取指定骨骼的完整 JSON 定义（含所有 cube 的 origin/size/uv/inflate 等字段）")
            .with_string_param("file_path",     "geometry.json 的绝对路径（与 json_content 二选一）", false)
            .with_string_param("json_content",  "直接传入 geometry.json 文本内容（与 file_path 二选一）", false)
            .with_string_param("bone_name",     "骨骼名称（来自 model_parse 输出的列表）", true)
            .with_string_param("geo_identifier","指定 geometry identifier，缺省第一个", false)
            .with_read_only_hint(true).build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                auto root = model_resolve(p);
                std::string bone_name = p.value("bone_name", "");
                std::string id        = p.value("geo_identifier", "");
                return model_ok(model_get_bone_detail(root, bone_name, id));
            } catch (const std::exception& e) { return model_err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 4. model_op — 核心增删改操作（写回文件）
    //    支持单次模式（op + 参数）和批量模式（ops 数组）
    // ════════════════════════════════════════════════════════

    // 辅助：执行单个 op（在已找到的 geo 引用上），返回结果字符串；失败则抛出 runtime_error
    auto exec_one_op = [](json& geo, const json& src) -> std::string {
        // 内部辅助：从 src 中解析 JSON 参数
        auto parse_jp = [&src](const char* key) -> json {
            if (!src.contains(key) || src[key].is_null()) return json(nullptr);
            if (src[key].is_string()) {
                std::string s = src[key].get<std::string>();
                if (s.empty()) return json(nullptr);
                return json::parse(s, nullptr, true, true);
            }
            return src[key];
        };
        std::string op = src.value("op", "");

        if (op == "add_bone") {
            std::string bone_name = src.value("bone_name", "");
            std::string parent    = src.value("parent", "");
            json pivot    = parse_jp("pivot");
            json rotation = parse_jp("rotation");
            return op_add_bone(geo, bone_name, parent, pivot, rotation);

        } else if (op == "remove_bone") {
            std::string bone_name = src.value("bone_name", "");
            bool cascade = src.contains("cascade") && src["cascade"].is_boolean()
                           ? src["cascade"].get<bool>() : true;
            return op_remove_bone(geo, bone_name, cascade);

        } else if (op == "rename_bone") {
            std::string bone_name = src.value("bone_name", "");
            std::string new_name  = src.value("new_name", "");
            return op_rename_bone(geo, bone_name, new_name);

        } else if (op == "set_bone_transform") {
            std::string bone_name = src.value("bone_name", "");
            json pivot    = parse_jp("pivot");
            json rotation = parse_jp("rotation");
            return op_set_bone_transform(geo, bone_name, pivot, rotation);

        } else if (op == "add_cube") {
            std::string bone_name = src.value("bone_name", "");
            json origin   = parse_jp("origin");
            json size     = parse_jp("size");
            json uv       = parse_jp("uv");
            json inflate  = parse_jp("inflate");
            json rotation = parse_jp("cube_rotation");
            json cpivot   = parse_jp("cube_pivot");
            if (origin.is_null() || size.is_null() || uv.is_null())
                throw std::runtime_error("add_cube 需要 origin, size, uv 参数");
            return op_add_cube(geo, bone_name, origin, size, uv, inflate, rotation, cpivot);

        } else if (op == "remove_cube") {
            std::string bone_name = src.value("bone_name", "");
            int cube_index = src.contains("cube_index") && src["cube_index"].is_number()
                             ? src["cube_index"].get<int>() : -1;
            if (cube_index < 0) throw std::runtime_error("remove_cube 需要 cube_index >= 0");
            return op_remove_cube(geo, bone_name, cube_index);

        } else if (op == "edit_cube") {
            std::string bone_name = src.value("bone_name", "");
            int cube_index = src.contains("cube_index") && src["cube_index"].is_number()
                             ? src["cube_index"].get<int>() : -1;
            if (cube_index < 0) throw std::runtime_error("edit_cube 需要 cube_index >= 0");
            json origin   = parse_jp("origin");
            json size     = parse_jp("size");
            json uv       = parse_jp("uv");
            json inflate  = parse_jp("inflate");
            json rotation = parse_jp("cube_rotation");
            json cpivot   = parse_jp("cube_pivot");
            return op_edit_cube(geo, bone_name, cube_index, origin, size, uv, inflate, rotation, cpivot);

        } else if (op == "mirror_bone") {
            std::string bone_name = src.value("bone_name", "");
            std::string new_name  = src.value("new_name", "");
            bool mirror_uv = src.contains("mirror_uv") && src["mirror_uv"].is_boolean()
                             ? src["mirror_uv"].get<bool>() : true;
            return op_mirror_bone(geo, bone_name, new_name, mirror_uv);

        } else if (op == "set_texture_size") {
            int tw = src.contains("tex_width")  && src["tex_width"].is_number()  ? src["tex_width"].get<int>()  : 64;
            int th = src.contains("tex_height") && src["tex_height"].is_number() ? src["tex_height"].get<int>() : 64;
            return op_set_texture_size(geo, tw, th);

        } else if (op == "add_locator") {
            std::string bone_name    = src.value("bone_name", "");
            std::string locator_name = src.value("locator_name", "");
            json position = parse_jp("position");
            if (position.is_null()) throw std::runtime_error("add_locator 需要 position [x,y,z]");
            return op_add_locator(geo, bone_name, locator_name, position);

        } else {
            throw std::runtime_error("未知 op: " + op +
                "  有效值: add_bone/remove_bone/rename_bone/set_bone_transform/"
                "add_cube/remove_cube/edit_cube/mirror_bone/set_texture_size/add_locator");
        }
    };

    srv.register_tool(
        mcp::tool_builder("model_op")
            .with_description(
                "对 geometry.json 执行骨骼/cube 增删改操作，操作完立即写回文件。\n"
                "必须提供 file_path（绝对路径）和 op（操作类型）。\n"
                "【批量模式】传 ops（JSON数组），每项含 op 字段及对应参数，全部执行后一次写回文件，高效减少会话轮次。\n"
                "【单次模式】直接传 op + 参数（向后兼容）。\n"
                "op 可选值:\n"
                "  add_bone        — 新增骨骼 (bone_name, parent?, pivot, rotation?)\n"
                "  remove_bone     — 删除骨骼 (bone_name, cascade?=true)\n"
                "  rename_bone     — 重命名骨骼 (bone_name, new_name)，同步更新子骨骼的parent引用\n"
                "  set_bone_transform — 修改骨骼pivot/rotation (bone_name, pivot?, rotation?)\n"
                "  add_cube        — 向骨骼添加cube (bone_name, origin, size, uv, inflate?, rotation?, cube_pivot?)\n"
                "  remove_cube     — 删除cube (bone_name, cube_index)\n"
                "  edit_cube       — 修改cube字段 (bone_name, cube_index, origin?, size?, uv?, inflate?, rotation?, cube_pivot?)\n"
                "  mirror_bone     — X轴镜像骨骼 (bone_name, new_name, mirror_uv?=true)\n"
                "  set_texture_size — 修改贴图尺寸 (tex_width, tex_height)\n"
                "  add_locator     — 添加定位点 (bone_name, locator_name, position)")
            .with_string_param("file_path",      "geometry.json 的绝对路径（必须）", true)
            .with_string_param("geo_identifier", "指定 geometry identifier，缺省第一个", false)
            // 批量模式
            .with_string_param("ops",            "批量操作数组（JSON字符串），每项含op字段及对应参数，与单次op二选一", false)
            // 单次模式
            .with_string_param("op",             "操作类型（见描述）", false)
            // 骨骼操作参数
            .with_string_param("bone_name",  "目标骨骼名", false)
            .with_string_param("new_name",   "新骨骼名（rename_bone/mirror_bone用）", false)
            .with_string_param("parent",     "父骨骼名（add_bone用）", false)
            .with_string_param("locator_name","定位点名称（add_locator用）", false)
            .with_boolean_param("cascade",   "删除时是否级联删除子骨骼，默认true（remove_bone用）", false)
            .with_boolean_param("mirror_uv", "镜像时是否同时镜像UV，默认true（mirror_bone用）", false)
            // 向量参数（JSON数组）
            .with_string_param("pivot",      "骨骼旋转轴心 [x,y,z]（JSON数组字符串）", false)
            .with_string_param("rotation",   "骨骼旋转 [x,y,z]（度，JSON数组字符串）", false)
            // cube参数
            .with_number_param("cube_index", "cube索引（remove_cube/edit_cube用，从0开始）", false)
            .with_string_param("origin",     "cube origin [x,y,z]（JSON数组字符串）", false)
            .with_string_param("size",       "cube size [x,y,z]（JSON数组字符串）", false)
            .with_string_param("uv",         "cube UV：[u,v] 或 per-face对象（JSON字符串）", false)
            .with_string_param("inflate",    "cube/骨骼膨胀量（数字，JSON字符串）", false)
            .with_string_param("cube_rotation","cube自身旋转 [x,y,z]（JSON数组字符串）", false)
            .with_string_param("cube_pivot", "cube自身旋转轴心 [x,y,z]（JSON数组字符串）", false)
            .with_string_param("position",   "定位点坐标 [x,y,z]（add_locator用，JSON数组字符串）", false)
            // 贴图尺寸
            .with_number_param("tex_width",  "贴图宽度（set_texture_size用）", false)
            .with_number_param("tex_height", "贴图高度（set_texture_size用）", false)
            .build(),
        [exec_one_op](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string fpath = p.value("file_path", "");
                if (fpath.empty()) return model_err("model_op 必须提供 file_path（绝对路径）");
                std::string id = p.value("geo_identifier", "");

                auto root = model_load_file(fpath);
                json& geo = find_geo(root, id);

                std::ostringstream out;

                // ── 批量模式：ops 数组 ──
                bool has_ops = p.contains("ops") && !p["ops"].is_null();
                if (has_ops) {
                    json ops_arr;
                    if (p["ops"].is_string()) {
                        ops_arr = json::parse(p["ops"].get<std::string>(), nullptr, true, true);
                    } else {
                        ops_arr = p["ops"];
                    }
                    if (!ops_arr.is_array())
                        return model_err("ops 必须是 JSON 数组，每项含 op 字段");

                    int n = static_cast<int>(ops_arr.size());
                    for (int i = 0; i < n; ++i) {
                        const json& item = ops_arr[i];
                        out << "[" << (i+1) << "/" << n << "] ";
                        out << exec_one_op(geo, item) << "\n";
                    }
                } else {
                    // ── 单次模式：op 参数 ──
                    std::string op = p.value("op", "");
                    if (op.empty()) return model_err("请提供 op（单次）或 ops（批量）参数");
                    out << exec_one_op(geo, p) << "\n";
                }

                model_save_file(fpath, root);
                out << "[已写回文件: " << fpath << "]";
                return model_ok(out.str());

            } catch (const std::exception& e) { return model_err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 5. model_mirror_symmetry — 完整骨骼对称复制（写回文件）
    // ════════════════════════════════════════════════════════
    srv.register_tool(
        mcp::tool_builder("model_mirror_symmetry")
            .with_description("将 source_bone 的所有 cube 沿 X 轴镜像，生成新的 target_bone 并写回文件。"
                              "适用于左右对称建模（如 leftArm → rightArm）。"
                              "镜像规则: pivot.x 取反, cube.origin.x = -origin.x - size.x")
            .with_string_param("file_path",      "geometry.json 的绝对路径（必须）", true)
            .with_string_param("source_bone",    "源骨骼名（如 leftArm）", true)
            .with_string_param("target_bone",    "目标骨骼名（如 rightArm）", true)
            .with_string_param("geo_identifier", "指定 geometry identifier，缺省第一个", false)
            .with_boolean_param("mirror_uv",     "是否镜像UV（默认 true）", false)
            .build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string fpath   = p.value("file_path", "");
                if (fpath.empty()) return model_err("必须提供 file_path");
                std::string src  = p.value("source_bone", "");
                std::string dst  = p.value("target_bone", "");
                std::string id   = p.value("geo_identifier", "");
                bool mirror_uv   = p.contains("mirror_uv") && p["mirror_uv"].is_boolean()
                                   ? p["mirror_uv"].get<bool>() : true;

                auto root = model_load_file(fpath);
                std::string result = model_mirror_symmetry(root, id, src, dst, mirror_uv);
                model_save_file(fpath, root);
                return model_ok(result + "\n[已写回文件: " + fpath + "]");
            } catch (const std::exception& e) { return model_err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 6. model_create_from_template — 从模板创建新模型
    // ════════════════════════════════════════════════════════
    srv.register_tool(
        mcp::tool_builder("model_create_from_template")
            .with_description(
                "从骨骼模板创建新的 geometry.json 文件并保存到 save_path。\n"
                "template 可选值:\n"
                "  blank      — 仅含 root 骨骼的空白模型\n"
                "  humanoid   — 标准人形骨骼（root/waist/body/head/hat/cape/jacket/"
                               "leftArm/leftSleeve/leftItem/rightArm/rightSleeve/rightItem/"
                               "leftLeg/leftPants/rightLeg/rightPants），参考 geometry.humanoid.custom\n"
                "  sword      — 近战武器骨骼（root/handle/guard/blade）\n"
                "  item_flat  — 平面物品（root/item）\n"
                "  quadruped  — 四足生物（root/body/head/tail/leg0~3）")
            .with_string_param("save_path",      "保存路径（绝对路径，如 D:/mod/RP/models/entity/sword.geo.json）", true)
            .with_string_param("identifier",     "几何体ID（如 'geometry.my_sword'）", true)
            .with_string_param("template",       "模板类型: blank/humanoid/sword/item_flat/quadruped", true)
            .with_number_param("texture_width",  "贴图宽度，默认64", false)
            .with_number_param("texture_height", "贴图高度，默认64", false)
            .build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string save_path  = p.value("save_path", "");
                std::string identifier = p.value("identifier", "");
                std::string tmpl       = p.value("template", "blank");
                int tw = p.contains("texture_width")  && p["texture_width"].is_number()  ? p["texture_width"].get<int>()  : 64;
                int th = p.contains("texture_height") && p["texture_height"].is_number() ? p["texture_height"].get<int>() : 64;

                if (save_path.empty())  return model_err("必须提供 save_path");
                if (identifier.empty()) return model_err("必须提供 identifier（如 geometry.my_model）");

                auto root = model_create_template(identifier, tw, th, tmpl);
                model_save_file(save_path, root);

                std::ostringstream ss;
                ss << "OK: created geometry '" << identifier << "' from template '" << tmpl << "'\n"
                   << "贴图: " << tw << "x" << th << "\n"
                   << "已保存: " << save_path << "\n"
                   << "提示: 使用 model_parse(file_path) 查看骨骼结构，然后用 model_op 添加 cubes";
                return model_ok(ss.str());
            } catch (const std::exception& e) { return model_err(e.what()); }
        });
}

} // namespace mcdk
