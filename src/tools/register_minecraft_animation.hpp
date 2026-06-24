#pragma once
// register_minecraft_animation.hpp — 动画工具统一入口 `minecraft_animation`
//
// 把原先 register_animation.hpp 中 4 个独立动画工具合并为单一命令式 MCP tool：
//   get_animation_reference / animation_parse / animation_get_bone_channel / anim_op
//
// 本文件只负责命令分发和 CLI 参数适配；动画增删改仍复用
// animation_editor.hpp 的 exec_anim_op 及 op_anim_* 函数，不重写业务逻辑。
#include "tools/command_parser.hpp"
#include "tools/animation_editor.hpp"
#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <nlohmann/json.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace mcdk {
namespace minecraft_animation_detail {

constexpr const char* kToolName = "minecraft_animation";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

// ── help 文本（含速查手册）──
inline std::string animation_help_text() {
    return R"(minecraft_animation — Minecraft 动画（animations.json）工具统一入口（命令式用法）
用法: minecraft_animation(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/parse，与 help/parse 等价。

【通用规则】
1. 参数含空格（如 JSON 文本）必须用 "..." 或 '...' 包裹成整体。
2. 大部分子命令支持 --file <绝对路径> 或 --content <JSON文本> 二选一指定动画内容；
   大文件推荐用 --file。Windows 路径请用正斜杠（D:/mod/animations/x.anim.json）。
3. --content 传 JSON 时推荐用单引号包裹（JSON 内部双引号无需转义）。
4. 通道值（value）在 op 的 --ops 数组里传 JSON 字符串:
     "value": "[0,-30,0]"  或  "value": "[\"math.sin(q.anim_time*180)*30\",0,0]"
5. 写入类命令不会自动创建父目录，目标目录必须已存在。

【子命令】

  help
                               获取动画速查手册（文件格式/keyframe/Molang/走路公式）+ 命令用法。

  parse (--file <路径> | --content <JSON>) [--filter <关键词>]
                               解析动画文件，输出所有动画摘要（loop/时长/骨骼通道/帧数）。
                               --filter 按动画名称关键词过滤（可选）。

  bone (--file <路径> | --content <JSON>) --anim <动画名> --bone <骨骼名> [--channel rotation|position|scale]
                               获取指定动画中某骨骼的完整 keyframe 数据（精确时间+值）。
                               --channel 不填则输出该骨骼全部通道。

  op --file <路径> --ops '<JSON操作数组>'
                               对动画文件执行增删改，全部成功后一次写回文件。
                               文件不存在时自动创建空动画文件，但 --file 的父目录必须已存在。
                               --ops 为 JSON 数组字符串，每项含 op 字段:
                                 add_anim (anim_name, loop?=true, loop_str?, animation_length?)
                                   loop 取值: true(循环) | false(单次) | "hold_on_last_frame"(停在末帧)
                                 remove_anim (anim_name)
                                 set_anim_prop (anim_name, prop, value)
                                   prop: loop/animation_length/blend_weight/override_previous_animation/
                                         anim_time_update/start_delay/loop_delay
                                 set_keyframe (anim_name, bone_name, channel='rotation', time, value)
                                   time: 秒字符串如 "0.3"; value: JSON数组字符串如 "[0,-30,0]"
                                          或 molang 数组 "[\"math.sin(q.anim_time*180)*30\",0,0]"
                                 remove_keyframe (anim_name, bone_name, channel, time)
                                 set_constant (anim_name, bone_name, channel, value)
                                 remove_bone_channel (anim_name, bone_name, channel?)
                                 mirror_bone_rotation (anim_name, source_bone, target_bone, phase_offset?=0, anim_length?)
                                 scale_time (anim_name, time_scale)
                               示例:
                                 op --file D:/mod/x.anim.json --ops '[{"op":"add_anim","anim_name":"animation.zombie.walk","loop":true,"animation_length":1.2},{"op":"set_keyframe","anim_name":"animation.zombie.walk","bone_name":"leftLeg","channel":"rotation","time":"0.0","value":"[0,0,0]"}]'

flag 别名: --file=--path-file/--input, --content=--json, --anim=--anim-name/--animation,
           --bone=--bone-name/--name, --channel=--ch, --filter=--kw/--f
)";
}

inline std::string animation_reference_text() {
    return R"(=== 基岩版 animations.json 速查手册 ===

一、文件格式
  {
    "format_version": "1.8.0",   ← 推荐（兼容性最佳）
    "animations": {
      "animation.xxx.yyy": { ... },
      ...
    }
  }
  命名规范: 必须以 "animation." 开头，只能小写+点+下划线

二、动画对象属性
  loop                bool | "hold_on_last_frame"
                      — true: 循环播放（走路/待机）
                      — false: 播放一次
                      — "hold_on_last_frame": 播放后停在最后一帧（睡眠/死亡）
  animation_length    float  — 动画总时长（秒），不填则自动取最大keyframe时间
  blend_weight        float | molang — 混合权重（0=不影响，1=完全覆盖）
  override_previous_animation bool — 是否覆盖前序动画
  anim_time_update    molang — 自定义时间推进表达式（默认=query.anim_time）
  start_delay         float  — 开始延迟（秒）
  loop_delay          float  — 循环延迟（秒）

三、bones 骨骼通道
  "bones": {
    "boneName": {
      "rotation": <channel_value>,
      "position": <channel_value>,
      "scale":    <channel_value>
    }
  }
  rotation 单位：度（°），基准=骨骼在 geometry.json 中的 rotation
  position 单位：模型空间格，叠加在 geometry.json pivot 基础上
  scale    单位：倍数（1.0=原始大小）

四、channel_value 通道值格式
  1. 常量值（数组）: [x, y, z]
     示例: "rotation": [0, -90, 0]

  2. 常量 Molang（数组内字符串）:
     "rotation": ["math.sin(q.anim_time * 180) * 30", 0, 0]

  3. Keyframe 对象:
     "rotation": {
       "0.0":  [0, 0, 0],
       "0.3":  [-30, 0, 0],
       "0.6":  [0, 0, 0]
     }
     键名为时间（秒，字符串），值为 [x,y,z] 或 molang 数组

  4. Easing Keyframe（带插值控制）:
     "0.3": {
       "pre":  [0, 0, 0],
       "post": [-30, 0, 0],
       "lerp_mode": "catmullrom"   ← linear(默认)/catmullrom
     }

五、常用 Molang 查询（动画中最常用）
  q.anim_time              — 当前动画时间（秒，0→animation_length循环）
  q.life_time              — 实体存活时间（秒，不受loop影响）
  q.modified_distance_moved — 实体移动的距离（适合走路动画速度同步）
  q.modified_move_speed    — 移动速度（0=静止）
  q.is_on_ground           — 是否在地面（bool）
  q.is_moving              — 是否移动中（bool）
  variable.attack_time     — 攻击动画时间（0→1）
  math.sin(x)              — 正弦，x单位是度
  math.cos(x)              — 余弦
  math.abs(x)              — 绝对值

六、走路动画经典 Molang 公式
  腿部左右摆动（基于移动距离，静止自动停）:
    rotation: ["math.sin(q.modified_distance_moved * 38.17) * 30 * q.modified_move_speed", 0, 0]

  手臂反向摆动（与腿相反）:
    rotation: ["-math.sin(q.modified_distance_moved * 38.17) * 30 * q.modified_move_speed", 0, 0]

  完美循环正弦（基于 anim_time，速度150对应时长2.4s）:
    rotation: ["math.sin(q.anim_time * 150) * 30", 0, 0]

七、实体动画引用（client_entity.json）
  "animations": {
    "walk": "animation.my_entity.walk",
    "attack": "animation.my_entity.attack"
  },
  "scripts": {
    "animate": [
      {"walk": "q.modified_move_speed > 0"},
      "attack"
    ]
  }

八、工具使用指引（本入口 minecraft_animation 的子命令）
  先了解结构: parse --file <path>          （列出所有动画）
  查看骨骼通道: bone --file <path> --anim <name> --bone <name>
  增删改:       op --file <path> --ops '[...]'   （批量，一次写回）
  典型工作流:   op(add_anim + set_keyframe) → parse 验证 → bone 检查关键帧
)";
}

inline mcp::json help_result() {
    return text_result(animation_help_text() + "\n\n" + animation_reference_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + animation_help_text());
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

// 组装 anim_resolve 所需的 params json
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
    std::string filter = read_flag(pc, {"filter", "kw", "f"});
    try {
        auto root = anim_resolve(params);
        return text_result(anim_parse_summary(root, filter));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

inline mcp::json dispatch_bone(const ParsedCommand& pc) {
    nlohmann::json params = resolve_params(pc);
    if (params.empty())
        return error_with_help("bone 需要 --file 或 --content。");
    std::string anim = read_flag(pc, {"anim", "anim-name", "animation"});
    std::string bone = read_flag(pc, {"bone", "bone-name", "name"});
    if (anim.empty() || bone.empty())
        return error_with_help("bone 需要 --anim <动画名> --bone <骨骼名>。");
    std::string channel = read_flag(pc, {"channel", "ch"});
    try {
        auto root = anim_resolve(params);
        return text_result(anim_get_bone_channel(root, anim, bone, channel));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

inline mcp::json dispatch_op(const ParsedCommand& pc) {
    std::string fpath = read_flag(pc, {"file", "path-file", "input"});
    if (fpath.empty())
        return error_with_help("op 需要 --file <绝对路径>（路径用正斜杠）。");
    std::string ops_str = read_flag(pc, {"ops"});
    if (ops_str.empty())
        return error_with_help("op 需要 --ops '<JSON操作数组>'。");
    try {
        auto ops = nlohmann::json::parse(ops_str);
        if (!ops.is_array())
            return error_with_help("--ops 必须是 JSON 数组。");
        // 文件不存在时自动创建（与原 anim_op 行为一致）
        nlohmann::json root;
        try {
            root = anim_load_file(fpath);
        } catch (...) {
            root = anim_create_file();
        }
        std::ostringstream out;
        int n = static_cast<int>(ops.size());
        for (int i = 0; i < n; ++i) {
            out << "[" << (i + 1) << "/" << n << "] " << exec_anim_op(root, ops[i]) << "\n";
        }
        anim_save_file(fpath, root);
        out << "[已写回文件: " << fpath << "]";
        return text_result(out.str());
    } catch (const nlohmann::json::parse_error& e) {
        return error_with_help(std::string("--ops JSON 解析失败: ") + e.what());
    } catch (const std::exception& e) {
        return error_with_help(e.what());
    }
}

// ── 主分发器 ──
inline mcp::json dispatch_minecraft_animation(const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = pc.sub;

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "reference" || sub == "ref")
        return text_result(animation_reference_text());
    if (sub == "parse")
        return dispatch_parse(pc);
    if (sub == "bone")
        return dispatch_bone(pc);
    if (sub == "op")
        return dispatch_op(pc);

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_animation_detail

inline void register_minecraft_animation_tools(mcp::server& srv) {
    auto tool = mcp::tool_builder(minecraft_animation_detail::kToolName)
        .with_description(
            "Minecraft 动画（animations.json）工具统一入口：速查手册、动画解析（摘要）、"
            "骨骼通道 keyframe 查看、动画增删改（批量 op）。"
            "采用命令式用法，开发或修改动画时请先调用 command=\"help\" 查看完整命令与速查手册。")
        .with_string_param("command",
            "命令语句，如 'parse --file D:/mod/x.anim.json'、'bone --file <path> --anim <name> --bone <name>' 或 "
            "'op --file <path> --ops [...]'；首次使用请传 'help'。"
            "写入目标的父目录必须已存在。", true)
        .build();

    srv.register_tool(tool,
        [](const mcp::json& params, const std::string&) -> mcp::json {
            return minecraft_animation_detail::dispatch_minecraft_animation(params.value("command", ""));
        });
}

} // namespace mcdk
