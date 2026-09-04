#include "python_review/module_import_policy.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

namespace mcdk::python_review::detail {
namespace {

// Extracted from ModSDK 3.9.101.294845_vanilla/minecraft: the 66 Python module
// names and their imported roots, excluding ordinary standard-library modules.
// Keep sorted so reviews and changes to the generated source list remain auditable.
constexpr std::string_view kMinecraftInternalApiModules[] = {
    "MUI",
    "PackListType",
    "QSecImp",
    "_audio",
    "_block",
    "_camera",
    "_ccvoice",
    "_chacha",
    "_clientlevel",
    "_common_level",
    "_crashhelper",
    "_debug",
    "_device",
    "_dns",
    "_downloader",
    "_effect",
    "_entitymodule",
    "_eventeditormodule",
    "_fakeworld",
    "_game_ruler",
    "_gui",
    "_http",
    "_httpBeast",
    "_item",
    "_log",
    "_mcmathunderly",
    "_minecraft",
    "_mobEffect",
    "_modelmodule",
    "_network",
    "_notify",
    "_paperdoll",
    "_particle",
    "_particle_system",
    "_physx",
    "_player",
    "_postprocess",
    "_proxy",
    "_qsechelper",
    "_recipe",
    "_record",
    "_resource",
    "_resource_server",
    "_rnglmodule",
    "_rnmodule",
    "_scanner",
    "_sensor_module",
    "_server_proxy",
    "_serverblock",
    "_serverenderdragonmodule",
    "_serverentitymodule",
    "_serverlevel",
    "_servermodelmodule",
    "_setting",
    "_sfx",
    "_storge",
    "_ui_editor",
    "_utility",
    "_world",
    "_zipmgr",
    "application",
    "audio",
    "block",
    "camera",
    "ccvoice",
    "chacha",
    "client",
    "client_physx",
    "clienthttp",
    "clientlevel",
    "common_level",
    "crashhelper",
    "debug",
    "device",
    "dns",
    "downloader",
    "effect",
    "entity_module",
    "enum_define",
    "event_editor_module",
    "fakeworld",
    "game_ruler",
    "gui",
    "gui_2d",
    "http_util",
    "init",
    "init_new",
    "item",
    "lan_scanner",
    "launcher",
    "localplayermodule",
    "log_mgr",
    "logout",
    "main_window_game_ctrl",
    "mc_game_ctrl",
    "mcmathunderly",
    "mcp_log",
    "mcp_mod_log",
    "minecraft",
    "minecraftEnum",
    "mobEffect",
    "mobile_logger",
    "mod_log",
    "model",
    "nbt",
    "neteaseHttp",
    "netgame_api",
    "network",
    "network_proxy",
    "notify",
    "paperdoll",
    "particle",
    "particle_system",
    "petUtilsGac",
    "player",
    "postprocess",
    "protocol",
    "proxy_module",
    "pyaes",
    "python_decorator",
    "python_tools",
    "react_native",
    "recipe",
    "record",
    "record_encrypt",
    "record_encrypt_mgr",
    "resource",
    "resource_management",
    "rngl",
    "sensor",
    "server_ender_dragon_module",
    "server_entity_module",
    "server_physx",
    "server_proxy_module",
    "server_resource",
    "serverblock",
    "serverhttp",
    "serverlevel",
    "servermodel",
    "setting",
    "sfx",
    "singleton",
    "storge",
    "tan_lobby_ctrl",
    "ui_editor",
    "ui_message",
    "utility",
    "webview",
    "world",
    "zipmgr",
};

constexpr ModuleImportPolicyDefinition kPolicies[] = {
    {
        "platform-security",
        "platform.restricted-module-import",
        &ReviewConfig::rule_restricted_module_import,
        true,
        Severity::Warning,
        0.99,
        1,
        Actionability::ShouldFix,
        "导入了违反平台安全规则的模块",
        "该模块违反线上平台安全规则",
        "仅可在本地自测代码中使用；线上产品应移除该模块或改用 ModSDK 安全 API。",
        false,
    },
    {
        "minecraft-internal-api",
        "platform.internal-api-import",
        &ReviewConfig::rule_internal_api_import,
        true,
        Severity::Warning,
        0.90,
        1,
        Actionability::ShouldFix,
        "导入了 Minecraft 内部 API",
        "内部 API，不承诺稳定性，不推荐使用",
        "请改用稳定的 ModSDK 公开 API；Minecraft 内部 API 不承诺稳定性，不推荐使用。",
        true,
    },
};

struct NamedModule {
    std::string_view name;
    std::string_view description;
};

constexpr NamedModule kPlatformSecurityModules[] = {
    {"os",          "访问操作系统与文件系统"},
    {"sys",         "访问解释器运行时与导入状态"},
    {"__builtin__", "访问 __import__、eval、execfile 等解释器内建能力"},
    {"importlib",   "动态导入模块"},
    {"imp",         "动态查找和加载模块（Py2 旧接口）"},
    {"runpy",       "按模块名或路径执行 Python 代码"},
    {"zipimport",   "从归档文件动态加载 Python 代码"},
    {"subprocess",  "启动和控制外部进程"},
    {"commands",    "执行系统命令（Py2 旧接口）"},
    {"popen2",      "启动外部进程（Py2 旧接口）"},
    {"ctypes",      "调用本机动态库和原生内存"},
    {"socket",      "绕过 ModSDK 网络接口建立原始连接"},
};

std::vector<ModuleImportPolicyBinding> make_bindings() {
    std::vector<ModuleImportPolicyBinding> bindings;
    bindings.reserve(std::size(kMinecraftInternalApiModules) +
                     std::size(kPlatformSecurityModules));

    for (const std::string_view module : kMinecraftInternalApiModules) {
        bindings.push_back({module, &kPolicies[1], kPolicies[1].severity,
                            kPolicies[1].default_description});
    }
    for (const NamedModule& module : kPlatformSecurityModules) {
        bindings.push_back({module.name, &kPolicies[0], kPolicies[0].severity,
                            module.description});
    }
    return bindings;
}

} // namespace

ModuleImportPolicyRegistry::ModuleImportPolicyRegistry(
    std::span<const ModuleImportPolicyDefinition> policies,
    std::vector<ModuleImportPolicyBinding> bindings)
    : bindings_(std::move(bindings)) {
    for (size_t i = 0; i < policies.size(); ++i) {
        const ModuleImportPolicyDefinition& policy = policies[i];
        if (policy.policy_id.empty() || policy.rule_id.empty() || policy.title.empty() ||
            policy.default_description.empty() || policy.suggestion.empty() ||
            policy.confidence < 0.0 || policy.confidence > 1.0 || policy.tier < 1) {
            throw std::invalid_argument("module import policy has incomplete identity/configuration");
        }
        for (size_t j = 0; j < i; ++j) {
            if (policies[j].policy_id == policy.policy_id)
                throw std::invalid_argument("duplicate module import policy_id: " +
                                            std::string(policy.policy_id));
            if (policies[j].rule_id == policy.rule_id)
                throw std::invalid_argument("duplicate module import rule_id: " +
                                            std::string(policy.rule_id));
        }
    }

    for (ModuleImportPolicyBinding& binding : bindings_) {
        if (binding.module.empty() || binding.module.find('.') != std::string_view::npos ||
            !binding.policy) {
            throw std::invalid_argument("module import policy binding is incomplete");
        }
        const bool known_policy = std::any_of(
            policies.begin(), policies.end(), [&](const ModuleImportPolicyDefinition& policy) {
                return &policy == binding.policy;
            });
        if (!known_policy)
            throw std::invalid_argument("module import policy binding references an unknown policy");
        if (binding.description.empty())
            binding.description = binding.policy->default_description;
    }

    std::sort(bindings_.begin(), bindings_.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.module != rhs.module) return lhs.module < rhs.module;
        return lhs.policy->policy_id < rhs.policy->policy_id;
    });
    for (size_t i = 1; i < bindings_.size(); ++i) {
        if (bindings_[i - 1].module == bindings_[i].module &&
            bindings_[i - 1].policy == bindings_[i].policy) {
            throw std::invalid_argument("duplicate module/policy binding: " +
                                        std::string(bindings_[i].module));
        }
    }

    ranges_.reserve(bindings_.size());
    for (size_t begin = 0; begin < bindings_.size();) {
        size_t end = begin + 1;
        while (end < bindings_.size() && bindings_[end].module == bindings_[begin].module)
            ++end;
        ranges_.emplace(bindings_[begin].module, Range{begin, end - begin});
        begin = end;
    }
}

std::span<const ModuleImportPolicyBinding>
ModuleImportPolicyRegistry::find(std::string_view module) const {
    const size_t dot = module.find('.');
    const std::string_view root = module.substr(0, dot);
    const auto found = ranges_.find(root);
    if (found == ranges_.end()) return {};
    return {bindings_.data() + found->second.offset, found->second.count};
}

const ModuleImportPolicyRegistry& module_import_policy_registry() {
    static const ModuleImportPolicyRegistry registry{kPolicies, make_bindings()};
    return registry;
}

size_t minecraft_internal_api_module_count() noexcept {
    return std::size(kMinecraftInternalApiModules);
}

bool module_import_policy_enabled(const ModuleImportPolicyDefinition& policy,
                                  const ReviewConfig& config) noexcept {
    for (const auto& [policy_id, enabled] : config.module_import_policy_overrides) {
        if (policy_id == policy.policy_id) return enabled;
    }
    return policy.legacy_enabled ? config.*(policy.legacy_enabled) : policy.default_enabled;
}

} // namespace mcdk::python_review::detail
