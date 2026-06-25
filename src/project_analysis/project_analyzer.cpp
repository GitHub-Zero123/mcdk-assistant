#include "project_analysis/project_analyzer.hpp"

#include "common/path_utils.hpp"

#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-python.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <unordered_set>

namespace mcdk::project_analysis {
namespace fs = std::filesystem;

namespace {

struct SymbolInfo {
    std::string kind;
    std::string name;
    int line = 0;
};

struct ImportRecord {
    std::string raw_module;
    std::string imported_name;
    bool is_from_import = false;
    int line = 0;
    bool internal = false;
    std::string resolved_module;
    std::string source_kind;
    std::string external_kind;
    std::string evidence;
    double confidence = 0.0;
};

struct FileAnalysis {
    fs::path path;
    fs::path mod_root;
    std::string mod_name;
    std::string module_name;
    bool is_package_init = false;
    bool parse_ok = false;
    int error_nodes = 0;
    int missing_nodes = 0;
    bool fallback_used = false;
    double confidence = 0.0;
    std::string health = "D";
    std::vector<ImportRecord> imports;
    std::vector<SymbolInfo> symbols;
};

struct ModPackage {
    std::string name;
    fs::path root_dir;
    fs::path entry_file;
    std::vector<fs::path> files;
    std::vector<std::string> modules;
};

struct ProjectIndex {
    fs::path behavior_pack_root;
    bool manifest_found = false;
    std::vector<ModPackage> mod_packages;
    std::unordered_map<std::string, FileAnalysis> files_by_module;
    std::unordered_map<std::string, std::vector<std::string>> forward_deps;
    std::unordered_map<std::string, std::vector<std::string>> reverse_deps;
    std::unordered_map<std::string, std::vector<std::string>> external_usage;
    std::vector<std::string> diagnostics;
};

struct ScopeResolution {
    fs::path requested_path;
    fs::path normalized_target;
    fs::path mod_root;
    fs::path behavior_pack_root;
    std::string mod_name;
    std::string scope_mode = "local-only";
    int upward_steps = 0;
};

struct TargetSelection {
    std::vector<std::string> modules;
    bool is_python_file = false;
    bool is_directory = false;
};

std::string trim(std::string value) {
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::vector<std::string> split(const std::string& text, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

std::string join(const std::vector<std::string>& parts, std::string_view delim) {
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i) oss << delim;
        oss << parts[i];
    }
    return oss.str();
}

std::string format_double(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

bool path_exists(const fs::path& path) {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool check_is_directory(const fs::path& path) {
    std::error_code ec;
    return fs::is_directory(path, ec);
}

bool check_is_regular_file(const fs::path& path) {
    std::error_code ec;
    return fs::is_regular_file(path, ec);
}

fs::path normalize_path(const fs::path& path) {
    std::error_code ec;
    auto abs = fs::absolute(path, ec);
    if (ec) return path.lexically_normal();
    return abs.lexically_normal();
}

bool is_python_file(const fs::path& path) {
    return path.extension() == ".py";
}

bool should_skip_dir(const fs::path& dir, bool ignore_third_party_analysis = false) {
    auto name = dir.filename().string();
    if (ignore_third_party_analysis && name == "QuModLibs") return true;
    return name == "__pycache__" || name == ".git" || name == ".svn" || name == ".idea" || name == ".vs";
}

std::string read_text_file(const fs::path& file_path) {
    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs.is_open()) return {};
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3);
    }
    return text;
}

bool is_static_third_party_module(const std::string& module_name) {
    if (module_name.empty()) return false;
    for (const auto& part : split(module_name, '.')) {
        if (part == "QuModLibs") return true;
    }
    return false;
}

bool is_probable_stdlib(const std::string& first_segment) {
    static const std::unordered_set<std::string> kStdlib = {
        "os", "sys", "math", "time", "json", "random", "re", "types", "copy", "traceback",
        "collections", "itertools", "functools", "threading", "logging", "socket", "struct",
        "weakref", "operator", "heapq", "bisect", "StringIO", "cStringIO", "codecs", "inspect",
        "urllib", "urllib2", "httplib", "subprocess", "glob", "shutil", "tempfile", "datetime",
        "csv", "base64", "hashlib", "uuid", "pickle", "Queue", "multiprocessing", "pkgutil"
    };
    return kStdlib.find(first_segment) != kStdlib.end();
}

std::string first_segment(const std::string& module_name) {
    auto pos = module_name.find('.');
    return pos == std::string::npos ? module_name : module_name.substr(0, pos);
}

std::string node_text(TSNode node, const std::string& source) {
    auto start = ts_node_start_byte(node);
    auto end = ts_node_end_byte(node);
    if (start > source.size() || end > source.size() || start > end) return {};
    return source.substr(start, end - start);
}

std::string path_to_utf8(const fs::path& p) {
    return mcdk::path::to_utf8(p);
}

std::string generic_u8string_to_string(const fs::path& p) {
    auto u8 = p.generic_u8string();
    return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
}

std::string module_name_for_file(const fs::path& mod_root, const std::string& mod_name, const fs::path& file_path) {
    std::error_code ec;
    fs::path rel = fs::relative(file_path, mod_root, ec);
    if (ec) rel = file_path.lexically_relative(mod_root);
    std::vector<std::string> parts;
    parts.push_back(mod_name);
    for (const auto& part : rel) {
        std::string seg = part.string();
        if (seg.empty()) continue;
        if (seg == "modMain.py") {
            parts.push_back("modMain");
            break;
        }
        if (seg == "__init__.py") break;
        if (part == rel.filename()) {
            if (part.extension() == ".py") seg = part.stem().string();
        }
        if (seg != "__init__" && seg != ".") parts.push_back(seg);
    }
    return join(parts, ".");
}

std::string package_name_for_module(const FileAnalysis& file) {
    if (file.is_package_init) return file.module_name;
    auto pos = file.module_name.rfind('.');
    if (pos == std::string::npos) return file.module_name;
    return file.module_name.substr(0, pos);
}

void dedup_imports(std::vector<ImportRecord>& imports) {
    std::set<std::tuple<std::string, std::string, bool, int>> seen;
    std::vector<ImportRecord> unique;
    for (const auto& item : imports) {
        auto key = std::make_tuple(item.raw_module, item.imported_name, item.is_from_import, item.line);
        if (seen.insert(key).second) unique.push_back(item);
    }
    imports.swap(unique);
}

void dedup_symbols(std::vector<SymbolInfo>& symbols) {
    std::set<std::tuple<std::string, std::string, int>> seen;
    std::vector<SymbolInfo> unique;
    for (const auto& item : symbols) {
        auto key = std::make_tuple(item.kind, item.name, item.line);
        if (seen.insert(key).second) unique.push_back(item);
    }
    symbols.swap(unique);
}

std::vector<std::string> discover_modules_under_directory(const fs::path& root_dir,
                                                          const std::string& mod_name) {
    std::vector<std::string> modules;
    if (!check_is_directory(root_dir)) return modules;
    std::error_code ec;
    fs::recursive_directory_iterator it(root_dir, ec), end;
    while (!ec && it != end) {
        if (it->is_directory(ec) && should_skip_dir(it->path())) {
            it.disable_recursion_pending();
        } else if (it->is_regular_file(ec) && is_python_file(it->path())) {
            modules.push_back(module_name_for_file(root_dir, mod_name, it->path()));
        }
        it.increment(ec);
    }
    std::sort(modules.begin(), modules.end());
    modules.erase(std::unique(modules.begin(), modules.end()), modules.end());
    return modules;
}

std::vector<ModPackage> discover_mod_packages(const fs::path& behavior_pack_root,
                                              const AnalysisOptions& options) {
    std::vector<ModPackage> mods;
    if (!check_is_directory(behavior_pack_root)) return mods;
    std::error_code ec;
    fs::recursive_directory_iterator it(behavior_pack_root, ec), end;
    while (!ec && it != end) {
        if (it->is_directory(ec) && should_skip_dir(it->path(), options.ignore_third_party_analysis)) {
            it.disable_recursion_pending();
        } else if (it->is_regular_file(ec) && it->path().filename() == "modMain.py") {
            ModPackage pkg;
            pkg.entry_file = normalize_path(it->path());
            pkg.root_dir = normalize_path(it->path().parent_path());
            pkg.name = pkg.root_dir.filename().string();

            std::error_code inner_ec;
            fs::recursive_directory_iterator fit(pkg.root_dir, inner_ec), fend;
            while (!inner_ec && fit != fend) {
                if (fit->is_directory(inner_ec) && should_skip_dir(fit->path(), options.ignore_third_party_analysis)) {
                    fit.disable_recursion_pending();
                } else if (fit->is_regular_file(inner_ec) && is_python_file(fit->path())) {
                    pkg.files.push_back(normalize_path(fit->path()));
                }
                fit.increment(inner_ec);
            }
            std::sort(pkg.files.begin(), pkg.files.end());
            for (const auto& file : pkg.files) {
                pkg.modules.push_back(module_name_for_file(pkg.root_dir, pkg.name, file));
            }
            mods.push_back(std::move(pkg));
            it.disable_recursion_pending();
        }
        it.increment(ec);
    }
    std::sort(mods.begin(), mods.end(), [](const ModPackage& a, const ModPackage& b) {
        return generic_u8string_to_string(a.root_dir) < generic_u8string_to_string(b.root_dir);
    });
    mods.erase(std::unique(mods.begin(), mods.end(), [](const ModPackage& a, const ModPackage& b) {
        return a.root_dir == b.root_dir;
    }), mods.end());
    return mods;
}

void collect_imports_from_text_block(const std::string& text, int base_line, std::vector<ImportRecord>& imports) {
    static const std::regex import_re(R"(([A-Za-z_][A-Za-z0-9_\.]*)(?:\s+as\s+[A-Za-z_][A-Za-z0-9_]*)?)");
    std::string normalized = text;
    std::replace(normalized.begin(), normalized.end(), '\n', ' ');

    if (normalized.rfind("import ", 0) == 0) {
        auto body = trim(normalized.substr(7));
        for (auto it = std::sregex_iterator(body.begin(), body.end(), import_re);
             it != std::sregex_iterator(); ++it) {
            std::string name = trim((*it)[1].str());
            if (!name.empty()) {
                imports.push_back({name, "", false, base_line, false, "", "ast", "", trim(text), 0.95});
            }
        }
        return;
    }

    static const std::regex from_re(R"(^\s*from\s+([\.A-Za-z_][\.A-Za-z0-9_]*)\s+import\s+(.+)$)");
    std::smatch m;
    if (std::regex_search(normalized, m, from_re)) {
        auto module = trim(m[1].str());
        auto names_text = trim(m[2].str());
        if (!names_text.empty() && names_text.front() == '(' && names_text.back() == ')') {
            names_text = trim(names_text.substr(1, names_text.size() - 2));
        }
        for (auto& name : split(names_text, ',')) {
            name = trim(name);
            if (name.empty()) continue;
            auto as_pos = name.find(" as ");
            if (as_pos != std::string::npos) name = trim(name.substr(0, as_pos));
            imports.push_back({module, name, true, base_line, false, "", "ast", "", trim(text), 0.90});
        }
    }
}

void collect_fallback_signals(const std::string& source, FileAnalysis& file) {
    std::istringstream iss(source);
    std::string line;
    int line_no = 0;
    static const std::regex import_line_re(R"(^\s*(import\s+.+|from\s+[\.A-Za-z_][\.A-Za-z0-9_]*\s+import\s+.+)\s*$)");
    static const std::regex class_re(R"(^\s*class\s+([A-Za-z_][A-Za-z0-9_]*)\b)");
    static const std::regex def_re(R"(^\s*def\s+([A-Za-z_][A-Za-z0-9_]*)\s*\()");
    while (std::getline(iss, line)) {
        ++line_no;
        std::smatch m;
        if (std::regex_search(line, m, import_line_re)) {
            collect_imports_from_text_block(m[1].str(), line_no, file.imports);
            file.fallback_used = true;
        }
        if (std::regex_search(line, m, class_re)) {
            file.symbols.push_back({"class", m[1].str(), line_no});
            file.fallback_used = true;
        }
        if (std::regex_search(line, m, def_re)) {
            file.symbols.push_back({"function", m[1].str(), line_no});
            file.fallback_used = true;
        }
    }
}

void walk_tree(TSNode node, const std::string& source, FileAnalysis& file) {
    std::string type = ts_node_type(node);
    if (type == "ERROR") file.error_nodes++;
    if (ts_node_is_missing(node)) file.missing_nodes++;

    if (type == "import_statement" || type == "import_from_statement") {
        auto text = node_text(node, source);
        collect_imports_from_text_block(text, static_cast<int>(ts_node_start_point(node).row) + 1, file.imports);
    } else if (type == "class_definition") {
        TSNode name = ts_node_child_by_field_name(node, "name", 4);
        auto symbol = trim(node_text(name, source));
        if (!symbol.empty()) {
            file.symbols.push_back({"class", symbol, static_cast<int>(ts_node_start_point(node).row) + 1});
        }
    } else if (type == "function_definition") {
        TSNode name = ts_node_child_by_field_name(node, "name", 4);
        auto symbol = trim(node_text(name, source));
        if (!symbol.empty()) {
            file.symbols.push_back({"function", symbol, static_cast<int>(ts_node_start_point(node).row) + 1});
        }
    }

    auto count = ts_node_child_count(node);
    for (uint32_t i = 0; i < count; ++i) {
        walk_tree(ts_node_child(node, i), source, file);
    }
}

FileAnalysis analyze_python_file(const fs::path& file_path,
                                 const fs::path& mod_root,
                                 const std::string& mod_name) {
    FileAnalysis file;
    file.path = normalize_path(file_path);
    file.mod_root = normalize_path(mod_root);
    file.mod_name = mod_name;
    file.is_package_init = file.path.filename() == "__init__.py";
    file.module_name = module_name_for_file(file.mod_root, file.mod_name, file.path);

    auto source = read_text_file(file.path);
    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_python());
    TSTree* tree = ts_parser_parse_string(parser, nullptr, source.c_str(), static_cast<uint32_t>(source.size()));
    TSNode root = ts_tree_root_node(tree);

    walk_tree(root, source, file);
    file.parse_ok = !ts_node_has_error(root) && file.error_nodes == 0 && file.missing_nodes == 0;
    if (!file.parse_ok || file.imports.empty()) {
        collect_fallback_signals(source, file);
    }

    dedup_imports(file.imports);
    dedup_symbols(file.symbols);

    if (file.parse_ok) {
        file.health = "A";
        file.confidence = 0.98;
    } else if (file.error_nodes <= 2 && file.missing_nodes <= 2) {
        file.health = "B";
        file.confidence = file.fallback_used ? 0.78 : 0.84;
    } else if (!file.imports.empty() || !file.symbols.empty()) {
        file.health = "C";
        file.confidence = file.fallback_used ? 0.62 : 0.58;
    } else {
        file.health = "D";
        file.confidence = 0.30;
    }

    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return file;
}

std::string resolve_relative_module(const ImportRecord& imp, const FileAnalysis& file) {
    if (imp.raw_module.empty() || imp.raw_module[0] != '.') return imp.raw_module;
    int dot_count = 0;
    while (dot_count < static_cast<int>(imp.raw_module.size()) && imp.raw_module[dot_count] == '.') ++dot_count;
    auto remainder = imp.raw_module.substr(dot_count);
    auto package = package_name_for_module(file);
    auto base_parts = split(package, '.');
    int ascend = std::max(0, dot_count - 1);
    while (ascend-- > 0 && !base_parts.empty()) base_parts.pop_back();
    if (!remainder.empty()) {
        auto remain_parts = split(remainder, '.');
        base_parts.insert(base_parts.end(), remain_parts.begin(), remain_parts.end());
    }
    return join(base_parts, ".");
}

std::vector<std::string> resolve_internal_candidates(const ImportRecord& imp, const FileAnalysis& file) {
    std::vector<std::string> out;
    auto base = resolve_relative_module(imp, file);
    if (!base.empty()) out.push_back(base);
    if (imp.is_from_import && !imp.imported_name.empty() && imp.imported_name != "*") {
        if (!base.empty()) out.push_back(base + "." + imp.imported_name);
        else out.push_back(imp.imported_name);
    }
    return out;
}

void build_dependency_graph(ProjectIndex& index, const AnalysisOptions& options) {
    std::unordered_set<std::string> module_set;
    for (const auto& [module_name, _] : index.files_by_module) module_set.insert(module_name);

    for (auto& [module_name, file] : index.files_by_module) {
        auto& deps = index.forward_deps[module_name];
        std::unordered_set<std::string> dep_seen;
        for (auto& imp : file.imports) {
            auto candidates = resolve_internal_candidates(imp, file);
            bool matched = false;
            for (const auto& candidate : candidates) {
                if (candidate.empty()) continue;
                if (module_set.find(candidate) != module_set.end()) {
                    imp.internal = true;
                    imp.resolved_module = candidate;
                    imp.confidence = std::max(imp.confidence, imp.is_from_import ? 0.90 : 0.95);
                    if (dep_seen.insert(candidate).second) deps.push_back(candidate);
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                auto base = resolve_relative_module(imp, file);
                auto first = first_segment(base.empty() ? imp.raw_module : base);
                if (options.ignore_third_party_analysis &&
                    is_static_third_party_module(base.empty() ? imp.raw_module : base)) {
                    imp.external_kind = "third-party-static";
                    imp.resolved_module = "QuModLibs";
                    imp.confidence = 0.80;
                    index.external_usage[imp.resolved_module].push_back(module_name + ": line " + std::to_string(imp.line));
                } else if (is_probable_stdlib(first)) {
                    imp.external_kind = "stdlib";
                    imp.resolved_module = first;
                    imp.confidence = 0.50;
                    index.external_usage[first].push_back(module_name + ": line " + std::to_string(imp.line));
                } else {
                    imp.external_kind = "unknown-external";
                    imp.resolved_module = first.empty() ? imp.raw_module : first;
                    imp.confidence = 0.35;
                    if (options.include_unresolved) {
                        index.external_usage[imp.resolved_module].push_back(module_name + ": line " + std::to_string(imp.line));
                    }
                }
            }
        }
        std::sort(deps.begin(), deps.end());
        deps.erase(std::unique(deps.begin(), deps.end()), deps.end());
    }

    for (const auto& [from, deps] : index.forward_deps) {
        for (const auto& to : deps) {
            index.reverse_deps[to].push_back(from);
        }
    }
    for (auto& [module, refs] : index.reverse_deps) {
        std::sort(refs.begin(), refs.end());
        refs.erase(std::unique(refs.begin(), refs.end()), refs.end());
    }
}

ProjectIndex build_index_for_behavior_pack(const fs::path& behavior_pack_root,
                                           const AnalysisOptions& options) {
    ProjectIndex index;
    index.behavior_pack_root = normalize_path(behavior_pack_root);
    index.manifest_found = path_exists(index.behavior_pack_root / "manifest.json");
    index.mod_packages = discover_mod_packages(index.behavior_pack_root, options);
    if (index.mod_packages.empty()) {
        index.diagnostics.push_back("未发现任何 modMain.py，无法识别有效 mod 包。");
        return index;
    }

    for (const auto& pkg : index.mod_packages) {
        for (const auto& file_path : pkg.files) {
            auto file = analyze_python_file(file_path, pkg.root_dir, pkg.name);
            index.files_by_module[file.module_name] = std::move(file);
        }
    }
    build_dependency_graph(index, options);
    return index;
}

bool directory_contains_mod_main(const fs::path& root, int max_depth = 8) {
    if (!check_is_directory(root)) return false;
    std::error_code ec;
    fs::recursive_directory_iterator it(root, ec), end;
    while (!ec && it != end) {
        auto rel = it->path().lexically_relative(root);
        if (static_cast<int>(std::distance(rel.begin(), rel.end())) > max_depth) {
            if (it->is_directory(ec)) it.disable_recursion_pending();
            it.increment(ec);
            continue;
        }
        if (it->is_directory(ec) && should_skip_dir(it->path(), false)) {
            it.disable_recursion_pending();
        } else if (it->is_regular_file(ec) && it->path().filename() == "modMain.py") {
            return true;
        }
        it.increment(ec);
    }
    return false;
}

ScopeResolution resolve_scope_for_target(const fs::path& target_path, int max_upward_levels) {
    ScopeResolution scope;
    scope.requested_path = target_path;
    scope.normalized_target = normalize_path(target_path);

    fs::path start = scope.normalized_target;
    if (check_is_regular_file(start)) start = start.parent_path();
    start = normalize_path(start);

    // 如果起始目录本身包含 manifest.json，说明这就是行为包根，无需上溯。
    if (path_exists(start / "manifest.json")) {
        scope.behavior_pack_root = start;
        scope.scope_mode = "behavior-pack";
        scope.upward_steps = 0;
        return scope;
    }

    // 网易 MOD 工程经常缺少位于用户所选根目录的 manifest.json，但其下层会有一个或多个 modMain.py。
    // 这种情况应把用户传入目录当作“可发现行为包根”，否则引用分析会退化成 local-only 空结果。
    if (directory_contains_mod_main(start)) {
        scope.behavior_pack_root = start;
        scope.scope_mode = "behavior-pack-discovered";
        scope.upward_steps = 0;
        return scope;
    }

    fs::path current = start;
    for (int depth = 0; depth <= std::max(1, max_upward_levels); ++depth) {
        scope.upward_steps = depth;
        if (scope.mod_root.empty() && path_exists(current / "modMain.py")) {
            scope.mod_root = current;
            scope.mod_name = current.filename().string();
            scope.scope_mode = "mod-package";
        }
        if (path_exists(current / "manifest.json")) {
            scope.behavior_pack_root = current;
            if (!scope.mod_root.empty()) scope.scope_mode = "behavior-pack";
            break;
        }
        auto parent = current.parent_path();
        if (parent == current || parent.empty()) break;
        current = parent;
    }

    return scope;
}

ProjectIndex build_index_for_scope(const ScopeResolution& scope, const AnalysisOptions& options) {
    if (!scope.behavior_pack_root.empty()) {
        return build_index_for_behavior_pack(scope.behavior_pack_root, options);
    }

    ProjectIndex index;
    if (scope.mod_root.empty()) {
        index.behavior_pack_root = scope.normalized_target;
        index.diagnostics.push_back("未向上解析到 modMain.py 或 manifest.json，将退化为局部分析。");
        return index;
    }

    index.behavior_pack_root = scope.mod_root;
    ModPackage pkg;
    pkg.name = scope.mod_name;
    pkg.root_dir = scope.mod_root;
    pkg.entry_file = scope.mod_root / "modMain.py";
    std::error_code ec;
    fs::recursive_directory_iterator it(scope.mod_root, ec), end;
    while (!ec && it != end) {
        if (it->is_directory(ec) && should_skip_dir(it->path(), options.ignore_third_party_analysis)) {
            it.disable_recursion_pending();
        } else if (it->is_regular_file(ec) && is_python_file(it->path())) {
            pkg.files.push_back(normalize_path(it->path()));
        }
        it.increment(ec);
    }
    std::sort(pkg.files.begin(), pkg.files.end());
    for (const auto& file_path : pkg.files) {
        auto file = analyze_python_file(file_path, pkg.root_dir, pkg.name);
        pkg.modules.push_back(file.module_name);
        index.files_by_module[file.module_name] = std::move(file);
    }
    index.mod_packages.push_back(std::move(pkg));
    build_dependency_graph(index, options);
    return index;
}

TargetSelection select_target_modules(const ProjectIndex& index,
                                      const ScopeResolution& scope) {
    TargetSelection selection;
    auto target = scope.normalized_target;
    selection.is_python_file = check_is_regular_file(target) && is_python_file(target);
    selection.is_directory = check_is_directory(target);

    if (selection.is_python_file && !scope.mod_root.empty()) {
        auto module = module_name_for_file(scope.mod_root, scope.mod_name, target);
        if (index.files_by_module.find(module) != index.files_by_module.end()) selection.modules.push_back(module);
        return selection;
    }

    if (selection.is_directory) {
        auto dir_norm = generic_u8string_to_string(normalize_path(target));
        for (const auto& [module_name, file] : index.files_by_module) {
            auto file_dir = generic_u8string_to_string(normalize_path(file.path.parent_path()));
            auto file_full = generic_u8string_to_string(normalize_path(file.path));
            if (file_dir == dir_norm || file_full.rfind(dir_norm + "/", 0) == 0 || file_dir.rfind(dir_norm + "/", 0) == 0) {
                selection.modules.push_back(module_name);
            }
        }
    }

    std::sort(selection.modules.begin(), selection.modules.end());
    selection.modules.erase(std::unique(selection.modules.begin(), selection.modules.end()), selection.modules.end());
    return selection;
}

std::vector<std::pair<std::string, int>> compute_indirect_referrers(const ProjectIndex& index,
                                                                    const std::unordered_set<std::string>& targets,
                                                                    int max_depth) {
    std::vector<std::pair<std::string, int>> result;
    std::unordered_map<std::string, int> seen_depth;
    std::vector<std::pair<std::string, int>> frontier;
    for (const auto& target : targets) frontier.push_back({target, 0});

    while (!frontier.empty()) {
        auto [current, depth] = frontier.back();
        frontier.pop_back();
        if (depth >= max_depth) continue;
        auto it = index.reverse_deps.find(current);
        if (it == index.reverse_deps.end()) continue;
        for (const auto& parent : it->second) {
            auto seen = seen_depth.find(parent);
            if (seen != seen_depth.end() && seen->second <= depth + 1) continue;
            seen_depth[parent] = depth + 1;
            if (targets.find(parent) == targets.end()) result.push_back({parent, depth + 1});
            frontier.push_back({parent, depth + 1});
        }
    }

    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second < b.second;
        return a.first < b.first;
    });
    result.erase(std::unique(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return a.first == b.first;
    }), result.end());
    return result;
}

void append_scope_summary(std::ostringstream& oss, const ScopeResolution& scope) {
    oss << "作用域解析\n";
    oss << "- 请求目标: " << path_to_utf8(scope.normalized_target) << "\n";
    oss << "- 解析模式: " << scope.scope_mode << "\n";
    if (!scope.mod_root.empty()) {
        oss << "- mod 包根: " << path_to_utf8(scope.mod_root) << "\n";
        oss << "- mod 包名: " << scope.mod_name << "\n";
    }
    if (!scope.behavior_pack_root.empty()) {
        oss << "- 行为包根: " << path_to_utf8(scope.behavior_pack_root) << "\n";
    }
    oss << "- 向上解析层数: " << scope.upward_steps << "\n\n";
}

std::string summarize_file_health(const ProjectIndex& index, int limit) {
    struct Row { std::string module; std::string health; double confidence; int errors; int missing; bool fallback; };
    std::vector<Row> rows;
    for (const auto& [module_name, file] : index.files_by_module) {
        rows.push_back({module_name, file.health, file.confidence, file.error_nodes, file.missing_nodes, file.fallback_used});
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        if (a.health != b.health) return a.health > b.health;
        return a.module < b.module;
    });
    std::ostringstream oss;
    int count = 0;
    for (const auto& row : rows) {
        if (count++ >= limit) break;
        oss << "- [" << row.health << "] " << row.module
            << " (confidence=" << format_double(row.confidence)
            << ", errors=" << row.errors
            << ", missing=" << row.missing;
        if (row.fallback) oss << ", fallback=yes";
        oss << ")\n";
    }
    return oss.str();
}

std::string summarize_external_usage(const ProjectIndex& index, int limit) {
    if (index.external_usage.empty()) return "- 无\n";
    std::vector<std::pair<std::string, std::vector<std::string>>> rows(index.external_usage.begin(), index.external_usage.end());
    std::sort(rows.begin(), rows.end(), [](const auto& a, const auto& b) {
        if (a.second.size() != b.second.size()) return a.second.size() > b.second.size();
        return a.first < b.first;
    });
    std::ostringstream oss;
    int count = 0;
    for (const auto& [name, uses] : rows) {
        if (count++ >= limit) break;
        oss << "- " << name << " 被 " << uses.size() << " 处引用";
        if (!uses.empty()) oss << "，示例: " << uses.front();
        oss << "\n";
    }
    return oss.str();
}

std::string summarize_top_modules(const ProjectIndex& index, const ModPackage& pkg, int limit, bool include_symbols) {
    struct Row { std::string module; size_t out_degree; size_t in_degree; size_t symbols; };
    std::vector<Row> rows;
    for (const auto& module_name : pkg.modules) {
        auto fit = index.files_by_module.find(module_name);
        if (fit == index.files_by_module.end()) continue;
        size_t out_degree = 0;
        size_t in_degree = 0;
        auto out_it = index.forward_deps.find(module_name);
        if (out_it != index.forward_deps.end()) out_degree = out_it->second.size();
        auto in_it = index.reverse_deps.find(module_name);
        if (in_it != index.reverse_deps.end()) in_degree = in_it->second.size();
        rows.push_back({module_name, out_degree, in_degree, fit->second.symbols.size()});
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        if ((a.out_degree + a.in_degree) != (b.out_degree + b.in_degree))
            return (a.out_degree + a.in_degree) > (b.out_degree + b.in_degree);
        return a.module < b.module;
    });

    std::ostringstream oss;
    int count = 0;
    for (const auto& row : rows) {
        if (count++ >= limit) break;
        oss << "- " << row.module << " (in=" << row.in_degree << ", out=" << row.out_degree;
        if (include_symbols) oss << ", symbols=" << row.symbols;
        oss << ")\n";
    }
    return oss.str();
}

std::string summarize_symbols(const FileAnalysis& file, int limit = 12) {
    if (file.symbols.empty()) return "无";
    std::ostringstream oss;
    int shown = 0;
    for (const auto& symbol : file.symbols) {
        if (shown >= limit) break;
        if (shown) oss << ", ";
        oss << symbol.kind << " " << symbol.name << "@" << symbol.line;
        ++shown;
    }
    if (static_cast<int>(file.symbols.size()) > shown) {
        oss << ", ... +" << (file.symbols.size() - shown);
    }
    return oss.str();
}

std::string summarize_imports(const FileAnalysis& file, bool only_internal, int limit = 10) {
    std::vector<std::string> rows;
    for (const auto& imp : file.imports) {
        if (only_internal && !imp.internal) continue;
        std::ostringstream item;
        item << (imp.resolved_module.empty() ? imp.raw_module : imp.resolved_module);
        if (!imp.imported_name.empty()) item << "." << imp.imported_name;
        item << "@" << imp.line << "[" << (imp.internal ? "internal" : imp.external_kind) << "]";
        rows.push_back(item.str());
    }
    std::sort(rows.begin(), rows.end());
    rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
    if (rows.empty()) return "无";
    std::ostringstream oss;
    const int shown = std::min(limit, static_cast<int>(rows.size()));
    for (int i = 0; i < shown; ++i) {
        if (i) oss << "; ";
        oss << rows[i];
    }
    if (static_cast<int>(rows.size()) > shown) oss << "; ... +" << (rows.size() - shown);
    return oss.str();
}

std::string summarize_directory_tree(const ProjectIndex& index, const ModPackage& pkg) {
    struct DirRow { int files = 0; int modules = 0; int symbols = 0; int in_degree = 0; int out_degree = 0; };
    std::map<std::string, DirRow> rows;

    for (const auto& module_name : pkg.modules) {
        auto fit = index.files_by_module.find(module_name);
        if (fit == index.files_by_module.end()) continue;
        std::error_code ec;
        auto rel = fs::relative(fit->second.path.parent_path(), pkg.root_dir, ec);
        std::string dir = ec || rel.empty() || rel == "." ? "." : generic_u8string_to_string(rel);
        auto& row = rows[dir];
        row.files += 1;
        row.modules += 1;
        row.symbols += static_cast<int>(fit->second.symbols.size());
        auto out_it = index.forward_deps.find(module_name);
        if (out_it != index.forward_deps.end()) row.out_degree += static_cast<int>(out_it->second.size());
        auto in_it = index.reverse_deps.find(module_name);
        if (in_it != index.reverse_deps.end()) row.in_degree += static_cast<int>(in_it->second.size());
    }

    std::ostringstream oss;
    for (const auto& [dir, row] : rows) {
        oss << "  - " << dir << " (files=" << row.files
            << ", modules=" << row.modules
            << ", symbols=" << row.symbols
            << ", in=" << row.in_degree
            << ", out=" << row.out_degree << ")\n";
    }
    return oss.str();
}

std::string first_relative_dir(const FileAnalysis& file, const fs::path& root) {
    std::error_code ec;
    auto rel = fs::relative(file.path, root, ec);
    if (ec || rel.empty()) return ".";
    auto it = rel.begin();
    if (it == rel.end()) return ".";
    if (std::next(it) == rel.end()) return ".";
    return it->string();
}

std::string summarize_subsystems(const ProjectIndex& index, const ModPackage& pkg, int limit) {
    struct Row { std::string name; int modules = 0; int symbols = 0; int in_degree = 0; int out_degree = 0; std::string example; };
    std::map<std::string, Row> grouped;
    for (const auto& module_name : pkg.modules) {
        auto fit = index.files_by_module.find(module_name);
        if (fit == index.files_by_module.end()) continue;
        auto key = first_relative_dir(fit->second, pkg.root_dir);
        auto& row = grouped[key];
        row.name = key;
        row.modules += 1;
        row.symbols += static_cast<int>(fit->second.symbols.size());
        if (row.example.empty()) row.example = module_name;
        auto out_it = index.forward_deps.find(module_name);
        if (out_it != index.forward_deps.end()) row.out_degree += static_cast<int>(out_it->second.size());
        auto in_it = index.reverse_deps.find(module_name);
        if (in_it != index.reverse_deps.end()) row.in_degree += static_cast<int>(in_it->second.size());
    }

    std::vector<Row> rows;
    for (auto& [_, row] : grouped) rows.push_back(row);
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        const auto aw = a.in_degree + a.out_degree + a.modules;
        const auto bw = b.in_degree + b.out_degree + b.modules;
        if (aw != bw) return aw > bw;
        return a.name < b.name;
    });

    std::ostringstream oss;
    int shown = 0;
    for (const auto& row : rows) {
        if (shown++ >= limit) break;
        oss << "- " << row.name
            << "：modules=" << row.modules
            << ", symbols=" << row.symbols
            << ", in=" << row.in_degree
            << ", out=" << row.out_degree
            << ", example=" << row.example << "\n";
    }
    if (static_cast<int>(rows.size()) > shown) oss << "- ... 其余 " << (rows.size() - shown) << " 个子系统省略\n";
    return oss.str();
}

std::string summarize_dependency_hubs(const ProjectIndex& index, int limit) {
    struct Row { std::string module; size_t in_degree; size_t out_degree; };
    std::vector<Row> rows;
    for (const auto& [module_name, file] : index.files_by_module) {
        size_t in_degree = 0;
        size_t out_degree = 0;
        auto out_it = index.forward_deps.find(module_name);
        if (out_it != index.forward_deps.end()) out_degree = out_it->second.size();
        auto in_it = index.reverse_deps.find(module_name);
        if (in_it != index.reverse_deps.end()) in_degree = in_it->second.size();
        if (in_degree || out_degree) rows.push_back({module_name, in_degree, out_degree});
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        const auto aw = a.in_degree + a.out_degree;
        const auto bw = b.in_degree + b.out_degree;
        if (aw != bw) return aw > bw;
        return a.module < b.module;
    });
    std::ostringstream oss;
    int shown = 0;
    for (const auto& row : rows) {
        if (shown++ >= limit) break;
        oss << "- " << row.module << " (被依赖=" << row.in_degree << ", 依赖=" << row.out_degree << ")\n";
    }
    return oss.str();
}

std::string summarize_dependencies_aggregate(const ProjectIndex& index,
                                             const std::vector<std::string>& modules,
                                             int limit) {
    struct Row { std::string dep; int count = 0; int internal_count = 0; std::set<std::string> examples; };
    std::map<std::string, Row> grouped;
    for (const auto& module : modules) {
        auto fit = index.files_by_module.find(module);
        if (fit == index.files_by_module.end()) continue;
        std::set<std::string> seen_in_module;
        for (const auto& imp : fit->second.imports) {
            auto dep = imp.resolved_module.empty() ? imp.raw_module : imp.resolved_module;
            if (dep.empty()) continue;
            auto& row = grouped[dep];
            row.dep = dep;
            if (seen_in_module.insert(dep).second) ++row.count;
            if (imp.internal) ++row.internal_count;
            if (row.examples.size() < 3) row.examples.insert(module + ":" + std::to_string(imp.line));
        }
    }

    std::vector<Row> rows;
    for (auto& [_, row] : grouped) rows.push_back(row);
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.internal_count != b.internal_count) return a.internal_count > b.internal_count;
        return a.dep < b.dep;
    });

    std::ostringstream oss;
    int shown = 0;
    for (const auto& row : rows) {
        if (shown++ >= limit) break;
        oss << "- " << row.dep << "：被目标范围内 " << row.count << " 个模块依赖"
            << "，internal_refs=" << row.internal_count;
        if (!row.examples.empty()) oss << "，示例=" << join(std::vector<std::string>(row.examples.begin(), row.examples.end()), ", ");
        oss << "\n";
    }
    if (static_cast<int>(rows.size()) > shown) oss << "- ... 其余 " << (rows.size() - shown) << " 个依赖省略\n";
    return oss.str();
}

std::string summarize_module_inventory(const ProjectIndex& index, const ModPackage& pkg, int max_modules) {
    struct Row { std::string module; size_t in_degree; size_t out_degree; size_t symbols; };
    std::vector<Row> rows;
    for (const auto& module_name : pkg.modules) {
        auto fit = index.files_by_module.find(module_name);
        if (fit == index.files_by_module.end()) continue;
        size_t in_degree = 0;
        size_t out_degree = 0;
        auto out_it = index.forward_deps.find(module_name);
        if (out_it != index.forward_deps.end()) out_degree = out_it->second.size();
        auto in_it = index.reverse_deps.find(module_name);
        if (in_it != index.reverse_deps.end()) in_degree = in_it->second.size();
        rows.push_back({module_name, in_degree, out_degree, fit->second.symbols.size()});
    }
    std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b) {
        const auto aw = a.in_degree + a.out_degree;
        const auto bw = b.in_degree + b.out_degree;
        if (aw != bw) return aw > bw;
        if (a.symbols != b.symbols) return a.symbols > b.symbols;
        return a.module < b.module;
    });

    std::ostringstream oss;
    int shown = 0;
    for (const auto& row : rows) {
        if (shown++ >= max_modules) break;
        auto fit = index.files_by_module.find(row.module);
        if (fit == index.files_by_module.end()) continue;
        const auto& file = fit->second;
        std::error_code ec;
        auto rel = fs::relative(file.path, pkg.root_dir, ec);

        oss << "- " << row.module << "\n";
        oss << "  - role_hint: " << (ec ? path_to_utf8(file.path) : generic_u8string_to_string(rel)) << "\n";
        oss << "  - health: " << file.health
            << ", confidence=" << format_double(file.confidence)
            << ", fallback=" << (file.fallback_used ? "yes" : "no") << "\n";
        oss << "  - importance: in=" << row.in_degree << ", out=" << row.out_degree
            << ", symbols=" << row.symbols << "\n";
        oss << "  - public_symbols_sample: " << summarize_symbols(file, 10) << "\n";
        oss << "  - internal_deps_sample: " << summarize_imports(file, true, 8) << "\n";
    }
    if (static_cast<int>(rows.size()) > max_modules) {
        oss << "- ... 其余 " << (rows.size() - max_modules)
            << " 个模块已省略；若需深挖，可使用 minecraft_py(command=\"imports <目标目录或文件>\")。\n";
    }
    return oss.str();
}

std::string format_behavior_pack_report(const ProjectIndex& index, const AnalysisOptions& options) {
    std::ostringstream oss;
    oss << "Python 2 行为包架构分析\n";
    oss << "- 行为包路径: " << path_to_utf8(index.behavior_pack_root) << "\n";
    oss << "- manifest.json: " << (index.manifest_found ? "已发现" : "未发现") << "\n";
    oss << "- mod 包数量: " << index.mod_packages.size() << "\n";
    oss << "- Python 模块数量: " << index.files_by_module.size() << "\n\n";

    if (!index.diagnostics.empty()) {
        oss << "诊断\n";
        for (const auto& item : index.diagnostics) oss << "- " << item << "\n";
        oss << "\n";
    }

    oss << "AI 阅读建议\n";
    oss << "- 先看“子系统画像”和“关键依赖枢纽”确定架构层次。\n";
    oss << "- 再使用 minecraft_py(command=\"imports <目标目录或文件>\") 获取局部调用上下文。\n";
    oss << "- 下方模块画像是抽样摘要，不再 dump 全量模块，避免淹没主线。\n\n";

    oss << "入口与子包总览\n";
    for (const auto& pkg : index.mod_packages) {
        oss << "- " << pkg.name << "\n";
        oss << "  - root: " << path_to_utf8(pkg.root_dir) << "\n";
        oss << "  - entry: " << pkg.name << ".modMain\n";
        oss << "  - files: " << pkg.files.size() << ", modules: " << pkg.modules.size() << "\n";
        oss << "  - 关键模块:\n";
        oss << summarize_top_modules(index, pkg, std::max(3, options.depth * 2), options.include_symbols);
    }
    oss << "\n";

    oss << "子系统画像（按目录聚合）\n";
    for (const auto& pkg : index.mod_packages) {
        oss << "# " << pkg.name << "\n";
        oss << summarize_subsystems(index, pkg, std::max(8, options.depth * 4));
    }
    oss << "\n";

    oss << "关键依赖枢纽\n";
    oss << summarize_dependency_hubs(index, std::max(8, options.depth * 4)) << "\n";

    oss << "代表模块画像（按重要度抽样）\n";
    for (const auto& pkg : index.mod_packages) {
        oss << "# " << pkg.name << "\n";
        oss << summarize_module_inventory(index, pkg, std::max(8, options.depth * 4)) << "\n";
    }

    oss << "文件健康摘要\n";
    oss << summarize_file_health(index, std::max(6, options.depth * 5)) << "\n";

    oss << "项目外依赖\n";
    oss << summarize_external_usage(index, std::max(6, options.depth * 4)) << "\n";

    oss << "模糊性与风险\n";
    int unresolved_count = 0;
    for (const auto& [module_name, file] : index.files_by_module) {
        for (const auto& imp : file.imports) {
            if (!imp.internal && imp.external_kind == "unknown-external") ++unresolved_count;
        }
    }
    oss << "- 未解析外部引用: " << unresolved_count << "\n";
    oss << "- 容错回退文件数: "
        << std::count_if(index.files_by_module.begin(), index.files_by_module.end(), [](const auto& item) {
               return item.second.fallback_used;
           }) << "\n";
    oss << "- 说明：标准库与 QuModLibs 仅给出调用分布，不参与深度依赖计算。\n";
    return oss.str();
}

std::string format_reference_report(const ProjectIndex& index,
                                    const ScopeResolution& scope,
                                    const TargetSelection& selection,
                                    const AnalysisOptions& options) {
    std::ostringstream oss;
    oss << "Python 2 引用架构分析\n";
    append_scope_summary(oss, scope);

    if (selection.modules.empty()) {
        oss << "未能在当前作用域内解析到目标模块。\n";
        if (!index.mod_packages.empty()) {
            oss << "\n当前作用域可用模块清单\n";
            for (const auto& pkg : index.mod_packages) {
                oss << "# " << pkg.name << "\n";
                oss << summarize_module_inventory(index, pkg, std::max(10, options.depth * 6)) << "\n";
            }
        }
        return oss.str();
    }

    std::unordered_set<std::string> target_set(selection.modules.begin(), selection.modules.end());
    std::set<std::string> direct_refs;
    for (const auto& module : selection.modules) {
        auto it = index.reverse_deps.find(module);
        if (it == index.reverse_deps.end()) continue;
        direct_refs.insert(it->second.begin(), it->second.end());
    }
    for (const auto& self : selection.modules) direct_refs.erase(self);

    auto indirect = options.include_indirect
        ? compute_indirect_referrers(index, target_set, std::max(1, options.depth + 1))
        : std::vector<std::pair<std::string, int>>{};

    oss << "目标摘要\n";
    oss << "- 命中模块数: " << selection.modules.size() << "\n";
    oss << "- 输出策略: 聚合优先 + 代表模块抽样；避免对大目录逐文件 dump。\n";
    if (selection.modules.size() <= 8) {
        for (const auto& module : selection.modules) {
            auto it = index.files_by_module.find(module);
            if (it == index.files_by_module.end()) continue;
            const auto& file = it->second;
            oss << "- " << module << " -> " << path_to_utf8(file.path)
                << " [health=" << file.health
                << ", confidence=" << format_double(file.confidence) << "]\n";
            oss << "  - symbols: " << summarize_symbols(file, 12) << "\n";
            oss << "  - internal_imports: " << summarize_imports(file, true, 8) << "\n";
            oss << "  - all_imports: " << summarize_imports(file, false, 8) << "\n";
        }
    } else {
        ModPackage target_brief;
        target_brief.name = "target";
        target_brief.root_dir = scope.mod_root.empty() ? scope.behavior_pack_root : scope.mod_root;
        target_brief.modules = selection.modules;
        oss << "- 目标范围过大，以下仅展示高价值代表模块：\n";
        oss << summarize_module_inventory(index, target_brief, std::max(8, options.depth * 3));
    }
    oss << "\n";

    oss << "直接引用者\n";
    if (direct_refs.empty()) {
        oss << "- 无项目内直接引用者\n";
    } else {
        for (const auto& ref : direct_refs) oss << "- " << ref << "\n";
    }
    oss << "\n";

    oss << "间接引用链\n";
    if (indirect.empty()) {
        oss << "- 无或未启用\n";
    } else {
        int shown = 0;
        for (const auto& [module, depth] : indirect) {
            if (shown++ >= std::max(8, options.depth * 6)) break;
            oss << "- depth=" << depth << " <- " << module << "\n";
        }
    }
    oss << "\n";

    oss << "目标范围依赖聚合\n";
    oss << summarize_dependencies_aggregate(index, selection.modules, std::max(10, options.depth * 5)) << "\n";

    oss << "目标代表模块画像\n";
    ModPackage selected;
    selected.name = "selected";
    selected.root_dir = scope.mod_root.empty() ? scope.behavior_pack_root : scope.mod_root;
    selected.modules = selection.modules;
    oss << summarize_module_inventory(index, selected, std::max(8, options.depth * 4));
    oss << "\n";

    oss << "模糊说明\n";
    oss << "- 行为包上下文: " << (scope.behavior_pack_root.empty() ? "未找到" : "已找到") << "\n";
    oss << "- 说明：项目外引用仅提供调用信息，不参与深度反向依赖扩展。\n";
    return oss.str();
}

} // namespace

std::string ProjectAnalyzer::analyze_behavior_pack(const fs::path& behavior_pack_path,
                                                   const AnalysisOptions& options) const {
    auto root = normalize_path(behavior_pack_path);
    if (!path_exists(root) || !check_is_directory(root)) {
        return "无效行为包路径: " + path_to_utf8(root);
    }
    auto index = build_index_for_behavior_pack(root, options);
    return format_behavior_pack_report(index, options);
}

std::string ProjectAnalyzer::analyze_target_references(const fs::path& target_path,
                                                       const AnalysisOptions& options) const {
    auto normalized = normalize_path(target_path);
    if (!path_exists(normalized)) {
        return "无效目标路径: " + path_to_utf8(normalized);
    }

    auto scope = resolve_scope_for_target(normalized, options.max_scope_upward_levels);
    auto index = build_index_for_scope(scope, options);
    auto selection = select_target_modules(index, scope);
    return format_reference_report(index, scope, selection, options);
}

} // namespace mcdk::project_analysis
