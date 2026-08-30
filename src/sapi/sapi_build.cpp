// SAPI 索引的构建侧：解析 .d.ts 源码并写出缓存。
// 只有 mcdk-index-compiler 需要这部分 —— 它是 tree-sitter-typescript 的唯一使用者。
// 刻意与 sapi_index.cpp 分开：链接器在符号解析阶段就会把整个 parser.c.obj
// （约 1.3MB 的语法分析表）从静态库里拉进来，之后 /OPT:REF 只能剔除函数、
// 无法再丢弃已进入的全局数据表。只要运行时目标文件里不存在这个引用，
// parser.c.obj 就根本不会被拉入。

#include "sapi/sapi_index.hpp"
#include "sapi/sapi_internal.hpp"

#include "common/path_utils.hpp"

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-typescript.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <system_error>
#include <unordered_map>
#include <unordered_set>

namespace mcdk::sapi {
namespace fs = std::filesystem;

namespace {

using namespace detail;

std::string slice(const std::string& source, TSNode node) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    if (start > source.size() || end > source.size() || end < start) return {};
    return source.substr(start, end - start);
}


std::string node_type(TSNode node) {
    const char* type = ts_node_type(node);
    return type ? std::string(type) : std::string();
}

std::optional<TSNode> field_child(TSNode node, const char* field) {
    TSNode child = ts_node_child_by_field_name(node, field, static_cast<uint32_t>(std::strlen(field)));
    if (ts_node_is_null(child)) return std::nullopt;
    return child;
}

std::string name_from_field(const std::string& source, TSNode node) {
    auto name = field_child(node, "name");
    if (name) return trim(slice(source, *name));
    return {};
}

int line_start(TSNode node) {
    return static_cast<int>(ts_node_start_point(node).row) + 1;
}

int line_end(TSNode node) {
    return static_cast<int>(ts_node_end_point(node).row) + 1;
}


std::string preceding_doc_comment(const std::string& source, TSNode node) {
    size_t start = ts_node_start_byte(node);
    if (start == 0 || start > source.size()) return {};

    size_t pos = start;
    while (true) {
        while (pos > 0 && std::isspace(static_cast<unsigned char>(source[pos - 1]))) --pos;
        size_t line_begin = 0;
        if (pos > 0 && preceding_line_is_directive(source, pos, line_begin)) {
            pos = line_begin;
            continue;
        }
        break;
    }
    if (pos >= 2 && source[pos - 1] == '/' && source[pos - 2] == '*') {
        size_t begin = source.rfind("/**", pos - 2);
        if (begin != std::string::npos) return clean_doc_comment(source.substr(begin, pos - begin));
    }

    size_t line_end_pos = pos;
    std::vector<std::string> lines;
    while (line_end_pos > 0) {
        size_t line_begin = source.rfind('\n', line_end_pos - 1);
        if (line_begin == std::string::npos) line_begin = 0;
        else ++line_begin;
        std::string line = source.substr(line_begin, line_end_pos - line_begin);
        std::string stripped = trim(line);
        if (stripped.rfind("//", 0) != 0) break;
        lines.push_back(stripped);
        if (line_begin == 0) break;
        line_end_pos = line_begin - 1;
        while (line_end_pos > 0 && (source[line_end_pos - 1] == '\r' || source[line_end_pos - 1] == '\n'))
            --line_end_pos;
    }
    if (lines.empty()) return {};
    std::reverse(lines.begin(), lines.end());
    std::ostringstream raw;
    for (const auto& line : lines) raw << line << '\n';
    return clean_doc_comment(raw.str());
}

// 提取文件头 @packageDocumentation JSDoc（含 Manifest Details 的 ```json 版本块/示例）。
// 该块描述整个包，不属于任何符号，常规符号遍历会跳过它。

bool count_tree_errors(TSNode root, int& errors, int& missing) {
    std::vector<TSNode> stack;
    stack.push_back(root);
    size_t visited = 0;
    while (!stack.empty()) {
        if (++visited > kMaxTreeWalkNodes) return false;
        TSNode node = stack.back();
        stack.pop_back();
        if (ts_node_is_error(node)) ++errors;
        if (ts_node_is_missing(node)) ++missing;
        uint32_t n = ts_node_child_count(node);
        for (uint32_t i = 0; i < n; ++i)
            stack.push_back(ts_node_child(node, i));
    }
    return true;
}


void collect_enum_member(const std::string& source, TSNode child, SapiSymbol& symbol) {
    std::string text = slice(source, child);
    SapiMember member;
    member.kind = "enum-value";
    member.name = name_from_field(source, child);
    auto value = field_child(child, "value");
    if (value) member.value = strip_quotes(slice(source, *value));
    if (member.name.empty()) member.name = trim(text);
    if (member.name.empty()) return;
    member.fqname = symbol.fqname + "." + member.name;
    member.signature = std::regex_replace(trim(text), std::regex(R"(\s+)"), " ");
    member.doc = preceding_doc_comment(source, child);
    member.line_start = line_start(child);
    member.line_end = line_end(child);
    symbol.members.push_back(std::move(member));
}

void collect_members(const std::string& source, TSNode body, SapiSymbol& symbol) {
    bool is_enum_body = node_type(body) == "enum_body";
    uint32_t n = ts_node_child_count(body);
    for (uint32_t i = 0; i < n; ++i) {
        TSNode child = ts_node_child(body, i);
        std::string type = node_type(child);
        if (is_enum_body) {
            // 枚举成员有两种形态：带值的 `Angle = 'Angle'`（enum_assignment）
            // 与裸成员 `Angle`（property_identifier，TS 自动编号）。
            if (type == "enum_assignment" || type == "property_identifier")
                collect_enum_member(source, child, symbol);
            continue;
        }
        if (!is_member_node(type)) continue;

        std::string text = slice(source, child);
        SapiMember member;
        member.kind = member_kind_for_node_type(type);
        member.name = name_from_field(source, child);
        if (member.name.empty()) member.name = member_name_from_text(text);
        member.fqname = symbol.fqname + "." + member.name;
        member.signature = signature_from_text(text);
        member.doc = preceding_doc_comment(source, child);
        member.line_start = line_start(child);
        member.line_end = line_end(child);
        collect_type_refs_from_text(member.signature, symbol.type_refs);
        if (!member.name.empty()) symbol.members.push_back(std::move(member));
    }
}

bool collect_symbols_iterative(const std::string& source,
                               TSNode node,
                               const std::string& module_name,
                               const std::string& rel_file,
                               SapiIndex& index,
                               SapiModule& module) {
    std::vector<TSNode> stack;
    stack.push_back(node);
    size_t visited = 0;

    while (!stack.empty()) {
        if (++visited > kMaxTreeWalkNodes) return false;
        TSNode current = stack.back();
        stack.pop_back();

        std::string type = node_type(current);
        TSNode decl_node = current;
        std::string decl_type = type;

        if (type == "export_statement") {
            auto decl = field_child(current, "declaration");
            if (decl) {
                decl_node = *decl;
                decl_type = node_type(decl_node);
            }
        }

        bool handled = false;
        if (is_symbol_node(decl_type)) {
            std::string kind = kind_for_node_type(decl_type);
            std::string text = slice(source, decl_node);
            std::string name = name_from_field(source, decl_node);
            if (name.empty()) name = declaration_name_from_text(text, kind);

            if (!name.empty()) {
                SapiSymbol symbol;
                symbol.module = module_name;
                symbol.kind = kind;
                symbol.name = name;
                symbol.fqname = module_name + "." + name;
                symbol.source_file = rel_file;
                symbol.line_start = line_start(decl_node);
                symbol.line_end = line_end(decl_node);
                symbol.signature = signature_from_text(text);
                symbol.doc = preceding_doc_comment(source, current);
                if (symbol.doc.empty()) symbol.doc = preceding_doc_comment(source, decl_node);
                collect_type_refs_from_text(symbol.signature, symbol.type_refs);

                auto body = field_child(decl_node, "body");
                if (body) collect_members(source, *body, symbol);

                int id = static_cast<int>(index.symbols.size());
                index.symbols.push_back(std::move(symbol));
                module.symbol_ids.push_back(id);
                handled = true;
            }
        }

        if (handled) continue;

        uint32_t n = ts_node_child_count(current);
        for (uint32_t i = 0; i < n; ++i)
            stack.push_back(ts_node_child(current, n - 1 - i));
    }
    return true;
}

void parse_sapi_file(const fs::path& root, const fs::path& file, SapiIndex& index) {
    std::string source = read_file(file);
    if (source.empty()) return;

    fs::path rel_path = fs::relative(file, root);
    std::string rel_file = mcdk::path::to_utf8(rel_path);
    for (char& c : rel_file) if (c == '\\') c = '/';

    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_typescript());
    TSTree* tree = ts_parser_parse_string(parser, nullptr, source.data(),
                                          static_cast<uint32_t>(source.size()));
    TSNode root_node = ts_tree_root_node(tree);

    int errors = 0;
    int missing = 0;
    bool error_walk_complete = count_tree_errors(root_node, errors, missing);
    if (!error_walk_complete) {
        index.diagnostics.push_back(rel_file + ": parse error scan truncated after " +
                                    std::to_string(kMaxTreeWalkNodes) + " nodes");
    }
    if (errors > 0 || missing > 0) {
        ++index.parse_error_files;
        index.error_nodes += errors;
        index.missing_nodes += missing;
        index.diagnostics.push_back(rel_file + ": parse errors=" + std::to_string(errors) +
                                    ", missing=" + std::to_string(missing));
    }

    SapiModule module;
    module.name = module_from_manifest_or_file(source, rel_path);
    module.source_file = rel_file;
    collect_imports(source, module);
    bool symbol_walk_complete = collect_symbols_iterative(source, root_node, module.name, rel_file, index, module);
    if (!symbol_walk_complete) {
        index.diagnostics.push_back(rel_file + ": symbol scan truncated after " +
                                    std::to_string(kMaxTreeWalkNodes) + " nodes");
    }

    // 把模块级 @packageDocumentation 作为一个 kind=module 的合成符号入库，
    // 让它可被 search 命中、被 symbol <模块名> 查到，也供 module 命令展示版本块。
    std::string pkg_doc = extract_package_doc(source);
    if (!pkg_doc.empty()) {
        SapiSymbol module_sym;
        module_sym.module = module.name;
        module_sym.kind = "module";
        module_sym.name = module.name;
        module_sym.fqname = module.name;
        module_sym.signature = "module " + module.name;
        module_sym.doc = std::move(pkg_doc);
        module_sym.source_file = rel_file;
        module_sym.line_start = 1;
        module_sym.line_end = 1;
        index.symbols.push_back(std::move(module_sym));
    }

    index.modules.push_back(std::move(module));

    ts_tree_delete(tree);
    ts_parser_delete(parser);
}


} // namespace

SapiIndex build_sapi_index(const std::filesystem::path& sapi_root) {
    SapiIndex index;
    if (!fs::exists(sapi_root)) {
        index.diagnostics.push_back("SAPI root not found: " + mcdk::path::to_utf8(sapi_root));
        return index;
    }

    std::vector<fs::path> files;
    std::vector<std::pair<fs::path, int>> dirs;
    dirs.push_back({sapi_root, 0});

    while (!dirs.empty()) {
        auto [dir, depth] = dirs.back();
        dirs.pop_back();
        if (depth > kMaxDirectoryDepth) {
            index.diagnostics.push_back("directory scan depth limit reached: " + mcdk::path::to_utf8(dir));
            continue;
        }

        std::error_code ec;
        fs::directory_iterator it(dir, fs::directory_options::skip_permission_denied, ec), end;
        if (ec) {
            index.diagnostics.push_back("directory scan failed: " + mcdk::path::to_utf8(dir));
            continue;
        }

        for (; it != end; it.increment(ec)) {
            if (ec) {
                index.diagnostics.push_back("directory iterator error under: " + mcdk::path::to_utf8(dir));
                ec.clear();
                continue;
            }
            if (it->is_directory(ec)) {
                dirs.push_back({it->path(), depth + 1});
                continue;
            }
            if (!it->is_regular_file(ec)) continue;
            std::string path = mcdk::path::to_utf8(it->path());
            if (path.size() >= 5 && path.substr(path.size() - 5) == ".d.ts") {
                files.push_back(it->path());
                if (files.size() >= kMaxSapiFiles) {
                    index.diagnostics.push_back("SAPI file scan limit reached: " + std::to_string(kMaxSapiFiles));
                    dirs.clear();
                    break;
                }
            }
        }
    }
    std::sort(files.begin(), files.end());
    index.files_scanned = static_cast<int>(files.size());

    for (const auto& file : files)
        parse_sapi_file(sapi_root, file, index);

    return index;
}

bool save_sapi_cache(const std::filesystem::path& cache_path,
                     const SapiIndex& index,
                     std::string* error) {
    std::string path = mcdk::path::to_utf8(cache_path);
    FILE* fp = std::fopen(path.c_str(), "wb");
    if (!fp) {
        if (error) *error = "cannot open for writing: " + path;
        return false;
    }

    std::fwrite(kMagic, 1, 8, fp);
    write_u32(fp, kVersion);
    write_i32(fp, index.files_scanned);
    write_i32(fp, index.parse_error_files);
    write_i32(fp, index.error_nodes);
    write_i32(fp, index.missing_nodes);

    write_u32(fp, static_cast<uint32_t>(index.modules.size()));
    for (const auto& module : index.modules) {
        write_string(fp, module.name);
        write_string(fp, module.source_file);
        write_u32(fp, static_cast<uint32_t>(module.imports.size()));
        for (const auto& imp : module.imports) {
            write_string(fp, imp.module);
            write_string_vec(fp, imp.symbols);
            write_i32(fp, imp.line);
        }
        write_u32(fp, static_cast<uint32_t>(module.symbol_ids.size()));
        for (int id : module.symbol_ids) write_i32(fp, id);
    }

    write_u32(fp, static_cast<uint32_t>(index.symbols.size()));
    for (const auto& sym : index.symbols) {
        write_string(fp, sym.module);
        write_string(fp, sym.kind);
        write_string(fp, sym.name);
        write_string(fp, sym.fqname);
        write_string(fp, sym.signature);
        write_string(fp, sym.doc);
        write_string(fp, sym.source_file);
        write_i32(fp, sym.line_start);
        write_i32(fp, sym.line_end);
        write_string_vec(fp, sym.type_refs);
        write_u32(fp, static_cast<uint32_t>(sym.members.size()));
        for (const auto& member : sym.members) {
            write_string(fp, member.kind);
            write_string(fp, member.name);
            write_string(fp, member.fqname);
            write_string(fp, member.signature);
            write_string(fp, member.doc);
            write_string(fp, member.value);
            write_i32(fp, member.line_start);
            write_i32(fp, member.line_end);
        }
    }

    write_string_vec(fp, index.diagnostics);

    bool ok = std::fflush(fp) == 0 && std::ferror(fp) == 0;
    std::fclose(fp);
    if (!ok && error) *error = "write error: " + path;
    return ok;
}

} // namespace mcdk::sapi
