#include "sapi/sapi_index.hpp"

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

constexpr char kMagic[8] = {'M','C','D','K','S','A','P','I'};
constexpr uint32_t kVersion = 2;  // v2: SapiMember 增加 enum 字面值 value
constexpr size_t kMaxTreeWalkNodes = 2'000'000;
constexpr int kMaxDirectoryDepth = 16;
constexpr size_t kMaxSapiFiles = 50'000;

std::string read_file(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

std::string slice(const std::string& source, TSNode node) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    if (start > source.size() || end > source.size() || end < start) return {};
    return source.substr(start, end - start);
}

std::string trim(std::string value) {
    size_t b = 0;
    while (b < value.size() && std::isspace(static_cast<unsigned char>(value[b]))) ++b;
    size_t e = value.size();
    while (e > b && std::isspace(static_cast<unsigned char>(value[e - 1]))) --e;
    return value.substr(b, e - b);
}

std::string to_lower_ascii(std::string value) {
    for (char& c : value)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}

std::vector<std::string> split_identifier_tokens(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current;
    char prev = 0;

    auto flush = [&]() {
        if (!current.empty()) {
            tokens.push_back(to_lower_ascii(std::move(current)));
            current.clear();
        }
    };

    for (char c : text) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            bool boundary = false;
            if (!current.empty()) {
                bool prev_lower = std::islower(static_cast<unsigned char>(prev)) != 0;
                bool prev_digit = std::isdigit(static_cast<unsigned char>(prev)) != 0;
                bool cur_upper = std::isupper(uc) != 0;
                bool cur_digit = std::isdigit(uc) != 0;
                boundary = (prev_lower && cur_upper) || (prev_digit != cur_digit);
            }
            if (boundary) flush();
            current.push_back(c);
            prev = c;
        } else {
            flush();
            prev = 0;
        }
    }
    flush();
    return tokens;
}

std::vector<std::string> query_tokens(const std::string& query) {
    std::vector<std::string> raw = split_identifier_tokens(query);
    std::vector<std::string> out;
    std::unordered_set<std::string> seen;
    for (auto& token : raw) {
        if (token.empty()) continue;
        if (seen.insert(token).second) out.push_back(std::move(token));
    }
    return out;
}

int edit_distance_limited(const std::string& a, const std::string& b, int limit) {
    if (limit < 0) return limit + 1;
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());
    if (std::abs(n - m) > limit) return limit + 1;
    if (n == 0) return m;
    if (m == 0) return n;

    std::vector<int> prev(m + 1), cur(m + 1);
    for (int j = 0; j <= m; ++j) prev[j] = j;
    for (int i = 1; i <= n; ++i) {
        cur[0] = i;
        int row_min = cur[0];
        for (int j = 1; j <= m; ++j) {
            int cost = a[i - 1] == b[j - 1] ? 0 : 1;
            cur[j] = std::min({prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + cost});
            row_min = std::min(row_min, cur[j]);
        }
        if (row_min > limit) return limit + 1;
        prev.swap(cur);
    }
    return prev[m];
}

bool token_near_match(const std::string& query, const std::string& field) {
    if (field.find(query) == 0 || query.find(field) == 0) return true;
    if (query.size() < 4 || field.size() < 4) return false;
    if (query.size() > 32 || field.size() > 32) return false;
    if (query.front() != field.front()) return false;
    int limit = (query.size() >= 5 || field.size() >= 5) ? 2 : 1;
    return edit_distance_limited(query, field, limit) <= limit;
}

bool contains_ascii_ci(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    return to_lower_ascii(haystack).find(to_lower_ascii(needle)) != std::string::npos;
}

double token_coverage_score(const std::vector<std::string>& query,
                            const std::vector<std::string>& field,
                            double all_score,
                            double partial_score,
                            bool fuzzy = true) {
    if (query.empty() || field.empty()) return 0.0;
    std::unordered_set<std::string> field_set(field.begin(), field.end());
    int hits = 0;
    for (const auto& q : query) {
        if (field_set.count(q)) {
            ++hits;
            continue;
        }
        bool prefix = false;
        for (const auto& f : field) {
            if (fuzzy ? token_near_match(q, f) : (f.find(q) == 0 || q.find(f) == 0)) {
                prefix = true;
                break;
            }
        }
        if (prefix) ++hits;
    }
    if (hits == 0) return 0.0;
    if (hits == static_cast<int>(query.size())) return all_score;
    return partial_score * (static_cast<double>(hits) / static_cast<double>(query.size()));
}

std::string strip_quotes(std::string value) {
    value = trim(std::move(value));
    if (value.size() >= 2 &&
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

bool is_ident_start(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

bool is_ident_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

std::string first_identifier_after_keyword(const std::string& text, const std::string& keyword) {
    size_t pos = text.find(keyword);
    if (pos == std::string::npos) return {};
    pos += keyword.size();
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
    if (pos >= text.size() || !is_ident_start(text[pos])) return {};
    size_t start = pos++;
    while (pos < text.size() && is_ident_char(text[pos])) ++pos;
    return text.substr(start, pos - start);
}

std::string declaration_name_from_text(const std::string& text, const std::string& kind) {
    if (kind == "class") return first_identifier_after_keyword(text, "class");
    if (kind == "interface") return first_identifier_after_keyword(text, "interface");
    if (kind == "enum") return first_identifier_after_keyword(text, "enum");
    if (kind == "type") return first_identifier_after_keyword(text, "type");
    if (kind == "function") return first_identifier_after_keyword(text, "function");
    if (kind == "const") {
        std::string name = first_identifier_after_keyword(text, "const");
        if (!name.empty()) return name;
        return first_identifier_after_keyword(text, "let");
    }
    return {};
}

std::string kind_for_node_type(const std::string& type) {
    if (type == "class_declaration" || type == "abstract_class_declaration") return "class";
    if (type == "interface_declaration") return "interface";
    if (type == "enum_declaration") return "enum";
    if (type == "type_alias_declaration") return "type";
    if (type == "function_declaration" || type == "function_signature") return "function";
    if (type == "lexical_declaration" || type == "variable_declaration") return "const";
    return {};
}

bool is_symbol_node(const std::string& type) {
    return !kind_for_node_type(type).empty();
}

bool is_member_node(const std::string& type) {
    return type == "method_signature" ||
           type == "abstract_method_signature" ||
           type == "property_signature" ||
           type == "public_field_definition" ||
           type == "index_signature";
}

std::string member_kind_for_node_type(const std::string& type) {
    if (type == "method_signature" || type == "abstract_method_signature") return "method";
    if (type == "index_signature") return "index";
    return "property";
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

std::string signature_from_text(std::string text) {
    text = trim(std::move(text));
    size_t brace = text.find('{');
    size_t semi = text.find(';');
    size_t cut = std::min(brace == std::string::npos ? text.size() : brace,
                          semi == std::string::npos ? text.size() : semi);
    if (cut < text.size()) text = text.substr(0, cut + (semi == cut ? 1 : 0));
    text = std::regex_replace(text, std::regex(R"(\s+)"), " ");
    return trim(std::move(text));
}

std::string module_from_file(const fs::path& rel_path) {
    std::string rel = mcdk::path::to_utf8(rel_path);
    for (char& c : rel) if (c == '\\') c = '/';
    if (rel.size() >= 5 && rel.substr(rel.size() - 5) == ".d.ts")
        rel.resize(rel.size() - 5);
    if (rel == "vanilla-data/index") return "@minecraft/vanilla-data";
    if (rel.find("vanilla-data/") == 0)
        return "@minecraft/" + rel;
    return "@minecraft/" + rel;
}

std::string module_from_manifest_or_file(const std::string& source, const fs::path& rel_path) {
    std::regex re(R"REGEX("module_name"\s*:\s*"([^"]+)")REGEX");
    std::smatch m;
    if (std::regex_search(source, m, re)) return m[1].str();
    return module_from_file(rel_path);
}

std::string clean_doc_comment(std::string raw) {
    raw = trim(std::move(raw));
    if (raw.empty()) return {};
    if (raw.rfind("/**", 0) == 0) {
        raw = raw.substr(3);
        if (raw.size() >= 2 && raw.substr(raw.size() - 2) == "*/")
            raw.resize(raw.size() - 2);
    } else if (raw.rfind("//", 0) == 0) {
        std::istringstream iss(raw);
        std::string line;
        std::ostringstream out;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.rfind("//", 0) == 0) line = trim(line.substr(2));
            out << line << '\n';
        }
        return trim(out.str());
    }

    std::istringstream iss(raw);
    std::string line;
    std::ostringstream out;
    while (std::getline(iss, line)) {
        line = trim(line);
        if (!line.empty() && line[0] == '*') line = trim(line.substr(1));
        out << line << '\n';
    }
    return trim(out.str());
}

// 紧邻声明的一行 `// ...` 是否为编译器/工具指令（@ts-ignore、eslint、prettier 等），
// 这类行夹在 JSDoc 与声明之间，不是文档，应当跳过以免遮挡上方真正的 /** */ 块。
bool preceding_line_is_directive(const std::string& source, size_t end_pos, size_t& line_begin_out) {
    size_t nl = end_pos == 0 ? std::string::npos : source.rfind('\n', end_pos - 1);
    size_t begin = (nl == std::string::npos) ? 0 : nl + 1;
    line_begin_out = begin;
    std::string line = trim(source.substr(begin, end_pos - begin));
    if (line.rfind("//", 0) != 0) return false;
    std::string body = trim(line.substr(2));
    return !body.empty() &&
           (body.front() == '@' ||
            body.find("eslint") != std::string::npos ||
            body.find("prettier") != std::string::npos);
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
std::string extract_package_doc(const std::string& source) {
    size_t marker = source.find("@packageDocumentation");
    if (marker == std::string::npos) return {};
    size_t begin = source.rfind("/**", marker);
    size_t end = source.find("*/", marker);
    if (begin == std::string::npos || end == std::string::npos) return {};
    end += 2;
    return clean_doc_comment(source.substr(begin, end - begin));
}

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

void collect_type_refs_from_text(const std::string& text, std::vector<std::string>& refs) {
    static const std::unordered_set<std::string> kSkip = {
        "export", "declare", "class", "interface", "enum", "type", "extends", "implements",
        "readonly", "private", "public", "protected", "static", "abstract", "constructor",
        "string", "number", "boolean", "void", "undefined", "null", "true", "false", "any",
        "unknown", "never", "object", "const", "let", "var", "new", "Error", "Array", "Promise",
        "Record", "Map", "Set", "ReadonlyArray"
    };
    std::unordered_set<std::string> seen(refs.begin(), refs.end());
    std::regex ident_re(R"(\b[A-Z][A-Za-z0-9_]*\b)");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), ident_re);
         it != std::sregex_iterator(); ++it) {
        std::string tok = (*it).str();
        if (kSkip.count(tok)) continue;
        if (seen.insert(tok).second) refs.push_back(std::move(tok));
    }
}

void collect_imports(const std::string& source, SapiModule& module) {
    std::istringstream iss(source);
    std::string line;
    int line_no = 0;
    std::regex import_line_re(R"REGEX(^\s*import\b.*['"][^'"]+['"])REGEX");
    while (std::getline(iss, line)) {
        ++line_no;
        if (!std::regex_search(line, import_line_re)) continue;

        SapiImport imp;
        imp.line = line_no;
        std::string text = line;
        std::regex source_re(R"(from\s+['"]([^'"]+)['"]|import\s+['"]([^'"]+)['"])");
        std::smatch m;
        if (std::regex_search(text, m, source_re))
            imp.module = m[1].matched ? m[1].str() : m[2].str();

        std::regex named_re(R"(\{([^}]+)\})");
        if (std::regex_search(text, m, named_re)) {
            std::string names = m[1].str();
            std::regex ident_re(R"([A-Za-z_$][A-Za-z0-9_$]*)");
            for (auto it = std::sregex_iterator(names.begin(), names.end(), ident_re);
                 it != std::sregex_iterator(); ++it) {
                std::string name = (*it).str();
                if (name != "as") imp.symbols.push_back(std::move(name));
            }
        }
        if (!imp.module.empty()) module.imports.push_back(std::move(imp));
    }
}

std::string member_name_from_text(const std::string& text) {
    std::string s = trim(text);
    s = std::regex_replace(s, std::regex(R"(^(public|private|protected|readonly|static|abstract)\s+)"), "");
    if (s.rfind("get ", 0) == 0) s = trim(s.substr(4));
    if (s.rfind("set ", 0) == 0) s = trim(s.substr(4));
    if (s.rfind("[", 0) == 0) return "[]";
    size_t pos = 0;
    while (pos < s.size() && (std::isspace(static_cast<unsigned char>(s[pos])) || s[pos] == '*')) ++pos;
    if (pos >= s.size() || !is_ident_start(s[pos])) return {};
    size_t start = pos++;
    while (pos < s.size() && is_ident_char(s[pos])) ++pos;
    return s.substr(start, pos - start);
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

void write_u32(FILE* fp, uint32_t v) { std::fwrite(&v, 4, 1, fp); }
uint32_t read_u32(FILE* fp) {
    uint32_t v = 0;
    if (std::fread(&v, 4, 1, fp) != 1) return 0;
    return v;
}

void write_i32(FILE* fp, int v) { write_u32(fp, static_cast<uint32_t>(v)); }
int read_i32(FILE* fp) { return static_cast<int>(read_u32(fp)); }

void write_string(FILE* fp, const std::string& value) {
    write_u32(fp, static_cast<uint32_t>(value.size()));
    if (!value.empty()) std::fwrite(value.data(), 1, value.size(), fp);
}

bool read_string(FILE* fp, std::string& value) {
    uint32_t len = read_u32(fp);
    value.resize(len);
    if (len == 0) return true;
    return std::fread(value.data(), 1, len, fp) == len;
}

void write_string_vec(FILE* fp, const std::vector<std::string>& values) {
    write_u32(fp, static_cast<uint32_t>(values.size()));
    for (const auto& value : values) write_string(fp, value);
}

bool read_string_vec(FILE* fp, std::vector<std::string>& values) {
    uint32_t n = read_u32(fp);
    values.resize(n);
    for (auto& value : values)
        if (!read_string(fp, value)) return false;
    return true;
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

bool load_sapi_cache(const std::filesystem::path& cache_path,
                     SapiIndex& index,
                     std::string* error) {
    std::string path = mcdk::path::to_utf8(cache_path);
    FILE* fp = std::fopen(path.c_str(), "rb");
    if (!fp) {
        if (error) *error = "cannot open for reading: " + path;
        return false;
    }

    char magic[8];
    if (std::fread(magic, 1, 8, fp) != 8 || std::memcmp(magic, kMagic, 8) != 0) {
        std::fclose(fp);
        if (error) *error = "bad SAPI cache magic";
        return false;
    }
    uint32_t version = read_u32(fp);
    if (version != kVersion) {
        std::fclose(fp);
        if (error) *error = "unsupported SAPI cache version: " + std::to_string(version);
        return false;
    }

    SapiIndex out;
    out.files_scanned = read_i32(fp);
    out.parse_error_files = read_i32(fp);
    out.error_nodes = read_i32(fp);
    out.missing_nodes = read_i32(fp);

    uint32_t module_count = read_u32(fp);
    out.modules.resize(module_count);
    for (auto& module : out.modules) {
        if (!read_string(fp, module.name) || !read_string(fp, module.source_file)) {
            std::fclose(fp); return false;
        }
        uint32_t import_count = read_u32(fp);
        module.imports.resize(import_count);
        for (auto& imp : module.imports) {
            if (!read_string(fp, imp.module) || !read_string_vec(fp, imp.symbols)) {
                std::fclose(fp); return false;
            }
            imp.line = read_i32(fp);
        }
        uint32_t ids = read_u32(fp);
        module.symbol_ids.resize(ids);
        for (auto& id : module.symbol_ids) id = read_i32(fp);
    }

    uint32_t symbol_count = read_u32(fp);
    out.symbols.resize(symbol_count);
    for (auto& sym : out.symbols) {
        if (!read_string(fp, sym.module) ||
            !read_string(fp, sym.kind) ||
            !read_string(fp, sym.name) ||
            !read_string(fp, sym.fqname) ||
            !read_string(fp, sym.signature) ||
            !read_string(fp, sym.doc) ||
            !read_string(fp, sym.source_file)) {
            std::fclose(fp); return false;
        }
        sym.line_start = read_i32(fp);
        sym.line_end = read_i32(fp);
        if (!read_string_vec(fp, sym.type_refs)) {
            std::fclose(fp); return false;
        }
        uint32_t members = read_u32(fp);
        sym.members.resize(members);
        for (auto& member : sym.members) {
            if (!read_string(fp, member.kind) ||
                !read_string(fp, member.name) ||
                !read_string(fp, member.fqname) ||
                !read_string(fp, member.signature) ||
                !read_string(fp, member.doc) ||
                !read_string(fp, member.value)) {
                std::fclose(fp); return false;
            }
            member.line_start = read_i32(fp);
            member.line_end = read_i32(fp);
        }
    }

    if (!read_string_vec(fp, out.diagnostics)) {
        std::fclose(fp); return false;
    }

    bool ok = std::ferror(fp) == 0;
    std::fclose(fp);
    if (!ok) {
        if (error) *error = "read error: " + path;
        return false;
    }
    index = std::move(out);
    return true;
}

std::filesystem::path default_sapi_cache_path(const std::filesystem::path& exe_dir) {
    return exe_dir / "mcdk_sapi_index_cache.bin";
}

namespace {

std::unordered_map<std::string, std::vector<int>> build_name_map(const SapiIndex& index) {
    std::unordered_map<std::string, std::vector<int>> map;
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        map[to_lower_ascii(symbol.name)].push_back(i);
        map[to_lower_ascii(symbol.fqname)].push_back(i);
        map[to_lower_ascii(symbol.module + "." + symbol.name)].push_back(i);
    }
    return map;
}

std::optional<std::string> base_type_from_signature(const std::string& signature) {
    static const std::regex re(R"(\bextends\s+([A-Za-z_$][A-Za-z0-9_.$]*))");
    std::smatch match;
    if (!std::regex_search(signature, match, re) || match.size() < 2) return std::nullopt;
    std::string base = match[1].str();
    size_t dot = base.rfind('.');
    if (dot != std::string::npos) base = base.substr(dot + 1);
    return base;
}

std::vector<int> find_symbols_by_name(const SapiIndex& index, const std::string& name) {
    std::vector<int> ids;
    std::string needle = to_lower_ascii(name);
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        if (to_lower_ascii(symbol.name) == needle || to_lower_ascii(symbol.fqname) == needle)
            ids.push_back(i);
    }
    return ids;
}

std::vector<SapiSearchResult> lookup_inherited_member(const SapiIndex& index,
                                                      const std::string& owner_name,
                                                      const std::string& member_name) {
    std::vector<SapiSearchResult> results;
    auto owners = find_symbols_by_name(index, owner_name);
    if (owners.empty()) return results;

    std::string member_lower = to_lower_ascii(member_name);
    std::set<int> visited;

    struct QueueItem {
        int symbol_id = -1;
        std::vector<std::string> path;
    };
    std::vector<QueueItem> queue;
    for (int id : owners) {
        if (id < 0 || id >= static_cast<int>(index.symbols.size())) continue;
        queue.push_back({id, {index.symbols[id].name}});
        visited.insert(id);
    }

    for (size_t head = 0; head < queue.size() && results.size() < 16; ++head) {
        const auto item = queue[head];
        const auto& symbol = index.symbols[item.symbol_id];
        auto base = base_type_from_signature(symbol.signature);
        if (!base) continue;

        auto base_ids = find_symbols_by_name(index, *base);
        for (int base_id : base_ids) {
            if (base_id < 0 || base_id >= static_cast<int>(index.symbols.size())) continue;
            const auto& base_symbol = index.symbols[base_id];
            std::vector<std::string> path = item.path;
            path.push_back(base_symbol.name);

            for (int m = 0; m < static_cast<int>(base_symbol.members.size()); ++m) {
                if (to_lower_ascii(base_symbol.members[m].name) != member_lower) continue;
                SapiSearchResult result;
                result.symbol_id = base_id;
                result.member_index = m;
                result.score = 940.0;
                result.match_kind = "inherited-member";
                result.inherited_from = item.path.front() + "." + member_name;
                result.inheritance_path = std::move(path);
                results.push_back(std::move(result));
            }

            if (visited.insert(base_id).second) queue.push_back({base_id, std::move(path)});
        }
    }

    return results;
}

std::vector<int> resolve_type_refs(const SapiIndex& index, const std::vector<std::string>& refs) {
    auto name_map = build_name_map(index);
    std::vector<int> ids;
    std::unordered_set<int> seen;
    for (const auto& ref : refs) {
        auto it = name_map.find(to_lower_ascii(ref));
        if (it == name_map.end()) continue;
        for (int id : it->second) {
            if (seen.insert(id).second) ids.push_back(id);
        }
    }
    return ids;
}

std::vector<int> expand_refs(const SapiIndex& index,
                             int symbol_id,
                             int member_index,
                             int depth,
                             std::vector<int>* out_depths = nullptr,
                             size_t limit = 24) {
    std::vector<int> result;
    if (out_depths) out_depths->clear();
    if (depth <= 0 || symbol_id < 0 || symbol_id >= static_cast<int>(index.symbols.size())) return result;

    std::set<int> seen;
    struct QueueItem {
        int id = -1;
        int remaining = 0;
        int depth = 1;
    };
    std::vector<QueueItem> queue;

    auto enqueue_refs = [&](const std::vector<std::string>& refs, int next_depth, int ref_depth) {
        for (int id : resolve_type_refs(index, refs)) {
            if (id == symbol_id) continue;
            if (seen.insert(id).second) {
                result.push_back(id);
                if (out_depths) out_depths->push_back(ref_depth);
                queue.push_back({id, next_depth, ref_depth});
                if (result.size() >= limit) return;
            }
        }
    };

    const auto& symbol = index.symbols[symbol_id];
    if (member_index >= 0 && member_index < static_cast<int>(symbol.members.size())) {
        std::vector<std::string> refs;
        collect_type_refs_from_text(symbol.members[member_index].signature, refs);
        enqueue_refs(refs, depth - 1, 1);
    } else {
        enqueue_refs(symbol.type_refs, depth - 1, 1);
    }

    for (size_t head = 0; head < queue.size() && result.size() < limit; ++head) {
        const auto item = queue[head];
        if (item.remaining <= 0) continue;
        enqueue_refs(index.symbols[item.id].type_refs, item.remaining - 1, item.depth + 1);
    }

    return result;
}

double score_symbol_candidate(const SapiSymbol& symbol,
                              const std::string& raw_query,
                              const std::vector<std::string>& q_tokens,
                              std::string& match_kind) {
    std::string q_lower = to_lower_ascii(raw_query);
    std::string name_lower = to_lower_ascii(symbol.name);
    std::string fq_lower = to_lower_ascii(symbol.fqname);
    std::string module_lower = to_lower_ascii(symbol.module);

    double score = 0.0;
    if (q_lower == fq_lower) { match_kind = "exact-fqname"; return 1000.0; }
    if (q_lower == name_lower) { match_kind = "exact-symbol"; return 900.0; }
    if (fq_lower.find(q_lower) != std::string::npos && !q_lower.empty()) {
        score = std::max(score, 720.0);
        match_kind = "fqname-contains";
    }
    if (name_lower.find(q_lower) == 0 && !q_lower.empty()) {
        score = std::max(score, 680.0);
        match_kind = "symbol-prefix";
    }

    auto name_tokens = split_identifier_tokens(symbol.name);
    double name_score = token_coverage_score(q_tokens, name_tokens, 620.0, 360.0);
    if (name_score > score) {
        score = name_score;
        match_kind = "symbol-name-tokens";
    }

    auto module_tokens = split_identifier_tokens(symbol.module);
    double module_score = token_coverage_score(q_tokens, module_tokens, 260.0, 120.0);
    if (module_score > score) {
        score = module_score;
        match_kind = "module";
    }

    auto sig_tokens = split_identifier_tokens(symbol.signature);
    double sig_score = token_coverage_score(q_tokens, sig_tokens, 220.0, 90.0, false);
    if (sig_score > score) {
        score = sig_score;
        match_kind = "signature";
    }

    if (!symbol.doc.empty()) {
        int hits = 0;
        std::string doc_lower = to_lower_ascii(symbol.doc);
        for (const auto& token : q_tokens)
            if (doc_lower.find(token) != std::string::npos) ++hits;
        if (hits > 0) {
            double per = static_cast<double>(hits) / std::max<size_t>(1, q_tokens.size());
            // 模块符号的文档即其主要内容（Manifest Details/版本/示例），权重高于普通符号 doc，
            // 但仍低于精确/前缀名匹配，避免淹没真实 API。
            bool is_module = symbol.kind == "module";
            double doc_score = is_module ? 200.0 + 140.0 * per : 60.0 + 40.0 * per;
            if (doc_score > score) {
                score = doc_score;
                match_kind = is_module ? "module-doc" : "doc";
            }
        }
    }

    return score;
}

double score_member_candidate(const SapiMember& member,
                              const std::string& raw_query,
                              const std::vector<std::string>& q_tokens,
                              std::string& match_kind) {
    std::string q_lower = to_lower_ascii(raw_query);
    std::string name_lower = to_lower_ascii(member.name);
    std::string fq_lower = to_lower_ascii(member.fqname);

    double score = 0.0;
    if (q_lower == fq_lower) { match_kind = "exact-member-fqname"; return 980.0; }
    if (q_lower == name_lower) { match_kind = "exact-member"; return 860.0; }
    if (fq_lower.find(q_lower) != std::string::npos && !q_lower.empty()) {
        score = std::max(score, 700.0);
        match_kind = "member-fqname-contains";
    }
    if (name_lower.find(q_lower) == 0 && !q_lower.empty()) {
        score = std::max(score, 660.0);
        match_kind = "member-prefix";
    }

    auto name_tokens = split_identifier_tokens(member.name);
    double name_score = token_coverage_score(q_tokens, name_tokens, 640.0, 340.0);
    if (name_score > score) {
        score = name_score;
        match_kind = "member-name-tokens";
    }

    auto fq_tokens = split_identifier_tokens(member.fqname);
    double fq_score = token_coverage_score(q_tokens, fq_tokens, 760.0, 360.0, false);
    if (fq_score > score) {
        score = fq_score;
        match_kind = "member-fqname-tokens";
    }

    auto sig_tokens = split_identifier_tokens(member.signature);
    double sig_score = token_coverage_score(q_tokens, sig_tokens, 300.0, 120.0, false);
    if (sig_score > score) {
        score = sig_score;
        match_kind = "member-signature";
    }

    if (!member.doc.empty()) {
        int hits = 0;
        std::string doc_lower = to_lower_ascii(member.doc);
        for (const auto& token : q_tokens)
            if (doc_lower.find(token) != std::string::npos) ++hits;
        if (hits > 0) {
            double doc_score = 70.0 + 60.0 * hits / std::max<size_t>(1, q_tokens.size());
            if (doc_score > score) {
                score = doc_score;
                match_kind = "member-doc";
            }
        }
    }

    return score;
}

void attach_refs(const SapiIndex& index, std::vector<SapiSearchResult>& results, int ref_depth) {
    for (auto& result : results)
        result.referenced_symbols = expand_refs(index, result.symbol_id, result.member_index,
                                                ref_depth, &result.referenced_depths);
}

} // namespace

std::vector<SapiSearchResult> search_sapi_symbols(const SapiIndex& index,
                                                  const std::string& query,
                                                  int top_k,
                                                  int ref_depth) {
    std::vector<SapiSearchResult> results;
    std::string q = trim(query);
    if (q.empty()) return results;
    if (top_k <= 0) top_k = 8;
    ref_depth = std::clamp(ref_depth, 0, 3);

    auto q_tokens = query_tokens(q);
    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        std::string match;
        double score = score_symbol_candidate(symbol, q, q_tokens, match);
        if (score > 0.0) results.push_back({i, -1, score, match, {}});

        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            std::string member_match;
            double member_score = score_member_candidate(symbol.members[m], q, q_tokens, member_match);
            if (member_score > 0.0) {
                results.push_back({i, m, member_score, member_match, {}});
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.symbol_id != b.symbol_id) return a.symbol_id < b.symbol_id;
        return a.member_index < b.member_index;
    });

    std::set<std::pair<int, int>> seen;
    std::vector<SapiSearchResult> deduped;
    deduped.reserve(std::min<int>(top_k, static_cast<int>(results.size())));
    for (auto& result : results) {
        std::pair<int, int> key{result.symbol_id, result.member_index};
        if (!seen.insert(key).second) continue;
        deduped.push_back(std::move(result));
        if (static_cast<int>(deduped.size()) >= top_k) break;
    }
    attach_refs(index, deduped, ref_depth);
    return deduped;
}

std::vector<SapiSearchResult> lookup_sapi_symbol(const SapiIndex& index,
                                                 const std::string& name,
                                                 int ref_depth) {
    std::vector<SapiSearchResult> exact_symbols;
    std::vector<SapiSearchResult> exact_members;
    std::vector<SapiSearchResult> member_name_matches;
    std::string q = trim(name);
    if (q.empty()) return {};
    std::string q_lower = to_lower_ascii(q);
    ref_depth = std::clamp(ref_depth, 0, 3);

    for (int i = 0; i < static_cast<int>(index.symbols.size()); ++i) {
        const auto& symbol = index.symbols[i];
        if (to_lower_ascii(symbol.fqname) == q_lower || to_lower_ascii(symbol.name) == q_lower) {
            exact_symbols.push_back({i, -1, 1000.0, "symbol", {}});
        }
        for (int m = 0; m < static_cast<int>(symbol.members.size()); ++m) {
            const auto& member = symbol.members[m];
            if (to_lower_ascii(member.fqname) == q_lower ||
                to_lower_ascii(symbol.name + "." + member.name) == q_lower) {
                exact_members.push_back({i, m, 980.0, "member", {}});
            } else if (to_lower_ascii(member.name) == q_lower) {
                member_name_matches.push_back({i, m, 860.0, "member-name", {}});
            }
        }
    }

    std::vector<SapiSearchResult> results;
    if (!exact_symbols.empty()) {
        results = std::move(exact_symbols);
    } else if (!exact_members.empty()) {
        results = std::move(exact_members);
    } else {
        size_t dot = q.rfind('.');
        if (dot != std::string::npos && dot + 1 < q.size()) {
            auto inherited = lookup_inherited_member(index, q.substr(0, dot), q.substr(dot + 1));
            if (!inherited.empty()) results = std::move(inherited);
        }
        if (results.empty()) results = std::move(member_name_matches);
    }

    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.symbol_id != b.symbol_id) return a.symbol_id < b.symbol_id;
        return a.member_index < b.member_index;
    });
    if (results.size() > 16) results.resize(16);
    attach_refs(index, results, ref_depth);
    return results;
}

} // namespace mcdk::sapi
