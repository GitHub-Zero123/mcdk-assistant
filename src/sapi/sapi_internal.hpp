#pragma once

// SAPI 索引的内部实现细节：缓存格式常量、纯字符串处理与二进制读写辅助。
// 这些helper 同时被运行时侧（sapi_index.cpp）和编译期侧（sapi_build.cpp）使用，
// 匿名命名空间无法跨 TU 共享，故提到这里。此头不含任何 tree-sitter 依赖 ——
// 运行时二进制不应因为 SAPI 而链入 TypeScript 语法分析表。

#include "sapi/sapi_index.hpp"
#include "common/path_utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mcdk::sapi::detail {

namespace fs = std::filesystem;

inline constexpr char kMagic[8] = {'M','C','D','K','S','A','P','I'};
inline constexpr uint32_t kVersion = 2;  // v2: SapiMember 增加 enum 字面值 value
inline constexpr size_t kMaxTreeWalkNodes = 2'000'000;
inline constexpr int kMaxDirectoryDepth = 16;
inline constexpr size_t kMaxSapiFiles = 50'000;

inline std::string read_file(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


inline std::string trim(std::string value) {
    size_t b = 0;
    while (b < value.size() && std::isspace(static_cast<unsigned char>(value[b]))) ++b;
    size_t e = value.size();
    while (e > b && std::isspace(static_cast<unsigned char>(value[e - 1]))) --e;
    return value.substr(b, e - b);
}

inline std::string to_lower_ascii(std::string value) {
    for (char& c : value)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}

inline std::vector<std::string> split_identifier_tokens(const std::string& text) {
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

inline std::vector<std::string> query_tokens(const std::string& query) {
    std::vector<std::string> raw = split_identifier_tokens(query);
    std::vector<std::string> out;
    std::unordered_set<std::string> seen;
    for (auto& token : raw) {
        if (token.empty()) continue;
        if (seen.insert(token).second) out.push_back(std::move(token));
    }
    return out;
}

inline int edit_distance_limited(const std::string& a, const std::string& b, int limit) {
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

inline bool token_near_match(const std::string& query, const std::string& field) {
    if (field.find(query) == 0 || query.find(field) == 0) return true;
    if (query.size() < 4 || field.size() < 4) return false;
    if (query.size() > 32 || field.size() > 32) return false;
    if (query.front() != field.front()) return false;
    int limit = (query.size() >= 5 || field.size() >= 5) ? 2 : 1;
    return edit_distance_limited(query, field, limit) <= limit;
}

inline bool contains_ascii_ci(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    return to_lower_ascii(haystack).find(to_lower_ascii(needle)) != std::string::npos;
}

inline double token_coverage_score(const std::vector<std::string>& query,
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

inline std::string strip_quotes(std::string value) {
    value = trim(std::move(value));
    if (value.size() >= 2 &&
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

inline bool is_ident_start(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

inline bool is_ident_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

inline std::string first_identifier_after_keyword(const std::string& text, const std::string& keyword) {
    size_t pos = text.find(keyword);
    if (pos == std::string::npos) return {};
    pos += keyword.size();
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos;
    if (pos >= text.size() || !is_ident_start(text[pos])) return {};
    size_t start = pos++;
    while (pos < text.size() && is_ident_char(text[pos])) ++pos;
    return text.substr(start, pos - start);
}

inline std::string declaration_name_from_text(const std::string& text, const std::string& kind) {
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

inline std::string kind_for_node_type(const std::string& type) {
    if (type == "class_declaration" || type == "abstract_class_declaration") return "class";
    if (type == "interface_declaration") return "interface";
    if (type == "enum_declaration") return "enum";
    if (type == "type_alias_declaration") return "type";
    if (type == "function_declaration" || type == "function_signature") return "function";
    if (type == "lexical_declaration" || type == "variable_declaration") return "const";
    return {};
}

inline bool is_symbol_node(const std::string& type) {
    return !kind_for_node_type(type).empty();
}

inline bool is_member_node(const std::string& type) {
    return type == "method_signature" ||
           type == "abstract_method_signature" ||
           type == "property_signature" ||
           type == "public_field_definition" ||
           type == "index_signature";
}

inline std::string member_kind_for_node_type(const std::string& type) {
    if (type == "method_signature" || type == "abstract_method_signature") return "method";
    if (type == "index_signature") return "index";
    return "property";
}


inline std::string signature_from_text(std::string text) {
    text = trim(std::move(text));
    size_t brace = text.find('{');
    size_t semi = text.find(';');
    size_t cut = std::min(brace == std::string::npos ? text.size() : brace,
                          semi == std::string::npos ? text.size() : semi);
    if (cut < text.size()) text = text.substr(0, cut + (semi == cut ? 1 : 0));
    text = std::regex_replace(text, std::regex(R"(\s+)"), " ");
    return trim(std::move(text));
}

inline std::string module_from_file(const fs::path& rel_path) {
    std::string rel = mcdk::path::to_utf8(rel_path);
    for (char& c : rel) if (c == '\\') c = '/';
    if (rel.size() >= 5 && rel.substr(rel.size() - 5) == ".d.ts")
        rel.resize(rel.size() - 5);
    if (rel == "vanilla-data/index") return "@minecraft/vanilla-data";
    if (rel.find("vanilla-data/") == 0)
        return "@minecraft/" + rel;
    return "@minecraft/" + rel;
}

inline std::string module_from_manifest_or_file(const std::string& source, const fs::path& rel_path) {
    std::regex re(R"REGEX("module_name"\s*:\s*"([^"]+)")REGEX");
    std::smatch m;
    if (std::regex_search(source, m, re)) return m[1].str();
    return module_from_file(rel_path);
}

inline std::string clean_doc_comment(std::string raw) {
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
inline bool preceding_line_is_directive(const std::string& source, size_t end_pos, size_t& line_begin_out) {
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


inline std::string extract_package_doc(const std::string& source) {
    size_t marker = source.find("@packageDocumentation");
    if (marker == std::string::npos) return {};
    size_t begin = source.rfind("/**", marker);
    size_t end = source.find("*/", marker);
    if (begin == std::string::npos || end == std::string::npos) return {};
    end += 2;
    return clean_doc_comment(source.substr(begin, end - begin));
}


inline void collect_type_refs_from_text(const std::string& text, std::vector<std::string>& refs) {
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

inline void collect_imports(const std::string& source, SapiModule& module) {
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

inline std::string member_name_from_text(const std::string& text) {
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


inline void write_u32(FILE* fp, uint32_t v) { std::fwrite(&v, 4, 1, fp); }
inline uint32_t read_u32(FILE* fp) {
    uint32_t v = 0;
    if (std::fread(&v, 4, 1, fp) != 1) return 0;
    return v;
}

inline void write_i32(FILE* fp, int v) { write_u32(fp, static_cast<uint32_t>(v)); }
inline int read_i32(FILE* fp) { return static_cast<int>(read_u32(fp)); }

inline void write_string(FILE* fp, const std::string& value) {
    write_u32(fp, static_cast<uint32_t>(value.size()));
    if (!value.empty()) std::fwrite(value.data(), 1, value.size(), fp);
}

inline bool read_string(FILE* fp, std::string& value) {
    uint32_t len = read_u32(fp);
    value.resize(len);
    if (len == 0) return true;
    return std::fread(value.data(), 1, len, fp) == len;
}

inline void write_string_vec(FILE* fp, const std::vector<std::string>& values) {
    write_u32(fp, static_cast<uint32_t>(values.size()));
    for (const auto& value : values) write_string(fp, value);
}

inline bool read_string_vec(FILE* fp, std::vector<std::string>& values) {
    uint32_t n = read_u32(fp);
    values.resize(n);
    for (auto& value : values)
        if (!read_string(fp, value)) return false;
    return true;
}


} // namespace mcdk::sapi::detail
