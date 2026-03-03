# MCP Pixel Art Toolset — 像素画能力增强计划

> **目标**: 让 AI 具备专业级像素画（游戏物品、UI 元素）的 **作图 / 修改 / 解析** 能力，输出格式统一为 **PNG（含 Alpha 通道）**。
> 
> **约束**: C++20 / MSVC / 单可执行文件，不能引入 Python/Pillow，需使用纯 C++ 图像库。

---

## 1. AI 自带能力评估

| 能力 | 支持情况 | 说明 |
|---|---|---|
| 图像语义理解（色块/形状/风格识别） | ✅ 支持 | Claude / GPT-4o 的 Vision 能力 |
| 调色板颜色提取（给定参考图） | ✅ 支持 | Vision 可识别主要颜色，配合 MCP 精确提取 |
| 逐像素精确读取 / 输出二进制 PNG | ❌ 不支持 | 必须通过 MCP 工具桥接 |
| 对 64x64 以内图直接"逐行描述" | ⚠️ 有限 | Token 消耗大且易幻觉，通过 MCP `canvas_preview` 反馈闭环解决 |
| 理解像素画风格、配色规则 | ✅ 支持 | 可理解抖动、描边、调色板限制等概念 |

**结论**: AI 的"创意/感知"能力强，但落地到文件需要 MCP 作为**画布代理（Canvas Proxy）**。

---

## 2. 整体架构：画布代理模式

```
用户请求  ──>  AI (Vision + 推理)  ──>  MCP Tool 调用序列
                                            │
                              ┌─────────────▼──────────────────┐
                              │  Canvas Manager (C++)          │
                              │  - 内存 Canvas (RGBA u8 数组)  │
                              │  - 绘制算法 (Bresenham 等)     │
                              │  - PNG 读写 (stb_image)        │
                              │  - Base64 编码 (预览)          │
                              └─────────────┬──────────────────┘
                                            │
                              保存到磁盘 / Base64 预览反馈给 AI
```

**工作流**:
1. AI 分析用户需求（文字描述或参考图）→ 分解为绘画步骤。
2. AI 通过 MCP 工具序列操作内存 Canvas。
3. 关键步骤后 `canvas_preview` 返回 Base64 PNG，AI 视觉自检。
4. 完成后 `canvas_save` 导出 PNG 到指定路径。

---

## 3. 技术栈 (C++ 生态)

| 组件 | 技术选型 | 方式 | 理由 |
|---|---|---|---|
| **PNG 读写** | `stb_image` + `stb_image_write` | Header-only，加入 `libs/stb/` | 轻量、无依赖、完整支持 RGBA PNG |
| **Base64 编码** | 自实现（~50行）或 `cpp-base64` | Header-only | 用于 `canvas_preview` 返回内联图像 |
| **颜色量化/调色板** | 自实现 Median-Cut 算法 | 纯 C++ | `extract_palette` 工具依赖 |
| **抖动算法** | 自实现 Floyd-Steinberg | 纯 C++ | `apply_dithering` 工具依赖 |
| **MCP 框架** | 已有 `libs/cpp-mcp` | 直接复用 | — |
| **JSON 解析** | 已有 `nlohmann/json` | 直接复用 | 工具参数解析 |

### 新增依赖：`stb_image`（单文件 header-only）

```cmake
# CMakeLists.txt 新增
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

target_link_libraries(${PROJECT_NAME} PRIVATE stb)
```

只需下载 `stb_image.h` 和 `stb_image_write.h` 到 `libs/stb/`，在一个 `.cpp` 文件中定义实现宏即可：

```cpp
// src/tools/stb_impl.cpp
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
```

---

## 4. MCP 工具集定义 (API Design)

### 4.1 画布管理

| 工具 | 参数 | 说明 |
|---|---|---|
| `canvas_new` | `width, height, bg_color="#00000000"` | 创建新画布（默认透明背景） |
| `canvas_load` | `path` | 从 PNG 文件加载画布（用于修改已有图） |
| `canvas_save` | `path` | 保存为 PNG（含 Alpha 通道，`stb_image_write_png`） |
| `canvas_info` | — | 返回当前画布尺寸、颜色数量 |
| `canvas_preview` | `scale=4` | 返回放大后的 PNG Base64（AI 视觉自检用） |

### 4.2 基础绘制

| 工具 | 参数 | 说明 |
|---|---|---|
| `draw_pixel` | `x, y, color` | 绘制单个像素（RGBA hex，如 `#FF0000FF`） |
| `draw_pixels_batch` | `pixels: [{x,y,color}]` | **批量设置像素**（主力工具，一次提交全部数据） |
| `draw_line` | `x0,y0, x1,y1, color` | Bresenham 直线 |
| `draw_rect` | `x,y, w,h, color, filled` | 矩形（空心/实心） |
| `draw_circle` | `cx,cy, r, color, filled` | 像素圆（Midpoint Circle 算法） |

### 4.3 填充

| 工具 | 参数 | 说明 |
|---|---|---|
| `fill_flood` | `x, y, new_color, tolerance=0` | 油漆桶填充（BFS，支持 Alpha 容差） |
| `fill_rect` | `x,y, w,h, color` | 矩形区域纯色填充 |
| `fill_gradient` | `x,y, w,h, color_a, color_b, direction` | 线性渐变（适合 UI 背景色） |

### 4.4 像素画专项质量工具

| 工具 | 参数 | 说明 |
|---|---|---|
| `apply_outline` | `color="#000000FF", thickness=1` | 对非透明区域自动添加描边（保证物品在任意背景可见） |
| `apply_shadow` | `offset_x=1, offset_y=1, color, opacity` | 像素风格阴影（右下角移位叠加） |
| `apply_highlight` | `color, position="TL"` | 高光描边（左上角 1px 提亮，增强立体感） |
| `apply_palette_quantize` | `palette:[colors], dither=true` | 强制量化到指定调色板（Median-Cut + Floyd-Steinberg） |
| `apply_dithering` | `mode="floyd_steinberg"\|"ordered4"` | 独立抖动滤镜 |
| `pixelate` | `factor` | 将图像像素化（高清草图 → 像素风） |

### 4.5 图层系统

| 工具 | 参数 | 说明 |
|---|---|---|
| `layer_push` | `name` | 压入新图层（在内存中维护图层栈） |
| `layer_merge_all` | — | 合并所有图层到 Base |
| `layer_set_opacity` | `name, opacity` | 设置图层透明度（用于 UI 半透明叠加） |

### 4.6 解析与读取

| 工具 | 参数 | 说明 |
|---|---|---|
| `read_pixel` | `x, y` | 返回单像素 RGBA |
| `read_area` | `x, y, w, h` | 返回区域颜色二维数组（供 AI 精确分析） |
| `extract_palette` | `path, max_colors=16` | 提取图片主要调色板（Median-Cut 聚类） |
| `get_color_stats` | — | 返回当前画布颜色数量、使用最多的颜色 |

### 4.7 变换工具

| 工具 | 参数 | 说明 |
|---|---|---|
| `transform_flip` | `axis: "H"\|"V"` | 水平/垂直翻转 |
| `transform_rotate` | `angle: 90\|180\|270` | 无损旋转 |
| `transform_scale` | `width, height` | **只允许 Nearest Neighbor**，杜绝插值模糊 |
| `transform_crop` | `x, y, w, h` | 裁切 |
| `replace_color` | `old_color, new_color, tolerance=0` | 全局色彩替换 |

---

## 5. 像素画质量规范（面向 MCMOD 游戏物品 & UI）

### 5.1 游戏物品材质规范

- **尺寸**: 16x16（原版）或 32x32（高清包），最大 64x64。
- **调色板**: 建议 ≤ 32 色，每种物品主色不超过 5 个渐变层。
- **描边**: 物品必须有 1px 黑色（或深色）外描边，`apply_outline` 自动处理。
- **高光/阴影**: 左上角 1px 高光，右下角 1-2px 阴影，增强立体感。
- **透明**: 背景必须为 Alpha=0，像素边缘不得有半透明抗锯齿像素（硬边缘）。

### 5.2 UI 元素规范

- **尺寸**: 9-slice 切片（按钮框 3x3 分区），总尺寸 32x32 ～ 256x64。
- **背景**: 常用半透明深色叠加，不要纯黑。
- **网格对齐**: 所有元素必须对齐像素网格。

### 5.3 AI 作图质量检查流程

```
1. canvas_preview()        →  AI 视觉检查整体构图
2. read_area()             →  AI 精确校验边缘描边完整性
3. get_color_stats()       →  确认调色板未超规格
4. apply_outline()         →  自动修复缺失描边
5. canvas_save("out.png")  →  导出最终 PNG
```

---

## 6. 核心实现：Canvas Manager（C++ 内部模块）

```cpp
// src/tools/canvas_manager.hpp（伪代码结构）

struct Pixel { uint8_t r, g, b, a; };

class CanvasManager {
    int width_, height_;
    std::vector<Pixel> pixels_;   // RGBA 平坦数组
public:
    void init(int w, int h, Pixel bg);
    bool loadPng(const std::string& path);   // stb_image
    bool savePng(const std::string& path);   // stb_image_write
    std::string previewBase64(int scale);    // 内存编码后 Base64
    
    void drawPixel(int x, int y, Pixel c);
    void drawBatch(const std::vector<DrawCmd>& cmds);
    void drawLine(int x0,int y0,int x1,int y1, Pixel c);  // Bresenham
    void drawRect(int x,int y,int w,int h, Pixel c, bool filled);
    void floodFill(int x, int y, Pixel newColor, int tolerance);  // BFS
    
    void applyOutline(Pixel color, int thickness);
    void applyShadow(int dx, int dy, Pixel color, float opacity);
    void applyPaletteQuantize(const std::vector<Pixel>& palette, bool dither);
    void applyFloydSteinberg(const std::vector<Pixel>& palette);
    
    std::vector<Pixel> extractPalette(int maxColors);  // Median-Cut
    std::vector<std::vector<Pixel>> readArea(int x,int y,int w,int h);
    Pixel readPixel(int x, int y);
    
    void scaleNearest(int newW, int newH);  // 绝对不用双线性
    void flip(bool horizontal);
    void rotate90(int times);
};
```

**注册为 MCP Tool 的方式与现有工具一致**，参考 `src/tools/register_netease.hpp`。

---

## 7. 实施路线图

| 阶段 | 内容 | 优先级 |
|---|---|---|
| **Phase 1** | 引入 `stb_image`，实现 `CanvasManager` 基础（new/load/save/preview/draw_pixels_batch） | 🔴 最高 |
| **Phase 2** | 基础绘制（line/rect/circle）+ `fill_flood` + `read_area` | 🔴 最高 |
| **Phase 3** | 像素画质量工具（outline/shadow/highlight/quantize） | 🟠 高 |
| **Phase 4** | 变换工具（flip/rotate/scale_nearest/crop/replace_color） | 🟡 中 |
| **Phase 5** | 图层系统 + `extract_palette` + `apply_dithering` | 🟢 低 |

---

## 8. 关键设计决策

1. **`draw_pixels_batch` 是最重要的工具**: AI 生成像素画的核心是一次性提交像素坐标+颜色的 JSON 数组，而不是逐像素调用，避免 Tool 调用次数爆炸（一张 32x32 图最多 1024 次）。
2. **`canvas_preview` 提供视觉反馈闭环**: 每次关键步骤后 AI 可以"看一眼"当前效果，实现自我纠错。此工具的输出应为 `image/png` 类型的 Base64，使 Claude Vision 可直接解析。
3. **Nearest Neighbor 是唯一合法缩放**: 所有 `transform_scale` 必须用最近邻插值，杜绝任何模糊。
4. **`stb_image` 的 PNG 写入无压缩质量问题**: `stb_image_write_png` 默认无损，颜色精度完全保持，适合像素画。
5. **调色板硬约束**: 通过 `apply_palette_quantize` 强制量化，避免 AI 随意生成渐变导致游戏引擎无法识别的颜色超标。
