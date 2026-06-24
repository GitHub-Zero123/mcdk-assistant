#pragma once
// register_minecraft_model.hpp — 模型工具统一入口 `minecraft_model`
//
// 把原先 register_model.hpp 中 6 个独立模型工具合并为单一命令式 MCP tool：
//   get_model_reference / model_parse / model_get_bone / model_op /
//   model_mirror_symmetry / model_create_from_template
//
// 本文件只负责命令分发和 CLI 参数适配；骨骼/cube 增删改仍复用
// model_editor.hpp 的 exec_model_op 及 op_* 函数，不重写业务逻辑。
#include "tools/command_parser.hpp"
#include "tools/model_editor.hpp"
#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace mcdk {
namespace minecraft_model_detail {

constexpr const char* kToolName = "minecraft_model";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

// ── help 文本（含速查手册）──
inline std::string model_help_text() {
    return R"(minecraft_model — Minecraft 模型（geometry.json）工具统一入口（命令式用法）
用法: minecraft_model(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/parse，与 help/parse 等价。

【通用规则】
1. 参数含空格（如 JSON 文本）必须用 "..." 或 '...' 包裹成整体。
2. 大部分子命令支持 --file <绝对路径> 或 --content <JSON文本> 二选一指定模型内容；
   大文件推荐用 --file。Windows 路径请用正斜杠（D:/mod/models/entity/x.geo.json）。
3. --content 传 JSON 时推荐用单引号包裹（JSON 内部双引号无需转义）：
     parse --content '{"format_version":"1.12.0",...}'
4. 向量参数（pivot/rotation/origin/size/uv/position）在 op 的 --ops 数组里传 JSON 字符串：
     "pivot": "[0,0,0]"  （字符串形式，内部会 json::parse）
5. 写入类命令不会自动创建父目录，目标目录必须已存在。

【子命令】

  help
                               获取模型速查手册（文件格式/骨骼/cube/UV/locator/坐标系）+ 命令用法。

  parse (--file <路径> | --content <JSON>) [--id <geo标识符>]
                               解析模型，输出骨骼节点列表（name/parent/pivot/rotation/cubes数）。
                               --id 指定解析哪个 geometry（如 geometry.humanoid.custom），缺省第一个。

  bone (--file <路径> | --content <JSON>) --bone <骨骼名> [--id <geo标识符>]
                               获取指定骨骼的完整 JSON 定义（含所有 cube 的 origin/size/uv/inflate 等）。

  op --file <路径> [--id <geo标识符>] --ops '<JSON操作数组>'
                               对模型执行骨骼/cube 增删改，全部成功后一次写回文件。
                               --file 的父目录必须已存在。
                               --ops 为 JSON 数组字符串，每项含 op 字段，向量参数传 JSON 字符串:
                                 add_bone (bone_name, parent?, pivot, rotation?)
                                 remove_bone (bone_name, cascade?=true)
                                 rename_bone (bone_name, new_name)   同步更新子骨骼 parent 引用
                                 set_bone_transform (bone_name, pivot?, rotation?)
                                 add_cube (bone_name, origin, size, uv, inflate?, rotation?, cube_pivot?)
                                 remove_cube (bone_name, cube_index)
                                 edit_cube (bone_name, cube_index, origin?, size?, uv?, inflate?, rotation?, cube_pivot?)
                                 mirror_bone (bone_name, new_name, mirror_uv?=true)   X轴镜像
                                 set_texture_size (tex_width, tex_height)
                                 add_locator (bone_name, locator_name, position)
                               示例:
                                 op --file D:/mod/x.geo.json --ops '[{"op":"add_bone","bone_name":"head","pivot":"[0,8,0]"},{"op":"add_cube","bone_name":"head","origin":"[-4,8,-4]","size":"[8,8,8]","uv":"[0,0]"}]'

  mirror --file <路径> --src <骨骼名> --dst <骨骼名> [--id <geo标识符>] [--no-uv]
                               将 --src 骨骼的所有 cube 沿 X 轴镜像，生成 --dst 骨骼并写回文件。
                               --file 的父目录必须已存在。
                               适用于左右对称建模（如 leftArm → rightArm）。默认镜像 UV，--no-uv 关闭。
                               镜像规则: pivot.x 取反, cube.origin.x = -origin.x - size.x

  create --save <路径> --id <geo标识符> --template <类型> [--tw <宽>] [--th <高>]
                               从骨骼模板创建新模型并保存到 --save 路径。
                               --save 的父目录必须已存在。
                               --template 可选值:
                                 blank (仅 root 空白) | humanoid (标准人形) |
                                 sword (近战武器) | item_flat (平面物品) | quadruped (四足)
                               --tw/--th 贴图尺寸，默认 64x64。

flag 别名: --file=--path-file/--input, --content=--json, --id=--geo/--geo-id,
           --bone=--bone-name/--name, --src=--source, --dst=--target/--dst-bone,
           --save=--save-path/--out, --tw=--tex-width, --th=--tex-height
)";
}

inline std::string model_reference_text() {
    return R"(=== 基岩版 geometry.json 速查手册 ===

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

十、工具使用指引（本入口 minecraft_model 的子命令）
  先了解结构:   parse --file <path>          （列出所有骨骼）
  查看骨骼详情: bone --file <path> --bone <name>
  增删改:       op --file <path> --ops '[...]'   （批量，一次写回）
  对称建模:     mirror --file <path> --src leftArm --dst rightArm
  新建模型:     create --save <path> --id geometry.x --template humanoid
  典型工作流:   create → parse → op(add_bone/add_cube) → parse 验证
)";
}

inline mcp::json help_result() {
    return text_result(model_help_text() + "\n\n" + model_reference_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + model_help_text());
}

// ── flag 读取辅助 ──
inline std::string read_flag(const ParsedCommand& pc,
                             const std::vector<std::string>& aliases,
                             const std::string& def = "") {
    for (const auto& a : aliases) {
        auto it = pc.flags.find(a);
        if (it != pc.flags.end()) return it->second;
    }
    return def;
}

inline bool has_any_flag(const ParsedCommand& pc,
                         const std::vector<std::string>& aliases) {
    for (const auto& a : aliases)
        if (pc.flags.find(a) != pc.flags.end()) return true;
    return false;
}

inline int read_flag_int(const ParsedCommand& pc,
                         const std::vector<std::string>& aliases, int def) {
    for (const auto& a : aliases) {
        auto it = pc.flags.find(a);
        if (it != pc.flags.end() && !it->second.empty()) {
            try {
                size_t pos = 0;
                int v = std::stoi(it->second, &pos);
                if (pos == it->second.size()) return v;
            } catch (...) {}
        }
    }
    return def;
}

// 组装 model_resolve 所需的 params json（file_path 或 json_content）
inline nlohmann::json resolve_params(const ParsedCommand& pc) {
    std::string fpath = read_flag(pc, {"file", "path-file", "input"});
    std::string content = read_flag(pc, {"content", "json"});
    nlohmann::json params = nlohmann::json::object();
    if (!fpath.empty()) params["file_path"] = fpath;
    if (!content.empty()) params["json_content"] = content;
    return params;
}

// ── 各子命令分发 ──

inline mcp::json dispatch_parse(const ParsedCommand& pc) {
    nlohmann::json params = resolve_params(pc);
    if (params.empty())
        return error_with_help("parse 需要 --file 或 --content。");
    std::string id = read_flag(pc, {"id", "geo", "geo-id", "geo_identifier"});
    try {
        auto root = model_resolve(params);
        return text_result(model_parse_summary(root, id));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

inline mcp::json dispatch_bone(const ParsedCommand& pc) {
    nlohmann::json params = resolve_params(pc);
    if (params.empty())
        return error_with_help("bone 需要 --file 或 --content。");
    std::string bone = read_flag(pc, {"bone", "bone-name", "name"});
    if (bone.empty())
        return error_with_help("bone 需要 --bone <骨骼名>。");
    std::string id = read_flag(pc, {"id", "geo", "geo-id", "geo_identifier"});
    try {
        auto root = model_resolve(params);
        return text_result(model_get_bone_detail(root, bone, id));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

inline mcp::json dispatch_op(const ParsedCommand& pc) {
    std::string fpath = read_flag(pc, {"file", "path-file", "input"});
    if (fpath.empty())
        return error_with_help("op 需要 --file <绝对路径>（路径用正斜杠）。");
    std::string id = read_flag(pc, {"id", "geo", "geo-id", "geo_identifier"});
    std::string ops_str = read_flag(pc, {"ops"});
    if (ops_str.empty())
        return error_with_help("op 需要 --ops '<JSON操作数组>'。");
    try {
        auto ops = nlohmann::json::parse(ops_str);
        if (!ops.is_array())
            return error_with_help("--ops 必须是 JSON 数组。");
        auto root = model_load_file(fpath);
        nlohmann::json& geo = find_geo(root, id);
        std::ostringstream out;
        int n = static_cast<int>(ops.size());
        for (int i = 0; i < n; ++i) {
            out << "[" << (i + 1) << "/" << n << "] " << exec_model_op(geo, ops[i]) << "\n";
        }
        model_save_file(fpath, root);
        out << "[已写回文件: " << fpath << "]";
        return text_result(out.str());
    } catch (const nlohmann::json::parse_error& e) {
        return error_with_help(std::string("--ops JSON 解析失败: ") + e.what());
    } catch (const std::exception& e) {
        return error_with_help(e.what());
    }
}

inline mcp::json dispatch_mirror(const ParsedCommand& pc) {
    std::string fpath = read_flag(pc, {"file", "path-file", "input"});
    if (fpath.empty())
        return error_with_help("mirror 需要 --file <绝对路径>。");
    std::string src = read_flag(pc, {"src", "source"});
    std::string dst = read_flag(pc, {"dst", "target", "dst-bone"});
    if (src.empty() || dst.empty())
        return error_with_help("mirror 需要 --src <骨骼名> --dst <骨骼名>。");
    std::string id = read_flag(pc, {"id", "geo", "geo-id", "geo_identifier"});
    bool mirror_uv = !has_any_flag(pc, {"no-uv", "no-mirror-uv", "nouV"});
    try {
        auto root = model_load_file(fpath);
        std::string result = model_mirror_symmetry(root, id, src, dst, mirror_uv);
        model_save_file(fpath, root);
        return text_result(result + "\n[已写回文件: " + fpath + "]");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

inline mcp::json dispatch_create(const ParsedCommand& pc) {
    std::string save = read_flag(pc, {"save", "save-path", "out"});
    std::string id   = read_flag(pc, {"id", "geo", "geo-id", "identifier"});
    std::string tmpl = read_flag(pc, {"template", "t"});
    if (save.empty() || id.empty() || tmpl.empty())
        return error_with_help("create 需要 --save <路径> --id <geo标识符> --template <类型>。");
    int tw = read_flag_int(pc, {"tw", "tex-width", "texture_width"}, 64);
    int th = read_flag_int(pc, {"th", "tex-height", "texture_height"}, 64);
    try {
        auto root = model_create_template(id, tw, th, tmpl);
        model_save_file(save, root);
        std::ostringstream ss;
        ss << "OK: created geometry '" << id << "' from template '" << tmpl << "'\n"
           << "贴图: " << tw << "x" << th << "\n"
           << "已保存: " << save << "\n"
           << "提示: 使用 parse --file " << save << " 查看骨骼结构，然后用 op 添加 cubes";
        return text_result(ss.str());
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// ── 主分发器 ──
inline mcp::json dispatch_minecraft_model(const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = pc.sub;

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "reference" || sub == "ref")
        return text_result(model_reference_text());
    if (sub == "parse")
        return dispatch_parse(pc);
    if (sub == "bone")
        return dispatch_bone(pc);
    if (sub == "op")
        return dispatch_op(pc);
    if (sub == "mirror")
        return dispatch_mirror(pc);
    if (sub == "create")
        return dispatch_create(pc);

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_model_detail

inline void register_minecraft_model_tools(mcp::server& srv) {
    auto tool = mcp::tool_builder(minecraft_model_detail::kToolName)
        .with_description(
            "Minecraft 模型（geometry.json）工具统一入口：速查手册、模型解析（骨骼列表）、"
            "骨骼详情查看、骨骼/cube 增删改（批量 op）、X轴对称复制、模板创建。"
            "采用命令式用法，开发或修改模型时请先调用 command=\"help\" 查看完整命令与速查手册。")
        .with_string_param("command",
            "命令语句，如 'parse --file D:/mod/x.geo.json'、'op --file <path> --ops [...]' 或 "
            "'create --save <path> --id geometry.x --template humanoid'；首次使用请传 'help'。"
            "写入目标的父目录必须已存在。", true)
        .build();

    srv.register_tool(tool,
        [](const mcp::json& params, const std::string&) -> mcp::json {
            return minecraft_model_detail::dispatch_minecraft_model(params.value("command", ""));
        });
}

} // namespace mcdk
