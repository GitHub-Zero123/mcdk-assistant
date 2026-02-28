#include "search/search_service.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#ifdef _WIN32
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

// 搜索分发：根据前缀选择对应方法
static std::vector<mcdk::SearchResult> dispatch_search(mcdk::SearchService& svc, const std::string& input, std::string& query) {
    struct Prefix { const char* tag; int kind; };
    Prefix prefixes[] = {{"api:", 0}, {"event:", 1}, {"enum:", 2}, {"all:", 3}};

    for (auto& p : prefixes) {
        size_t len = std::strlen(p.tag);
        if (input.size() > len && input.substr(0, len) == p.tag) {
            query = input.substr(len);
            switch (p.kind) {
            case 0: return svc.search_api(query);
            case 1: return svc.search_event(query);
            case 2: return svc.search_enum(query);
            case 3: return svc.search_all(query);
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
    std::cout << "[test] commands: 'api:<query>' 'event:<query>' 'enum:<query>' 'all:<query>' (default: all)\n";
    std::cout << "[test] enter keywords (empty to page/quit):\n" << std::endl;

    std::vector<mcdk::SearchResult> results;
    size_t page_offset = 0;
    constexpr size_t PAGE_SIZE = 5;

    auto show_page = [&]() {
        size_t end = page_offset + PAGE_SIZE;
        if (end > results.size()) end = results.size();
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
    };

    std::string line;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) {
            if (page_offset < results.size()) { show_page(); continue; }
            if (!results.empty()) { results.clear(); page_offset = 0; continue; }
            break;
        }

        std::string query;
        results = dispatch_search(svc, line, query);
        page_offset = 0;
        if (results.empty()) { std::cout << "(no results)\n" << std::endl; continue; }
        std::cout << "--- " << results.size() << " results ---\n";
        show_page();
    }
    return 0;
}
