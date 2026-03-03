#pragma once
// register_jsonui.hpp — JSON UI 全栈工具注册
// 包含: get_jsonui_reference, generate_ui_fullstack, diagnose_ui, query_ui_control, dump_ui_tree, patch_ui_file
#include "tools/ui_templates.h"
#include "tools/ui_diagnoser.h"
#include "tools/ui_patcher.h"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>

namespace mcdk {

// 辅助：从文件或参数获取 JSON UI 内容
static inline std::string resolve_json_content(const mcp::json& params) {
    std::string content = params.value("json_content", "");
    std::string fpath   = params.value("file_path", "");
    if (!fpath.empty()) {
        std::ifstream ifs(fpath, std::ios::binary);
        if (!ifs.is_open())
            throw mcp::mcp_exception(mcp::error_code::invalid_params,
                "无法打开文件: " + fpath);
        return std::string((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    }
    if (content.empty())
        throw mcp::mcp_exception(mcp::error_code::invalid_params,
            "必须提供 json_content 或 file_path 参数");
    return content;
}

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
十、UI 文件分析与修改工具使用指引
  【优先】在修改已有 UI 文件前，务必先调用以下工具了解结构：
    1. dump_ui_tree(file_path) — 一次性获取完整控件结构树，快速掌握全局布局
       - max_depth: 限制展开层数（大文件建议先用 2-3 层）
       - root_path: 从指定路径开始展开（如 "main/panel"）
       - search: 按关键词/正则搜索控件名，定位目标控件
    2. query_ui_control(file_path, path) — 查看指定控件的详细属性
    3. diagnose_ui(file_path) — 检查文件格式错误
  以上工具均支持 file_path 参数（传文件绝对路径），大文件无需传入完整内容。

  【修改】对大型 UI 文件使用增量修改，避免重写整个文件：
    4. patch_ui_file(file_path, patches) — 精确修改指定控件，不影响其他部分
       支持操作: set_prop(设置属性), remove_prop(删除属性),
                 add_ctrl(添加控件), remove_ctrl(删除控件), replace_ctrl(替换控件),
                 add_top(添加顶层), remove_top(删除顶层)
       自动备份 .bak 文件，原子执行（失败则不写入）
  典型工作流: dump_ui_tree → query_ui_control → patch_ui_file → diagnose_ui

十一、参考文档
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
        .with_description("诊断 JSON UI 文件中的常见错误（缺少namespace、binding_name格式、size格式、控件key重复等）。"
                          "支持 file_path（文件绝对路径）或 json_content（文本内容）二选一，大文件推荐用 file_path。"
                          "修复建议可配合 read_knowledge 查阅网易UI说明文档")
        .with_string_param("file_path", "JSON UI 文件绝对路径（与 json_content 二选一，大文件推荐）", false)
        .with_string_param("json_content", "JSON UI 文件的完整文本内容（与 file_path 二选一）", false)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(diag_tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        auto results = diagnose_ui(resolve_json_content(params));
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

    // ── query_ui_control ──────────────────────────────
    auto query_tool = mcp::tool_builder("query_ui_control")
        .with_description(
            "查询 JSON UI 文件中指定路径的控件信息（属性、直接子控件列表），不展开子节点的 controls 内容。"
            "用于快速浏览 UI 结构。支持 file_path（文件绝对路径）或 json_content 二选一，大文件推荐用 file_path。"
            "path 格式: \"main/panel/form/inputBox\"（第一段为顶层控件名，后续为 controls 路径）。"
            "不传 path 则列出所有顶层控件名")
        .with_string_param("file_path", "JSON UI 文件绝对路径（与 json_content 二选一，大文件推荐）", false)
        .with_string_param("json_content", "JSON UI 文件的完整文本内容（与 file_path 二选一）", false)
        .with_string_param("path", "控件路径，如 \"main/panel/form\"。不传则列出所有顶层控件", false)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(query_tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        std::string content = resolve_json_content(params);
        std::string path    = params.value("path", "");

        nlohmann::json root;
        try {
            root = nlohmann::json::parse(content, nullptr, true, true);
        } catch (const nlohmann::json::parse_error& e) {
            throw mcp::mcp_exception(mcp::error_code::invalid_params,
                std::string("JSON 解析失败: ") + e.what());
        }

        if (!root.is_object()) {
            throw mcp::mcp_exception(mcp::error_code::invalid_params, "JSON 根节点不是对象");
        }

        // 不传 path → 列出所有顶层控件名
        if (path.empty()) {
            std::string result = "顶层控件列表:\n";
            for (auto it = root.begin(); it != root.end(); ++it) {
                if (it.key() == "namespace") {
                    result += "  namespace: " + it.value().get<std::string>() + "\n";
                    continue;
                }
                std::string type_info;
                if (it.value().is_object() && it.value().contains("type"))
                    type_info = " (type: " + it.value()["type"].get<std::string>() + ")";
                result += "  " + it.key() + type_info + "\n";
            }
            return {{"content", mcp::json::array({{{"type","text"},{"text", result}}})}};
        }

        // 按 / 分割路径
        std::vector<std::string> segments;
        {
            std::istringstream ss(path);
            std::string seg;
            while (std::getline(ss, seg, '/')) {
                if (!seg.empty()) segments.push_back(seg);
            }
        }
        if (segments.empty()) {
            throw mcp::mcp_exception(mcp::error_code::invalid_params, "path 不能为空段");
        }

        // 第一段：查找顶层控件（支持 @ 继承写法匹配）
        std::string top_key = segments[0];
        const nlohmann::json* current = nullptr;
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string k = it.key();
            auto at = k.find('@');
            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
            if (bare == top_key) {
                current = &it.value();
                break;
            }
        }
        if (!current || !current->is_object()) {
            throw mcp::mcp_exception(mcp::error_code::invalid_params,
                "未找到顶层控件: " + top_key);
        }

        // 后续段：在 controls 中逐层查找
        for (size_t i = 1; i < segments.size(); ++i) {
            const std::string& seg = segments[i];
            bool found = false;
            if (current->contains("controls") && (*current)["controls"].is_array()) {
                for (const auto& item : (*current)["controls"]) {
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
                throw mcp::mcp_exception(mcp::error_code::invalid_params,
                    "路径 \"" + path + "\" 中未找到控件: " + seg);
            }
        }

        // 输出当前控件信息（不展开 controls 内部内容）
        std::string result = "控件: " + path + "\n";
        if (current->is_object()) {
            for (auto it = current->begin(); it != current->end(); ++it) {
                if (it.key() == "controls") {
                    // 只列出直接子控件的 key
                    result += "  controls: [";
                    bool first = true;
                    if (it.value().is_array()) {
                        for (const auto& child : it.value()) {
                            if (!child.is_object()) continue;
                            for (auto ct = child.begin(); ct != child.end(); ++ct) {
                                if (!first) result += ", ";
                                result += ct.key();
                                // 附带 type 信息
                                if (ct.value().is_object() && ct.value().contains("type"))
                                    result += "(" + ct.value()["type"].get<std::string>() + ")";
                                first = false;
                            }
                        }
                    }
                    result += "]\n";
                } else {
                    // 输出属性值（简短格式）
                    std::string val = it.value().dump();
                    if (val.size() > 120) val = val.substr(0, 117) + "...";
                    result += "  " + it.key() + ": " + val + "\n";
                }
            }
        } else {
            result += "  (值: " + current->dump() + ")\n";
        }

        return {{"content", mcp::json::array({{{"type","text"},{"text", result}}})}};
    });

    // ── dump_ui_tree ──────────────────────────────────
    // 辅助：递归构建树状字符串
    struct TreeCtx {
        std::string output;
        int max_depth;
        std::string search_pattern;
        std::regex search_regex;
        bool has_search;
        int match_context; // 匹配时向上/下扩展的层数
    };

    auto dump_tree_node = [](auto& self, const nlohmann::json& node,
                              const std::string& key, const std::string& prefix,
                              const std::string& child_prefix, int depth,
                              TreeCtx& ctx, const std::string& full_path) -> void {
        // 构建当前节点描述
        std::string desc = key;
        if (node.is_object()) {
            if (node.contains("type"))
                desc += " (" + node["type"].get<std::string>() + ")";
            // 附加关键属性摘要
            std::string extras;
            if (node.contains("size")) extras += " size:" + node["size"].dump();
            if (node.contains("anchor_from")) extras += " anchor:" + node["anchor_from"].get<std::string>();
            if (node.contains("layer")) extras += " L:" + std::to_string(node["layer"].get<int>());
            if (node.contains("visible") && !node["visible"].get<bool>()) extras += " [hidden]";
            if (!extras.empty()) desc += extras;
        }

        // 搜索模式：检查是否匹配
        bool matches = true;
        if (ctx.has_search) {
            try {
                matches = std::regex_search(key, ctx.search_regex) ||
                          std::regex_search(full_path, ctx.search_regex);
            } catch (...) {
                matches = (key.find(ctx.search_pattern) != std::string::npos ||
                           full_path.find(ctx.search_pattern) != std::string::npos);
            }
        }

        // 无搜索 或 匹配时输出
        if (!ctx.has_search || matches) {
            ctx.output += prefix + desc + "\n";
            // 搜索模式命中时：额外输出完整属性 + 子控件列表（类似 query_ui_control）
            if (ctx.has_search && matches && node.is_object()) {
                std::string detail_prefix = child_prefix + "  ";
                for (auto it = node.begin(); it != node.end(); ++it) {
                    if (it.key() == "controls") {
                        ctx.output += detail_prefix + "controls: [";
                        bool first = true;
                        if (it.value().is_array()) {
                            for (const auto& child : it.value()) {
                                if (!child.is_object()) continue;
                                for (auto ct = child.begin(); ct != child.end(); ++ct) {
                                    if (!first) ctx.output += ", ";
                                    ctx.output += ct.key();
                                    if (ct.value().is_object() && ct.value().contains("type"))
                                        ctx.output += "(" + ct.value()["type"].get<std::string>() + ")";
                                    first = false;
                                }
                            }
                        }
                        ctx.output += "]\n";
                    } else {
                        std::string val = it.value().dump();
                        if (val.size() > 120) val = val.substr(0, 117) + "...";
                        ctx.output += detail_prefix + it.key() + ": " + val + "\n";
                    }
                }
                ctx.output += detail_prefix + "path: " + full_path + "\n";
            }
        }

        // 深度限制
        if (ctx.max_depth > 0 && depth >= ctx.max_depth) {
            if (!ctx.has_search && node.is_object() && node.contains("controls"))
                ctx.output += child_prefix + "└─ ...(depth limit)\n";
            return;
        }

        // 递归子控件
        if (!node.is_object() || !node.contains("controls") || !node["controls"].is_array())
            return;

        const auto& controls = node["controls"];
        std::vector<std::pair<std::string, const nlohmann::json*>> children;
        for (const auto& item : controls) {
            if (!item.is_object()) continue;
            for (auto it = item.begin(); it != item.end(); ++it) {
                children.push_back({it.key(), &it.value()});
            }
        }

        for (size_t i = 0; i < children.size(); ++i) {
            bool last = (i == children.size() - 1);
            std::string p = child_prefix + (last ? "└─ " : "├─ ");
            std::string cp = child_prefix + (last ? "   " : "│  ");
            std::string fp = full_path + "/" + children[i].first;
            // 去掉 @ 后缀用于路径
            auto at = children[i].first.find('@');
            std::string bare = (at != std::string::npos) ? children[i].first.substr(0, at) : children[i].first;
            std::string bare_fp = full_path + "/" + bare;
            self(self, *children[i].second, children[i].first, p, cp, depth + 1, ctx, bare_fp);
        }
    };

    auto tree_tool = mcp::tool_builder("dump_ui_tree")
        .with_description(
            "生成 JSON UI 文件的控件结构树（带缩进的树状图）。"
            "支持 file_path 或 json_content 二选一。"
            "可通过 max_depth 限制展开层数，root_path 指定从哪个控件开始，"
            "search 按正则/关键词匹配控件名并输出匹配结果及完整属性详情。"
            "【推荐在开始修改 UI 文件前先调用此工具了解整体结构】")
        .with_string_param("file_path", "JSON UI 文件绝对路径（与 json_content 二选一，大文件推荐）", false)
        .with_string_param("json_content", "JSON UI 文件的完整文本内容（与 file_path 二选一）", false)
        .with_string_param("root_path", "从指定路径开始展开，如 \"main/panel\"。不传则从所有顶层控件开始", false)
        .with_number_param("max_depth", "最大展开深度（0=无限制，默认0）", false)
        .with_string_param("search", "搜索关键词或正则表达式，匹配控件名/路径，只输出匹配的控件及其完整属性和子控件列表", false)
        .with_read_only_hint(true).with_idempotent_hint(true).build();

    srv.register_tool(tree_tool, [dump_tree_node](const mcp::json& params, const std::string&) -> mcp::json {
        std::string content = resolve_json_content(params);
        std::string root_path = params.value("root_path", "");
        int max_depth = params.contains("max_depth") && params["max_depth"].is_number()
                        ? params["max_depth"].get<int>() : 0;
        std::string search = params.value("search", "");

        nlohmann::json root;
        try {
            root = nlohmann::json::parse(content, nullptr, true, true);
        } catch (const nlohmann::json::parse_error& e) {
            throw mcp::mcp_exception(mcp::error_code::invalid_params,
                std::string("JSON 解析失败: ") + e.what());
        }

        TreeCtx ctx;
        ctx.max_depth = max_depth;
        ctx.search_pattern = search;
        ctx.has_search = !search.empty();
        if (ctx.has_search) {
            try { ctx.search_regex = std::regex(search, std::regex::icase); }
            catch (...) { /* fallback to string find */ }
        }

        // 确定起始节点
        if (!root_path.empty()) {
            // 按路径定位
            std::vector<std::string> segs;
            {
                std::istringstream ss(root_path);
                std::string seg;
                while (std::getline(ss, seg, '/'))
                    if (!seg.empty()) segs.push_back(seg);
            }

            const nlohmann::json* current = nullptr;
            std::string current_key;
            // 第一段：顶层
            if (!segs.empty()) {
                for (auto it = root.begin(); it != root.end(); ++it) {
                    std::string k = it.key();
                    auto at = k.find('@');
                    std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
                    if (bare == segs[0]) {
                        current = &it.value();
                        current_key = it.key();
                        break;
                    }
                }
                if (!current)
                    throw mcp::mcp_exception(mcp::error_code::invalid_params,
                        "未找到顶层控件: " + segs[0]);
            }
            // 后续段
            for (size_t i = 1; i < segs.size() && current; ++i) {
                bool found = false;
                if (current->contains("controls") && (*current)["controls"].is_array()) {
                    for (const auto& item : (*current)["controls"]) {
                        if (!item.is_object()) continue;
                        for (auto jt = item.begin(); jt != item.end(); ++jt) {
                            std::string k = jt.key();
                            auto at = k.find('@');
                            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
                            if (bare == segs[i]) {
                                current = &jt.value();
                                current_key = jt.key();
                                found = true;
                                break;
                            }
                        }
                        if (found) break;
                    }
                }
                if (!found)
                    throw mcp::mcp_exception(mcp::error_code::invalid_params,
                        "路径 \"" + root_path + "\" 中未找到: " + segs[i]);
            }

            ctx.output = root_path + "\n";
            dump_tree_node(dump_tree_node, *current, current_key, "", "", 0, ctx, root_path);
        } else {
            // 从所有顶层控件开始
            std::string ns;
            if (root.contains("namespace"))
                ns = root["namespace"].get<std::string>();
            if (!ns.empty())
                ctx.output = "namespace: " + ns + "\n";

            std::vector<std::pair<std::string, const nlohmann::json*>> tops;
            for (auto it = root.begin(); it != root.end(); ++it) {
                if (it.key() == "namespace") continue;
                tops.push_back({it.key(), &it.value()});
            }
            for (size_t i = 0; i < tops.size(); ++i) {
                bool last = (i == tops.size() - 1);
                std::string p = last ? "└─ " : "├─ ";
                std::string cp = last ? "   " : "│  ";
                auto at = tops[i].first.find('@');
                std::string bare = (at != std::string::npos) ? tops[i].first.substr(0, at) : tops[i].first;
                dump_tree_node(dump_tree_node, *tops[i].second, tops[i].first, p, cp, 0, ctx, bare);
            }
        }

        if (ctx.output.empty())
            ctx.output = "(无匹配结果)";

        return {{"content", mcp::json::array({{{"type","text"},{"text", ctx.output}}})}};
    });

    // ── patch_ui_file ─────────────────────────────────
    auto patch_tool = mcp::tool_builder("patch_ui_file")
        .with_description(
            "对 JSON UI 文件进行增量补丁修改（不重写整个文件）。"
            "patches 为操作数组，支持: set_prop(设置属性), remove_prop(删除属性), "
            "add_ctrl(添加控件), remove_ctrl(删除控件), replace_ctrl(替换控件), "
            "add_top(添加顶层控件), remove_top(删除顶层控件)。"
            "自动创建 .bak 备份，原子执行（全部成功才写入）")
        .with_string_param("file_path", "JSON UI 文件绝对路径", true)
        .with_array_param("patches", "补丁操作数组", "object")
        .with_boolean_param("backup", "是否创建 .bak 备份，默认true", false)
        .build();

    srv.register_tool(patch_tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        std::string file_path = params.value("file_path", "");
        if (file_path.empty())
            throw mcp::mcp_exception(mcp::error_code::invalid_params, "file_path 不能为空");

        if (!params.contains("patches") || !params["patches"].is_array())
            throw mcp::mcp_exception(mcp::error_code::invalid_params, "patches 必须是数组");

        bool backup = true;
        if (params.contains("backup") && params["backup"].is_boolean())
            backup = params["backup"].get<bool>();

        auto result = apply_ui_patches(file_path, params["patches"], backup);

        if (!result.success) {
            return {{"content", mcp::json::array({{
                {"type","text"},
                {"text", "[FAILED] " + result.error +
                         "\n已应用: " + std::to_string(result.applied_count) + " 个补丁（未写入文件）"}
            }})}};
        }

        return {{"content", mcp::json::array({{
            {"type","text"},
            {"text", "成功应用 " + std::to_string(result.applied_count) + " 个补丁到: " + file_path +
                     (backup ? "\n备份: " + file_path + ".bak" : "")}
        }})}};
    });
}

} // namespace mcdk
