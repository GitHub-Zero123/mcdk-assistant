#include "app/runtime_paths.hpp"
#include "app/server_runtime.hpp"
#include "common/path_utils.hpp"
#include "sapi/sapi_index.hpp"
#include "search/search_service.hpp"

#include <filesystem>
#include <iostream>

int main() {
    mcdk::app::init_console_encoding();

    namespace fs = std::filesystem;

    const auto runtime_paths  = mcdk::app::detect_runtime_paths();
    const auto& dicts_dir     = runtime_paths.dicts_dir;
    const auto& knowledge_dir = runtime_paths.knowledge_dir;
    const auto& cache_path    = runtime_paths.cache_path;
    const auto sapi_cache_path = mcdk::sapi::default_sapi_cache_path(runtime_paths.exe_dir);

    const bool has_dicts     = fs::exists(dicts_dir);
    const bool has_knowledge = fs::exists(knowledge_dir);
    const bool has_cache     = fs::is_regular_file(cache_path);
    const bool has_sapi_cache = fs::is_regular_file(sapi_cache_path);

    std::cerr << "[MCDK-IndexCompiler] dicts: "
              << mcdk::path::to_utf8(dicts_dir)
              << (has_dicts ? "" : " (NOT FOUND)") << std::endl;
    std::cerr << "[MCDK-IndexCompiler] knowledge: "
              << mcdk::path::to_utf8(knowledge_dir)
              << (has_knowledge ? "" : " (NOT FOUND)") << std::endl;
    std::cerr << "[MCDK-IndexCompiler] cache: "
              << mcdk::path::to_utf8(cache_path)
              << (has_cache ? "" : " (NOT FOUND)") << std::endl;
    std::cerr << "[MCDK-IndexCompiler] sapi cache: "
              << mcdk::path::to_utf8(sapi_cache_path)
              << (has_sapi_cache ? "" : " (NOT FOUND)") << std::endl;

    if (!has_dicts) {
        std::cerr << "[MCDK-IndexCompiler] error: dicts directory not found: "
                  << mcdk::path::to_utf8(dicts_dir) << std::endl;
        return 1;
    }

    if (!has_knowledge) {
        std::cerr << "[MCDK-IndexCompiler] error: knowledge directory not found: "
                  << mcdk::path::to_utf8(knowledge_dir) << std::endl;
        return 1;
    }

    try {
        std::cerr << "[MCDK-IndexCompiler] building general knowledge index..." << std::endl;
        mcdk::SearchService search_svc(dicts_dir, knowledge_dir, cache_path, true);
        std::cerr << "[MCDK-IndexCompiler] general docs indexed: "
                  << search_svc.doc_count() << std::endl;
        std::cerr << "[MCDK-IndexCompiler] game assets indexed: "
                  << search_svc.game_assets_count() << std::endl;
        std::cerr << "[MCDK-IndexCompiler] general cache output: "
                  << mcdk::path::to_utf8(cache_path) << std::endl;

        const auto sapi_root = knowledge_dir / "SAPI";
        if (fs::exists(sapi_root)) {
            std::cerr << "[MCDK-IndexCompiler] building SAPI index..." << std::endl;
            auto sapi_index = mcdk::sapi::build_sapi_index(sapi_root);
            std::string error;
            if (!mcdk::sapi::save_sapi_cache(sapi_cache_path, sapi_index, &error)) {
                std::cerr << "[MCDK-IndexCompiler] SAPI cache write failed: "
                          << error << std::endl;
                return 1;
            }
            std::cerr << "[MCDK-IndexCompiler] SAPI files scanned: "
                      << sapi_index.files_scanned << std::endl;
            std::cerr << "[MCDK-IndexCompiler] SAPI modules: "
                      << sapi_index.modules.size() << std::endl;
            std::cerr << "[MCDK-IndexCompiler] SAPI symbols: "
                      << sapi_index.symbols.size() << std::endl;
            std::cerr << "[MCDK-IndexCompiler] SAPI parse error files: "
                      << sapi_index.parse_error_files << std::endl;
            std::cerr << "[MCDK-IndexCompiler] SAPI cache output: "
                      << mcdk::path::to_utf8(sapi_cache_path) << std::endl;
        } else {
            std::cerr << "[MCDK-IndexCompiler] SAPI source not found, skipped: "
                      << mcdk::path::to_utf8(sapi_root) << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "[MCDK-IndexCompiler] build failed: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[MCDK-IndexCompiler] build failed: unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
