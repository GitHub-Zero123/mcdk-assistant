#pragma once
// canvas_manager.hpp — 像素画画布核心管理器
// 依赖: stb_image / stb_image_write (libs/stb/)
//       base64 (libs/cpp-mcp/common/base64.hpp)
// 作者: MCDK-Assistant PixelArt Module

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <queue>
#include <unordered_set>
#include <functional>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <chrono>

// stb_image 由 stb_impl.cpp 提供实现，这里只声明接口
#include "stb_image.h"
#include "stb_image_write.h"

// base64 编码（来自 cpp-mcp 内置库）
#include "../../libs/cpp-mcp/common/base64.hpp"

// 兼容性：手写 clamp（防 IntelliSense 误报）
namespace mcdk_util {
    template<typename T>
    inline T clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }
}

namespace mcdk {

// ══════════════════════════════════════════════════════════
// Pixel: 4字节RGBA像素
// ══════════════════════════════════════════════════════════
struct Pixel {
    uint8_t r = 0, g = 0, b = 0, a = 255;

    bool operator==(const Pixel& o) const {
        return r == o.r && g == o.g && b == o.b && a == o.a;
    }
    bool operator!=(const Pixel& o) const { return !(*this == o); }

    // "#RRGGBBAA" 或 "#RRGGBB"（A默认255）
    static Pixel from_hex(const std::string& hex) {
        std::string h = hex;
        if (!h.empty() && h[0] == '#') h = h.substr(1);
        // 补齐到 8 位
        if (h.size() == 6) h += "FF";
        if (h.size() != 8) throw std::invalid_argument("Invalid color hex: " + hex);
        uint32_t v = (uint32_t)std::stoul(h, nullptr, 16);
        return { uint8_t(v >> 24), uint8_t(v >> 16), uint8_t(v >> 8), uint8_t(v) };
    }

    std::string to_hex() const {
        char buf[10];
        std::snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X", r, g, b, a);
        return std::string(buf);
    }

    // 计算与另一像素的色差（曼哈顿距离，含Alpha）
    int distance(const Pixel& o) const {
        return std::abs((int)r - o.r) + std::abs((int)g - o.g)
             + std::abs((int)b - o.b) + std::abs((int)a - o.a);
    }

    // Alpha混合: this 在 dst 上叠加
    Pixel blend_over(const Pixel& dst) const {
        if (a == 255) return *this;
        if (a == 0)   return dst;
        float fa = a / 255.0f;
        float fb = dst.a / 255.0f * (1.0f - fa);
        float fo = fa + fb;
        if (fo < 1e-6f) return {0,0,0,0};
        uint8_t nr = uint8_t((r * fa + dst.r * fb) / fo);
        uint8_t ng = uint8_t((g * fa + dst.g * fb) / fo);
        uint8_t nb = uint8_t((b * fa + dst.b * fb) / fo);
        uint8_t na = uint8_t(fo * 255);
        return {nr, ng, nb, na};
    }

    bool is_transparent() const { return a == 0; }
};

// ══════════════════════════════════════════════════════════
// CanvasManager: 单画布（无图层），RGBA 平坦数组
// ══════════════════════════════════════════════════════════
class CanvasManager {
public:
    CanvasManager() = default;

    // ── 生命周期 ─────────────────────────────────────────

    void init(int w, int h, Pixel bg = {0,0,0,0}) {
        if (w <= 0 || h <= 0 || w > 4096 || h > 4096)
            throw std::invalid_argument("Canvas size out of range (1..4096)");
        // 泄漏检测：如果有旧画布且超过30分钟未操作，强制清理并记录日志
        if (!pixels_.empty() && idle_seconds() > LEAK_TIMEOUT_SECONDS) {
            pixels_.clear();
            width_ = 0; height_ = 0;
            // （此处可扩展：写日志 / 告警）
        }
        width_  = w;
        height_ = h;
        pixels_.assign(w * h, bg);
        touch();
    }

    bool load_png(const std::string& path) {
        int w, h, ch;
        stbi_uc* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
        if (!data) return false;
        width_  = w;
        height_ = h;
        pixels_.resize(w * h);
        for (int i = 0; i < w * h; ++i) {
            pixels_[i] = { data[i*4], data[i*4+1], data[i*4+2], data[i*4+3] };
        }
        stbi_image_free(data);
        touch();
        return true;
    }

    bool save_png(const std::string& path) const {
        if (pixels_.empty()) return false;
        std::vector<uint8_t> raw(width_ * height_ * 4);
        for (int i = 0; i < width_ * height_; ++i) {
            raw[i*4+0] = pixels_[i].r;
            raw[i*4+1] = pixels_[i].g;
            raw[i*4+2] = pixels_[i].b;
            raw[i*4+3] = pixels_[i].a;
        }
        return stbi_write_png(path.c_str(), width_, height_, 4, raw.data(), width_ * 4) != 0;
    }

    // 返回放大 scale 倍后的 PNG Base64 字符串（用于 AI 预览）
    std::string preview_base64(int scale = 4) const {
        if (pixels_.empty()) return "";
        scale = mcdk_util::clamp(scale, 1, 16);
        int sw = width_ * scale, sh = height_ * scale;
        std::vector<uint8_t> raw(sw * sh * 4);
        for (int y = 0; y < height_; ++y)
            for (int x = 0; x < width_; ++x) {
                const Pixel& p = pixels_[y * width_ + x];
                for (int dy = 0; dy < scale; ++dy)
                    for (int dx = 0; dx < scale; ++dx) {
                        int idx = ((y*scale+dy)*sw + (x*scale+dx)) * 4;
                        raw[idx+0] = p.r; raw[idx+1] = p.g;
                        raw[idx+2] = p.b; raw[idx+3] = p.a;
                    }
            }
        // stb 写到内存
        std::vector<uint8_t> png_buf;
        stbi_write_png_to_func([](void* ctx, void* data, int size){
            auto* buf = reinterpret_cast<std::vector<uint8_t>*>(ctx);
            const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
            buf->insert(buf->end(), d, d + size);
        }, &png_buf, sw, sh, 4, raw.data(), sw * 4);

        std::string b64;
        base64::encode(png_buf.data(), png_buf.data() + png_buf.size(), std::back_inserter(b64));
        return b64;
    }

    int width()  const { return width_; }
    int height() const { return height_; }
    bool empty() const { return pixels_.empty(); }

    // ── 基础绘制 ─────────────────────────────────────────

    void set_pixel(int x, int y, Pixel c) {
        if (!in_bounds(x, y)) return;
        pixels_[y * width_ + x] = c;
    }

    Pixel get_pixel(int x, int y) const {
        if (!in_bounds(x, y)) return {0,0,0,0};
        return pixels_[y * width_ + x];
    }

    // 批量绘制，JSON: [{x,y,color}]
    struct DrawCmd { int x, y; Pixel color; };
    void draw_batch(const std::vector<DrawCmd>& cmds) {
        for (const auto& c : cmds) set_pixel(c.x, c.y, c.color);
        touch();
    }

    // Bresenham 直线
    void draw_line(int x0, int y0, int x1, int y1, Pixel c) {
        int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;
        while (true) {
            set_pixel(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 <  dx) { err += dx; y0 += sy; }
        }
        touch();
    }

    // 矩形（空心/实心）
    void draw_rect(int x, int y, int w, int h, Pixel c, bool filled) {
        if (filled) {
            for (int ry = y; ry < y + h; ++ry)
                for (int rx = x; rx < x + w; ++rx)
                    set_pixel(rx, ry, c);
        } else {
            for (int rx = x; rx < x + w; ++rx) {
                set_pixel(rx, y,       c);
                set_pixel(rx, y+h-1,   c);
            }
            for (int ry = y; ry < y + h; ++ry) {
                set_pixel(x,     ry,   c);
                set_pixel(x+w-1, ry,   c);
            }
        }
        touch();
    }

    // Midpoint Circle 算法
    void draw_circle(int cx, int cy, int r, Pixel c, bool filled) {
        auto plot8 = [&](int dx, int dy) {
            if (filled) {
                for (int px = cx - dx; px <= cx + dx; ++px) {
                    set_pixel(px, cy + dy, c);
                    set_pixel(px, cy - dy, c);
                }
                for (int px = cx - dy; px <= cx + dy; ++px) {
                    set_pixel(px, cy + dx, c);
                    set_pixel(px, cy - dx, c);
                }
            } else {
                set_pixel(cx+dx, cy+dy, c); set_pixel(cx-dx, cy+dy, c);
                set_pixel(cx+dx, cy-dy, c); set_pixel(cx-dx, cy-dy, c);
                set_pixel(cx+dy, cy+dx, c); set_pixel(cx-dy, cy+dx, c);
                set_pixel(cx+dy, cy-dx, c); set_pixel(cx-dy, cy-dx, c);
            }
        };
        int x = 0, y = r, d = 1 - r;
        plot8(x, y);
        while (x < y) {
            if (d < 0) d += 2*x + 3;
            else { d += 2*(x - y) + 5; --y; }
            ++x;
            plot8(x, y);
        }
        touch();
    }

    // ── 填充 ─────────────────────────────────────────────

    // 油漆桶填充（BFS，含容差）
    void flood_fill(int sx, int sy, Pixel new_color, int tolerance = 0) {
        if (!in_bounds(sx, sy)) return;
        Pixel target = get_pixel(sx, sy);
        if (target == new_color && tolerance == 0) return;

        std::queue<std::pair<int,int>> q;
        std::vector<bool> visited(width_ * height_, false);
        q.push({sx, sy});
        visited[sy * width_ + sx] = true;

        while (!q.empty()) {
            auto [x, y] = q.front(); q.pop();
            set_pixel(x, y, new_color);
            int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
            for (auto& d : dirs) {
                int nx = x + d[0], ny = y + d[1];
                if (!in_bounds(nx, ny)) continue;
                int idx = ny * width_ + nx;
                if (visited[idx]) continue;
                if (get_pixel(nx, ny).distance(target) <= tolerance) {
                    visited[idx] = true;
                    q.push({nx, ny});
                }
            }
        }
        touch();
    }

    // 线性渐变填充（方向: "H"水平 / "V"垂直）
    void fill_gradient(int x, int y, int w, int h,
                       Pixel ca, Pixel cb, const std::string& direction) {
        for (int ry = y; ry < y + h; ++ry) {
            for (int rx = x; rx < x + w; ++rx) {
                float t;
                if (direction == "V")
                    t = (h <= 1) ? 0.0f : (float)(ry - y) / (h - 1);
                else
                    t = (w <= 1) ? 0.0f : (float)(rx - x) / (w - 1);
                Pixel p;
                p.r = uint8_t(ca.r + (cb.r - ca.r) * t);
                p.g = uint8_t(ca.g + (cb.g - ca.g) * t);
                p.b = uint8_t(ca.b + (cb.b - ca.b) * t);
                p.a = uint8_t(ca.a + (cb.a - ca.a) * t);
                set_pixel(rx, ry, p);
            }
        }
        touch();
    }

    // ── 像素画质量工具 ──────────────────────────────────

    // 自动描边：对所有非透明像素的相邻透明像素绘制轮廓色
    void apply_outline(Pixel color, int thickness = 1) {
        std::vector<Pixel> orig = pixels_;
        int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        for (int t = 0; t < thickness; ++t) {
            std::vector<Pixel> cur = pixels_;
            for (int y = 0; y < height_; ++y) {
                for (int x = 0; x < width_; ++x) {
                    if (cur[y * width_ + x].a > 0) continue;
                    for (auto& d : dirs) {
                        int nx = x + d[0], ny = y + d[1];
                        if (!in_bounds(nx, ny)) continue;
                        if (cur[ny * width_ + nx].a > 0) {
                            pixels_[y * width_ + x] = color;
                            break;
                        }
                    }
                }
            }
        }
        touch();
    }

    // 像素风格阴影（右下偏移 + 颜色叠加）
    void apply_shadow(int offset_x, int offset_y, Pixel shadow_color, float opacity = 0.6f) {
        std::vector<Pixel> orig = pixels_;
        shadow_color.a = uint8_t(opacity * 255);
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                if (orig[y * width_ + x].a == 0) continue;
                int tx = x + offset_x, ty = y + offset_y;
                if (!in_bounds(tx, ty)) continue;
                if (pixels_[ty * width_ + tx].a == 0) {
                    pixels_[ty * width_ + tx] = shadow_color;
                }
            }
        }
        touch();
    }

    // 全局颜色替换
    void replace_color(Pixel old_c, Pixel new_c, int tolerance = 0) {
        for (auto& p : pixels_) {
            if (p.distance(old_c) <= tolerance) p = new_c;
        }
        touch();
    }

    // Floyd-Steinberg 抖动 + 调色板量化
    void apply_floyd_steinberg(const std::vector<Pixel>& palette) {
        // 转换为浮点误差扩散
        struct FPix { float r, g, b, a; };
        std::vector<FPix> fp(width_ * height_);
        for (int i = 0; i < width_ * height_; ++i)
            fp[i] = { (float)pixels_[i].r, (float)pixels_[i].g,
                      (float)pixels_[i].b, (float)pixels_[i].a };

        auto nearest = [&](FPix p) -> Pixel {
            float minD = 1e9f;
            Pixel best = palette[0];
            for (const auto& c : palette) {
                float d = (p.r-c.r)*(p.r-c.r) + (p.g-c.g)*(p.g-c.g)
                        + (p.b-c.b)*(p.b-c.b) + (p.a-c.a)*(p.a-c.a);
                if (d < minD) { minD = d; best = c; }
            }
            return best;
        };

        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                FPix& cur = fp[y * width_ + x];
                Pixel q = nearest(cur);
                pixels_[y * width_ + x] = q;
                float er = cur.r - q.r, eg = cur.g - q.g,
                      eb = cur.b - q.b, ea = cur.a - q.a;
                auto spread = [&](int nx, int ny, float f) {
                    if (!in_bounds(nx, ny)) return;
                    auto& t = fp[ny * width_ + nx];
                    t.r = mcdk_util::clamp(t.r + er * f, 0.f, 255.f);
                    t.g = mcdk_util::clamp(t.g + eg * f, 0.f, 255.f);
                    t.b = mcdk_util::clamp(t.b + eb * f, 0.f, 255.f);
                    t.a = mcdk_util::clamp(t.a + ea * f, 0.f, 255.f);
                };
                spread(x+1, y,   7.0f/16);
                spread(x-1, y+1, 3.0f/16);
                spread(x,   y+1, 5.0f/16);
                spread(x+1, y+1, 1.0f/16);
            }
        }
        touch();
    }

    // 调色板强制量化（含 dithering 选项）
    void apply_palette_quantize(const std::vector<Pixel>& palette, bool dither) {
        if (dither) {
            apply_floyd_steinberg(palette);
        } else {
            for (auto& p : pixels_) {
                int minD = INT_MAX;
                Pixel best = palette[0];
                for (const auto& c : palette) {
                    int d = p.distance(c);
                    if (d < minD) { minD = d; best = c; }
                }
                p = best;
            }
        }
        touch();
    }

    // 简单像素化（将图像降采样到 factor 分辨率后再还原）
    void pixelate(int factor) {
        if (factor <= 1) return;
        for (int y = 0; y < height_; y += factor) {
            for (int x = 0; x < width_; x += factor) {
                // 取区域平均色
                int sr=0, sg=0, sb=0, sa=0, cnt=0;
                for (int dy = 0; dy < factor && y+dy < height_; ++dy)
                    for (int dx = 0; dx < factor && x+dx < width_; ++dx) {
                        Pixel p = get_pixel(x+dx, y+dy);
                        sr += p.r; sg += p.g; sb += p.b; sa += p.a; ++cnt;
                    }
                Pixel avg{ uint8_t(sr/cnt), uint8_t(sg/cnt),
                           uint8_t(sb/cnt), uint8_t(sa/cnt) };
                for (int dy = 0; dy < factor && y+dy < height_; ++dy)
                    for (int dx = 0; dx < factor && x+dx < width_; ++dx)
                        set_pixel(x+dx, y+dy, avg);
            }
        }
        touch();
    }

    // ── 变换工具 ─────────────────────────────────────────

    void flip(bool horizontal) {
        if (horizontal) {
            for (int y = 0; y < height_; ++y)
                for (int x = 0; x < width_/2; ++x)
                    std::swap(pixels_[y*width_+x], pixels_[y*width_+(width_-1-x)]);
        } else {
            for (int y = 0; y < height_/2; ++y)
                for (int x = 0; x < width_; ++x)
                    std::swap(pixels_[y*width_+x], pixels_[(height_-1-y)*width_+x]);
        }
        touch();
    }

    // 顺时针旋转 90 度，times 次
    void rotate90(int times) {
        times = ((times % 4) + 4) % 4;
        for (int t = 0; t < times; ++t) {
            std::vector<Pixel> tmp(height_ * width_);
            for (int y = 0; y < height_; ++y)
                for (int x = 0; x < width_; ++x)
                    tmp[x * height_ + (height_ - 1 - y)] = pixels_[y * width_ + x];
            std::swap(width_, height_);
            pixels_ = std::move(tmp);
        }
        touch();
    }

    // 最近邻缩放（唯一合法的像素画缩放方式）
    void scale_nearest(int new_w, int new_h) {
        if (new_w <= 0 || new_h <= 0) return;
        std::vector<Pixel> tmp(new_w * new_h);
        for (int y = 0; y < new_h; ++y) {
            for (int x = 0; x < new_w; ++x) {
                int sx = x * width_  / new_w;
                int sy = y * height_ / new_h;
                tmp[y * new_w + x] = pixels_[sy * width_ + sx];
            }
        }
        width_  = new_w;
        height_ = new_h;
        pixels_ = std::move(tmp);
        touch();
    }

    // 裁切
    void crop(int cx, int cy, int cw, int ch) {
        cw = std::min(cw, width_  - cx);
        ch = std::min(ch, height_ - cy);
        if (cw <= 0 || ch <= 0) return;
        std::vector<Pixel> tmp(cw * ch);
        for (int y = 0; y < ch; ++y)
            for (int x = 0; x < cw; ++x)
                tmp[y*cw+x] = get_pixel(cx+x, cy+y);
        width_  = cw;
        height_ = ch;
        pixels_ = std::move(tmp);
        touch();
    }

    // ── 解析读取 ─────────────────────────────────────────

    // 返回区域颜色数组（JSON 用）
    std::vector<std::vector<std::string>> read_area(int rx, int ry, int rw, int rh) const {
        std::vector<std::vector<std::string>> result;
        for (int y = ry; y < ry + rh; ++y) {
            std::vector<std::string> row;
            for (int x = rx; x < rx + rw; ++x)
                row.push_back(get_pixel(x, y).to_hex());
            result.push_back(row);
        }
        return result;
    }

    // 统计颜色数量
    int color_count() const {
        struct PHash {
            size_t operator()(const Pixel& p) const {
                uint32_t v = (p.r << 24) | (p.g << 16) | (p.b << 8) | p.a;
                return std::hash<uint32_t>{}(v);
            }
        };
        struct PEq {
            bool operator()(const Pixel& a, const Pixel& b) const { return a == b; }
        };
        std::unordered_set<Pixel, PHash, PEq> s(pixels_.begin(), pixels_.end());
        return (int)s.size();
    }

    // Median-Cut 调色板提取（粗实现，最多 max_colors 色）
    std::vector<Pixel> extract_palette(int max_colors = 16) const {
        if (pixels_.empty()) return {};
        std::vector<Pixel> src;
        src.reserve(pixels_.size());
        for (const auto& p : pixels_)
            if (p.a > 0) src.push_back(p);
        if (src.empty()) return {};

        // 简化版：对 RGB 空间做递归中值切割
        std::function<std::vector<Pixel>(std::vector<Pixel>, int)> cut;
        cut = [&](std::vector<Pixel> bucket, int depth) -> std::vector<Pixel> {
            if (depth == 0 || bucket.size() <= 1) {
                // 返回平均色
                long long sr=0,sg=0,sb=0,sa=0;
                for (auto& p : bucket) { sr+=p.r;sg+=p.g;sb+=p.b;sa+=p.a; }
                int n = (int)bucket.size();
                return {{ uint8_t(sr/n), uint8_t(sg/n), uint8_t(sb/n), uint8_t(sa/n) }};
            }
            // 找范围最大的通道
            auto [minR,maxR] = std::minmax_element(bucket.begin(), bucket.end(),
                [](const Pixel& a, const Pixel& b){ return a.r < b.r; });
            auto [minG,maxG] = std::minmax_element(bucket.begin(), bucket.end(),
                [](const Pixel& a, const Pixel& b){ return a.g < b.g; });
            auto [minB,maxB] = std::minmax_element(bucket.begin(), bucket.end(),
                [](const Pixel& a, const Pixel& b){ return a.b < b.b; });
            int rng_r = maxR->r - minR->r;
            int rng_g = maxG->g - minG->g;
            int rng_b = maxB->b - minB->b;

            if (rng_r >= rng_g && rng_r >= rng_b)
                std::sort(bucket.begin(), bucket.end(), [](const Pixel& a, const Pixel& b){ return a.r < b.r; });
            else if (rng_g >= rng_b)
                std::sort(bucket.begin(), bucket.end(), [](const Pixel& a, const Pixel& b){ return a.g < b.g; });
            else
                std::sort(bucket.begin(), bucket.end(), [](const Pixel& a, const Pixel& b){ return a.b < b.b; });

            size_t mid = bucket.size() / 2;
            std::vector<Pixel> lo(bucket.begin(), bucket.begin() + mid);
            std::vector<Pixel> hi(bucket.begin() + mid, bucket.end());
            auto a = cut(std::move(lo), depth - 1);
            auto b = cut(std::move(hi), depth - 1);
            a.insert(a.end(), b.begin(), b.end());
            return a;
        };

        int depth = 0;
        while ((1 << depth) < max_colors) ++depth;
        auto palette = cut(src, depth);
        if ((int)palette.size() > max_colors) palette.resize(max_colors);
        return palette;
    }

    // 返回画布最后活跃时间距现在的秒数（用于泄漏检测）
    int64_t idle_seconds() const {
        if (pixels_.empty()) return 0;
        using namespace std::chrono;
        auto now = steady_clock::now();
        return duration_cast<seconds>(now - last_touch_).count();
    }

    // 手动刷新活跃时间戳（每次写操作内部已自动调用）
    void touch() {
        last_touch_ = std::chrono::steady_clock::now();
    }

private:
    int width_  = 0;
    int height_ = 0;
    std::vector<Pixel> pixels_;
    std::chrono::steady_clock::time_point last_touch_ = std::chrono::steady_clock::now();

    // 内存泄漏检测阈值：30分钟（1800秒）
    static constexpr int64_t LEAK_TIMEOUT_SECONDS = 1800;

    bool in_bounds(int x, int y) const {
        return x >= 0 && x < width_ && y >= 0 && y < height_;
    }
};

// 全局单实例（MCP Tool 共享使用）
inline CanvasManager& get_canvas() {
    static CanvasManager instance;
    return instance;
}

} // namespace mcdk
