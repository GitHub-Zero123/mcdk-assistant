#pragma once
// register_pixel_art.hpp — 像素画 MCP 工具注册
// 提供: canvas_new/load/save/preview, draw_pixel/batch/line/rect/circle,
//       fill_flood/gradient, apply_outline/shadow/palette_quantize/dithering,
//       read_pixel/area, extract_palette, transform_*
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include "canvas_manager.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>

namespace mcdk {

// ── 工具: 从 JSON 参数读取辅助 ──────────────────────────
static inline std::string jstr(const mcp::json& p, const char* k, const char* def = "") {
    return p.contains(k) && p[k].is_string() ? p[k].get<std::string>() : def;
}
static inline int jint(const mcp::json& p, const char* k, int def = 0) {
    return p.contains(k) && p[k].is_number() ? p[k].get<int>() : def;
}
static inline float jfloat(const mcp::json& p, const char* k, float def = 0.f) {
    return p.contains(k) && p[k].is_number() ? p[k].get<float>() : def;
}
static inline bool jbool(const mcp::json& p, const char* k, bool def = false) {
    return p.contains(k) && p[k].is_boolean() ? p[k].get<bool>() : def;
}

// 返回成功文本
static mcp::json ok(const std::string& msg) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", msg}}})}};
}
// 返回错误文本
static mcp::json err(const std::string& msg) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", "[ERROR] " + msg}}})}};
}

inline void register_pixel_art_tools(mcp::server& srv) {

    auto& canvas = get_canvas();

    // ════════════════════════════════════════════════════════
    // 画布管理
    // ════════════════════════════════════════════════════════

    // canvas_new
    srv.register_tool(
        mcp::tool_builder("canvas_new")
            .with_description("创建新的像素画画布（RGBA透明背景）。bg_color默认#00000000（透明）")
            .with_string_param("bg_color", "背景色，格式#RRGGBBAA，默认#00000000", false)
            .with_number_param("width",  "画布宽度(1-4096)", true)
            .with_number_param("height", "画布高度(1-4096)", true)
            .build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                int w = jint(p, "width"), h = jint(p, "height");
                std::string bg = jstr(p, "bg_color", "#00000000");
                // 检查旧画布是否泄漏（超过30分钟未操作则自动清理）
                bool leaked = !canvas.empty() && canvas.idle_seconds() > 1800;
                canvas.init(w, h, Pixel::from_hex(bg));
                std::string msg = "Canvas created: " + std::to_string(w) + "x" + std::to_string(h);
                if (leaked) msg += " [NOTE: Previous canvas was idle >30min and has been released]";
                return ok(msg);
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // canvas_load
    srv.register_tool(
        mcp::tool_builder("canvas_load")
            .with_description("从PNG文件加载画布（支持RGBA，用于修改已有贴图）。path必须为绝对路径")
            .with_string_param("path", "PNG文件绝对路径（如 D:/project/textures/ui/icon.png）", true)
            .build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string path = jstr(p, "path");
                if (!canvas.load_png(path)) return err("Failed to load: " + path);
                return ok("Loaded: " + path + " (" + std::to_string(canvas.width())
                         + "x" + std::to_string(canvas.height()) + ")");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // canvas_save
    srv.register_tool(
        mcp::tool_builder("canvas_save")
            .with_description("将当前画布保存为PNG文件（含Alpha通道，无损）。path必须为绝对路径")
            .with_string_param("path", "输出PNG绝对路径（如 D:/project/textures/ui/icon.png）", true)
            .build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                if (canvas.empty()) return err("Canvas is empty");
                std::string path = jstr(p, "path");
                if (!canvas.save_png(path)) return err("Failed to save: " + path);
                return ok("Saved: " + path);
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // canvas_info
    srv.register_tool(
        mcp::tool_builder("canvas_info")
            .with_description("获取当前画布尺寸和颜色统计")
            .with_read_only_hint(true).with_idempotent_hint(true).build(),
        [&canvas](const mcp::json&, const std::string&) -> mcp::json {
            if (canvas.empty()) return ok("No canvas loaded.");
            std::string info = "Width=" + std::to_string(canvas.width())
                             + " Height=" + std::to_string(canvas.height())
                             + " Colors=" + std::to_string(canvas.color_count());
            return ok(info);
        });

    // canvas_preview
    srv.register_tool(
        mcp::tool_builder("canvas_preview")
            .with_description("返回当前画布放大后的Base64 PNG预览（供AI视觉自检）。scale默认4")
            .with_number_param("scale", "放大倍数(1-16)，默认4", false)
            .with_read_only_hint(true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            if (canvas.empty()) return err("Canvas is empty");
            int sc = jint(p, "scale", 4);
            std::string b64 = canvas.preview_base64(sc);
            return {{"content", mcp::json::array({
                {{"type","image"},{"data", b64},{"mimeType","image/png"}}
            })}};
        });

    // ════════════════════════════════════════════════════════
    // 基础绘制
    // ════════════════════════════════════════════════════════

    // draw_pixel
    srv.register_tool(
        mcp::tool_builder("draw_pixel")
            .with_description("在指定坐标绘制单个像素")
            .with_number_param("x", "X坐标", true)
            .with_number_param("y", "Y坐标", true)
            .with_string_param("color", "颜色#RRGGBBAA", true)
            .build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.set_pixel(jint(p,"x"), jint(p,"y"), Pixel::from_hex(jstr(p,"color")));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // draw_pixels_batch (主力工具)
    srv.register_tool(
        mcp::tool_builder("draw_pixels_batch")
            .with_description("批量绘制像素（主力工具）。pixels为JSON数组：[{\"x\":0,\"y\":0,\"color\":\"#RRGGBBAA\"}]")
            .with_array_param("pixels", "像素列表，每项含x/y/color字段", "object")
            .build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                const auto& arr = p["pixels"];
                std::vector<CanvasManager::DrawCmd> cmds;
                cmds.reserve(arr.size());
                for (const auto& item : arr) {
                    cmds.push_back({
                        item["x"].get<int>(),
                        item["y"].get<int>(),
                        Pixel::from_hex(item["color"].get<std::string>())
                    });
                }
                canvas.draw_batch(cmds);
                return ok("Drew " + std::to_string(cmds.size()) + " pixels");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // draw_line
    srv.register_tool(
        mcp::tool_builder("draw_line")
            .with_description("绘制像素直线（Bresenham算法）")
            .with_number_param("x0","起点X",true).with_number_param("y0","起点Y",true)
            .with_number_param("x1","终点X",true).with_number_param("y1","终点Y",true)
            .with_string_param("color","颜色#RRGGBBAA",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.draw_line(jint(p,"x0"),jint(p,"y0"),jint(p,"x1"),jint(p,"y1"),
                                 Pixel::from_hex(jstr(p,"color")));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // draw_rect
    srv.register_tool(
        mcp::tool_builder("draw_rect")
            .with_description("绘制矩形（空心/实心）")
            .with_number_param("x","左上角X",true).with_number_param("y","左上角Y",true)
            .with_number_param("w","宽度",true).with_number_param("h","高度",true)
            .with_string_param("color","颜色#RRGGBBAA",true)
            .with_boolean_param("filled","是否实心，默认false",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.draw_rect(jint(p,"x"),jint(p,"y"),jint(p,"w"),jint(p,"h"),
                                 Pixel::from_hex(jstr(p,"color")), jbool(p,"filled",false));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // draw_circle
    srv.register_tool(
        mcp::tool_builder("draw_circle")
            .with_description("绘制像素圆（Midpoint Circle算法）")
            .with_number_param("cx","圆心X",true).with_number_param("cy","圆心Y",true)
            .with_number_param("r","半径",true)
            .with_string_param("color","颜色#RRGGBBAA",true)
            .with_boolean_param("filled","是否实心，默认false",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.draw_circle(jint(p,"cx"),jint(p,"cy"),jint(p,"r"),
                                   Pixel::from_hex(jstr(p,"color")), jbool(p,"filled",false));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 填充
    // ════════════════════════════════════════════════════════

    // fill_flood
    srv.register_tool(
        mcp::tool_builder("fill_flood")
            .with_description("油漆桶填充（BFS）。tolerance为色差容差（0=精确匹配）")
            .with_number_param("x","种子X",true).with_number_param("y","种子Y",true)
            .with_string_param("color","新颜色#RRGGBBAA",true)
            .with_number_param("tolerance","容差0-1020，默认0",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.flood_fill(jint(p,"x"),jint(p,"y"),
                                  Pixel::from_hex(jstr(p,"color")), jint(p,"tolerance",0));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // fill_rect
    srv.register_tool(
        mcp::tool_builder("fill_rect")
            .with_description("矩形区域纯色填充")
            .with_number_param("x","左上角X",true).with_number_param("y","左上角Y",true)
            .with_number_param("w","宽度",true).with_number_param("h","高度",true)
            .with_string_param("color","颜色#RRGGBBAA",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.draw_rect(jint(p,"x"),jint(p,"y"),jint(p,"w"),jint(p,"h"),
                                 Pixel::from_hex(jstr(p,"color")), true);
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // fill_gradient
    srv.register_tool(
        mcp::tool_builder("fill_gradient")
            .with_description("线性渐变填充。direction: \"H\"水平 / \"V\"垂直")
            .with_number_param("x","X",true).with_number_param("y","Y",true)
            .with_number_param("w","宽",true).with_number_param("h","高",true)
            .with_string_param("color_a","起始颜色#RRGGBBAA",true)
            .with_string_param("color_b","结束颜色#RRGGBBAA",true)
            .with_string_param("direction","H或V，默认H",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.fill_gradient(jint(p,"x"),jint(p,"y"),jint(p,"w"),jint(p,"h"),
                                     Pixel::from_hex(jstr(p,"color_a")),
                                     Pixel::from_hex(jstr(p,"color_b")),
                                     jstr(p,"direction","H"));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 像素画质量工具
    // ════════════════════════════════════════════════════════

    // apply_outline
    srv.register_tool(
        mcp::tool_builder("apply_outline")
            .with_description("对所有非透明像素自动添加外描边。默认1px黑色")
            .with_string_param("color","描边颜色，默认#000000FF",false)
            .with_number_param("thickness","描边厚度（像素），默认1",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.apply_outline(Pixel::from_hex(jstr(p,"color","#000000FF")),
                                     jint(p,"thickness",1));
                return ok("Outline applied");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // apply_shadow
    srv.register_tool(
        mcp::tool_builder("apply_shadow")
            .with_description("添加像素风格阴影（右下角偏移叠加）")
            .with_number_param("offset_x","X偏移，默认1",false)
            .with_number_param("offset_y","Y偏移，默认1",false)
            .with_string_param("color","阴影颜色，默认#000000FF",false)
            .with_number_param("opacity","不透明度0.0-1.0，默认0.6",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.apply_shadow(jint(p,"offset_x",1), jint(p,"offset_y",1),
                                    Pixel::from_hex(jstr(p,"color","#000000FF")),
                                    jfloat(p,"opacity",0.6f));
                return ok("Shadow applied");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // apply_dithering
    srv.register_tool(
        mcp::tool_builder("apply_dithering")
            .with_description("对画布应用Floyd-Steinberg抖动（需配合palette参数）。palette为颜色数组")
            .with_array_param("palette","调色板颜色列表（#RRGGBBAA字符串数组）","string").build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::vector<Pixel> pal;
                for (const auto& c : p["palette"])
                    pal.push_back(Pixel::from_hex(c.get<std::string>()));
                canvas.apply_floyd_steinberg(pal);
                return ok("Dithering applied with " + std::to_string(pal.size()) + " colors");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // apply_palette_quantize
    srv.register_tool(
        mcp::tool_builder("apply_palette_quantize")
            .with_description("强制将画布颜色量化到指定调色板（保证不超色限，适合游戏物品贴图）")
            .with_array_param("palette","调色板颜色列表（#RRGGBBAA字符串数组）","string")
            .with_boolean_param("dither","是否使用抖动，默认true",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::vector<Pixel> pal;
                for (const auto& c : p["palette"])
                    pal.push_back(Pixel::from_hex(c.get<std::string>()));
                canvas.apply_palette_quantize(pal, jbool(p,"dither",true));
                return ok("Quantized to " + std::to_string(pal.size()) + " colors");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // pixelate
    srv.register_tool(
        mcp::tool_builder("pixelate")
            .with_description("像素化效果（将图像降采样到 factor 块像素，适合将草图转为像素风格）")
            .with_number_param("factor","像素块大小（2-64）",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.pixelate(jint(p,"factor",4));
                return ok("Pixelated");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 解析读取
    // ════════════════════════════════════════════════════════

    // read_pixel
    srv.register_tool(
        mcp::tool_builder("read_pixel")
            .with_description("读取单个像素的RGBA颜色值")
            .with_number_param("x","X坐标",true).with_number_param("y","Y坐标",true)
            .with_read_only_hint(true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                return ok(canvas.get_pixel(jint(p,"x"),jint(p,"y")).to_hex());
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // read_area
    srv.register_tool(
        mcp::tool_builder("read_area")
            .with_description("读取矩形区域所有像素颜色（二维数组，供AI精确分析）")
            .with_number_param("x","左上角X",true).with_number_param("y","左上角Y",true)
            .with_number_param("w","宽度",true).with_number_param("h","高度",true)
            .with_read_only_hint(true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                auto grid = canvas.read_area(jint(p,"x"),jint(p,"y"),jint(p,"w"),jint(p,"h"));
                mcp::json jgrid = mcp::json::array();
                for (const auto& row : grid) {
                    mcp::json jrow = mcp::json::array();
                    for (const auto& c : row) jrow.push_back(c);
                    jgrid.push_back(jrow);
                }
                std::string txt = jgrid.dump(2);
                return {{"content", mcp::json::array({{{ "type","text"},{"text", txt}}})}};
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // extract_palette
    srv.register_tool(
        mcp::tool_builder("extract_palette")
            .with_description("从PNG文件提取主要颜色调色板（Median-Cut算法）。max_colors默认16")
            .with_string_param("path","PNG文件绝对路径（如 D:/project/textures/ui/icon.png），不传则分析当前画布",false)
            .with_number_param("max_colors","最大颜色数(1-256)，默认16",false)
            .with_read_only_hint(true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string path = jstr(p,"path","");
                CanvasManager tmp;
                CanvasManager* target = &canvas;
                if (!path.empty()) {
                    if (!tmp.load_png(path)) return err("Failed to load: " + path);
                    target = &tmp;
                }
                if (target->empty()) return err("No canvas loaded");
                int mc = jint(p,"max_colors",16);
                auto pal = target->extract_palette(mc);
                mcp::json arr = mcp::json::array();
                for (const auto& c : pal) arr.push_back(c.to_hex());
                return {{"content", mcp::json::array({{{ "type","text"},{"text", arr.dump()}}})}};
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // replace_color
    srv.register_tool(
        mcp::tool_builder("replace_color")
            .with_description("全局颜色替换（支持容差）")
            .with_string_param("old_color","要替换的颜色#RRGGBBAA",true)
            .with_string_param("new_color","替换为的颜色#RRGGBBAA",true)
            .with_number_param("tolerance","容差0-1020，默认0",false).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.replace_color(Pixel::from_hex(jstr(p,"old_color")),
                                     Pixel::from_hex(jstr(p,"new_color")),
                                     jint(p,"tolerance",0));
                return ok("OK");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // ════════════════════════════════════════════════════════
    // 变换工具
    // ════════════════════════════════════════════════════════

    // transform_flip
    srv.register_tool(
        mcp::tool_builder("transform_flip")
            .with_description("翻转画布。axis: \"H\"水平翻转 / \"V\"垂直翻转")
            .with_string_param("axis","H或V",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.flip(jstr(p,"axis","H") == "H");
                return ok("Flipped");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // transform_rotate
    srv.register_tool(
        mcp::tool_builder("transform_rotate")
            .with_description("旋转画布（顺时针）。angle: 90/180/270")
            .with_number_param("angle","旋转角度90/180/270",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                int angle = jint(p,"angle",90);
                if (angle != 90 && angle != 180 && angle != 270)
                    return err("angle must be 90/180/270");
                canvas.rotate90(angle / 90);
                return ok("Rotated " + std::to_string(angle) + " degrees");
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // transform_scale
    srv.register_tool(
        mcp::tool_builder("transform_scale")
            .with_description("缩放画布（强制最近邻插值，保证像素清晰，杜绝双线性模糊）")
            .with_number_param("width","目标宽度",true)
            .with_number_param("height","目标高度",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.scale_nearest(jint(p,"width"), jint(p,"height"));
                return ok("Scaled to " + std::to_string(canvas.width()) + "x" + std::to_string(canvas.height()));
            } catch (const std::exception& e) { return err(e.what()); }
        });

    // transform_crop
    srv.register_tool(
        mcp::tool_builder("transform_crop")
            .with_description("裁切画布")
            .with_number_param("x","起始X",true).with_number_param("y","起始Y",true)
            .with_number_param("w","宽度",true).with_number_param("h","高度",true).build(),
        [&canvas](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                canvas.crop(jint(p,"x"),jint(p,"y"),jint(p,"w"),jint(p,"h"));
                return ok("Cropped to " + std::to_string(canvas.width()) + "x" + std::to_string(canvas.height()));
            } catch (const std::exception& e) { return err(e.what()); }
        });

} // register_pixel_art_tools

} // namespace mcdk
