#pragma once

#include "search/bm25.h"
#include <cppjieba/Jieba.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

namespace mcdk {

// 文档分类（基于 ModAPI/ 下的子目录）
enum class DocCategory { Unknown, API, Event, Enum, Beta };

class SearchService {
public:
    SearchService(const std::string& dicts_dir, const std::string& knowledge_dir)
        : jieba_(
            dicts_dir + "/jieba.dict.utf8",
            dicts_dir + "/hmm_model.utf8",
            dicts_dir + "/user.dict.utf8",
            dicts_dir + "/idf.utf8",
            dicts_dir + "/stop_words.utf8"
          )
        , knowledge_dir_(knowledge_dir)
    {
        load_stop_words(dicts_dir + "/stop_words.utf8");
        load_knowledge();
        build_indices();
    }

    std::vector<SearchResult> search_api(const std::string& keyword, int top_k = -1) const {
        return search_category(api_index_, keyword, top_k);
    }

    std::vector<SearchResult> search_event(const std::string& keyword, int top_k = -1) const {
        return search_category(event_index_, keyword, top_k);
    }

    std::vector<SearchResult> search_enum(const std::string& keyword, int top_k = -1) const {
        return search_category(enum_index_, keyword, top_k);
    }

    std::vector<SearchResult> search_all(const std::string& keyword, int top_k = -1) const {
        auto a = search_category(api_index_, keyword, -1);
        auto b = search_category(event_index_, keyword, -1);
        auto c = search_category(enum_index_, keyword, -1);
        a.insert(a.end(), b.begin(), b.end());
        a.insert(a.end(), c.begin(), c.end());
        std::sort(a.begin(), a.end(), [](const SearchResult& x, const SearchResult& y) {
            return x.score > y.score;
        });
        if (top_k > 0 && static_cast<size_t>(top_k) < a.size()) a.resize(top_k);
        return a;
    }

    size_t doc_count() const {
        return api_index_.engine.doc_count() + event_index_.engine.doc_count() + enum_index_.engine.doc_count();
    }

private:
    struct CategoryIndex {
        std::vector<DocFragment>                 fragments;
        std::vector<std::vector<std::string>>    tokenized_docs;
        BM25Engine                               engine;
    };

    cppjieba::Jieba                jieba_;
    std::string                    knowledge_dir_;
    std::unordered_set<std::string> stop_words_;
    CategoryIndex                  api_index_;
    CategoryIndex                  event_index_;
    CategoryIndex                  enum_index_;

    std::vector<SearchResult> search_category(const CategoryIndex& idx, const std::string& keyword, int top_k) const {
        std::vector<std::string> query_tokens;
        tokenize(keyword, query_tokens);
        return idx.engine.search(query_tokens, top_k);
    }

    static DocCategory classify_path(const std::string& rel_path) {
        if (rel_path.find("/接口/") != std::string::npos || rel_path.find("接口/") == 0)
            return DocCategory::API;
        if (rel_path.find("/事件/") != std::string::npos || rel_path.find("事件/") == 0)
            return DocCategory::Event;
        if (rel_path.find("/枚举值/") != std::string::npos || rel_path.find("枚举值/") == 0)
            return DocCategory::Enum;
        if (rel_path.find("/beta/") != std::string::npos || rel_path.find("beta/") == 0)
            return DocCategory::Beta;
        return DocCategory::Unknown;
    }

    // 返回 nullptr 表示该分类不纳入索引
    CategoryIndex* index_for(DocCategory cat) {
        switch (cat) {
        case DocCategory::API:   return &api_index_;
        case DocCategory::Event: return &event_index_;
        case DocCategory::Enum:  return &enum_index_;
        default:                 return nullptr;
        }
    }

    void load_stop_words(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return;
        std::string line;
        while (std::getline(ifs, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (!line.empty()) stop_words_.insert(line);
        }
    }

    void tokenize(const std::string& text, std::vector<std::string>& tokens) const {
        std::vector<std::string> raw;
        jieba_.CutForSearch(text, raw);
        tokens.clear();
        for (auto& w : raw) {
            if (w.empty() || w == " " || w == "\t" || w == "\n") continue;
            if (stop_words_.count(w)) continue;
            tokens.push_back(std::move(w));
        }
    }

    static std::string path_to_utf8(const std::filesystem::path& p) {
        auto u8 = p.generic_u8string();
        return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
    }

    void load_knowledge() {
        namespace fs = std::filesystem;
        if (!fs::exists(knowledge_dir_)) {
            std::cerr << "[mcdk] knowledge dir not found: " << knowledge_dir_ << std::endl;
            return;
        }

        for (const auto& entry : fs::recursive_directory_iterator(knowledge_dir_)) {
            if (!entry.is_regular_file()) continue;
            auto ext = entry.path().extension().string();
            if (ext != ".md" && ext != ".MD") continue;

            std::string rel_path = path_to_utf8(fs::relative(entry.path(), knowledge_dir_));
            load_markdown_file(entry.path(), rel_path);
        }

        std::cout << "[mcdk] loaded " << doc_count() << " fragments from " << knowledge_dir_ << std::endl;
    }

    void load_markdown_file(const std::filesystem::path& abs_path, const std::string& rel_path) {
        std::ifstream ifs(abs_path);
        if (!ifs.is_open()) return;

        DocCategory cat = classify_path(rel_path);
        CategoryIndex* idx = index_for(cat);
        if (!idx) return; // Unknown/Beta 不纳入索引

        std::string line;
        std::ostringstream current_content;
        int fragment_start = 1;
        int line_num = 0;
        bool has_content = false;

        auto flush_fragment = [&]() {
            std::string content = current_content.str();
            if (!content.empty() && has_content) {
                idx->fragments.push_back({std::move(content), rel_path, fragment_start, line_num});
            }
            current_content.str("");
            current_content.clear();
            has_content = false;
            fragment_start = line_num + 1;
        };

        while (std::getline(ifs, line)) {
            ++line_num;
            if (!line.empty() && line.back() == '\r') line.pop_back();

            if (line.size() >= 2 && line[0] == '#') {
                size_t level = 0;
                while (level < line.size() && line[level] == '#') ++level;
                if (level >= 2 && level <= 4) {
                    flush_fragment();
                    fragment_start = line_num;
                }
            }

            current_content << line << "\n";
            if (!line.empty()) has_content = true;
        }

        flush_fragment();
    }

    void build_indices() {
        auto build_one = [this](CategoryIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i) {
                tokenize(idx.fragments[i].content, idx.tokenized_docs[i]);
            }
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[mcdk] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };
        build_one(api_index_,   "API");
        build_one(event_index_, "Event");
        build_one(enum_index_,  "Enum");
    }
};

} // namespace mcdk
