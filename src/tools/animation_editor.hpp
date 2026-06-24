#pragma once
// animation_editor.hpp — 基岩版 animations.json 核心编辑逻辑
// 支持：解析 / 获取骨骼通道 / anim_op（增删改 keyframe/anim）
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

namespace mcdk {

using json = nlohmann::json;

// ── 文件 IO ────────────────────────────────────────────────

inline json anim_load_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("无法打开文件: " + path);
    try { return json::parse(f, nullptr, true, true); }
    catch (const std::exception& e) {
        throw std::runtime_error("JSON 解析失败: " + std::string(e.what()));
    }
}

inline void anim_save_file(const std::string& path, const json& root) {
    std::ofstream f(path);
    if (!f.is_open()) throw std::runtime_error("无法写入文件: " + path);
    f << root.dump(4);
}

// ── 参数解析辅助 ───────────────────────────────────────────

inline json anim_resolve(const json& params) {
    if (params.contains("file_path") && params["file_path"].is_string()) {
        std::string fp = params["file_path"].get<std::string>();
        if (!fp.empty()) return anim_load_file(fp);
    }
    if (params.contains("json_content") && params["json_content"].is_string()) {
        std::string s = params["json_content"].get<std::string>();
        if (!s.empty()) return json::parse(s, nullptr, true, true);
    }
    throw std::runtime_error("必须提供 file_path（绝对路径）或 json_content（JSON文本）之一");
}

// ── 内部辅助：获取 animations 对象 ────────────────────────

inline json& get_animations(json& root) {
    if (!root.contains("animations") || !root["animations"].is_object())
        throw std::runtime_error("文件中没有 'animations' 对象");
    return root["animations"];
}

inline const json& get_animations_const(const json& root) {
    if (!root.contains("animations") || !root["animations"].is_object())
        throw std::runtime_error("文件中没有 'animations' 对象");
    return root["animations"];
}

// 获取指定动画（不找则抛出）
inline json& get_anim(json& root, const std::string& anim_name) {
    json& anims = get_animations(root);
    if (!anims.contains(anim_name))
        throw std::runtime_error("未找到动画: " + anim_name);
    return anims[anim_name];
}

// 格式化骨骼通道值（数字/molang/keyframe对象）
inline std::string fmt_channel(const json& ch) {
    if (ch.is_null()) return "(none)";
    if (ch.is_array()) {
        // 直接值 [x,y,z] 或 molang 数组
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < ch.size(); ++i) {
            if (i) ss << ", ";
            if (ch[i].is_string()) ss << "\"" << ch[i].get<std::string>() << "\"";
            else ss << ch[i].dump();
        }
        ss << "]";
        return ss.str();
    }
    if (ch.is_object()) {
        // keyframe 对象 {"0.0": [...], "0.5": [...]}
        std::vector<std::string> keys;
        for (auto& [k, _] : ch.items()) keys.push_back(k);
        std::sort(keys.begin(), keys.end(), [](const std::string& a, const std::string& b) {
            try { return std::stod(a) < std::stod(b); } catch (...) { return a < b; }
        });
        std::ostringstream ss;
        ss << "keyframes(" << keys.size() << "): ";
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i) ss << " | ";
            ss << keys[i] << "s→";
            const auto& v = ch[keys[i]];
            if (v.is_object() && (v.contains("pre") || v.contains("post"))) {
                ss << "{easing}";
            } else {
                ss << fmt_channel(v);
            }
        }
        return ss.str();
    }
    if (ch.is_string()) return "\"" + ch.get<std::string>() + "\"";
    return ch.dump();
}

// ── anim_parse_summary：解析动画文件摘要 ─────────────────

inline std::string anim_parse_summary(const json& root, const std::string& filter = "") {
    std::ostringstream ss;

    if (!root.contains("animations") || !root["animations"].is_object()) {
        return "[ERROR] 文件中没有 'animations' 对象\n";
    }
    const json& anims = root["animations"];

    ss << "=== animations.json | 动画数量:" << anims.size() << " ===\n";

    for (auto& [anim_name, anim] : anims.items()) {
        // 支持按名称过滤
        if (!filter.empty() && anim_name.find(filter) == std::string::npos) continue;

        ss << "\n[" << anim_name << "]\n";

        // 基本属性
        std::string loop_str = "false";
        if (anim.contains("loop")) {
            if (anim["loop"].is_boolean()) loop_str = anim["loop"].get<bool>() ? "true" : "false";
            else if (anim["loop"].is_string()) loop_str = "\"" + anim["loop"].get<std::string>() + "\"";
        }
        ss << "  loop=" << loop_str;

        if (anim.contains("animation_length"))
            ss << "  时长=" << anim["animation_length"].dump() << "s";
        if (anim.contains("blend_weight"))
            ss << "  blend_weight=" << anim["blend_weight"].dump();
        if (anim.contains("override_previous_animation"))
            ss << "  override_prev=" << anim["override_previous_animation"].dump();
        ss << "\n";

        // 骨骼通道摘要
        if (anim.contains("bones") && anim["bones"].is_object()) {
            ss << "  骨骼通道（可用于 anim_op 的 bone_name）:\n";
            for (auto& [bone, channels] : anim["bones"].items()) {
                ss << "    " << bone << " —";
                bool has_rot = channels.contains("rotation");
                bool has_pos = channels.contains("position");
                bool has_scl = channels.contains("scale");
                if (has_rot) {
                    const auto& r = channels["rotation"];
                    if (r.is_object()) ss << " rotation:" << r.size() << "帧";
                    else ss << " rotation:constant";
                }
                if (has_pos) {
                    const auto& p = channels["position"];
                    if (p.is_object()) ss << " position:" << p.size() << "帧";
                    else ss << " position:constant";
                }
                if (has_scl) {
                    const auto& s = channels["scale"];
                    if (s.is_object()) ss << " scale:" << s.size() << "帧";
                    else ss << " scale:constant";
                }
                ss << "\n";
            }
        } else {
            ss << "  (无骨骼通道)\n";
        }

        // 动画事件
        if (anim.contains("anim_time_update"))
            ss << "  anim_time_update: " << anim["anim_time_update"].dump() << "\n";
        if (anim.contains("timeline") && anim["timeline"].is_object())
            ss << "  timeline事件: " << anim["timeline"].size() << "个\n";
    }

    return ss.str();
}

// ── anim_get_bone_channel：获取指定骨骼的完整通道数据 ────

inline std::string anim_get_bone_channel(const json& root,
                                          const std::string& anim_name,
                                          const std::string& bone_name,
                                          const std::string& channel = "") {
    json root_copy = root;
    json& anim = get_anim(root_copy, anim_name);

    if (!anim.contains("bones") || !anim["bones"].is_object())
        throw std::runtime_error("动画 '" + anim_name + "' 没有 bones 数据");
    if (!anim["bones"].contains(bone_name))
        throw std::runtime_error("骨骼 '" + bone_name + "' 在动画 '" + anim_name + "' 中不存在");

    const json& bone_data = anim["bones"][bone_name];
    std::ostringstream ss;
    ss << "=== " << anim_name << " / " << bone_name << " ===\n";

    auto print_channel = [&](const std::string& ch_name) {
        if (bone_data.contains(ch_name)) {
            ss << ch_name << ":\n";
            const json& ch = bone_data[ch_name];
            if (ch.is_object()) {
                // keyframe 对象，按时间排序输出
                std::vector<std::string> keys;
                for (auto& [k, _] : ch.items()) keys.push_back(k);
                std::sort(keys.begin(), keys.end(), [](const std::string& a, const std::string& b) {
                    try { return std::stod(a) < std::stod(b); } catch (...) { return a < b; }
                });
                for (auto& t : keys) {
                    const json& kf = ch[t];
                    if (kf.is_object() && (kf.contains("pre") || kf.contains("post"))) {
                        ss << "  " << t << "s: {pre=" << kf.value("pre", json(nullptr)).dump()
                           << ", post=" << kf.value("post", json(nullptr)).dump()
                           << ", lerp_mode=" << kf.value("lerp_mode", "linear") << "}\n";
                    } else {
                        ss << "  " << t << "s: " << fmt_channel(kf) << "\n";
                    }
                }
            } else {
                ss << "  constant: " << fmt_channel(ch) << "\n";
            }
        }
    };

    if (channel.empty() || channel == "rotation")  print_channel("rotation");
    if (channel.empty() || channel == "position")  print_channel("position");
    if (channel.empty() || channel == "scale")     print_channel("scale");

    return ss.str();
}

// ── anim_op 实现 ──────────────────────────────────────────

// 解析 keyframe 值（可以是数字数组、molang字符串数组、或单字符串）
// 支持格式：
//   "[0, -30, 0]"          → 数字数组
//   "[\"molang\", 0, 0]"   → molang数组
//   {"pre":[...],"post":[...]} → easing keyframe（直接JSON）
inline json parse_kf_value(const json& val) {
    if (val.is_null()) return json(nullptr);
    if (val.is_array() || val.is_object()) return val;
    if (val.is_string()) {
        std::string s = val.get<std::string>();
        if (s.empty()) return json(nullptr);
        // 尝试解析为JSON数组
        try {
            json parsed = json::parse(s, nullptr, true, true);
            return parsed;
        } catch (...) {
            // 不是JSON，当作单一molang字符串直接返回字符串
            return val;
        }
    }
    return val;
}

// set_keyframe：设置指定时间点的骨骼通道值
inline std::string op_anim_set_keyframe(json& anim,
                                         const std::string& bone_name,
                                         const std::string& channel,
                                         const std::string& time_str,
                                         const json& value) {
    if (!anim.contains("bones")) anim["bones"] = json::object();
    if (!anim["bones"].contains(bone_name)) anim["bones"][bone_name] = json::object();

    json& bone = anim["bones"][bone_name];
    json kf_val = parse_kf_value(value);
    if (kf_val.is_null()) throw std::runtime_error("set_keyframe 需要 value 参数");

    // 如果通道目前是 constant（数组），先转为 keyframe 对象
    if (bone.contains(channel) && bone[channel].is_array()) {
        json old_val = bone[channel];
        bone[channel] = json::object();
        bone[channel]["0.0"] = old_val;
    }
    if (!bone.contains(channel) || !bone[channel].is_object()) {
        bone[channel] = json::object();
    }

    bone[channel][time_str] = kf_val;

    return "OK: set keyframe [" + anim.value("_name_hint_", bone_name) + "] "
           + bone_name + "." + channel + " @" + time_str + "s = " + kf_val.dump();
}

// remove_keyframe：删除指定时间点
inline std::string op_anim_remove_keyframe(json& anim,
                                            const std::string& bone_name,
                                            const std::string& channel,
                                            const std::string& time_str) {
    if (!anim.contains("bones") || !anim["bones"].contains(bone_name))
        throw std::runtime_error("骨骼 '" + bone_name + "' 不存在");
    json& bone = anim["bones"][bone_name];
    if (!bone.contains(channel) || !bone[channel].is_object())
        throw std::runtime_error("通道 '" + channel + "' 不是 keyframe 对象");
    if (!bone[channel].contains(time_str))
        throw std::runtime_error("时间点 " + time_str + " 不存在");

    bone[channel].erase(time_str);
    // 如果 keyframe 清空了，删除整个通道
    if (bone[channel].empty()) bone.erase(channel);
    // 如果骨骼没有任何通道了，删除骨骼
    if (bone.empty()) anim["bones"].erase(bone_name);

    return "OK: removed keyframe " + bone_name + "." + channel + " @" + time_str + "s";
}

// set_constant：设置通道为常量值（非 keyframe 对象）
inline std::string op_anim_set_constant(json& anim,
                                         const std::string& bone_name,
                                         const std::string& channel,
                                         const json& value) {
    if (!anim.contains("bones")) anim["bones"] = json::object();
    if (!anim["bones"].contains(bone_name)) anim["bones"][bone_name] = json::object();
    json kf_val = parse_kf_value(value);
    if (kf_val.is_null()) throw std::runtime_error("set_constant 需要 value 参数");
    anim["bones"][bone_name][channel] = kf_val;
    return "OK: set constant " + bone_name + "." + channel + " = " + kf_val.dump();
}

// remove_bone_channel：删除整个骨骼或某通道
inline std::string op_anim_remove_bone_channel(json& anim,
                                                const std::string& bone_name,
                                                const std::string& channel) {
    if (!anim.contains("bones") || !anim["bones"].contains(bone_name))
        throw std::runtime_error("骨骼 '" + bone_name + "' 不存在");
    if (channel.empty()) {
        anim["bones"].erase(bone_name);
        return "OK: removed bone channel '" + bone_name + "' (all channels)";
    }
    json& bone = anim["bones"][bone_name];
    if (!bone.contains(channel))
        throw std::runtime_error("通道 '" + channel + "' 不存在");
    bone.erase(channel);
    if (bone.empty()) anim["bones"].erase(bone_name);
    return "OK: removed " + bone_name + "." + channel;
}

// mirror_bone_rotation：将 source_bone 的 rotation keyframe X轴取反复制到 target_bone
// 用于从左腿动画生成右腿（相位偏移 phase_offset 秒）
inline std::string op_anim_mirror_bone_rotation(json& anim,
                                                  const std::string& src_bone,
                                                  const std::string& dst_bone,
                                                  double phase_offset,
                                                  double anim_length) {
    if (!anim.contains("bones") || !anim["bones"].contains(src_bone))
        throw std::runtime_error("源骨骼 '" + src_bone + "' 不存在");
    const json& src = anim["bones"][src_bone];
    if (!src.contains("rotation"))
        throw std::runtime_error("源骨骼 '" + src_bone + "' 没有 rotation 通道");

    if (!anim.contains("bones")) anim["bones"] = json::object();
    if (!anim["bones"].contains(dst_bone)) anim["bones"][dst_bone] = json::object();
    json& dst = anim["bones"][dst_bone];

    const json& rot = src["rotation"];
    json new_rot = json::object();

    if (rot.is_object()) {
        for (auto& [t_str, kf] : rot.items()) {
            double t = 0;
            try { t = std::stod(t_str); } catch (...) {}
            // 应用相位偏移，并对 animation_length 取模
            double new_t = t + phase_offset;
            if (anim_length > 0) {
                while (new_t >= anim_length) new_t -= anim_length;
                while (new_t < 0) new_t += anim_length;
            }
            // 时间格式化（保留4位小数，去掉末尾0）
            char buf[32];
            snprintf(buf, sizeof(buf), "%.4g", new_t);
            std::string new_t_str = buf;

            // X轴取反（镜像左右腿）
            json new_kf = kf;
            if (kf.is_array() && kf.size() >= 1) {
                new_kf = kf;
                if (kf[0].is_number()) new_kf[0] = -kf[0].get<double>();
                // Y/Z保持不变
            }
            new_rot[new_t_str] = new_kf;
        }
    } else if (rot.is_array()) {
        // constant rotation，直接X取反
        json new_kf = rot;
        if (rot.size() >= 1 && rot[0].is_number())
            new_kf[0] = -rot[0].get<double>();
        dst["rotation"] = new_kf;
        return "OK: mirror_bone_rotation (constant) " + src_bone + " -> " + dst_bone;
    }

    dst["rotation"] = new_rot;
    return "OK: mirror_bone_rotation " + src_bone + " -> " + dst_bone
           + " (phase_offset=" + std::to_string(phase_offset) + "s, " + std::to_string(new_rot.size()) + " keyframes)";
}

// add_anim：新增动画条目
inline std::string op_anim_add_anim(json& root,
                                     const std::string& anim_name,
                                     bool loop,
                                     const std::string& loop_str,
                                     double animation_length) {
    if (!root.contains("animations")) root["animations"] = json::object();
    if (root["animations"].contains(anim_name))
        throw std::runtime_error("动画 '" + anim_name + "' 已存在，请先删除或用 set_anim_prop 修改");

    json anim = json::object();
    // loop 值处理
    if (!loop_str.empty()) {
        anim["loop"] = loop_str;  // "hold_on_last_frame" 等字符串
    } else {
        anim["loop"] = loop;
    }
    if (animation_length > 0) anim["animation_length"] = animation_length;
    anim["bones"] = json::object();
    root["animations"][anim_name] = anim;

    std::ostringstream ss;
    ss << "OK: added animation '" << anim_name << "'"
       << " loop=" << (loop_str.empty() ? (loop ? "true" : "false") : loop_str);
    if (animation_length > 0) ss << " 时长=" << animation_length << "s";
    return ss.str();
}

// remove_anim：删除动画
inline std::string op_anim_remove_anim(json& root, const std::string& anim_name) {
    if (!root.contains("animations") || !root["animations"].contains(anim_name))
        throw std::runtime_error("动画 '" + anim_name + "' 不存在");
    root["animations"].erase(anim_name);
    return "OK: removed animation '" + anim_name + "'";
}

// set_anim_prop：修改动画属性
inline std::string op_anim_set_prop(json& anim,
                                     const std::string& prop,
                                     const json& value) {
    if (prop == "loop" || prop == "animation_length" || prop == "blend_weight"
        || prop == "override_previous_animation" || prop == "anim_time_update"
        || prop == "start_delay" || prop == "loop_delay") {
        anim[prop] = value;
        return "OK: set " + prop + " = " + value.dump();
    }
    throw std::runtime_error("不支持的属性: " + prop +
        "  有效值: loop/animation_length/blend_weight/override_previous_animation/"
        "anim_time_update/start_delay/loop_delay");
}

// scale_time：整体拉伸/压缩所有 keyframe 时间轴（time_scale 倍数）
inline std::string op_anim_scale_time(json& anim, double time_scale) {
    if (time_scale <= 0) throw std::runtime_error("time_scale 必须大于 0");
    if (!anim.contains("bones")) return "OK: no bones to scale";

    int count = 0;
    for (auto& [bone_name, bone] : anim["bones"].items()) {
        for (auto& ch_name : {"rotation", "position", "scale"}) {
            if (bone.contains(ch_name) && bone[ch_name].is_object()) {
                json new_ch = json::object();
                for (auto& [t_str, kf] : bone[ch_name].items()) {
                    double t = 0;
                    try { t = std::stod(t_str); } catch (...) {}
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%.4g", t * time_scale);
                    new_ch[buf] = kf;
                    ++count;
                }
                bone[ch_name] = new_ch;
            }
        }
    }

    // 同步缩放 animation_length
    if (anim.contains("animation_length") && anim["animation_length"].is_number())
        anim["animation_length"] = anim["animation_length"].get<double>() * time_scale;

    return "OK: scaled time x" + std::to_string(time_scale) + " (" + std::to_string(count) + " keyframes)";
}

// create_animation_file：创建新的 animations.json 文件
inline json anim_create_file() {
    json root;
    root["format_version"] = "1.8.0";
    root["animations"] = json::object();
    return root;
}

// ── exec_anim_op：执行单个 op（anim_op 工具的核心分发）──────────
// 从 register_animation.hpp 的 exec_one_anim_op lambda 抽取，新旧入口共用。
// src 是一个 json 对象，含 "op" 字段及该 op 所需的参数。
// 向量/通道值参数（value 等）支持传 JSON 字符串或原生 JSON 值。
// 失败时抛 runtime_error，由调用方捕获。
inline std::string exec_anim_op(json& root, const json& src) {
    // 内部辅助：从 src 解析 JSON 参数（字符串则 parse，否则原样返回）
    auto pj = [&src](const char* key) -> json {
        if (!src.contains(key) || src[key].is_null()) return json(nullptr);
        if (src[key].is_string()) {
            std::string s = src[key].get<std::string>();
            if (s.empty()) return json(nullptr);
            try { return json::parse(s, nullptr, true, true); } catch (...) {}
        }
        return src[key];
    };

    std::string op        = src.value("op", "");
    std::string anim_name = src.value("anim_name", "");

    if (op == "add_anim") {
        bool loop            = src.value("loop", true);
        std::string loop_str = src.value("loop_str", "");
        double anim_length   = src.contains("animation_length") && src["animation_length"].is_number()
                               ? src["animation_length"].get<double>() : 0.0;
        if (anim_name.empty()) throw std::runtime_error("add_anim 需要 anim_name");
        return op_anim_add_anim(root, anim_name, loop, loop_str, anim_length);

    } else if (op == "remove_anim") {
        if (anim_name.empty()) throw std::runtime_error("remove_anim 需要 anim_name");
        return op_anim_remove_anim(root, anim_name);

    } else if (op == "set_anim_prop") {
        if (anim_name.empty()) throw std::runtime_error("set_anim_prop 需要 anim_name");
        std::string prop = src.value("prop", "");
        if (prop.empty()) throw std::runtime_error("set_anim_prop 需要 prop 参数");
        json value = pj("value");
        if (value.is_null()) throw std::runtime_error("set_anim_prop 需要 value 参数");
        json& anim = get_anim(root, anim_name);
        return op_anim_set_prop(anim, prop, value);

    } else if (op == "set_keyframe") {
        if (anim_name.empty()) throw std::runtime_error("set_keyframe 需要 anim_name");
        std::string bone_name = src.value("bone_name", "");
        std::string channel   = src.value("channel", "rotation");
        std::string time_str  = src.value("time", "");
        if (bone_name.empty()) throw std::runtime_error("set_keyframe 需要 bone_name");
        if (time_str.empty())  throw std::runtime_error("set_keyframe 需要 time（秒，如 '0.3'）");
        json value = pj("value");
        if (value.is_null()) throw std::runtime_error("set_keyframe 需要 value");
        json& anim = get_anim(root, anim_name);
        return op_anim_set_keyframe(anim, bone_name, channel, time_str, value);

    } else if (op == "remove_keyframe") {
        if (anim_name.empty()) throw std::runtime_error("remove_keyframe 需要 anim_name");
        std::string bone_name = src.value("bone_name", "");
        std::string channel   = src.value("channel", "rotation");
        std::string time_str  = src.value("time", "");
        if (bone_name.empty()) throw std::runtime_error("remove_keyframe 需要 bone_name");
        if (time_str.empty())  throw std::runtime_error("remove_keyframe 需要 time");
        json& anim = get_anim(root, anim_name);
        return op_anim_remove_keyframe(anim, bone_name, channel, time_str);

    } else if (op == "set_constant") {
        if (anim_name.empty()) throw std::runtime_error("set_constant 需要 anim_name");
        std::string bone_name = src.value("bone_name", "");
        std::string channel   = src.value("channel", "rotation");
        if (bone_name.empty()) throw std::runtime_error("set_constant 需要 bone_name");
        json value = pj("value");
        if (value.is_null()) throw std::runtime_error("set_constant 需要 value");
        json& anim = get_anim(root, anim_name);
        return op_anim_set_constant(anim, bone_name, channel, value);

    } else if (op == "remove_bone_channel") {
        if (anim_name.empty()) throw std::runtime_error("remove_bone_channel 需要 anim_name");
        std::string bone_name = src.value("bone_name", "");
        std::string channel   = src.value("channel", "");
        if (bone_name.empty()) throw std::runtime_error("remove_bone_channel 需要 bone_name");
        json& anim = get_anim(root, anim_name);
        return op_anim_remove_bone_channel(anim, bone_name, channel);

    } else if (op == "mirror_bone_rotation") {
        if (anim_name.empty()) throw std::runtime_error("mirror_bone_rotation 需要 anim_name");
        std::string src_bone  = src.value("source_bone", "");
        std::string dst_bone  = src.value("target_bone", "");
        double phase_offset   = src.contains("phase_offset") && src["phase_offset"].is_number()
                                ? src["phase_offset"].get<double>() : 0.0;
        double anim_length    = src.contains("anim_length") && src["anim_length"].is_number()
                                ? src["anim_length"].get<double>() : 0.0;
        if (src_bone.empty()) throw std::runtime_error("mirror_bone_rotation 需要 source_bone");
        if (dst_bone.empty()) throw std::runtime_error("mirror_bone_rotation 需要 target_bone");
        json& anim = get_anim(root, anim_name);
        return op_anim_mirror_bone_rotation(anim, src_bone, dst_bone, phase_offset, anim_length);

    } else if (op == "scale_time") {
        if (anim_name.empty()) throw std::runtime_error("scale_time 需要 anim_name");
        double time_scale = src.contains("time_scale") && src["time_scale"].is_number()
                            ? src["time_scale"].get<double>() : 1.0;
        json& anim = get_anim(root, anim_name);
        return op_anim_scale_time(anim, time_scale);

    } else {
        throw std::runtime_error("未知 op: " + op +
            "  有效值: add_anim/remove_anim/set_anim_prop/set_keyframe/remove_keyframe/"
            "set_constant/remove_bone_channel/mirror_bone_rotation/scale_time");
    }
}

} // namespace mcdk
