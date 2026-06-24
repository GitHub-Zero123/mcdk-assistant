#pragma once
// register_minecraft_ui.hpp — JSON UI 全栈工具统一入口 `minecraft_ui`
//
// 把原先 register_jsonui.hpp 中 7 个独立 JSON UI 工具合并为单一命令式 MCP tool：
//   get_jsonui_reference / generate_ui_fullstack / diagnose_ui /
//   query_ui_control / dump_ui_tree / search_ui_content / patch_ui_file
//
// 本文件只负责命令分发和 CLI 参数适配；模板生成、诊断、补丁仍复用
// ui_templates.h / ui_diagnoser.h / ui_patcher.h 的原有函数。
// query/tree/search 三个工具原本是内联在 register_jsonui.hpp 的 lambda 里，
// 现将其逻辑整体搬迁到本文件的 dispatch 函数中。
#include "tools/command_parser.hpp"
#include "tools/ui_templates.h"
#include "tools/ui_diagnoser.h"
#include "tools/ui_patcher.h"
#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <functional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace mcdk {
namespace minecraft_ui_detail {

constexpr const char* kToolName = "minecraft_ui";

// ── 标准返回格式 ──────────────────────────────────────────
inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

inline mcp::json labeled_text_result(const std::string& text, const std::string& label) {
    return {{"content", mcp::json::array({
                {{"type", "text"}, {"text", text}, {"label", label}}
            })}};
}

// ── help 文本（也是 get_jsonui_reference 的内容）──
// 注意：保留原 JSONUI_REFERENCE_TEXT 的完整内容（含工具使用指引第十节），
// 但将其中"调用以下工具"的措辞改为本入口的子命令名，避免 AI 找不到旧工具名。
inline std::string ui_reference_text() {
    return R"(=== MC JSON UI 全栈速查手册 ===

一、控件类型速查
  screen    — 顶层界面容器，每个 UI 文件的入口控件
  panel     — 通用容器，用于分组和布局子控件
  image     — 图片控件，支持 texture/uv/uv_size/nineslice/clip
               JSON 侧 texture 路径不带扩展名: "textures/ui/my_icon"
  label     — 文本控件，支持 text/color/font_size/font_scale_factor/shadow
              font_size 取值: "small" | "normal" | "large" | "extra_large"，默认 "normal"（推荐不低于此值，避免小屏模糊）
              font_scale_factor: 浮点数缩放因子（基于 font_size 再缩放），默认 1.0
  button    — 按钮控件，含 default/hover/pressed 三态，通过 $pressed_button_name 触发回调
  stack_panel — 线性布局容器，orientation: "horizontal"|"vertical"，子控件自动排列
  grid      — 网格布局，grid_dimensions: [列数, 行数]，配合 collection_name 做集合绑定
  scrolling_panel — 可滚动容器，通常继承 common.scrolling_panel
  edit_box  — 输入框，$place_holder_text 占位文本，$text_box_name 绑定名
  custom    — 自定义渲染控件（renderer 字段指定渲染器名）
  toggle    — 开关控件，含 checked/unchecked 两态

二、布局系统
  size: ["100%", "100%"] | ["200px", "50%"] | ["100%+10px", "100%cm"] | [固定数字, 固定数字] | ["fill", 30] | ["default", "default"]
    - "100%"    = 父控件的100%
    - "100%+10px" = 父控件100% + 10像素（无空格！）
    - "100%cm"  = 100% of child max（子控件最大值）
    - "fill"    = 填充 stack_panel 剩余空间
    - "default" = 上下文自适应：普通控件=100%，label=文字大小，grid元素=按网格分配
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

  ★ 布局最佳实践（必须遵守）:
  【size 规则 — 交叉百分比原则】
    - 核心技巧：一个方向用父控件百分比确定基准，另一个方向用对方维度百分比填充
      实现等比例缩放 + 分辨率适配，例:
      size: ["120%y+0px", "60%+0px"]  ← 宽度=1.2倍屏幕高度，高度=60%父高度
      这样控件宽高保持固定比例(2:1)，且在任何分辨率下都不越界
    - 移动端 Y 轴几乎必定 < X 轴，宽度用 "%y" 基准可避免越界:
      size: ["200%y+0px", "72%+0px"]  ← 宽度=2×高度方向基准
    - 外层容器: ["100%", "100%"] 或交叉百分比
    - 内部子控件: 百分比 + 少量像素修正 ["100%+0px", "7%+0px"]
    - 避免纯固定像素(如 [200, 100])，不同分辨率下不适配
    - offset 同理用百分比: ["0%+0px", "2%+0px"]
    - "default" 是特殊大小值：普通控件=100%（等价父容器）；文本控件(label)=自适应文字内容大小；
      grid 元素=自适应网格布局分配大小（如 2x3 网格自动算每格宽高）。grid 元素推荐用 "default"

  【图层(layer)规则】
    - layer 越大越靠前（覆盖在上面）
    - button 控件的 label/text 子控件 layer 必须 > 同级 image 子控件的 layer
      否则文字会被背景图遮挡不可见
    - 推荐层级: 背景 image=1, label/text=2, 交互按钮=3
    - 同一个 controls 数组内的控件，后面的默认覆盖前面的（同 layer 时）
    - button 三态: 子控件命名 default、hover、pressed（或继承 default@xxx.template）
      属性 default_control/hover_control/pressed_control 指定状态对应的子控件名
    - 按钮文本子控件推荐命名 button_label（网易标准命名）

  【stack_panel 布局注意】
    - stack_panel 内 anchor/offset 不生效，子控件按 orientation 方向依次排列
    - 用 size:[0,0] 的空 panel 做间距挤压，适合信息流/列表排版
    - 子控件 size 中排列方向维度用 "fill" 或固定值，另一维度用百分比

  【大小约束】
    - 子控件 size 不应大于父控件（超出会被裁剪或不可见）
    - 常见错误：父控件 size=[0,0] 但子控件有实际大小 → 子控件不显示
    - 父控件用 "100%cm"（child max）可自动适配子控件最大尺寸

  【grid 布局注意】
    - 避免父子控件互相依赖大小。竖向信息流常用：grid x="100%" y="100%cm"（自动延伸）
    - 元素控件 size 推荐 ["default","default"]，grid 会根据列数/行数自动分配每格宽高
      例: 2x3 网格中 "default" 会自动算出每格=父宽50% × 父高33%
    - 技巧：元素控件外套一层 panel，内容 size=["90%","90%"] 留间隙，避免网格填满无空隙不美观

  【通用原则】
    - 所有 screen 类型的 main 控件 type 必须是 "screen" 而非 "panel"
    - anchor_from 和 anchor_to 必须成对设置（不设或都不设）
    - 九宫切图 ($is_new_nine_slice) 用于拉伸不变形，按钮/输入框优先使用

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
              "size": ["4%y+0px","4%y+0px"], "$pressed_button_name": "button.close",
              "anchor_from": "top_right", "anchor_to": "top_right", "offset": ["-1%+0px", "1%+0px"],
              "controls": [{ "button_label": { "type": "label", "text": "X", "color": [1,1,1], "layer": 3 } }]
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

十、UI 文件分析与修改工具使用指引（本入口 minecraft_ui 的子命令）
  【优先】在修改已有 UI 文件前，务必先调用以下子命令了解结构：
    1. tree [--file <path> | --content <json>] [--root <path>] [--depth <n>] [--search <关键词或正则>]
       一次性获取完整控件结构树 + 统计摘要。大文件建议先用 depth 2-3。
       search 按关键词/正则匹配控件名/路径/属性值，输出匹配控件的完整属性+路径+子控件列表，
       搜索结果自带面包屑路径，一次调用即可获取深层控件全部信息。
       输出末尾包含统计：控件总数、最大深度、搜索命中数。
    2. query [--file <path> | --content <json>] [--path <控件路径>]
       查看指定路径控件的详细属性（不展开 controls 内部）。不传 path 则列出所有顶层控件名。
    3. diag [--file <path> | --content <json>]
       全面检查文件问题。检查项：namespace缺失、controls key重复、type有效性、size格式、
       anchor_from/to成对、binding_name格式、@引用存在性。
    4. search [--file <path> | --content <json>] --keyword <关键词或正则>
       按属性 key/value 内容搜索。适合快速查找纹理路径(textures/xxx)、绑定名(#xxx)、继承引用(@xxx)。
       返回匹配的 [控件路径] key: value，轻量精确。
  以上子命令均支持 --file（传文件绝对路径，大文件推荐）或 --content（传完整 JSON 文本）二选一。

  【修改】对大型 UI 文件使用增量修改，避免重写整个文件：
    5. patch --file <path> --ops '<JSON补丁数组>'
       精确修改指定控件，不影响其他部分。
       支持操作（每项含 op 字段，参数见下）:
         set_prop(设属性, 需 props 对象)        {"op":"set_prop","path":"main/panel","props":{"visible":false}}
         remove_prop(删属性, 需 keys 数组)       {"op":"remove_prop","path":"main/panel","keys":["visible","layer"]}
         add_ctrl(加控件, 需 key+value)          {"op":"add_ctrl","path":"main/panel","key":"newBtn","value":{...},"after":"topPanel"}
         remove_ctrl(删控件, 需 key)            {"op":"remove_ctrl","path":"main/panel","key":"oldPanel"}
         replace_ctrl(替换控件, 需 key+value)    {"op":"replace_ctrl","path":"main/panel","key":"topPanel","value":{...}}
         merge_ctrl(合并属性, 需 key+props)      {"op":"merge_ctrl","path":"main/panel","key":"topPanel","props":{...},"new_key":"..."}
         add_top(加顶层, 需 key+value)           {"op":"add_top","key":"my_component","value":{...}}
         remove_top(删顶层, 需 key)             {"op":"remove_top","key":"my_component"}
       原子执行（失败则不写入）
  典型工作流: tree(search) / search → patch → diag

十一、参考文档
  本速查手册仅为快速参考，完整控件属性、高级用法请查阅网易官方UI说明文档。
  使用 minecraft_docs 的 read 子命令读取以下文档获取详细信息：
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/30-UI说明文档.md  （JSON UI 完整说明）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/10-控件和控件属性.md （控件属性详解）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/40-UIAPI文档.md  （Python UI API）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/50-UI控件对象.md  （UI控件对象方法）
    - knowledge/NeteaseGuide/mcguide/18-界面与交互/13-继承和自定义控件.md （继承机制）
  遇到不确定的属性或用法时，务必先查阅上述文档。
)";
}

inline std::string ui_help_text() {
    return R"(minecraft_ui — MC JSON UI 全栈工具统一入口（命令式用法）
用法: minecraft_ui(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/tree，与 help/tree 等价。

【通用规则】
1. 参数含空格（如 JSON 文本、正则表达式）必须用 "..." 或 '...' 包裹成整体。
2. 大部分子命令支持 --file <绝对路径> 或 --content <JSON文本> 二选一指定 UI 内容；
   大文件推荐用 --file，避免命令行过长。Windows 路径请用正斜杠（D:/mod/ui/x.json）。
3. --content 传 JSON 时，两种引号包裹均可（command_parser 同时支持单双引号）：
     用单引号（推荐，JSON 内部双引号无需转义）: query --content '{"namespace":"UI","main":{...}}'
     用双引号（JSON 内部双引号须 \" 转义）:      query --content "{\"namespace\":\"UI\",...}"
4. 想了解 JSON UI 完整知识（控件/布局/绑定/模板库），调用 help 即可获取速查手册。

【子命令】

  help
                               获取 JSON UI 全栈速查手册（控件类型/布局/继承/绑定/Python对应/模板库/示例）
                               内含本工具各子命令的使用指引。

  gen --template <类型> --ns <命名空间> --mod <Python模块名>
                               生成全栈 JSON UI + Python UI 类模板。
                               --template 可选值:
                                 screen_basic(基础界面) | screen_list(滚动列表) | screen_grid(网格) |
                                 screen_form(表单) | screen_tabbed(标签页) | hud_overlay(HUD覆盖层) |
                                 widget_button(按钮组件) | widget_progress(进度条组件)
                               示例: gen --template screen_basic --ns MY_MOD_UI --mod mymod

  diag (--file <路径> | --content <JSON文本>)
                               诊断 JSON UI 文件常见错误（namespace/binding_name/size/控件key重复/
                               type有效性/anchor成对/图层遮挡等），返回逐条问题列表。

  query (--file <路径> | --content <JSON文本>) [--path <控件路径>]
                               查询指定路径控件信息（属性 + 直接子控件列表，不展开 controls 内部）。
                               --path 格式 "main/panel/form/inputBox"（首段顶层控件名，后续 controls 路径）。
                               不传 --path 则列出所有顶层控件名。

  tree (--file <路径> | --content <JSON文本>)
       [--root <控件路径>] [--depth <n>] [--search <关键词或正则>]
                               生成控件结构树（带缩进树状图）。
                               --root 从指定路径开始展开；--depth 限制层数（0=无限制，默认0）；
                               --search 按正则/关键词匹配控件名/路径/属性值，输出匹配控件完整属性+子控件列表。
                               输出末尾含统计（控件数/最大深度/命中数）。
                               【推荐修改 UI 前先调用了解整体结构】

  search (--file <路径> | --content <JSON文本>) --keyword <关键词或正则>
                               按属性 key/value 内容搜索（轻量精确）。
                               适合查找纹理路径(textures/xxx)、绑定名(#xxx)、继承引用(@xxx)。
                               返回匹配的 [控件路径] key: value 列表。

  patch --file <路径> --ops '<JSON补丁数组>'
                               对 JSON UI 文件增量补丁修改（不重写整个文件）。原子执行（全部成功才写入）。
                               --ops 为 JSON 数组字符串，每项含 op 字段，可选值与参数:
                                 set_prop(设属性, 需 props 对象) | remove_prop(删属性, 需 keys 数组) |
                                 add_ctrl(加控件, 需 key+value) | remove_ctrl(删控件, 需 key) |
                                 replace_ctrl(替换控件, 需 key+value) |
                                 merge_ctrl(合并属性, 需 key+props, 可选 new_key) |
                                 add_top(加顶层, 需 key+value) | remove_top(删顶层, 需 key)
                               set_prop 用 props 对象（非 key/value）:
                               示例: patch --file D:/mod/ui/x.json --ops '[{"op":"set_prop","path":"main","props":{"size":["100%","100%"],"visible":false}}]'

flag 别名: --file=--path-file/--input, --content=--json, --ns=--namespace, --mod=--module,
           --depth=--max-depth, --root=--root-path, --keyword=--kw
)";
}

inline mcp::json help_result() {
    // help 同时返回速查手册 + 命令用法（两者合一，AI 一次拿到全部知识）
    return text_result(ui_help_text() + "\n\n" + ui_reference_text());
}

inline mcp::json reference_result() {
    return text_result(ui_reference_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + ui_help_text());
}

// ── flag 读取辅助（支持别名）──
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
    for (const auto& a : aliases) {
        if (pc.flags.find(a) != pc.flags.end()) return true;
    }
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

// ── 内容解析：从 --file 或 --content 获取 UI JSON 文本 ──
inline std::string resolve_ui_content(const ParsedCommand& pc) {
    std::string fpath = read_flag(pc, {"file", "path-file", "input"});
    std::string content = read_flag(pc, {"content", "json"});
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
            "必须提供 --file 或 --content 参数");
    return content;
}

// 解析 JSON 根对象（所有只读子命令共用）
inline nlohmann::json parse_ui_root(const std::string& content) {
    nlohmann::json root;
    try {
        root = nlohmann::json::parse(content, nullptr, true, true);
    } catch (const nlohmann::json::parse_error& e) {
        throw mcp::mcp_exception(mcp::error_code::invalid_params,
            std::string("JSON 解析失败: ") + e.what());
    }
    if (!root.is_object())
        throw mcp::mcp_exception(mcp::error_code::invalid_params, "JSON 根节点不是对象");
    return root;
}

// 按 / 分割控件路径（首尾空段忽略）
inline std::vector<std::string> split_control_path(const std::string& path) {
    std::vector<std::string> segs;
    std::istringstream ss(path);
    std::string seg;
    while (std::getline(ss, seg, '/'))
        if (!seg.empty()) segs.push_back(seg);
    return segs;
}

// 在 root 中按顶层名（支持 @继承写法）查找顶层控件
inline const nlohmann::json* find_top_control(const nlohmann::json& root,
                                              const std::string& top_key) {
    for (auto it = root.begin(); it != root.end(); ++it) {
        std::string k = it.key();
        auto at = k.find('@');
        std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
        if (bare == top_key) return &it.value();
    }
    return nullptr;
}

// 在 controls 数组中按段名查找子控件
inline const nlohmann::json* find_child_control(const nlohmann::json& parent,
                                                const std::string& seg) {
    if (!parent.contains("controls") || !parent["controls"].is_array())
        return nullptr;
    for (const auto& item : parent["controls"]) {
        if (!item.is_object()) continue;
        for (auto jt = item.begin(); jt != item.end(); ++jt) {
            std::string k = jt.key();
            auto at = k.find('@');
            std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
            if (bare == seg) return &jt.value();
        }
    }
    return nullptr;
}

// ── 各子命令分发 ──

// gen: 生成全栈模板
inline mcp::json dispatch_gen(const ParsedCommand& pc) {
    std::string tmpl = read_flag(pc, {"template", "t", "type"});
    std::string ns   = read_flag(pc, {"ns", "namespace"});
    std::string mod  = read_flag(pc, {"mod", "module"});
    if (tmpl.empty() || ns.empty() || mod.empty())
        return error_with_help("gen 需要 --template <类型> --ns <命名空间> --mod <Python模块名>。");
    try {
        auto result = generate_ui_fullstack(tmpl, ns, mod);
        if (!result.error.empty())
            return error_with_help(result.error);

        mcp::json content = mcp::json::array();
        content.push_back({{"type","text"},{"text", result.json_ui},{"label","json_ui"}});
        if (!result.python_class.empty())
            content.push_back({{"type","text"},{"text", result.python_class},{"label","python_class"}});
        if (!result.ui_defs_entry.empty())
            content.push_back({{"type","text"},{"text", result.ui_defs_entry},{"label","ui_defs_entry"}});
        return {{"content", content}};
    } catch (const mcp::mcp_exception& e) { return error_with_help(e.what()); }
    catch (const std::exception& e) { return error_with_help(e.what()); }
}

// diag: 诊断
inline mcp::json dispatch_diag(const ParsedCommand& pc) {
    try {
        std::string content = resolve_ui_content(pc);
        auto results = diagnose_ui(content);
        if (results.empty())
            return text_result("未发现问题，JSON UI 格式检查通过。");
        std::string text;
        for (const auto& d : results) {
            text += "[" + d.severity + "]";
            if (d.line > 0) text += " 行" + std::to_string(d.line);
            text += " " + d.message + "\n";
        }
        return text_result(text);
    } catch (const mcp::mcp_exception& e) { return error_with_help(e.what()); }
    catch (const std::exception& e) { return error_with_help(e.what()); }
}

// query: 查询控件（原 query_ui_control 逻辑搬迁）
inline mcp::json dispatch_query(const ParsedCommand& pc) {
    try {
        std::string content = resolve_ui_content(pc);
        std::string path    = read_flag(pc, {"path", "p"});
        auto root = parse_ui_root(content);

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
            return text_result(result);
        }

        auto segments = split_control_path(path);
        if (segments.empty())
            return error_with_help("query 的 --path 不能为空段。");

        // 逐层查找
        const nlohmann::json* current = find_top_control(root, segments[0]);
        if (!current || !current->is_object())
            return error_with_help("未找到顶层控件: " + segments[0]);
        for (size_t i = 1; i < segments.size(); ++i) {
            current = find_child_control(*current, segments[i]);
            if (!current)
                return error_with_help("路径 \"" + path + "\" 中未找到控件: " + segments[i]);
        }

        // 输出当前控件信息（不展开 controls 内部内容）
        std::string result = "控件: " + path + "\n";
        if (current->is_object()) {
            for (auto it = current->begin(); it != current->end(); ++it) {
                if (it.key() == "controls") {
                    result += "  controls: [";
                    bool first = true;
                    if (it.value().is_array()) {
                        for (const auto& child : it.value()) {
                            if (!child.is_object()) continue;
                            for (auto ct = child.begin(); ct != child.end(); ++ct) {
                                if (!first) result += ", ";
                                result += ct.key();
                                if (ct.value().is_object() && ct.value().contains("type"))
                                    result += "(" + ct.value()["type"].get<std::string>() + ")";
                                first = false;
                            }
                        }
                    }
                    result += "]\n";
                } else {
                    std::string val = it.value().dump();
                    if (val.size() > 120) val = val.substr(0, 117) + "...";
                    result += "  " + it.key() + ": " + val + "\n";
                }
            }
        } else {
            result += "  (值: " + current->dump() + ")\n";
        }
        return text_result(result);
    } catch (const mcp::mcp_exception& e) { return error_with_help(e.what()); }
    catch (const std::exception& e) { return error_with_help(e.what()); }
}

// tree: 生成控件树（原 dump_ui_tree 逻辑搬迁）
inline mcp::json dispatch_tree(const ParsedCommand& pc) {
    try {
        std::string content  = resolve_ui_content(pc);
        std::string root_path = read_flag(pc, {"root", "root-path"});
        int max_depth         = read_flag_int(pc, {"depth", "max-depth"}, 0);
        std::string search    = read_flag(pc, {"search"});

        auto root = parse_ui_root(content);

        // 树构建上下文
        struct TreeCtx {
            std::string output;
            int max_depth;
            std::string search_pattern;
            std::regex search_regex;
            bool has_search;
            int total_controls;
            int max_tree_depth;
            int search_hits;
        };

        TreeCtx ctx;
        ctx.max_depth = max_depth;
        ctx.search_pattern = search;
        ctx.has_search = !search.empty();
        ctx.total_controls = 0;
        ctx.max_tree_depth = 0;
        ctx.search_hits = 0;
        if (ctx.has_search) {
            try { ctx.search_regex = std::regex(search, std::regex::icase); }
            catch (...) {}
        }

        // 递归构建（使用 std::function 因为 lambda 无法直接自递归）
        std::function<void(const nlohmann::json&, const std::string&, const std::string&,
                           const std::string&, int, const std::string&)> dump_node;
        dump_node = [&](const nlohmann::json& node, const std::string& key,
                        const std::string& prefix, const std::string& child_prefix,
                        int depth, const std::string& full_path) {
            ctx.total_controls++;
            if (depth > ctx.max_tree_depth) ctx.max_tree_depth = depth;

            std::string desc = key;
            if (node.is_object()) {
                if (node.contains("type"))
                    desc += " (" + node["type"].get<std::string>() + ")";
                std::string extras;
                if (node.contains("size")) extras += " size:" + node["size"].dump();
                if (node.contains("anchor_from")) extras += " anchor:" + node["anchor_from"].get<std::string>();
                if (node.contains("layer")) extras += " L:" + std::to_string(node["layer"].get<int>());
                if (node.contains("visible") && !node["visible"].get<bool>()) extras += " [hidden]";
                if (!extras.empty()) desc += extras;
            }

            bool matches = true;
            if (ctx.has_search) {
                auto at_pos = key.find('@');
                std::string bare_key = (at_pos != std::string::npos) ? key.substr(0, at_pos) : key;
                try {
                    matches = std::regex_search(bare_key, ctx.search_regex) ||
                              std::regex_search(full_path, ctx.search_regex);
                } catch (...) {
                    matches = (bare_key.find(ctx.search_pattern) != std::string::npos ||
                               full_path.find(ctx.search_pattern) != std::string::npos);
                }
                if (!matches && node.is_object()) {
                    for (auto it = node.begin(); it != node.end() && !matches; ++it) {
                        if (it.key() == "controls") continue;
                        std::string val = it.value().dump();
                        try { matches = std::regex_search(val, ctx.search_regex); }
                        catch (...) { matches = (val.find(ctx.search_pattern) != std::string::npos); }
                    }
                }
            }

            if (!ctx.has_search || matches) {
                if (ctx.has_search && matches) {
                    ctx.search_hits++;
                    ctx.output += "── [" + full_path + "] ──\n";
                    ctx.output += desc + "\n";
                    if (node.is_object()) {
                        for (auto it = node.begin(); it != node.end(); ++it) {
                            if (it.key() == "controls") {
                                ctx.output += "  controls: [";
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
                                ctx.output += "  " + it.key() + ": " + val + "\n";
                            }
                        }
                    }
                    ctx.output += "\n";
                } else {
                    ctx.output += prefix + desc + "\n";
                }
            }

            if (ctx.max_depth > 0 && depth >= ctx.max_depth) {
                if (!ctx.has_search && node.is_object() && node.contains("controls"))
                    ctx.output += child_prefix + "└─ ...(depth limit)\n";
                return;
            }

            if (!node.is_object() || !node.contains("controls") || !node["controls"].is_array())
                return;

            const auto& controls = node["controls"];
            std::vector<std::pair<std::string, const nlohmann::json*>> children;
            for (const auto& item : controls) {
                if (!item.is_object()) continue;
                for (auto it = item.begin(); it != item.end(); ++it)
                    children.push_back({it.key(), &it.value()});
            }
            for (size_t i = 0; i < children.size(); ++i) {
                bool last = (i == children.size() - 1);
                std::string p = child_prefix + (last ? "└─ " : "├─ ");
                std::string cp = child_prefix + (last ? "   " : "│  ");
                auto at = children[i].first.find('@');
                std::string bare = (at != std::string::npos) ? children[i].first.substr(0, at) : children[i].first;
                dump_node(*children[i].second, children[i].first, p, cp, depth + 1, full_path + "/" + bare);
            }
        };

        if (!root_path.empty()) {
            auto segs = split_control_path(root_path);
            if (segs.empty())
                return error_with_help("tree 的 --root 不能为空段。");

            // 查找顶层控件，同时记录其原始 key（含 @继承后缀）
            const nlohmann::json* current = nullptr;
            std::string current_key;
            for (auto it = root.begin(); it != root.end(); ++it) {
                std::string k = it.key();
                auto at = k.find('@');
                std::string bare = (at != std::string::npos) ? k.substr(0, at) : k;
                if (bare == segs[0]) {
                    current = &it.value();
                    current_key = k;
                    break;
                }
            }
            if (!current)
                return error_with_help("未找到顶层控件: " + segs[0]);

            // 后续段在 controls 中逐层查找
            for (size_t i = 1; i < segs.size() && current; ++i) {
                current = find_child_control(*current, segs[i]);
            }
            if (!current)
                return error_with_help("路径 \"" + root_path + "\" 中未找到控件。");

            ctx.output = root_path + "\n";
            dump_node(*current, current_key, "", "", 0, root_path);
        } else {
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
                dump_node(*tops[i].second, tops[i].first, p, cp, 0, bare);
            }
        }

        if (ctx.output.empty()) ctx.output = "(无匹配结果)";
        std::string stats = "--- 统计: " + std::to_string(ctx.total_controls) +
            " 个控件, 最大深度 " + std::to_string(ctx.max_tree_depth);
        if (ctx.has_search) stats += ", 命中 " + std::to_string(ctx.search_hits) + " 项";
        stats += " ---\n";
        ctx.output += stats;
        return text_result(ctx.output);
    } catch (const mcp::mcp_exception& e) { return error_with_help(e.what()); }
    catch (const std::exception& e) { return error_with_help(e.what()); }
}

// search: 按属性内容搜索（原 search_ui_content 逻辑搬迁）
inline mcp::json dispatch_search(const ParsedCommand& pc) {
    try {
        std::string content = resolve_ui_content(pc);
        std::string keyword = read_flag(pc, {"keyword", "kw"});
        if (keyword.empty())
            return error_with_help("search 需要 --keyword <关键词或正则>。");

        auto root = parse_ui_root(content);
        std::regex re;
        bool use_regex = true;
        try { re = std::regex(keyword, std::regex::icase); }
        catch (...) { use_regex = false; }

        struct Match { std::string path, key, value; };
        std::vector<Match> matches;

        auto match_str = [&](const std::string& s) -> bool {
            if (use_regex) {
                try { return std::regex_search(s, re); }
                catch (...) { return s.find(keyword) != std::string::npos; }
            }
            return s.find(keyword) != std::string::npos;
        };

        std::function<void(const nlohmann::json&, const std::string&)> search_node;
        search_node = [&](const nlohmann::json& node, const std::string& path) {
            if (!node.is_object()) return;
            // 检查控件 key 名
            {
                auto slash = path.rfind('/');
                std::string ctrl_key = (slash != std::string::npos) ? path.substr(slash + 1) : path;
                if (match_str(ctrl_key)) matches.push_back({path, "(控件名)", ctrl_key});
            }
            for (auto it = node.begin(); it != node.end(); ++it) {
                if (it.key() == "controls") continue;
                std::string k = it.key();
                std::string v = it.value().dump();
                if (match_str(k) || match_str(v)) {
                    if (v.size() > 200) v = v.substr(0, 197) + "...";
                    matches.push_back({path, k, v});
                }
            }
            if (node.contains("controls") && node["controls"].is_array()) {
                for (const auto& child : node["controls"]) {
                    if (!child.is_object()) continue;
                    for (auto ct = child.begin(); ct != child.end(); ++ct)
                        search_node(ct.value(), path + "/" + ct.key());
                }
            }
        };

        for (auto it = root.begin(); it != root.end(); ++it) {
            if (it.key() == "namespace") continue;
            search_node(it.value(), it.key());
        }

        std::string output;
        std::string ns = root.value("namespace", "");
        if (!ns.empty()) output += "namespace: " + ns + "\n";
        if (matches.empty()) {
            output += "未找到匹配 \"" + keyword + "\" 的属性\n";
        } else {
            for (const auto& m : matches)
                output += "[" + m.path + "] " + m.key + ": " + m.value + "\n";
            output += "--- 共 " + std::to_string(matches.size()) + " 条匹配 ---\n";
        }
        return text_result(output);
    } catch (const mcp::mcp_exception& e) { return error_with_help(e.what()); }
    catch (const std::exception& e) { return error_with_help(e.what()); }
}

// patch: 增量补丁（原 patch_ui_file 逻辑搬迁）
inline mcp::json dispatch_patch(const ParsedCommand& pc) {
    std::string file_path = read_flag(pc, {"file", "path-file", "input"});
    std::string ops_str   = read_flag(pc, {"ops", "patches"});
    if (file_path.empty())
        return error_with_help("patch 需要 --file <绝对路径>（路径用正斜杠）。");
    if (ops_str.empty())
        return error_with_help("patch 需要 --ops '<JSON补丁数组>'。");

    try {
        auto ops = nlohmann::json::parse(ops_str);
        if (!ops.is_array())
            return error_with_help("--ops 必须是 JSON 数组。");

        auto result = apply_ui_patches(file_path, ops);
        if (!result.success) {
            return text_result("[FAILED] " + result.error +
                "\n已应用: " + std::to_string(result.applied_count) + " 个补丁（未写入文件）");
        }
        return text_result("成功应用 " + std::to_string(result.applied_count) + " 个补丁到: " + file_path);
    } catch (const nlohmann::json::parse_error& e) {
        return error_with_help(std::string("--ops JSON 解析失败: ") + e.what());
    } catch (const std::exception& e) {
        return error_with_help(e.what());
    }
}

// ── 主分发器 ──
inline mcp::json dispatch_minecraft_ui(const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = pc.sub;

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();
    if (sub == "reference" || sub == "ref")
        return reference_result();
    if (sub == "gen" || sub == "generate")
        return dispatch_gen(pc);
    if (sub == "diag" || sub == "diagnose")
        return dispatch_diag(pc);
    if (sub == "query" || sub == "q")
        return dispatch_query(pc);
    if (sub == "tree" || sub == "dump")
        return dispatch_tree(pc);
    if (sub == "search" || sub == "find")
        return dispatch_search(pc);
    if (sub == "patch")
        return dispatch_patch(pc);

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_ui_detail

inline void register_minecraft_ui_tools(mcp::server& srv) {
    auto tool = mcp::tool_builder(minecraft_ui_detail::kToolName)
        .with_description(
            "Minecraft JSON UI 全栈工具统一入口（网易版/国际版通用）：UI 速查手册、"
            "全栈模板生成（JSON+Python）、UI 文件诊断/查询/结构树/内容搜索、增量补丁修改。"
            "采用命令式用法，开发或修改 JSON UI 时请先调用 command=\"help\" 查看完整命令与速查手册。")
        .with_string_param("command",
            "命令语句，如 'tree --file D:/mod/ui/x.json'、'gen --template screen_basic --ns MY_UI --mod mymod' 或 "
            "'patch --file <path> --ops [...]'；首次使用请传 'help' 查看全部命令。", true)
        .with_read_only_hint(false).build();

    srv.register_tool(tool,
        [](const mcp::json& params, const std::string&) -> mcp::json {
            std::string command = params.value("command", "");
            return minecraft_ui_detail::dispatch_minecraft_ui(command);
        });
}

} // namespace mcdk
