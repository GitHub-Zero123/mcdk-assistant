#include "search/search_service.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static std::string truncate_content(const std::string& content, int max_lines = 20, int head = 12, int tail = 6) {
    std::istringstream iss(content);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(iss, line)) lines.push_back(line);

    if (static_cast<int>(lines.size()) <= max_lines) return content;

    std::ostringstream oss;
    for (int i = 0; i < head; ++i) oss << lines[i] << "\n";
    oss << "    ... (" << (lines.size() - head - tail) << " lines omitted) ...\n";
    for (int i = static_cast<int>(lines.size()) - tail; i < static_cast<int>(lines.size()); ++i)
        oss << lines[i] << "\n";
    return oss.str();
}

// 检查是否 assets 搜索前缀，返回 scope (-1 表示不是 assets 搜索)
static int check_assets_prefix(const std::string& input, std::string& query) {
    struct AssetPrefix { const char* tag; int scope; };
    AssetPrefix ap[] = {{"assets_bp:", 1}, {"assets_rp:", 2}, {"assets:", 0}};
    for (auto& p : ap) {
        size_t len = std::strlen(p.tag);
        if (input.size() > len && input.substr(0, len) == p.tag) {
            query = input.substr(len);
            return p.scope;
        }
    }
    return -1;
}

// 搜索分发：根据前缀选择对应方法
static std::vector<mcdk::SearchResult> dispatch_search(mcdk::SearchService& svc, const std::string& input, std::string& query) {
    struct Prefix { const char* tag; int kind; };
    Prefix prefixes[] = {{"api:", 0}, {"event:", 1}, {"enum:", 2}, {"all:", 3}, {"wiki:", 4}, {"qumod:", 5}, {"guide:", 6}};

    for (auto& p : prefixes) {
        size_t len = std::strlen(p.tag);
        if (input.size() > len && input.substr(0, len) == p.tag) {
            query = input.substr(len);
            switch (p.kind) {
            case 0: return svc.search_api(query);
            case 1: return svc.search_event(query);
            case 2: return svc.search_enum(query);
            case 3: return svc.search_all(query);
            case 4: return svc.search_wiki(query);
            case 5: return svc.search_qumod(query);
            case 6: return svc.search_netease_guide(query);
            }
        }
    }
    query = input;
    return svc.search_all(query); // 默认全局搜索
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::cout << "[test] loading knowledge base..." << std::endl;
    mcdk::SearchService svc(MCDK_DICTS_DIR, MCDK_KNOWLEDGE_DIR);
    std::cout << "[test] indexed " << svc.doc_count() << " fragments\n";
    std::cout << "[test] GameAssets: " << svc.game_assets_count() << " files\n";
    std::cout << "[test] commands:\n";
    std::cout << "  api:<q>  event:<q>  enum:<q>  wiki:<q>  qumod:<q>  guide:<q>  all:<q>\n";
    std::cout << "  assets:<q>  assets_bp:<q>  assets_rp:<q>  (default: all)\n";
    std::cout << "[test] enter keywords (empty to page/quit):\n" << std::endl;

    // 普通搜索结果
    std::vector<mcdk::SearchResult> results;
    // 资产搜索结果
    std::vector<mcdk::SearchService::AssetResult> asset_results;
    bool is_asset_mode = false;
    size_t page_offset = 0;
    constexpr size_t PAGE_SIZE = 5;

    auto show_page = [&]() {
        if (is_asset_mode) {
            size_t end = std::min(page_offset + PAGE_SIZE, asset_results.size());
            for (size_t i = page_offset; i < end; ++i) {
                const auto& r = asset_results[i];
                std::cout << "[" << i + 1 << "/" << asset_results.size() << "] score=" << r.score
                          << "  file=" << r.rel_path << "\n"
                          << truncate_content(r.snippet) << "\n";
            }
            page_offset = end;
            if (page_offset < asset_results.size())
                std::cout << "--- Enter for next " << PAGE_SIZE << ", or type new query ---" << std::endl;
            else
                std::cout << "--- end ---\n" << std::endl;
        } else {
            size_t end = std::min(page_offset + PAGE_SIZE, results.size());
            for (size_t i = page_offset; i < end; ++i) {
                const auto& r = results[i];
                std::cout << "[" << i + 1 << "/" << results.size() << "] score=" << r.score
                          << "  file=" << r.fragment->file
                          << "  lines=" << r.fragment->line_start << "-" << r.fragment->line_end
                          << "\n" << truncate_content(r.fragment->content) << "\n";
            }
            page_offset = end;
            if (page_offset < results.size())
                std::cout << "--- Enter for next " << PAGE_SIZE << ", or type new query ---" << std::endl;
            else
                std::cout << "--- end ---\n" << std::endl;
        }
    };

    std::string line;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) {
            if (is_asset_mode && page_offset < asset_results.size()) { show_page(); continue; }
            if (!is_asset_mode && page_offset < results.size()) { show_page(); continue; }
            if (!results.empty() || !asset_results.empty()) {
                results.clear(); asset_results.clear(); page_offset = 0; continue;
            }
            break;
        }

        std::string query;
        int asset_scope = check_assets_prefix(line, query);
        page_offset = 0;

        if (asset_scope >= 0) {
            // 资产搜索模式
            is_asset_mode = true;
            results.clear();
            asset_results = svc.search_game_assets(query, asset_scope);
            if (asset_results.empty()) { std::cout << "(no results)\n" << std::endl; continue; }
            std::cout << "--- " << asset_results.size() << " asset results ---\n";
        } else {
            // 普通搜索模式
            is_asset_mode = false;
            asset_results.clear();
            results = dispatch_search(svc, line, query);
            if (results.empty()) { std::cout << "(no results)\n" << std::endl; continue; }
            std::cout << "--- " << results.size() << " results ---\n";
        }
        show_page();
    }
    return 0;
}
