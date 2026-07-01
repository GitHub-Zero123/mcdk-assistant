#include "app/runtime_paths.hpp"
#include "app/server_runtime.hpp"
#include "common/path_utils.hpp"
#include "sapi/sapi_index.hpp"
#include "search/search_service.hpp"

#ifdef MCDK_WITH_SOLUTIONS
#include <mcdk_solutions/solution_index.hpp>
#endif

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

#ifdef MCDK_WITH_SOLUTIONS
        // 解决方案层：扫描内容目录 → 编译高性能 bin，落在 exe 相邻目录（与 SAPI 缓存并列）。
        fs::path sol_content;
#ifdef MCDK_SOLUTIONS_CONTENT_DIR
        sol_content = fs::u8path(MCDK_SOLUTIONS_CONTENT_DIR);
#endif
        if (sol_content.empty() || !fs::exists(sol_content))
            sol_content = runtime_paths.exe_dir / "solutions";

        const auto sol_cache_path = mcdk::solutions::default_solution_cache_path(runtime_paths.exe_dir);
        if (fs::exists(sol_content)) {
            std::cerr << "[MCDK-IndexCompiler] building solutions index from: "
                      << mcdk::path::to_utf8(sol_content) << std::endl;
            auto sol_index = mcdk::solutions::build_solution_index(sol_content);
            std::string sol_error;
            if (!mcdk::solutions::save_solution_cache(sol_cache_path, sol_index, &sol_error)) {
                std::cerr << "[MCDK-IndexCompiler] solutions cache write failed: "
                          << sol_error << std::endl;
                return 1;
            }
            std::cerr << "[MCDK-IndexCompiler] solutions indexed: " << sol_index.solutions.size()
                      << " (dirs scanned " << sol_index.dirs_scanned
                      << ", parse errors " << sol_index.parse_errors << ")" << std::endl;
            std::cerr << "[MCDK-IndexCompiler] solutions cache output: "
                      << mcdk::path::to_utf8(sol_cache_path) << std::endl;
            for (const auto& d : sol_index.diagnostics)
                std::cerr << "[MCDK-IndexCompiler]   solutions diag: " << d << std::endl;
        } else {
            std::cerr << "[MCDK-IndexCompiler] solutions content not found, skipped: "
                      << mcdk::path::to_utf8(sol_content) << std::endl;
        }
#endif
    } catch (const std::exception& ex) {
        std::cerr << "[MCDK-IndexCompiler] build failed: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[MCDK-IndexCompiler] build failed: unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
