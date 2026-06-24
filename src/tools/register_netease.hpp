#pragma once
// register_netease.hpp — 网易版差异说明 & JSON UI 内置组件库 参考文本
//
// 【已合并】原 get_netease_diff / get_netease_jsonui 两个独立工具，现统一并入
// minecraft_docs 单工具的 `netease diff` / `netease jsonui` 子命令
// （见 register_minecraft_docs.hpp）。本文件保留两段参考文本为自由函数供路由器取用；
// 下方 register_netease_tools() 的注册代码整段注释保留，便于随时审视/回滚。
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>

namespace mcdk {

// ── 网易版 ↔ 国际版差异速查文本 ──
inline const char* netease_diff_text() {
    static const char* NETEASE_DIFF_TEXT =
        "网易中国版基岩版与国际版基岩版的关键差异（当Wiki文档提到以下国际版路径时，请自动替换为网易版路径）：\n"
        "\n"
        "[行为包目录映射]\n"
        "国际版 BP/items/        -> 网易版 BP/netease_items_beh/\n"
        "国际版 BP/blocks/       -> 网易版 BP/netease_blocks/\n"
        "国际版 BP/biomes/       -> 网易版 BP/netease_biomes/\n"
        "国际版 BP/features/     -> 网易版 BP/netease_features/\n"
        "国际版 BP/feature_rules/-> 网易版 BP/netease_feature_rules/\n"
        "国际版 BP/recipes/      -> 网易版 BP/netease_recipes/\n"
        "\n"
        "[资源包目录映射]\n"
        "国际版 RP/items/        -> 网易版 RP/netease_items_res/\n"
        "\n"
        "[脚本系统]\n"
        "国际版使用 Script API（JavaScript/TypeScript），网易版使用 Mod SDK（Python2）。\n"
        "Wiki中涉及 Script API / @minecraft/server 的内容不适用于网易版，"
        "网易版应使用 mod.server.extraServerApi / mod.client.extraClientApi。\n"
        "\n"
        "[开发框架]\n"
        "如果项目中存在 QuModLibs 库，优先使用 QuModLibs 的框架模块来简化网易Mod开发。\n"
        "\n"
        "[版本兼容性]\n"
        "网易版本号通常比国际版慢几个版本，编写JSON时应尽可能使用兼容性最高的 format_version，避免使用国际版最新特性。\n"
        "\n"
        "[JSON UI]\n"
        "JSON UI文件与网易版存在少许差异，涉及相关功能时请使用 minecraft_docs 的 `netease jsonui` 子命令获取网易版JSON UI控件库定义。";
    return NETEASE_DIFF_TEXT;
}

// ── 网易版 JSON UI 内置组件库定义文本 ──
inline const char* netease_jsonui_text() {
    static const char* NETEASE_JSONUI_TEXT = R"(网易版JSON UI内置组件库（namespace: netease_editor_template_namespace）
可以被自定义UI控件直接引用继承使用，也可以覆写属性扩展功能
以下是完整的组件定义：
{
  "namespace": "netease_editor_template_namespace",

  "netease_editor_template_image": {
    "type": "image",
    "size": [ "50%", "50%" ],
    "texture": "textures/netease/common/image/default",
    "uv_size": [ 107, 107 ]
  },

  "panel": {
    "type": "panel",
    "size": [ 100, 100 ]
  },

  "image": {
    "layer": 1,
    "size": [ 100, 100 ],
    "texture": "textures/netease/common/image/default",
    "type": "image",
    "clip_direction": "left",
    "clip_ratio": 0.0
  },

  "netease_editor_root_panel": {
    "type": "panel",
    "anchor_from": "top_left",
    "anchor_to": "top_left",
    "size": [ "100%", "100%" ],
    "layer": 1
  },

  "default": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$default_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "hover": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$hover_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "pressed": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": "$texture_layer",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$pressed_texture",
    "alpha": "$control_alpha",
    "type": "image"
  },

  "button_label": {
    "color": "$label_color",
    "font_scale_factor": "$label_font_scale_factor",
    "font_size": "$label_font_size",
    "font_type": "default",
    "layer": "$label_layer",
    "max_size": [ "100%", "100%" ],
    "offset": "$label_offset",
    "text": "$label_text",
    "text_alignment": "center",
    "alpha": "$control_alpha",
    "type": "label"
  },

  "scroll_box_image": {
    "is_new_nine_slice": "$is_box_nine_slice",
    "nine_slice_buttom": "$box_nine_slice_buttom",
    "nine_slice_left": "$box_nine_slice_left",
    "nine_slice_right": "$box_nine_slice_right",
    "nine_slice_top": "$box_nine_slice_top",
    "nineslice_size": "$box_nineslice_size",
    "texture": "$scroll_box_texture",
    "type": "image"
  },

  "scroll_track_image": {
    "is_new_nine_slice": "$is_track_nine_slice",
    "layer": 3,
    "nine_slice_buttom": "$track_nine_slice_buttom",
    "nine_slice_left": "$track_nine_slice_left",
    "nine_slice_right": "$track_nine_slice_right",
    "nine_slice_top": "$track_nine_slice_top",
    "nineslice_size": "$track_nineslice_size",
    "size": [ 3, "100%" ],
    "texture": "$scroll_track_texture",
    "type": "image"
  },

  "scroll_background_image": {
    "is_new_nine_slice": "$is_background_nine_slice",
    "nine_slice_buttom": "$background_nine_slice_buttom",
    "nine_slice_left": "$background_nine_slice_left",
    "nine_slice_right": "$background_nine_slice_right",
    "nine_slice_top": "$background_nine_slice_top",
    "nineslice_size": "$background_nineslice_size",
    "texture": "$scroll_background_texture",
    "type": "image"
  },

  "edit_box_background_default": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$edit_box_default_texture",
    "type": "image"
  },

  "edit_box_background_hover": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$edit_box_hover_texture",
    "type": "image"
  },

  "empty_progress_bar": {
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": 1,
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$progress_bar_empty_texture",
    "type": "image"
  },

  "filled_progress_bar": {
    "clip_direction": "$clip_direction",
    "clip_pixelperfect": false,
    "clip_ratio": "$clip_ratio",
    "is_new_nine_slice": "$is_new_nine_slice",
    "layer": 2,
    "nine_slice_buttom": "$nine_slice_buttom",
    "nine_slice_left": "$nine_slice_left",
    "nine_slice_right": "$nine_slice_right",
    "nine_slice_top": "$nine_slice_top",
    "nineslice_size": "$nineslice_size",
    "texture": "$progress_bar_filled_texture",
    "type": "image"
  }
}
)";
    return NETEASE_JSONUI_TEXT;
}

// ──────────────────────────────────────────────────────────────────────────
// 【已合并到 minecraft_docs — 注册代码整段注释保留，便于随时审视/回滚】
//
// 原 register_netease_tools() 注册的 get_netease_diff / get_netease_jsonui，
// 现统一由 minecraft_docs 的 `netease diff` / `netease jsonui` 子命令承担
// （复用上方 netease_diff_text() / netease_jsonui_text()）。
//
// inline void register_netease_tools(mcp::server& srv) {
//     auto diff_tool = mcp::tool_builder("get_netease_diff")
//         .with_description("获取网易版与国际版基岩版之间的差异说明")
//         .with_read_only_hint(true).with_idempotent_hint(true).build();
//     srv.register_tool(diff_tool, [](const mcp::json&, const std::string&) -> mcp::json {
//         return {{"content", mcp::json::array({{{"type","text"},{"text", netease_diff_text()}}})}};
//     });
//
//     auto jsonui_tool = mcp::tool_builder("get_netease_jsonui")
//         .with_description("获取网易版JSON UI内置组件库（netease_editor_template_namespace）的完整定义，包含可通过%引用的预定义控件")
//         .with_read_only_hint(true).with_idempotent_hint(true).build();
//     srv.register_tool(jsonui_tool, [](const mcp::json&, const std::string&) -> mcp::json {
//         return {{"content", mcp::json::array({{{"type","text"},{"text", netease_jsonui_text()}}})}};
//     });
// }
// ──────────────────────────────────────────────────────────────────────────

} // namespace mcdk
