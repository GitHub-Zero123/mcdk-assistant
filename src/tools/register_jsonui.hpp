#pragma once
// register_jsonui.hpp — JSON UI 全栈工具注册
// 包含: get_jsonui_reference, generate_ui_fullstack, diagnose_ui
#include "tools/ui_templates.h"
#include "tools/ui_diagnoser.h"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <string>

namespace mcdk {

// ── get_jsonui_reference 内容 ─────────────────────────
static const char* JSONUI_REFERENCE_TEXT = R"(
=== MC JSON UI 全栈速查手册 ===

一、控件类型速查
  screen    — 顶层界面容器，每个 UI 文件的入口控件
  panel     — 通用容器，用于分组和布局子控件
  image     — 图片控件，支持 texture/uv/uv_size/nineslice/clip
               JSON 侧 texture 路径不带扩展名: "textures/ui/my_icon"
  label     — 文本控件，支持 text/color/font_size/font_scale_factor/shadow
              font_size 取值: "small" | "middle" | "large"，【默认大小推荐为 "middle"（size：1.0）而非 "small"】
              font_scale_factor: 浮点数缩放因子（基于 font_size 再缩放），默认 1.0
  button    — 按钮控件，含 default/hover/pressed 三态，通过 $pressed_button_name 触发回调
  stack_panel — 线性布局容器，orientation: "horizontal"|"vertical"，子控件自动排列
  grid      — 网格布局，grid_dimensions: [列数, 行数]，配合 collection_name 做集合绑定
  scrolling_panel — 可滚动容器，通常继承 common.scrolling_panel
  edit_box  — 输入框，$place_holder_text 占位文本，$text_box_name 绑定名
  custom    — 自定义渲染控件（renderer 字段指定渲染器名）
  toggle    — 开关控件，含 checked/unchecked 两态

二、布局系统
  size: ["100%", "100%"] | ["200px", "50%"] | ["100%+10px", "100%cm"] | [固定数字, 固定数字] | ["fill", 30]
    - "100%"    = 父控件的100%
    - "100%+10px" = 父控件100% + 10像素（无空格！）
    - "100%cm"  = 100% of child max（子控件最大值）
    - "fill"    = 填充 stack_panel 剩余空间
    - 数字      = 固定像素值
  anchor_from / anchor_to: 九宫格定位
    top_left | top_middle | top_right
    left_middle | center | right_middle
    bottom_left | bottom_middle | bottom_right
  offset: [x, y] — 相对锚点的偏移（像素或百分比）
  max_size / min_size: 限制控件尺寸范围
  layer: 数字 — 渲染层级，数字越大越靠前
  clips_children: true — 裁剪超出边界的子控件
  visible: true/false — 控件可见性

三、继承与变量系统
  继承写法: "name@namespace.parent": { /* 覆写属性 */ }
  变量定义: "$var": "value"
  变量默认值: "$var|default": "fallback_value"
  controls 数组: 子控件列表，每项为 { "key": { ...控件定义 } }
  跨文件引用: "name@other_namespace.control" （需在 _ui_defs.json 中注册两个文件，原版资产无需注册）
  匿名继承: "name@namespace.parent": {} （不覆写任何属性）

四、数据绑定（JSON 侧）
  bindings 数组:
    [{ "binding_name": "#property_name", "binding_name_override": "#property_name" }]
    [{ "binding_name": "#visible", "binding_name_override": "#visible",
       "binding_condition": "always_when_visible" }]
  binding_condition 取值:
    "always" — 每帧更新
    "always_when_visible" — 可见时每帧更新
    "once" — 仅绑定一次
    "visibility_changed" — 可见性变化时更新
  collection 绑定:
    "binding_type": "collection_details"
    "collection_name": "集合名"

五、Python 侧对应关系
  UI 类注册:
    clientApi.RegisterUI("mod_name", "ui_key", "mod.path.ClassName.ClassName", "namespace.main")
  推送界面:
    clientApi.PushScreen("mod_name", "ui_key")
  【推荐】正向接口（主动调用）:
    self.GetBaseUIControl("/path/to/control")  — 获取控件对象
    ctrl.asLabel().SetText("文本")             — 设置文本
    ctrl.SetVisible(True/False)                — 设置可见性
    ctrl.asImage().SetSprite("textures/...")    — 设置图片
    ctrl.SetPosition((x, y))                   — 设置位置
    self.AddTouchEventHandler("/path", callback) — 注册触摸事件
  【备选】绑定接口（引擎反向拉取）:
    @ViewBinder.binding(ViewBinder.BF_BindString, "#binding_name")
    def OnGetText(self): return "动态文本"
    → 仅在正向接口无法满足时使用（如高频刷新的集合数据）
  【特殊】三方框架:
    对于 QuModLibs 框架库提供了专门的 UI 模块（先检查用户是否使用），优先使用框架提供的功能

六、网易模板库速查（netease_editor_template_namespace）
  按钮三态变量:
    $default_texture, $hover_texture, $pressed_texture
    $is_new_nine_slice, $nine_slice_buttom/left/right/top, $nineslice_size
    $texture_layer, $control_alpha
  按钮文本变量:
    $label_text, $label_color, $label_font_size, $label_font_scale_factor
    $label_layer, $label_offset
  滚动面板变量:
    $scroll_box_texture, $is_box_nine_slice, $box_nine_slice_*
    $scroll_track_texture, $is_track_nine_slice, $track_nine_slice_*
    $scroll_background_texture, $is_background_nine_slice, $background_nine_slice_*
  输入框背景变量:
    $edit_box_default_texture, $edit_box_hover_texture（+ 九宫格变量同上）
  进度条变量:
    $progress_bar_empty_texture, $progress_bar_filled_texture
    $clip_direction ("left"|"right"|"up"|"down"), $clip_ratio (0.0~1.0)

七、原版 common 常用组件
  common.button:
    $pressed_button_name — 按钮按下时的事件名（Python 通过 AddTouchEventHandler 监听）
    controls 中放 default/hover/pressed 三态图 + label
  common.scrolling_panel:
    $scrolling_content — 滚动内容控件引用（"namespace.control"）
    $scrolling_content_anchor_from / $scrolling_content_anchor_to

八、画布（main）设计原则
  【默认】顶层画布 main 的 type 必须为 "screen"（不是 panel，自定义控件除外），不继承任何基类，控件路径最短：
    "main": { "type": "screen", "size": ["100%","100%"], ... }
  【仅在需要安全区适配时】继承网易基类画布（会拉长控件路径）：
    "main@netease_editor_template_namespace.netease_editor_root_panel": { ... }
  安全区场景：需要适配刘海屏/全面屏时。普通界面无需继承，路径长了反而增加代码量。
  注意：只有 main（顶层画布）用 "screen"，内部子容器仍用 "panel"。

九、最小可用完整示例
  JSON UI 文件 (ui/my_screen.json):
    {
      "namespace": "MY_UI",
      "main": {
        "type": "screen",
        "size": ["100%", "100%"],
        "controls": [
          { "bg": { "type": "image", "size": ["100%","100%"], "texture": "textures/ui/bg32", "alpha": 0.6 } },
          { "title": { "type": "label", "text": "Hello", "color": [1,1,1], "anchor_from": "center", "anchor_to": "center" } },
          {
            "closeBtn@common.button": {
              "size": [20,20], "$pressed_button_name": "button.close",
              "anchor_from": "top_right", "anchor_to": "top_right", "offset": [-5, 5],
              "controls": [{ "x": { "type": "label", "text": "X", "color": [1,1,1] } }]
            }
          }
        ]
      }
    }
  _ui_defs.json 中添加: "ui/my_screen.json"

  Python UI 类:
    from mod.client.ui.screenNode import ScreenNode
    import mod.client.extraClientApi as clientApi

    def RegisterMyUI():
        clientApi.RegisterUI("mymod", "my_screen", "mymod.ui.MyScreenNode.MyScreenNode", "MY_UI.main")

    class MyScreenNode(ScreenNode):
        def __init__(self, namespace, name, param):
            ScreenNode.__init__(self, namespace, name, param)
        def Create(self):
            self.AddTouchEventHandler("/closeBtn", self.OnClose)
        def OnClose(self, args):
            if args["TouchEvent"] == 0:
                self.SetScreenVisible(False)
十、参考文档
  本速查手册仅为快速参考，完整控件属性、高级用法请查阅网易官方UI说明文档。
  使用 read_knowledge 工具读取以下文档获取详细信息：
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/30-UI说明文档.md  （JSON UI 完整说明）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/10-控件和控件属性.md （控件属性详解）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/40-UIAPI文档.md  （Python UI API）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/50-UI控件对象.md  （UI控件对象方法）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/13-继承和自定义控件.md （继承机制）
  遇到不确定的属性或用法时，务必先查阅上述文档。
)";

inline void register_jsonui_tools(mcp::server& srv) {

    // ── get_jsonui_reference ──────────────────────────
    auto ref_tool = mcp::tool_builder("get_jsonui_reference")
        .with_description("获取 MC JSON UI 全栈速查手册（控件类型、布局、继承、绑定、Python对应、模板库、完整示例），一次调用获取全部核心知识。详细属性请配合 read_knowledge 查阅网易UI说明文档")
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(ref_tool, [](const mcp::json&, const std::string&) -> mcp::json {
        return {{"content", mcp::json::array({{{"type","text"},{"text", JSONUI_REFERENCE_TEXT}}})}};
    });

    // ── generate_ui_fullstack ─────────────────────────
    auto gen_tool = mcp::tool_builder("generate_ui_fullstack")
        .with_description(
            "生成全栈 JSON UI + Python UI 类模板。template_type: "
            "screen_basic(基础界面) | screen_list(滚动列表) | screen_grid(网格) | "
            "screen_form(表单) | screen_tabbed(标签页) | hud_overlay(HUD覆盖层) | "
            "widget_button(按钮组件) | widget_progress(进度条组件)。"
            "生成后如需了解控件详细属性，请用 read_knowledge 查阅网易UI说明文档")
        .with_string_param("template_type", "模板类型", true)
        .with_string_param("namespace",     "JSON UI 命名空间，如 MY_MOD_UI", true)
        .with_string_param("mod_name",      "Python 模块名，如 mymod", true)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(gen_tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        auto result = generate_ui_fullstack(
            params.value("template_type", ""),
            params.value("namespace", ""),
            params.value("mod_name", "")
        );
        if (!result.error.empty())
            throw mcp::mcp_exception(mcp::error_code::invalid_params, result.error);

        mcp::json content = mcp::json::array();
        content.push_back({{"type","text"},{"text", result.json_ui},{"label","json_ui"}});
        if (!result.python_class.empty())
            content.push_back({{"type","text"},{"text", result.python_class},{"label","python_class"}});
        if (!result.ui_defs_entry.empty())
            content.push_back({{"type","text"},{"text", result.ui_defs_entry},{"label","ui_defs_entry"}});
        return {{"content", content}};
    });

    // ── diagnose_ui ───────────────────────────────────
    auto diag_tool = mcp::tool_builder("diagnose_ui")
        .with_description("诊断 JSON UI 文件内容中的常见错误（缺少namespace、binding_name格式、size格式、控件key重复等）。修复建议可配合 read_knowledge 查阅网易UI说明文档")
        .with_string_param("json_content", "JSON UI 文件的完整文本内容", true)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(diag_tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        auto results = diagnose_ui(params.value("json_content", ""));
        if (results.empty())
            return {{"content", mcp::json::array({{{"type","text"},{"text","未发现问题，JSON UI 格式检查通过。"}}})}};

        std::string text;
        for (const auto& d : results) {
            text += "[" + d.severity + "]";
            if (d.line > 0) text += " 行" + std::to_string(d.line);
            text += " " + d.message + "\n";
        }
        return {{"content", mcp::json::array({{{"type","text"},{"text", text}}})}};
    });
}

} // namespace mcdk
