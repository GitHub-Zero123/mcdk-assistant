#include "python_review/review_config.hpp"

#include "common/path_utils.hpp"

// toml++ 为头文件库（header-only），仅本 TU 引入，不污染热点分析器 TU。
// 关掉 toml++ 内部异常：让 toml::parse 返回可检查的 parse_result（operator bool / .error()），
// 用错误返回而非抛异常（与项目 /EHsc 无关，纯 API 选择）。
#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

#include <fstream>

namespace mcdk::python_review {
namespace fs = std::filesystem;

namespace {

// 读文件为字符串（UTF-8 安全，去 BOM）。Windows 上经 from_utf8 转宽路径打开，
// 避免中文路径被 toml::parse_file 按 ANSI 处理而失败。found 输出文件是否存在可读。
std::string read_config_file(const fs::path& p, bool& found) {
    found = false;
    std::error_code ec;
    if (!fs::is_regular_file(p, ec)) return {};
    std::ifstream ifs(p, std::ios::binary);
    if (!ifs.is_open()) return {};
    found = true;
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3);
    }
    return text;
}

// 把一份 TOML 文本合并进 cfg（缺省键保留原值）。返回是否解析成功。
bool apply_toml(const std::string& content, ReviewConfig& cfg, std::string& err) {
    toml::parse_result res = toml::parse(content);
    if (!res) {
        err = std::string(res.error().description());
        return false;
    }
    const toml::table& t = res.table();

    auto geti = [&](const char* sec, const char* key, int def) -> int {
        return static_cast<int>(t[sec][key].value_or(static_cast<int64_t>(def)));
    };
    auto getb = [&](const char* sec, const char* key, bool def) -> bool {
        return t[sec][key].value_or(def);
    };

    cfg.file_code_lines = geti("thresholds", "file_code_lines", cfg.file_code_lines);
    cfg.file_units      = geti("thresholds", "file_units",      cfg.file_units);
    cfg.func_loc        = geti("thresholds", "func_loc",        cfg.func_loc);
    cfg.func_loc_high   = geti("thresholds", "func_loc_high",   cfg.func_loc_high);
    cfg.try_small       = geti("thresholds", "try_small",       cfg.try_small);
    cfg.try_large       = geti("thresholds", "try_large",       cfg.try_large);
    cfg.dup_min_stmts   = geti("thresholds", "dup_min_stmts",   cfg.dup_min_stmts);
    cfg.dup_min_fp_len  = geti("thresholds", "dup_min_fp_len",  cfg.dup_min_fp_len);
    cfg.dup_min_count   = geti("thresholds", "dup_min_count",   cfg.dup_min_count);
    cfg.shallow_min_body= geti("thresholds", "shallow_min_body",cfg.shallow_min_body);
    cfg.max_params      = geti("thresholds", "max_params",      cfg.max_params);

    cfg.rule_try_masking        = getb("rules", "try_masking",        cfg.rule_try_masking);
    cfg.rule_mutable_default    = getb("rules", "mutable_default",    cfg.rule_mutable_default);
    cfg.rule_global_write       = getb("rules", "global_write",       cfg.rule_global_write);
    cfg.rule_stub_placeholder   = getb("rules", "stub_placeholder",   cfg.rule_stub_placeholder);
    cfg.rule_shallow_impl       = getb("rules", "shallow_impl",       cfg.rule_shallow_impl);
    cfg.rule_large_function     = getb("rules", "large_function",     cfg.rule_large_function);
    cfg.rule_large_file         = getb("rules", "large_file",         cfg.rule_large_file);
    cfg.rule_duplicate_function = getb("rules", "duplicate_function", cfg.rule_duplicate_function);
    cfg.rule_unowned_todo       = getb("rules", "unowned_todo",       cfg.rule_unowned_todo);
    cfg.rule_too_many_params    = getb("rules", "too_many_params",    cfg.rule_too_many_params);

    cfg.max_findings_per_rule = geti("output", "max_findings_per_rule", cfg.max_findings_per_rule);

    if (auto arr = t["advanced"]["extra_caller_supplied_params"].as_array()) {
        for (auto&& elem : *arr)
            if (auto s = elem.value<std::string>()) cfg.extra_caller_supplied_params.push_back(*s);
    }
    return true;
}

// 夹取到合理范围，防止用户填出退化/危险值（如 0 行阈值把整仓刷屏）。
void clamp_config(ReviewConfig& cfg) {
    auto lo = [](int& v, int min) { if (v < min) v = min; };
    lo(cfg.file_code_lines, 50);
    lo(cfg.file_units, 3);
    lo(cfg.func_loc, 10);
    lo(cfg.func_loc_high, cfg.func_loc);
    lo(cfg.try_small, 1);
    lo(cfg.try_large, cfg.try_small);
    lo(cfg.dup_min_stmts, 1);
    lo(cfg.dup_min_fp_len, 2);
    lo(cfg.dup_min_count, 2);
    lo(cfg.shallow_min_body, 1);
    lo(cfg.max_params, 1);
    if (cfg.max_findings_per_rule < 0) cfg.max_findings_per_rule = 0; // 0 = 不限
}

} // namespace

ReviewConfig resolve_config(const fs::path& behavior_pack_root,
                            const std::string& explicit_path,
                            std::vector<std::string>& warnings,
                            std::string& source_desc) {
    ReviewConfig cfg;
    source_desc = "内置默认值";

    // 1) 显式 --config。
    if (!explicit_path.empty()) {
        fs::path p = mcdk::path::from_utf8(explicit_path);
        bool found = false;
        std::string content = read_config_file(p, found);
        if (!found) {
            warnings.push_back("指定的 --config 不存在或不可读: " + explicit_path + "（回退默认配置）");
            return cfg;
        }
        std::string err;
        if (!apply_toml(content, cfg, err)) {
            warnings.push_back("配置解析失败: " + explicit_path + " — " + err + "（回退默认配置）");
            cfg = ReviewConfig{};
            return cfg;
        }
        clamp_config(cfg);
        source_desc = explicit_path;
        return cfg;
    }

    // 2) 行为包根下自动发现。
    for (const char* name : {"mcdk-review.toml", ".mcdk-review.toml"}) {
        fs::path p = behavior_pack_root / name;
        bool found = false;
        std::string content = read_config_file(p, found);
        if (!found) continue;
        std::string err;
        if (!apply_toml(content, cfg, err)) {
            warnings.push_back(std::string("配置解析失败: ") + name + " — " + err + "（回退默认配置）");
            cfg = ReviewConfig{};
            return cfg;
        }
        clamp_config(cfg);
        source_desc = std::string(name) + "（行为包根自动发现）";
        return cfg;
    }

    // 3) 默认值。
    return cfg;
}

std::string default_toml_template() {
    return R"(# mcdk-review 配置模板（TOML）。放在行为包根目录并命名为 mcdk-review.toml 即自动生效，
# 或用 review --config <path> 显式指定。所有键都可省略，省略即用内置默认值。

[thresholds]
file_code_lines  = 800   # 单文件"代码行"屎山阈值（需与 file_units 同时满足才报）
file_units       = 25    # 单文件 def+class 总数阈值（多重判定，避免误伤纯数据/配置文件）
func_loc         = 50    # 单函数行跨度：偏长起报（risk）
func_loc_high    = 100   # 单函数行跨度：过长（warning）
try_small        = 5     # try 体 <= 此值：窄防御，低级（hint）
try_large        = 20    # try 体 >= 此值：疑似吞掉大量业务，高级（warning）
dup_min_stmts    = 5     # 跨模块重复：单函数最少语句数（过滤 getter/空壳）
dup_min_fp_len   = 10    # 跨模块重复：结构指纹最短长度（过短易巧合）
dup_min_count    = 2     # 跨模块重复：最少重复处数
shallow_min_body = 3     # 假实现：函数体至少 N 条业务语句才判（排除 py2 裸写补全库的 1 行 return 固定值）
max_params       = 5     # 参数过多：非 self 参数超过此值触发（__init__/__new__ 豁免）

[rules]                  # 整族开关：false 可禁用对应规则
try_masking        = true
mutable_default    = true
global_write       = true
stub_placeholder   = true
shallow_impl       = true
large_function     = true
large_file         = true
duplicate_function = true
unowned_todo       = true
too_many_params    = true

[output]
max_findings_per_rule = 20  # 每条规则最多展示 N 处定位，超出仅计数；0 = 不限（控制 MCP 召回体积、避免撑爆上下文）

[advanced]
# 追加"调用方/引擎提供"参数名：这些可变默认参数（def f(x={})）不视为跨调用共享 bug。
# 内置已含 args/data/event/kwargs/params/context 等，此处仅追加项目自定义名。
extra_caller_supplied_params = []
)";
}

} // namespace mcdk::python_review
