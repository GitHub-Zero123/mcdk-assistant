#pragma once
// register_nbt.hpp — Minecraft NBT 文件 MCP 工具注册
// 工具列表:
//   get_nbt_reference — NBT 类型 / tagged-JSON 约定 / 路径语法 / 支持格式速查（无参数）
//   nbt_view          — 解析 .mcstructure 等 NBT 文件，输出类型化树或 tagged-JSON（只读）
//   nbt_editor        — 对 NBT 文件做 set/remove/rename/add（支持批量 ops），按原格式写回
#include "tools/nbt_codec.hpp"
#include <mcp_server.h>
#include <mcp_tool.h>
#include <mcp_message.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace mcdk {

// ── 标准返回封装 ──────────────────────────────────────────
static inline mcp::json nbt_ok(const std::string& text) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", text}}})}};
}
static inline mcp::json nbt_err(const std::string& text) {
    return {{"content", mcp::json::array({{{ "type","text"},{"text", "[ERROR] " + text}}})}};
}

// ── get_nbt_reference 速查文本 ────────────────────────────
static const char* NBT_REFERENCE_TEXT = R"(
=== Minecraft NBT 工具速查（nbt_view / nbt_editor）===

一、支持的文件格式
  ✓ 基岩 .mcstructure        小端、无压缩
  ✓ 基岩 level.dat           小端、带 8 字节头（version + length）
  ✓ 无压缩 .nbt              大端(Java) 或 小端(Bedrock) 自动识别
  ✗ gzip 压缩的 Java .nbt/level.dat —— 暂不支持（会报错提示）
  端序在加载时自动探测，写回时保持原端序/原头部不变。

二、NBT 13 种标签类型（tag_type 取值）
  byte short int long        整数（1/2/4/8 字节，有符号）
  float double               浮点（4/8 字节）
  string                     字符串
  byte_array int_array long_array   定长数值数组
  list                       同类型元素列表（elem_type 标明元素类型）
  compound                   有序键值对（保留键顺序）

三、路径语法（path 参数）
  以 '/' 分段：compound 段=键名，list 段=整数下标(从0)，空串/"/"=根。
  例: structure/palette/default/block_palette/0/name
  注意: 键名内含 '/' 无法寻址（已知限制）。

四、tagged-JSON 约定（add 的 json_value、nbt_view format=json，均无损）
  每个标签 = {"type": <类型名>, ["elem_type": <list元素类型>,] "value": <值>}
    标量:  {"type":"int","value":17959425}   {"type":"string","value":"minecraft:stone"}
    数组:  {"type":"int_array","value":[1,2,3]}
    列表:  {"type":"list","elem_type":"compound","value":[ <tagged>, ... ]}
    复合:  {"type":"compound","value":{ "name": <tagged>, "states": <tagged> }}
  提示: 可先用 nbt_view(format=json, path=...) 取一段子树作为模板再改。

五、nbt_editor 操作（op / 批量 ops）
  set    path 处设标量值: value(+可选 tag_type，沿用原类型；新建标量必须给 tag_type)
  remove 删除 path 处标签（compound 键 / list 下标）
  rename 重命名 compound 键: path + new_name
  add    在 path(compound/list) 下新增: 标量用 tag_type+value；复杂子树用 json_value；compound 必须给 name
  批量: 传 ops（JSON 数组字符串），每项含 op 及参数，全部执行后一次写回，可选 save_path 另存。

六、nbt_create（从零新建文件）
  template=mcstructure(默认): size + blocks + fill → 合法可加载的简易结构；再用 nbt_editor 细调。
  template=empty: 仅空根 compound。
  json_value: 通用模式，直接用 tagged-JSON 根创建任意 NBT。
  默认不覆盖已存在文件，需 overwrite=true。
)";

// ── 单个 op 分发（s 为 tagged ordered_json 参数对象）─────────
inline std::string nbt_apply_op(nbt::NbtFile& f, const nbt::ojson& s) {
    std::string op   = s.value("op", "");
    std::string path = s.value("path", "");
    if (op == "set")    return nbt::op_set(f.root, path, s.value("value", ""), s.value("tag_type", ""));
    if (op == "remove") return nbt::op_remove(f.root, path);
    if (op == "rename") return nbt::op_rename(f.root, path, s.value("new_name", ""));
    if (op == "add") {
        std::string jv;
        if (s.contains("json_value") && !s.at("json_value").is_null())
            jv = s.at("json_value").is_string() ? s.at("json_value").get<std::string>() : s.at("json_value").dump();
        return nbt::op_add(f.root, path, s.value("name", ""), s.value("tag_type", ""), s.value("value", ""), jv);
    }
    throw std::runtime_error("未知 op: " + op + "（有效: set/remove/rename/add）");
}

// ── 注册函数 ──────────────────────────────────────────────
inline void register_nbt_tools(mcp::server& srv) {

    // 1. get_nbt_reference
    srv.register_tool(
        mcp::tool_builder("get_nbt_reference")
            .with_description("获取 Minecraft NBT 工具速查：支持格式、13 种标签类型、路径语法、tagged-JSON 约定、nbt_editor 操作列表")
            .with_read_only_hint(true).with_idempotent_hint(true).build(),
        [](const mcp::json&, const std::string&) -> mcp::json {
            return nbt_ok(NBT_REFERENCE_TEXT);
        });

    // 2. nbt_view — 解析并查看（只读）
    srv.register_tool(
        mcp::tool_builder("nbt_view")
            .with_description(
                "解析并查看 Minecraft NBT 文件（.mcstructure / level.dat / 无压缩 .nbt）。\n"
                "默认输出类型化缩进树（含端序、是否 level.dat 头）；format=json 输出无损 tagged-JSON。\n"
                "可用 path 只看某子树，max_depth/max_items 控制输出体积（大数组会截断）。")
            .with_string_param("file_path", "NBT 文件绝对路径（必须）", true)
            .with_string_param("path",      "只查看的子树路径（'/' 分段，compound 用键名、list 用下标），缺省看整棵树", false)
            .with_string_param("format",    "输出格式: tree(默认) | json(tagged-JSON，无损)", false)
            .with_number_param("max_depth", "最大展开深度，默认 12", false)
            .with_number_param("max_items", "每层最多展示的条目/数组元素数，默认 64，超出截断", false)
            .with_read_only_hint(true).build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string fpath = p.value("file_path", "");
                if (fpath.empty()) return nbt_err("nbt_view 需要 file_path（绝对路径）");
                std::string path = p.value("path", "");
                std::string fmt  = p.value("format", "tree");
                int max_depth = (p.contains("max_depth") && p["max_depth"].is_number()) ? p["max_depth"].get<int>() : 12;
                int max_items = (p.contains("max_items") && p["max_items"].is_number()) ? p["max_items"].get<int>() : 64;

                nbt::NbtFile f = nbt::load_nbt_file(fpath);

                // 定位子树（缺省为根）
                nbt::NbtTag* node = &f.root;
                if (!path.empty()) {
                    auto segs = nbt::split_path(path);
                    node = nbt::navigate(f.root, segs, segs.size());
                    if (!node) return nbt_err("路径不存在: " + path);
                }

                if (fmt == "json") {
                    return nbt_ok(nbt::to_tagged_json(*node).dump(2, ' ', false, nbt::ojson::error_handler_t::replace));
                }
                if (node == &f.root) return nbt_ok(nbt::to_tree(f, max_depth, max_items));
                std::ostringstream o;
                nbt::render_node(o, path, *node, 0, 0, max_depth, max_items);
                return nbt_ok(o.str());
            } catch (const std::exception& e) { return nbt_err(e.what()); }
        });

    // 3. nbt_editor — 增删改并写回
    srv.register_tool(
        mcp::tool_builder("nbt_editor")
            .with_description(
                "对 Minecraft NBT 文件做增删改，操作完按原端序/原 level.dat 头写回（默认覆盖原文件，可用 save_path 另存）。\n"
                "【批量】传 ops（JSON 数组字符串），每项含 op 及参数，全部执行后一次写回。\n"
                "【单次】直接传 op + 参数。op 可选值:\n"
                "  set    — path 处设标量: value(+tag_type，缺省沿用原类型；新建必须给 tag_type)\n"
                "  remove — 删除 path 处标签（compound 键 / list 下标）\n"
                "  rename — 重命名 compound 键: path + new_name\n"
                "  add    — 在 path(compound/list) 下新增: 标量用 tag_type+value；复杂子树用 json_value(tagged-JSON)；compound 必须给 name\n"
                "tagged-JSON 与路径语法见 get_nbt_reference。")
            .with_string_param("file_path", "NBT 文件绝对路径（必须）", true)
            .with_string_param("save_path", "另存路径，缺省覆盖 file_path", false)
            .with_string_param("ops",       "批量操作数组（JSON 字符串），与单次 op 二选一", false)
            .with_string_param("op",        "操作类型: set/remove/rename/add（单次模式）", false)
            .with_string_param("path",      "目标路径（'/' 分段）", false)
            .with_string_param("name",      "新增到 compound 时的键名（add 用）", false)
            .with_string_param("new_name",  "新键名（rename 用）", false)
            .with_string_param("tag_type",  "标签类型: byte/short/int/long/float/double/string/...（set/add 用）", false)
            .with_string_param("value",     "标量值（按 tag_type 解析；set/add 用）", false)
            .with_string_param("json_value","复杂子树的 tagged-JSON 字符串（add 用，见 get_nbt_reference）", false)
            .build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string fpath = p.value("file_path", "");
                if (fpath.empty()) return nbt_err("nbt_editor 需要 file_path（绝对路径）");

                nbt::NbtFile f = nbt::load_nbt_file(fpath);
                std::ostringstream out;

                bool has_ops = p.contains("ops") && !p["ops"].is_null();
                if (has_ops) {
                    nbt::ojson ops_arr = p["ops"].is_string()
                        ? nbt::ojson::parse(p["ops"].get<std::string>())
                        : nbt::ojson::parse(p["ops"].dump());
                    if (!ops_arr.is_array()) return nbt_err("ops 必须是 JSON 数组，每项含 op 字段");
                    int n = static_cast<int>(ops_arr.size());
                    for (int i = 0; i < n; ++i)
                        out << "[" << (i + 1) << "/" << n << "] " << nbt_apply_op(f, ops_arr[i]) << "\n";
                } else {
                    std::string op = p.value("op", "");
                    if (op.empty()) return nbt_err("请提供 op（单次）或 ops（批量）参数");
                    // 单次模式：把 mcp::json 参数转为 ordered_json 供分发读取
                    out << nbt_apply_op(f, nbt::ojson::parse(p.dump())) << "\n";
                }

                std::string target = p.value("save_path", "");
                if (target.empty()) target = fpath;
                nbt::save_nbt_file(target, f);
                out << "[已写回文件: " << target << "]";
                return nbt_ok(out.str());
            } catch (const std::exception& e) { return nbt_err(e.what()); }
        });

    // 4. nbt_create — 从零创建新 NBT 文件（默认生成简易 mcstructure）
    srv.register_tool(
        mcp::tool_builder("nbt_create")
            .with_description(
                "从零创建一个新的 NBT 文件并保存。两种用法:\n"
                "【mcstructure 模板】template=mcstructure(默认): 给 size + blocks(方块表) + fill，"
                "生成合法可加载的简易结构（layer0 全填 fill 下标，layer1 全 -1）；之后可用 nbt_editor 细调每个位置。\n"
                "【通用】json_value: 直接用 tagged-JSON 根(compound)创建任意 NBT（见 get_nbt_reference）。\n"
                "默认不覆盖已存在文件，需 overwrite=true。")
            .with_string_param("file_path",  "保存的绝对路径（如 D:/mod/BP/structures/house.mcstructure）", true)
            .with_string_param("template",   "mcstructure(默认) | empty(仅空根 compound)", false)
            .with_string_param("size",       "结构尺寸 [x,y,z]（JSON 数组字符串，mcstructure 用，默认 [1,1,1]）", false)
            .with_string_param("blocks",     "方块名表（JSON 数组字符串，如 [\"minecraft:stone\"]，默认 [\"minecraft:stone\"]）", false)
            .with_number_param("fill",       "layer0 统一填充的 palette 下标（默认 0）", false)
            .with_number_param("block_version", "方块 version 兼容号（默认 18161159）", false)
            .with_string_param("json_value", "通用模式: tagged-JSON 根(compound) 字符串，提供则忽略 template", false)
            .with_boolean_param("little_endian", "通用/empty 模式字节序，默认 true(基岩)", false)
            .with_boolean_param("overwrite", "目标已存在时是否覆盖，默认 false", false)
            .build(),
        [](const mcp::json& p, const std::string&) -> mcp::json {
            try {
                std::string fpath = p.value("file_path", "");
                if (fpath.empty()) return nbt_err("nbt_create 需要 file_path（绝对路径）");
                bool overwrite = p.contains("overwrite") && p["overwrite"].is_boolean() && p["overwrite"].get<bool>();
                if (!overwrite && std::filesystem::exists(mcdk::path::from_utf8(fpath)))
                    return nbt_err("文件已存在；如需覆盖请加 overwrite=true: " + fpath);

                nbt::NbtFile f;
                std::string jv = p.value("json_value", "");
                std::string desc;
                if (!jv.empty()) {
                    f.root = nbt::from_tagged_json(nbt::ojson::parse(jv));
                    if (f.root.type != nbt::TAG_Compound) return nbt_err("NBT 根必须是 compound");
                    f.little_endian = !(p.contains("little_endian") && p["little_endian"].is_boolean() && !p["little_endian"].get<bool>());
                    desc = "从 json_value 创建";
                } else {
                    std::string tmpl = p.value("template", "mcstructure");
                    if (tmpl == "empty") {
                        f.root = nbt::tag_compound();
                        f.little_endian = !(p.contains("little_endian") && p["little_endian"].is_boolean() && !p["little_endian"].get<bool>());
                        desc = "创建空根 compound";
                    } else if (tmpl == "mcstructure") {
                        // size 默认 [1,1,1]
                        int sx = 1, sy = 1, sz = 1;
                        if (p.contains("size") && p["size"].is_string() && !p["size"].get<std::string>().empty()) {
                            auto sz_arr = nlohmann::json::parse(p["size"].get<std::string>());
                            if (!sz_arr.is_array() || sz_arr.size() != 3) return nbt_err("size 必须是含 3 个整数的 JSON 数组");
                            sx = sz_arr[0].get<int>(); sy = sz_arr[1].get<int>(); sz = sz_arr[2].get<int>();
                        }
                        // blocks 默认 ["minecraft:stone"]
                        std::vector<std::string> blocks{"minecraft:stone"};
                        if (p.contains("blocks") && p["blocks"].is_string() && !p["blocks"].get<std::string>().empty()) {
                            auto barr = nlohmann::json::parse(p["blocks"].get<std::string>());
                            if (!barr.is_array() || barr.empty()) return nbt_err("blocks 必须是非空 JSON 字符串数组");
                            blocks.clear();
                            for (auto& b : barr) blocks.push_back(b.get<std::string>());
                        }
                        int fill = (p.contains("fill") && p["fill"].is_number()) ? p["fill"].get<int>() : 0;
                        int bver = (p.contains("block_version") && p["block_version"].is_number()) ? p["block_version"].get<int>() : 18161159;
                        f = nbt::build_mcstructure(sx, sy, sz, blocks, fill, bver);
                        desc = "创建 mcstructure " + std::to_string(sx) + "x" + std::to_string(sy) + "x" + std::to_string(sz)
                             + "，palette " + std::to_string(blocks.size()) + " 项，fill=" + std::to_string(fill);
                    } else {
                        return nbt_err("未知 template: " + tmpl + "（可选: mcstructure / empty）");
                    }
                }

                nbt::save_nbt_file(fpath, f);
                return nbt_ok(desc + "\n[已保存: " + fpath + "]\n提示: 用 nbt_view 查看，用 nbt_editor 修改 structure/block_indices 等内容。");
            } catch (const std::exception& e) { return nbt_err(e.what()); }
        });
}

} // namespace mcdk
