#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace mcdk::sapi {

struct SapiImport {
    std::string module;
    std::vector<std::string> symbols;
    int line = 0;
};

struct SapiMember {
    std::string kind;
    std::string name;
    std::string fqname;
    std::string signature;
    std::string doc;
    int line_start = 0;
    int line_end = 0;
};

struct SapiSymbol {
    std::string module;
    std::string kind;
    std::string name;
    std::string fqname;
    std::string signature;
    std::string doc;
    std::string source_file;
    int line_start = 0;
    int line_end = 0;
    std::vector<SapiMember> members;
    std::vector<std::string> type_refs;
};

struct SapiModule {
    std::string name;
    std::string source_file;
    std::vector<SapiImport> imports;
    std::vector<int> symbol_ids;
};

struct SapiIndex {
    std::vector<SapiModule> modules;
    std::vector<SapiSymbol> symbols;
    int files_scanned = 0;
    int parse_error_files = 0;
    int error_nodes = 0;
    int missing_nodes = 0;
    std::vector<std::string> diagnostics;
};

struct SapiSearchResult {
    int symbol_id = -1;
    int member_index = -1;
    double score = 0.0;
    std::string match_kind;
    std::vector<int> referenced_symbols;
};

SapiIndex build_sapi_index(const std::filesystem::path& sapi_root);

std::vector<SapiSearchResult> search_sapi_symbols(const SapiIndex& index,
                                                  const std::string& query,
                                                  int top_k = 8,
                                                  int ref_depth = 1);

std::vector<SapiSearchResult> lookup_sapi_symbol(const SapiIndex& index,
                                                 const std::string& name,
                                                 int ref_depth = 1);

bool save_sapi_cache(const std::filesystem::path& cache_path,
                     const SapiIndex& index,
                     std::string* error = nullptr);

bool load_sapi_cache(const std::filesystem::path& cache_path,
                     SapiIndex& index,
                     std::string* error = nullptr);

std::filesystem::path default_sapi_cache_path(const std::filesystem::path& exe_dir);

} // namespace mcdk::sapi
