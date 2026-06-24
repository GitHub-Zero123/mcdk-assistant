#pragma once
// register_minecraft_pixelart.hpp — 像素画统一入口工具 `minecraft_pixelart`
//
// 把原先 register_pixel_art.hpp 中 28 个独立像素画工具合并为单一命令式 MCP tool：
//   canvas_new / canvas_load / canvas_save / canvas_info / canvas_preview /
//   draw_pixel / draw_pixels_batch / draw_line / draw_rect / draw_circle /
//   fill_flood / fill_rect / fill_gradient /
//   apply_outline / apply_shadow / apply_dithering / apply_palette_quantize /
//   pixelate / read_pixel / read_area / extract_palette / replace_color /
//   transform_flip / transform_rotate / transform_scale / transform_crop
//
// 本文件只负责命令分发和 CLI 参数适配；所有像素操作仍复用 canvas_manager.hpp
// 的 CanvasManager 单例（get_canvas()），不重写业务逻辑。
//
// 与原 register_pixel_art.hpp 中 jstr/jint/jfloat/jbool/ok/err 等辅助函数解耦：
// 这里改用 command_parser.hpp 的 flag_int/flag_str/has_flag 读取 CLI flag，
// 不再依赖 mcp::json 参数对象（CLI 化的本质就是从 JSON 参数迁移到 flag）。
#include "tools/command_parser.hpp"
#include "tools/canvas_manager.hpp"
#include <mcp_message.h>
#include <mcp_server.h>
#include <mcp_tool.h>

#include <climits>
#include <string>
#include <vector>

namespace mcdk {
namespace minecraft_pixelart_detail {

constexpr const char* kToolName = "minecraft_pixelart";

// ── 标准返回格式 ──────────────────────────────────────────
inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type", "text"}, {"text", text}}})}};
}

inline mcp::json image_result(const std::string& b64) {
    return {{"content", mcp::json::array({
                {{"type", "image"}, {"data", b64}, {"mimeType", "image/png"}}
            })}};
}

// ── help 文本 ─────────────────────────────────────────────
inline std::string pixelart_help_text() {
    return R"(minecraft_pixelart — Minecraft 像素画处理统一入口（命令式用法）
用法: minecraft_pixelart(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/info，与 help/info 等价。

【通用规则（务必遵守）】
1. 参数含空格（如 JSON 数组）必须用 "..." 或 '...' 包裹成整体。
2. 颜色统一格式 #RRGGBBAA（# 前缀必须有！A=透明度 FF=不透明 00=透明），
   或 #RRGGBB（A 默认 FF）。不支持颜色名（如 red）、不支持省略 #。
3. 坐标/尺寸均为整数像素值；--opacity 为小数（0.0-1.0）；--tol 为整数（0-1020）。
4. Windows 文件路径请用正斜杠（如 D:/mod/textures/icon.png），不要用反斜杠，
   反斜杠在命令解析中会被当作转义符处理。
5. 除下方子命令外，画布是单例：每次 new 会覆盖旧画布。所有绘制/读取都作用于当前画布。

【画布管理】
  new --w <n> --h <n> [--bg #RRGGBBAA]
                               创建新画布（RGBA），默认透明背景 #00000000
  load --path <png路径>         从 PNG 加载画布（支持 RGBA，覆盖当前画布）
  save --path <png路径>         保存当前画布为 PNG（无损，含 Alpha）
  info  (无参数)                获取当前画布尺寸和颜色数量；无画布时返回 "No canvas loaded."
  preview [--scale 1-16]       返回放大后的 PNG 预览（base64，供 AI 视觉自检），默认放大 4 倍
                               注意：此处的 --scale 是"预览放大倍数"，与 scale 子命令无关

【基础绘制】
  pixel --x <n> --y <n> --color <#RRGGBBAA>
                               绘制单个像素
  batch --pixels '<JSON数组>'
                               批量绘制（主力工具）。JSON 数组形如:
                               [{"x":0,"y":0,"color":"#FF0000FF"},{"x":1,"y":1,"color":"#00FF00FF"}]
  line --x0 <n> --y0 <n> --x1 <n> --y1 <n> --color <c>
                               绘制像素直线（Bresenham）
  rect --x <n> --y <n> --w <n> --h <n> --color <c> [--filled]
                               绘制矩形；--filled 为实心填充（不带则空心）
  circle --cx <n> --cy <n> --r <n> --color <c> [--filled]
                               绘制像素圆（Midpoint Circle）；--filled 为实心

【填充】
  flood --x <n> --y <n> --color <c> [--tol <0-1020>]
                               油漆桶填充（BFS），--tol 为色差容差（整数，默认 0）
  gradient --x <n> --y <n> --w <n> --h <n> --color-a <c> --color-b <c> [--direction H|V]
                               线性渐变填充，H=水平（默认），V=垂直

【像素画质量】
  outline [--color #000000FF] [--thickness <n>]
                               对所有非透明像素自动添加外描边，默认黑色 1px
  shadow [--offset-x <n>] [--offset-y <n>] [--color #000000FF] [--opacity <0.0-1.0>]
                               添加像素风格阴影（右下偏移叠加），默认偏移(1,1) 不透明度0.6
  dither --palette '<颜色数组>'
                               应用 Floyd-Steinberg 抖动（颜色数组形如 ["#FF0000FF","#00FF00FF"]）
  quantize --palette '<颜色数组>' [--no-dither]
                               强制量化到指定调色板；默认使用抖动，--no-dither 关闭抖动
  pixelate --factor <2-64>
                               像素化效果（将图像降采样到 factor 块像素）

【解析读取】
  rpixel --x <n> --y <n>       读取单个像素的 RGBA 颜色值（返回 #RRGGBBAA）
  rarea --x <n> --y <n> --w <n> --h <n>
                               读取矩形区域所有像素颜色（二维数组 JSON，供精确分析）
  palette [--path <png路径>] [--max <1-256>]
                               提取主要颜色调色板（Median-Cut）；--path 省略则分析当前画布，--max 默认 16
  recolor --from <#RRGGBBAA> --to <#RRGGBBAA> [--tol <0-1020>]
                               全局颜色替换（支持容差）

【变换】（均直接修改当前画布）
  flip [--axis H|V]            翻转画布，H=水平（默认），V=垂直
  rotate [--angle 90|180|270]  顺时针旋转画布，默认 90
  scale --w <n> --h <n>        缩放画布（最近邻插值，保证像素清晰，无模糊）
  crop --x <n> --y <n> --w <n> --h <n>
                               裁切画布

【help】 显示本帮助

flag 别名（提升容错，长短均可）:
  --w=--width, --h=--height, --bg=--background, --tol=--tolerance,
  --filled=--fill, --from=--old-color/--old, --to=--new-color/--new,
  --no-dither=--nodither, --color-a=--ca, --color-b=--cb,
  --offset-x=--ox, --offset-y=--oy, --r=--radius, --scale=无别名(preview专用)
)";
}

inline mcp::json help_result() {
    return text_result(pixelart_help_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + pixelart_help_text());
}

// ── flag 别名归一化 ───────────────────────────────────────
// command_parser 的 normalize_flag_key 只处理 -s/-k/-a/-n；其余别名在此补齐。
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
                         const std::vector<std::string>& aliases,
                         int def) {
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

inline float read_flag_float(const ParsedCommand& pc,
                             const std::vector<std::string>& aliases,
                             float def) {
    for (const auto& a : aliases) {
        auto it = pc.flags.find(a);
        if (it != pc.flags.end() && !it->second.empty()) {
            try {
                return std::stof(it->second);
            } catch (...) {}
        }
    }
    return def;
}

// ── 各子命令分发 ──────────────────────────────────────────

// canvas_new
inline mcp::json dispatch_new(const ParsedCommand& pc) {
    int w = read_flag_int(pc, {"w", "width"}, -1);
    int h = read_flag_int(pc, {"h", "height"}, -1);
    if (w <= 0 || h <= 0)
        return error_with_help("new 需要 --w 和 --h（1-4096 的正整数）。");
    std::string bg = read_flag(pc, {"bg", "background"}, "#00000000");
    try {
        auto& canvas = get_canvas();
        bool leaked = !canvas.empty() && canvas.idle_seconds() > 1800;
        canvas.init(w, h, Pixel::from_hex(bg));
        std::string msg = "Canvas created: " + std::to_string(w) + "x" + std::to_string(h);
        if (leaked) msg += " [NOTE: Previous canvas was idle >30min and has been released]";
        return text_result(msg);
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// canvas_load
inline mcp::json dispatch_load(const ParsedCommand& pc) {
    std::string path = read_flag(pc, {"path"});
    if (path.empty())
        return error_with_help("load 需要 --path（PNG 文件绝对路径）。");
    try {
        auto& canvas = get_canvas();
        if (!canvas.load_png(path)) return error_with_help("Failed to load: " + path);
        return text_result("Loaded: " + path + " (" + std::to_string(canvas.width())
                         + "x" + std::to_string(canvas.height()) + ")");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// canvas_save
inline mcp::json dispatch_save(const ParsedCommand& pc) {
    std::string path = read_flag(pc, {"path"});
    if (path.empty())
        return error_with_help("save 需要 --path（PNG 文件绝对路径）。");
    try {
        auto& canvas = get_canvas();
        if (canvas.empty()) return error_with_help("Canvas is empty");
        if (!canvas.save_png(path)) return error_with_help("Failed to save: " + path);
        return text_result("Saved: " + path);
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// canvas_info
inline mcp::json dispatch_info(const ParsedCommand&) {
    auto& canvas = get_canvas();
    if (canvas.empty()) return text_result("No canvas loaded.");
    return text_result("Width=" + std::to_string(canvas.width())
                     + " Height=" + std::to_string(canvas.height())
                     + " Colors=" + std::to_string(canvas.color_count()));
}

// canvas_preview
inline mcp::json dispatch_preview(const ParsedCommand& pc) {
    int scale = read_flag_int(pc, {"scale"}, 4);
    try {
        auto& canvas = get_canvas();
        if (canvas.empty()) return error_with_help("Canvas is empty");
        std::string b64 = canvas.preview_base64(scale);
        if (b64.empty()) return error_with_help("Canvas is empty");
        return image_result(b64);
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// draw_pixel
inline mcp::json dispatch_pixel(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    std::string color = read_flag(pc, {"color", "c"});
    if (x == INT_MIN || y == INT_MIN || color.empty())
        return error_with_help("pixel 需要 --x --y --color。");
    try {
        get_canvas().set_pixel(x, y, Pixel::from_hex(color));
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// draw_pixels_batch
inline mcp::json dispatch_batch(const ParsedCommand& pc) {
    std::string pixels_json = read_flag(pc, {"pixels"});
    if (pixels_json.empty())
        return error_with_help("batch 需要 --pixels（JSON 数组字符串）。");
    try {
        auto arr = mcp::json::parse(pixels_json);
        if (!arr.is_array())
            return error_with_help("--pixels 必须是 JSON 数组。");
        std::vector<CanvasManager::DrawCmd> cmds;
        cmds.reserve(arr.size());
        for (const auto& item : arr) {
            cmds.push_back({
                item["x"].get<int>(),
                item["y"].get<int>(),
                Pixel::from_hex(item["color"].get<std::string>())
            });
        }
        get_canvas().draw_batch(cmds);
        return text_result("Drew " + std::to_string(cmds.size()) + " pixels");
    } catch (const std::exception& e) { return error_with_help(std::string("batch 解析失败: ") + e.what()); }
}

// draw_line
inline mcp::json dispatch_line(const ParsedCommand& pc) {
    int x0 = read_flag_int(pc, {"x0"}, INT_MIN);
    int y0 = read_flag_int(pc, {"y0"}, INT_MIN);
    int x1 = read_flag_int(pc, {"x1"}, INT_MIN);
    int y1 = read_flag_int(pc, {"y1"}, INT_MIN);
    std::string color = read_flag(pc, {"color", "c"});
    if (x0 == INT_MIN || y0 == INT_MIN || x1 == INT_MIN || y1 == INT_MIN || color.empty())
        return error_with_help("line 需要 --x0 --y0 --x1 --y1 --color。");
    try {
        get_canvas().draw_line(x0, y0, x1, y1, Pixel::from_hex(color));
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// draw_rect / fill_rect 合并：--filled 控制实心
inline mcp::json dispatch_rect(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    int w = read_flag_int(pc, {"w", "width"}, INT_MIN);
    int h = read_flag_int(pc, {"h", "height"}, INT_MIN);
    std::string color = read_flag(pc, {"color", "c"});
    if (x == INT_MIN || y == INT_MIN || w == INT_MIN || h == INT_MIN || color.empty())
        return error_with_help("rect 需要 --x --y --w --h --color。");
    bool filled = has_any_flag(pc, {"filled", "fill"});
    try {
        get_canvas().draw_rect(x, y, w, h, Pixel::from_hex(color), filled);
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// draw_circle
inline mcp::json dispatch_circle(const ParsedCommand& pc) {
    int cx = read_flag_int(pc, {"cx"}, INT_MIN);
    int cy = read_flag_int(pc, {"cy"}, INT_MIN);
    int r  = read_flag_int(pc, {"r", "radius"}, INT_MIN);
    std::string color = read_flag(pc, {"color", "c"});
    if (cx == INT_MIN || cy == INT_MIN || r == INT_MIN || color.empty())
        return error_with_help("circle 需要 --cx --cy --r --color。");
    bool filled = has_any_flag(pc, {"filled", "fill"});
    try {
        get_canvas().draw_circle(cx, cy, r, Pixel::from_hex(color), filled);
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// fill_flood
inline mcp::json dispatch_flood(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    std::string color = read_flag(pc, {"color", "c"});
    if (x == INT_MIN || y == INT_MIN || color.empty())
        return error_with_help("flood 需要 --x --y --color。");
    int tol = read_flag_int(pc, {"tol", "tolerance"}, 0);
    try {
        get_canvas().flood_fill(x, y, Pixel::from_hex(color), tol);
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// fill_gradient
inline mcp::json dispatch_gradient(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    int w = read_flag_int(pc, {"w", "width"}, INT_MIN);
    int h = read_flag_int(pc, {"h", "height"}, INT_MIN);
    std::string ca = read_flag(pc, {"color-a", "ca", "color_a"});
    std::string cb = read_flag(pc, {"color-b", "cb", "color_b"});
    if (x == INT_MIN || y == INT_MIN || w == INT_MIN || h == INT_MIN || ca.empty() || cb.empty())
        return error_with_help("gradient 需要 --x --y --w --h --color-a --color-b。");
    std::string direction = read_flag(pc, {"direction", "dir", "d"}, "H");
    try {
        get_canvas().fill_gradient(x, y, w, h,
            Pixel::from_hex(ca), Pixel::from_hex(cb), direction);
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// apply_outline
inline mcp::json dispatch_outline(const ParsedCommand& pc) {
    std::string color = read_flag(pc, {"color", "c"}, "#000000FF");
    int thickness = read_flag_int(pc, {"thickness", "thick", "t"}, 1);
    try {
        get_canvas().apply_outline(Pixel::from_hex(color), thickness);
        return text_result("Outline applied");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// apply_shadow
inline mcp::json dispatch_shadow(const ParsedCommand& pc) {
    int ox = read_flag_int(pc, {"offset-x", "ox", "offset_x"}, 1);
    int oy = read_flag_int(pc, {"offset-y", "oy", "offset_y"}, 1);
    std::string color = read_flag(pc, {"color", "c"}, "#000000FF");
    float opacity = read_flag_float(pc, {"opacity", "op"}, 0.6f);
    try {
        get_canvas().apply_shadow(ox, oy, Pixel::from_hex(color), opacity);
        return text_result("Shadow applied");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// apply_dithering
inline mcp::json dispatch_dither(const ParsedCommand& pc) {
    std::string pal_json = read_flag(pc, {"palette"});
    if (pal_json.empty())
        return error_with_help("dither 需要 --palette（颜色数组 JSON 字符串）。");
    try {
        auto arr = mcp::json::parse(pal_json);
        if (!arr.is_array() || arr.empty())
            return error_with_help("--palette 必须是非空 JSON 数组。");
        std::vector<Pixel> pal;
        pal.reserve(arr.size());
        for (const auto& c : arr)
            pal.push_back(Pixel::from_hex(c.get<std::string>()));
        get_canvas().apply_floyd_steinberg(pal);
        return text_result("Dithering applied with " + std::to_string(pal.size()) + " colors");
    } catch (const std::exception& e) { return error_with_help(std::string("dither 解析失败: ") + e.what()); }
}

// apply_palette_quantize
inline mcp::json dispatch_quantize(const ParsedCommand& pc) {
    std::string pal_json = read_flag(pc, {"palette"});
    if (pal_json.empty())
        return error_with_help("quantize 需要 --palette（颜色数组 JSON 字符串）。");
    bool dither = !has_any_flag(pc, {"no-dither", "nodither"});
    try {
        auto arr = mcp::json::parse(pal_json);
        if (!arr.is_array() || arr.empty())
            return error_with_help("--palette 必须是非空 JSON 数组。");
        std::vector<Pixel> pal;
        pal.reserve(arr.size());
        for (const auto& c : arr)
            pal.push_back(Pixel::from_hex(c.get<std::string>()));
        get_canvas().apply_palette_quantize(pal, dither);
        return text_result("Quantized to " + std::to_string(pal.size()) + " colors");
    } catch (const std::exception& e) { return error_with_help(std::string("quantize 解析失败: ") + e.what()); }
}

// pixelate
inline mcp::json dispatch_pixelate(const ParsedCommand& pc) {
    int factor = read_flag_int(pc, {"factor", "f"}, 0);
    if (factor < 2)
        return error_with_help("pixelate 需要 --factor（2-64）。");
    try {
        get_canvas().pixelate(factor);
        return text_result("Pixelated");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// read_pixel
inline mcp::json dispatch_rpixel(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    if (x == INT_MIN || y == INT_MIN)
        return error_with_help("rpixel 需要 --x --y。");
    try {
        return text_result(get_canvas().get_pixel(x, y).to_hex());
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// read_area
inline mcp::json dispatch_rarea(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    int w = read_flag_int(pc, {"w", "width"}, INT_MIN);
    int h = read_flag_int(pc, {"h", "height"}, INT_MIN);
    if (x == INT_MIN || y == INT_MIN || w == INT_MIN || h == INT_MIN)
        return error_with_help("rarea 需要 --x --y --w --h。");
    try {
        auto grid = get_canvas().read_area(x, y, w, h);
        mcp::json jgrid = mcp::json::array();
        for (const auto& row : grid) {
            mcp::json jrow = mcp::json::array();
            for (const auto& c : row) jrow.push_back(c);
            jgrid.push_back(jrow);
        }
        return text_result(jgrid.dump(2));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// extract_palette
inline mcp::json dispatch_palette(const ParsedCommand& pc) {
    std::string path = read_flag(pc, {"path"}, "");
    int max_colors = read_flag_int(pc, {"max", "max-colors", "max_colors"}, 16);
    try {
        if (path.empty()) {
            auto& canvas = get_canvas();
            if (canvas.empty()) return error_with_help("No canvas loaded");
            auto pal = canvas.extract_palette(max_colors);
            mcp::json arr = mcp::json::array();
            for (const auto& c : pal) arr.push_back(c.to_hex());
            return text_result(arr.dump());
        } else {
            CanvasManager tmp;
            if (!tmp.load_png(path)) return error_with_help("Failed to load: " + path);
            auto pal = tmp.extract_palette(max_colors);
            mcp::json arr = mcp::json::array();
            for (const auto& c : pal) arr.push_back(c.to_hex());
            return text_result(arr.dump());
        }
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// replace_color
inline mcp::json dispatch_recolor(const ParsedCommand& pc) {
    std::string from = read_flag(pc, {"from", "old-color", "old", "old_color"});
    std::string to   = read_flag(pc, {"to", "new-color", "new", "new_color"});
    if (from.empty() || to.empty())
        return error_with_help("recolor 需要 --from 和 --to（颜色 #RRGGBBAA）。");
    int tol = read_flag_int(pc, {"tol", "tolerance"}, 0);
    try {
        get_canvas().replace_color(Pixel::from_hex(from), Pixel::from_hex(to), tol);
        return text_result("OK");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// transform_flip
inline mcp::json dispatch_flip(const ParsedCommand& pc) {
    std::string axis = read_flag(pc, {"axis", "a"}, "H");
    try {
        get_canvas().flip(axis == "H");
        return text_result("Flipped");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// transform_rotate
inline mcp::json dispatch_rotate(const ParsedCommand& pc) {
    int angle = read_flag_int(pc, {"angle"}, 90);
    if (angle != 90 && angle != 180 && angle != 270)
        return error_with_help("rotate 的 --angle 必须是 90/180/270。");
    try {
        get_canvas().rotate90(angle / 90);
        return text_result("Rotated " + std::to_string(angle) + " degrees");
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// transform_scale
inline mcp::json dispatch_scale(const ParsedCommand& pc) {
    int w = read_flag_int(pc, {"w", "width"}, 0);
    int h = read_flag_int(pc, {"h", "height"}, 0);
    if (w <= 0 || h <= 0)
        return error_with_help("scale 需要 --w --h（正整数）。");
    try {
        auto& canvas = get_canvas();
        canvas.scale_nearest(w, h);
        return text_result("Scaled to " + std::to_string(canvas.width())
                         + "x" + std::to_string(canvas.height()));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// transform_crop
inline mcp::json dispatch_crop(const ParsedCommand& pc) {
    int x = read_flag_int(pc, {"x"}, INT_MIN);
    int y = read_flag_int(pc, {"y"}, INT_MIN);
    int w = read_flag_int(pc, {"w", "width"}, INT_MIN);
    int h = read_flag_int(pc, {"h", "height"}, INT_MIN);
    if (x == INT_MIN || y == INT_MIN || w == INT_MIN || h == INT_MIN)
        return error_with_help("crop 需要 --x --y --w --h。");
    try {
        auto& canvas = get_canvas();
        canvas.crop(x, y, w, h);
        return text_result("Cropped to " + std::to_string(canvas.width())
                         + "x" + std::to_string(canvas.height()));
    } catch (const std::exception& e) { return error_with_help(e.what()); }
}

// ── 主分发器 ──────────────────────────────────────────────
inline mcp::json dispatch_minecraft_pixelart(const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = pc.sub;  // parse_command 已 to_lower 并剥离前导 '/'

    if (sub.empty() || sub == "help" || sub == "?")
        return help_result();

    if (sub == "new")      return dispatch_new(pc);
    if (sub == "load")     return dispatch_load(pc);
    if (sub == "save")     return dispatch_save(pc);
    if (sub == "info")     return dispatch_info(pc);
    if (sub == "preview")  return dispatch_preview(pc);

    if (sub == "pixel")    return dispatch_pixel(pc);
    if (sub == "batch")    return dispatch_batch(pc);
    if (sub == "line")     return dispatch_line(pc);
    if (sub == "rect")     return dispatch_rect(pc);
    if (sub == "circle")   return dispatch_circle(pc);

    if (sub == "flood")    return dispatch_flood(pc);
    if (sub == "gradient") return dispatch_gradient(pc);

    if (sub == "outline")  return dispatch_outline(pc);
    if (sub == "shadow")   return dispatch_shadow(pc);
    if (sub == "dither")   return dispatch_dither(pc);
    if (sub == "quantize") return dispatch_quantize(pc);
    if (sub == "pixelate") return dispatch_pixelate(pc);

    if (sub == "rpixel")   return dispatch_rpixel(pc);
    if (sub == "rarea")    return dispatch_rarea(pc);
    if (sub == "palette")  return dispatch_palette(pc);
    if (sub == "recolor")  return dispatch_recolor(pc);

    if (sub == "flip")     return dispatch_flip(pc);
    if (sub == "rotate")   return dispatch_rotate(pc);
    if (sub == "scale")    return dispatch_scale(pc);
    if (sub == "crop")     return dispatch_crop(pc);

    return error_with_help("未知子命令: '" + pc.sub + "'。");
}

} // namespace minecraft_pixelart_detail

inline void register_minecraft_pixelart_tools(mcp::server& srv) {
    auto tool = mcp::tool_builder(minecraft_pixelart_detail::kToolName)
        .with_description(
            "Minecraft 像素画处理统一入口：画布创建/加载/保存、像素绘制（点/线/矩形/圆/批量）、"
            "填充（油漆桶/渐变）、像素画质量处理（描边/阴影/抖动/调色板量化/像素化）、"
            "颜色读取与替换、画布变换（翻转/旋转/缩放/裁切）。"
            "采用命令式用法，处理像素画或游戏贴图时请先调用 command=\"help\" 查看完整命令与子命令用法。")
        .with_string_param("command",
            "命令语句，如 'new --w 16 --h 16'、'pixel --x 0 --y 0 --color #FF0000FF' 或 "
            "'batch --pixels [{...}]'；首次使用请传 'help' 查看全部命令。", true)
        .build();

    srv.register_tool(tool,
        [](const mcp::json& params, const std::string&) -> mcp::json {
            std::string command = params.value("command", "");
            return minecraft_pixelart_detail::dispatch_minecraft_pixelart(command);
        });
}

} // namespace mcdk
