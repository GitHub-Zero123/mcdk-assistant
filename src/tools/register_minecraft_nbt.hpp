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
    return R"(minecraft_nbt - Minecraft NBT file tool (command-style)
Usage: minecraft_nbt(command="<subcommand> [args...] [--option value]")
Command names may start with '/', for example /help or /view.
Important: arguments containing spaces or JSON arrays/objects must be wrapped in quotes.

Subcommands:
  help | reference
      Show NBT format, tagged-JSON, path syntax, and edit operation reference.

  view --file <path> [--path <nbt/path>] [--format tree|json] [--max-depth <n>] [--max-items <n>]
      Read an NBT file (.mcstructure, level.dat, uncompressed .nbt) and print a tree or tagged JSON.

  create --file <path> [--template mcstructure|empty] [--size "[x,y,z]"] [--blocks "[\"minecraft:stone\"]"]
         [--fill <n>] [--block-version <n>] [--little-endian true|false] [--overwrite]
      Create a new NBT file.

  edit --file <path> [--save <path>] (--ops '<JSON array>' | --op set|remove|rename|add ...)
      Edit an NBT file and write it back.
      Single-op common options: --path, --name, --tag-type, --value, --new-name, --json-value.

Examples:
  view --file D:/world/structures/a.mcstructure --max-depth 6
  view --file D:/world/structures/a.mcstructure --path structure/palette/default/block_palette/0/name
  create --file D:/tmp/test.mcstructure --template mcstructure --size "[2,2,2]" --overwrite
  edit --file D:/tmp/test.mcstructure --ops '[{"op":"set","path":"format_version","value":"2"}]')";
}

inline mcp::json help_result() {
    return text_result(nbt_help_text() + "\n\n" + NBT_REFERENCE_TEXT);
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
            "Minecraft NBT file tool (.mcstructure / level.dat / uncompressed .nbt): "
            "view, edit, create, and reference via command-style subcommands. "
            "Call command=\"help\" first for syntax.")
        .with_string_param("command",
            "Command such as 'view --file D:/x.mcstructure', "
            "'create --file D:/x.mcstructure --template mcstructure', "
            "or 'edit --file <path> --ops [...]'.", true)
        .with_read_only_hint(false).with_idempotent_hint(false).build();

    srv.register_tool(tool, [](const mcp::json& params, const std::string&) -> mcp::json {
        return minecraft_nbt_detail::dispatch_minecraft_nbt(params.value("command", ""));
    });
}

} // namespace mcdk
