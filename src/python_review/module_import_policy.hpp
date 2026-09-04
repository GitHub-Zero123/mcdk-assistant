#pragma once

#include "python_review/review_config.hpp"
#include "python_review/review_types.hpp"

#include <cstddef>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mcdk::python_review::detail {

struct ModuleImportPolicyDefinition {
    std::string_view policy_id;
    std::string_view rule_id;
    bool ReviewConfig::* legacy_enabled;
    bool default_enabled;
    Severity severity;
    double confidence;
    int tier;
    Actionability actionability;
    std::string_view title;
    std::string_view default_description;
    std::string_view suggestion;
    bool suppress_for_project_module;
};

struct ModuleImportPolicyBinding {
    std::string_view module;
    const ModuleImportPolicyDefinition* policy;
    Severity severity;
    std::string_view description;
};

// Immutable after construction. Definitions and all referenced string storage must outlive
// the registry. Bindings are sorted into one contiguous array; the hash table stores only
// (offset, count), avoiding one vector allocation per module.
class ModuleImportPolicyRegistry {
public:
    ModuleImportPolicyRegistry(
        std::span<const ModuleImportPolicyDefinition> policies,
        std::vector<ModuleImportPolicyBinding> bindings);

    std::span<const ModuleImportPolicyBinding> find(std::string_view module) const;
    size_t module_count() const noexcept { return ranges_.size(); }
    size_t binding_count() const noexcept { return bindings_.size(); }

private:
    struct Range {
        size_t offset;
        size_t count;
    };

    std::vector<ModuleImportPolicyBinding> bindings_;
    std::unordered_map<std::string_view, Range> ranges_;
};

const ModuleImportPolicyRegistry& module_import_policy_registry();
size_t minecraft_internal_api_module_count() noexcept;
bool module_import_policy_enabled(const ModuleImportPolicyDefinition& policy,
                                  const ReviewConfig& config) noexcept;

} // namespace mcdk::python_review::detail
