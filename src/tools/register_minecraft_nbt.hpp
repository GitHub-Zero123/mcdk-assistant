#pragma once
// register_minecraft_nbt.hpp - command-style wrapper for Minecraft NBT tools.
//
// This keeps the NBT implementation from register_nbt.hpp, but exposes it with
// the same single-command shape as minecraft_docs/ui/model/animation/pixelart.

#include "tools/command_parser.hpp"
#include "tools/register_nbt.hpp"
#include <mcp_server.h>
#include <mcp_tool.h>

#include <string>

namespace mcdk {
namespace minecraft_nbt_detail {

constexpr const char* kToolName = "minecraft_nbt";

inline mcp::json text_result(const std::string& text) {
    return {{"content", mcp::json::array({{{"type","text"},{"text", text}}})}};
}

inline bool flag_bool(const ParsedCommand& pc, const std::string& key, bool def = false) {
    auto it = pc.flags.find(key);
    if (it == pc.flags.end()) return def;
    if (it->second.empty()) return true;
    std::string v = command_parser_detail::to_lower_ascii(it->second);
    if (v == "1" || v == "true" || v == "yes" || v == "on") return true;
    if (v == "0" || v == "false" || v == "no" || v == "off") return false;
    return def;
}

inline std::string flag_any(const ParsedCommand& pc,
                            const std::initializer_list<const char*>& keys,
                            const std::string& def = "") {
    for (const char* key : keys) {
        auto it = pc.flags.find(key);
        if (it != pc.flags.end()) return it->second;
    }
    return def;
}

inline int flag_int_any(const ParsedCommand& pc,
                        const std::initializer_list<const char*>& keys,
                        int def) {
    for (const char* key : keys) {
        if (has_flag(pc, key)) return flag_int(pc, key, def);
    }
    return def;
}

inline std::string nbt_help_text() {
    return R"(minecraft_nbt — Minecraft NBT 文件工具统一入口（命令式用法）
用法: minecraft_nbt(command="<子命令> [参数...] [--选项 值]")
命令名前可加 '/'，例如 /help、/view，与 help/view 等价。
重要: 参数含空格、JSON 数组或 JSON 对象时，必须用 "..." 或 '...' 包裹成整体。
写入类命令不会自动创建父目录，目标目录必须已存在。

【子命令】
  help | reference
      查看 NBT 格式、tagged-JSON、路径语法和编辑操作速查。

  view --file <path> [--path <nbt/path>] [--format tree|json] [--max-depth <n>] [--max-items <n>]
      读取 NBT 文件（.mcstructure / level.dat / 未压缩 .nbt），输出树或 tagged-JSON。

  create --file <path> [--template mcstructure|empty] [--size "[x,y,z]"] [--blocks "[\"minecraft:stone\"]"]
         [--fill <n>] [--block-version <n>] [--little-endian true|false] [--overwrite]
      创建新的 NBT 文件。
      父目录必须已存在；已有文件默认受保护，只有传入 --overwrite 才会覆盖。

  edit --file <path> [--save <path>] (--ops '<JSON array>' | --op set|remove|rename|add ...)
      编辑 NBT 文件。不传 --save 时会直接写回原文件。
      使用 --save 另存时，另存路径的父目录必须已存在。
      单操作常用选项: --path、--name、--tag-type、--value、--new-name、--json-value。

示例:
  view --file D:/world/structures/a.mcstructure --max-depth 6
  view --file D:/world/structures/a.mcstructure --path structure/palette/default/block_palette/0/name
  create --file D:/tmp/test.mcstructure --template mcstructure --size "[2,2,2]" --overwrite
  edit --file D:/tmp/test.mcstructure --save D:/tmp/test.copy.mcstructure --ops '[{"op":"set","path":"format_version","value":"2"}]')";
}

inline std::string nbt_reference_text() {
    return R"(=== NBT 速查手册 ===

安全工作流:
  1. 先用 view 查看文件结构并定位 path。
  2. 试改时优先使用 edit --save <新文件> 写到副本。
  3. 只有确认要覆盖原文件时，才省略 --save。

支持的文件:
  - 基岩版 .mcstructure: 小端、未压缩。
  - 基岩版 level.dat: 小端，带 8 字节 Bedrock 头。
  - 未压缩 .nbt: Java 大端或 Bedrock 小端，自动探测。
  - gzip 压缩的 Java .nbt / level.dat 暂不支持。
  保存时会保留探测到的端序和头部格式。

路径语法:
  - 用 '/' 分隔路径段。
  - compound 路径段是键名。
  - list 路径段是数字下标，从 0 开始。
  - 空路径表示根 compound。
  - 示例: structure/palette/default/block_palette/0/name
  - 当前限制: 键名中包含 '/' 时无法寻址。

--tag-type 可用类型:
  byte, short, int, long, float, double, string,
  byte_array, int_array, long_array, list, compound

tagged-JSON:
  每个 tag 写成 {"type":"int","value":123}。
  list 需要 elem_type，例如:
    {"type":"list","elem_type":"string","value":[{"type":"string","value":"a"}]}
  compound 的子节点也使用 tagged 值:
    {"type":"compound","value":{"name":{"type":"string","value":"minecraft:stone"}}}
  可以先用 view --format json 复制一段子树，作为 --json-value 模板再修改。

编辑操作:
  set
      设置 --path 处的标量值。已有 tag 省略 --tag-type 时沿用原类型。
      新建标量 tag 时必须提供 --tag-type。
      示例: edit --file D:/x.mcstructure --op set --path format_version --value 2

  remove
      删除 compound 键或 list 项。
      示例: edit --file D:/x.mcstructure --op remove --path metadata/foo

  rename
      重命名 compound 键。
      示例: edit --file D:/x.mcstructure --op rename --path metadata/foo --new-name bar

  add
      在 compound/list 下新增子节点。向 compound 新增时必须提供 --name。
      新增标量子节点时使用 --tag-type 和 --value。
      新增复杂子树时使用 --json-value 传 tagged-JSON。

批量操作:
  使用 --ops '<JSON array>' 执行多项操作，并在全部成功后保存一次。
  JSON 通常包含空格、引号、方括号和花括号，必须把整个 JSON 数组用引号包裹:
    edit --file D:/x.mcstructure --ops '[{"op":"set","path":"format_version","value":"2"}]'
)";
}

inline mcp::json help_result() {
    return text_result(nbt_help_text() + "\n\n" + nbt_reference_text());
}

inline mcp::json error_with_help(const std::string& message) {
    return text_result(message + "\n\n" + nbt_help_text());
}

inline void put_if_present(mcp::json& params,
                           const ParsedCommand& pc,
                           const std::initializer_list<const char*>& flags,
                           const char* target_key) {
    std::string value = flag_any(pc, flags);
    if (!value.empty()) params[target_key] = value;
}

inline mcp::json base_params(const ParsedCommand& pc, const std::string& action) {
    mcp::json params = {{"action", action}};
    std::string file = flag_any(pc, {"file", "path-file", "input", "file_path"});
    if (file.empty() && !pc.positional.empty()) file = pc.positional;
    if (!file.empty()) params["file_path"] = file;
    return params;
}

inline mcp::json dispatch_view(const ParsedCommand& pc) {
    mcp::json params = base_params(pc, "view");
    put_if_present(params, pc, {"path", "nbt-path"}, "path");
    put_if_present(params, pc, {"format", "fmt"}, "format");
    if (has_flag(pc, "max-depth")) params["max_depth"] = flag_int(pc, "max-depth", 12);
    if (has_flag(pc, "max_depth")) params["max_depth"] = flag_int(pc, "max_depth", 12);
    if (has_flag(pc, "max-items")) params["max_items"] = flag_int(pc, "max-items", 64);
    if (has_flag(pc, "max_items")) params["max_items"] = flag_int(pc, "max_items", 64);
    return nbt_do_view(params);
}

inline mcp::json dispatch_create(const ParsedCommand& pc) {
    mcp::json params = base_params(pc, "create");
    put_if_present(params, pc, {"template"}, "template");
    put_if_present(params, pc, {"size"}, "size");
    put_if_present(params, pc, {"blocks"}, "blocks");
    put_if_present(params, pc, {"json-value", "json_value"}, "json_value");
    if (has_flag(pc, "fill")) params["fill"] = flag_int(pc, "fill", 0);
    params["block_version"] = flag_int_any(pc, {"block-version", "block_version"}, 18161159);
    if (has_flag(pc, "little-endian") || has_flag(pc, "little_endian"))
        params["little_endian"] = flag_bool(pc, has_flag(pc, "little-endian") ? "little-endian" : "little_endian", true);
    if (has_flag(pc, "overwrite")) params["overwrite"] = flag_bool(pc, "overwrite", true);
    return nbt_do_create(params);
}

inline mcp::json dispatch_edit(const ParsedCommand& pc) {
    mcp::json params = base_params(pc, "edit");
    put_if_present(params, pc, {"save", "save-path", "save_path", "out"}, "save_path");
    put_if_present(params, pc, {"ops"}, "ops");
    put_if_present(params, pc, {"op"}, "op");
    put_if_present(params, pc, {"path", "nbt-path"}, "path");
    put_if_present(params, pc, {"name"}, "name");
    put_if_present(params, pc, {"new-name", "new_name"}, "new_name");
    put_if_present(params, pc, {"tag-type", "tag_type"}, "tag_type");
    put_if_present(params, pc, {"value"}, "value");
    put_if_present(params, pc, {"json-value", "json_value"}, "json_value");
    return nbt_do_edit(params);
}

inline mcp::json dispatch_minecraft_nbt(const std::string& command) {
    ParsedCommand pc = parse_command(command);
    std::string sub = command_parser_detail::to_lower_ascii(pc.sub);

    try {
        if (sub.empty() || sub == "help" || sub == "?") return help_result();
        if (sub == "reference" || sub == "ref") return help_result();
        if (sub == "view") return dispatch_view(pc);
        if (sub == "create") return dispatch_create(pc);
        if (sub == "edit") return dispatch_edit(pc);
        return error_with_help("Unknown subcommand: '" + pc.sub + "'.");
    } catch (const std::exception& e) {
        return error_with_help(e.what());
    }
}

} // namespace minecraft_nbt_detail

inline void register_minecraft_nbt_tools(mcp::server& srv) {
    auto tool = mcp::tool_builder(minecraft_nbt_detail::kToolName)
        .with_description(
            "Minecraft NBT 文件工具（.mcstructure / level.dat / 未压缩 .nbt）："
            "通过命令式子命令 view/edit/create/reference 查看、编辑、创建和查阅速查。"
            "首次使用请传 command=\"help\" 查看语法。")
        .with_string_param("command",
            "命令语句，如 'view --file D:/x.mcstructure'、"
            "'create --file D:/x.mcstructure --template mcstructure'、"
            "'edit --file <path> --save <copy> --ops [...]' 或 '/help'。"
            "路径含空格、JSON 数组/对象参数需要用引号包裹；写入目标的父目录必须已存在。", true)
        .with_read_only_hint(false).with_idempotent_hint(false).build();

    srv.register_tool(tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        return minecraft_nbt_detail::dispatch_minecraft_nbt(params.value("command", ""));
    });
}

} // namespace mcdk
