#include "search/search_service.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

static void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}

static void write(const fs::path& path, const std::string& text) {
    fs::create_directories(path.parent_path());
    std::ofstream(path, std::ios::binary) << text;
}

static void verify(mcdk::SearchService& svc) {
    auto hits = svc.search_shader_sdk("StandardSurfaceOutput", 5);
    require(!hits.empty(), "ShaderSDK search is empty");
    for (const auto& hit : hits)
        require(hit.fragment->file.rfind("ShaderSDK/", 0) == 0, "cross-category leakage");
    require(svc.search_shader_sdk("渲染龙", 5).size() > 0, "Chinese search is empty");
    auto all = svc.search_all("StandardSurfaceOutput", 10);
    require(std::any_of(all.begin(), all.end(), [](const auto& hit) {
        return hit.fragment->file.rfind("ShaderSDK/", 0) == 0;
    }), "all search excludes ShaderSDK");
    require(svc.doc_count() == 2, "document count includes assets or excludes ShaderSDK");
    auto root = svc.list_cached_files();
    require(std::find(root.dirs.begin(), root.dirs.end(), "ShaderSDK") != root.dirs.end(),
            "root list excludes ShaderSDK");
    require(svc.list_cached_files("ShaderSDK").files.size() == 1, "ShaderSDK list failed");
    auto doc = svc.read_cached_file("ShaderSDK/surface.md");
    require(doc.found && doc.content.find("渲染龙") != std::string::npos, "cached read failed");
    require(!svc.search_game_assets("shader_asset", 1, 5).empty(), "BP section displaced");
    require(!svc.search_game_assets("shader_asset", 2, 5).empty(), "RP section displaced");
}

int main() {
    const auto temp = fs::temp_directory_path() / ("mcdk-shadersdk-test-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        const auto knowledge = temp / "knowledge";
        const auto cache = temp / "index.cache";
        write(knowledge / "ShaderSDK/surface.md", "# StandardSurfaceOutput\n渲染龙 ShaderSDK surface output.\n");
        write(knowledge / "BedrockWiki/surface.md", "# StandardSurfaceOutput\nOther category.\n");
        write(knowledge / "GameAssets/behavior_packs/test/shader_asset.json", "{\"shader_asset\": true}");
        write(knowledge / "GameAssets/resource_packs/test/shader_asset.json", "{\"shader_asset\": true}");
        {
            mcdk::SearchService svc(MCDK_DICTS_DIR, knowledge, cache, true);
            verify(svc);
        }
        {
            mcdk::SearchService svc(MCDK_DICTS_DIR, knowledge, cache);
            verify(svc);
        }
        fs::remove_all(knowledge);
        {
            mcdk::SearchService svc(MCDK_DICTS_DIR, cache, true);
            // list/read must work even before the first search, without source files.
            require(svc.list_cached_files("ShaderSDK").found, "lazy list failed");
            require(svc.read_cached_file("ShaderSDK/surface.md").found, "lazy read failed");
            verify(svc);
        }
        fs::remove_all(temp);
        std::cout << "ShaderSDK search/cache tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        fs::remove_all(temp);
        std::cerr << e.what() << '\n';
        return 1;
    }
}
