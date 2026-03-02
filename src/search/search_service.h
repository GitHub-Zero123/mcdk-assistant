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
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace mcdk {

// 文档分类（基于 ModAPI/ 下的子目录）
enum class DocCategory { Unknown, API, Event, Enum, Beta, Wiki, QuMod, NeteaseGuide };

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

    std::vector<SearchResult> search_netease_guide(const std::string& keyword, int top_k = -1) const {
        return search_category(netease_guide_index_, keyword, top_k);
    }

    std::vector<SearchResult> search_all(const std::string& keyword, int top_k = -1) const {
        auto a = search_category(api_index_, keyword, -1);
        auto b = search_category(event_index_, keyword, -1);
        auto c = search_category(enum_index_, keyword, -1);
        auto d = search_category_en(wiki_index_, keyword, -1);
        auto e = search_category(qumod_index_, keyword, -1);
        auto f = search_category(netease_guide_index_, keyword, -1);
        a.insert(a.end(), b.begin(), b.end());
        a.insert(a.end(), c.begin(), c.end());
        a.insert(a.end(), d.begin(), d.end());
        a.insert(a.end(), e.begin(), e.end());
        a.insert(a.end(), f.begin(), f.end());
        std::sort(a.begin(), a.end(), [](const SearchResult& x, const SearchResult& y) {
            return x.score > y.score;
        });
        if (top_k > 0 && static_cast<size_t>(top_k) < a.size()) a.resize(top_k);
        return a;
    }

    std::vector<SearchResult> search_wiki(const std::string& keyword, int top_k = -1) const {
        return search_category_en(wiki_index_, keyword, top_k);
    }

    std::vector<SearchResult> search_qumod(const std::string& keyword, int top_k = -1) const {
        return search_category(qumod_index_, keyword, top_k);
    }

    size_t doc_count() const {
        return api_index_.engine.doc_count() + event_index_.engine.doc_count()
             + enum_index_.engine.doc_count() + wiki_index_.engine.doc_count()
             + qumod_index_.engine.doc_count() + netease_guide_index_.engine.doc_count();
    }

    // GameAssets 搜索结果条目
    struct AssetResult {
        std::string rel_path;   // 相对于 knowledge/GameAssets/ 的路径
        std::string snippet;    // 文件内容片段（命中内容行附近）
        double      score;      // 综合得分（路径匹配 + 内容 BM25）
    };

    // scope: 0=全部, 1=仅行为包, 2=仅资源包
    // 同时按路径名模糊匹配 + 文件内容 BM25 搜索，合并得分后返回
    std::vector<AssetResult> search_game_assets(const std::string& keyword, int scope, int top_k = -1) const {
        // 英文 token 列表
        std::vector<std::string> tokens;
        tokenize_en(keyword, tokens);

        // keyword 整体小写（路径匹配用）
        std::string kw_lower = keyword;
        for (auto& c : kw_lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        // 按 scope 选择候选列表
        const GameAssetIndex* idx_bp = (scope == 0 || scope == 1) ? &game_assets_bp_ : nullptr;
        const GameAssetIndex* idx_rp = (scope == 0 || scope == 2) ? &game_assets_rp_ : nullptr;

        // 对两个 index 分别做 BM25 搜索，结果合并到 path->score map
        std::unordered_map<std::string, double> score_map;  // rel_path -> bm25_score
        std::unordered_map<std::string, std::string> snippet_map;

        auto collect_bm25 = [&](const GameAssetIndex& idx) {
            auto bm25_results = idx.engine.search(tokens, -1);
            for (const auto& r : bm25_results) {
                const std::string& path = r.fragment->file;
                double& s = score_map[path];
                s += r.score * 1.0;  // 内容 BM25 得分
                if (snippet_map.find(path) == snippet_map.end()) {
                    // 取内容前 300 字符作为 snippet
                    const std::string& content = r.fragment->content;
                    snippet_map[path] = content.size() > 300 ? content.substr(0, 300) + "..." : content;
                }
            }
        };

        if (idx_bp) collect_bm25(*idx_bp);
        if (idx_rp) collect_bm25(*idx_rp);

        // 路径名匹配额外加分
        auto add_path_score = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.entries) {
                const std::string& rel = entry.rel_path;
                std::string rel_lower = rel;
                for (auto& c : rel_lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

                double path_score = 0.0;
                // 整体 keyword 命中路径：+10
                if (!kw_lower.empty() && rel_lower.find(kw_lower) != std::string::npos)
                    path_score += 10.0;
                // 各 token 命中路径：每个 +3
                for (const auto& tok : tokens) {
                    if (!tok.empty() && rel_lower.find(tok) != std::string::npos)
                        path_score += 3.0;
                }
                if (path_score > 0.0) {
                    score_map[rel] += path_score;
                    // 路径命中但尚无 snippet 时，用文件内容填充
                    if (snippet_map.find(rel) == snippet_map.end() && !entry.content.empty()) {
                        snippet_map[rel] = entry.content.size() > 300
                            ? entry.content.substr(0, 300) + "..." : entry.content;
                    }
                }
            }
        };

        if (idx_bp) add_path_score(*idx_bp);
        if (idx_rp) add_path_score(*idx_rp);

        // 汇总并排序
        std::vector<AssetResult> results;
        results.reserve(score_map.size());
        for (auto& [path, score] : score_map) {
            std::string snip;
            auto it = snippet_map.find(path);
            if (it != snippet_map.end()) snip = it->second;
            results.push_back({path, std::move(snip), score});
        }
        std::sort(results.begin(), results.end(), [](const AssetResult& a, const AssetResult& b) {
            return a.score != b.score ? a.score > b.score : a.rel_path < b.rel_path;
        });
        if (top_k > 0 && static_cast<size_t>(top_k) < results.size())
            results.resize(static_cast<size_t>(top_k));
        return results;
    }

    size_t game_assets_count() const {
        return game_assets_bp_.entries.size() + game_assets_rp_.entries.size();
    }

private:
    struct CategoryIndex {
        std::vector<DocFragment>                 fragments;
        std::vector<std::vector<std::string>>    tokenized_docs;
        BM25Engine                               engine;
    };

    // GameAssets 单条文件记录
    struct AssetEntry {
        std::string rel_path;  // 相对于 knowledge/GameAssets/ 的路径（如 behavior_packs/vanilla/...）
        std::string content;   // 文件完整文本内容
    };

    // GameAssets 分包索引（BP 或 RP）
    struct GameAssetIndex {
        std::vector<AssetEntry>                  entries;
        std::vector<DocFragment>                 fragments;   // 每个 entry 作为整体一个 fragment
        std::vector<std::vector<std::string>>    tokenized_docs;
        BM25Engine                               engine;
    };

    cppjieba::Jieba                jieba_;
    std::string                    knowledge_dir_;
    std::unordered_set<std::string> stop_words_;
    CategoryIndex                  api_index_;
    CategoryIndex                  event_index_;
    CategoryIndex                  enum_index_;
    CategoryIndex                  wiki_index_;
    CategoryIndex                  qumod_index_;
    CategoryIndex                  netease_guide_index_;
    GameAssetIndex                 game_assets_bp_;  // behavior_packs/
    GameAssetIndex                 game_assets_rp_;  // resource_packs/

    std::vector<SearchResult> search_category(const CategoryIndex& idx, const std::string& keyword, int top_k) const {
        std::vector<std::string> query_tokens;
        tokenize(keyword, query_tokens);
        return idx.engine.search(query_tokens, top_k);
    }

    std::vector<SearchResult> search_category_en(const CategoryIndex& idx, const std::string& keyword, int top_k) const {
        std::vector<std::string> query_tokens;
        tokenize_en(keyword, query_tokens);
        return idx.engine.search(query_tokens, top_k);
    }

    static DocCategory classify_path(const std::string& rel_path) {
        if (rel_path.find("NeteaseGuide/") == 0 || rel_path.find("/NeteaseGuide/") != std::string::npos)
            return DocCategory::NeteaseGuide;
        if (rel_path.find("QuModDocs/") == 0 || rel_path.find("/QuModDocs/") != std::string::npos)
            return DocCategory::QuMod;
        if (rel_path.find("BedrockWiki/") == 0 || rel_path.find("/BedrockWiki/") != std::string::npos)
            return DocCategory::Wiki;
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
        case DocCategory::API:          return &api_index_;
        case DocCategory::Event:        return &event_index_;
        case DocCategory::Enum:         return &enum_index_;
        case DocCategory::Wiki:         return &wiki_index_;
        case DocCategory::QuMod:        return &qumod_index_;
        case DocCategory::NeteaseGuide: return &netease_guide_index_;
        default:                        return nullptr;
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

    // 英文分词：按空格/标点拆分并转小写
    static void tokenize_en(const std::string& text, std::vector<std::string>& tokens) {
        tokens.clear();
        std::string word;
        for (char c : text) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
                word += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            } else if (!word.empty()) {
                tokens.push_back(std::move(word));
                word.clear();
            }
        }
        if (!word.empty()) tokens.push_back(std::move(word));
    }

    static std::string path_to_utf8(const std::filesystem::path& p) {
        auto u8 = p.generic_u8string();
        return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
    }

    void load_knowledge() {
        namespace fs = std::filesystem;
        if (!fs::exists(knowledge_dir_)) {
            std::cerr << "[MCDK] knowledge dir not found: " << knowledge_dir_ << std::endl;
            return;
        }

        // 加载 Markdown 文档（BM25 索引）
        for (const auto& entry : fs::recursive_directory_iterator(knowledge_dir_)) {
            if (!entry.is_regular_file()) continue;
            std::string rel_path = path_to_utf8(fs::relative(entry.path(), knowledge_dir_));

            // GameAssets 下的文件建路径+内容索引，不走普通 BM25
            if (rel_path.find("GameAssets/") == 0 || rel_path.find("GameAssets\\") == 0) {
                // 路径相对于 knowledge/GameAssets/
                std::string assets_rel = rel_path.substr(std::string("GameAssets/").size());
                // 统一斜杠
                for (auto& c : assets_rel) if (c == '\\') c = '/';

                // 读取文件内容
                std::ifstream ga_ifs(entry.path());
                std::string content;
                if (ga_ifs.is_open()) {
                    std::ostringstream ss;
                    ss << ga_ifs.rdbuf();
                    content = ss.str();
                }

                GameAssetIndex* ga_idx = nullptr;
                if (assets_rel.find("behavior_packs/") == 0)
                    ga_idx = &game_assets_bp_;
                else if (assets_rel.find("resource_packs/") == 0)
                    ga_idx = &game_assets_rp_;

                if (ga_idx) {
                    std::string ga_path = "GameAssets/" + assets_rel;
                    ga_idx->entries.push_back({ga_path, content});
                    ga_idx->fragments.push_back({content, ga_path, 1, 0});
                }
                continue;
            }

            auto ext = entry.path().extension().string();
            if (ext != ".md" && ext != ".MD") continue;
            load_markdown_file(entry.path(), rel_path);
        }

        std::cout << "[MCDK] loaded " << doc_count() << " fragments from " << knowledge_dir_ << std::endl;
        std::cout << "[MCDK] GameAssets: " << game_assets_bp_.entries.size() << " BP + "
                  << game_assets_rp_.entries.size() << " RP files indexed" << std::endl;
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
        auto build_cn = [this](CategoryIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i) {
                tokenize(idx.fragments[i].content, idx.tokenized_docs[i]);
            }
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };
        auto build_en = [](CategoryIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i) {
                tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
            }
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };
        build_cn(api_index_,            "API");
        build_cn(event_index_,          "Event");
        build_cn(enum_index_,           "Enum");
        build_en(wiki_index_,           "Wiki");
        build_cn(qumod_index_,          "QuMod");
        build_cn(netease_guide_index_,  "NeteaseGuide");

        // GameAssets：用英文分词对文件内容建 BM25 索引
        auto build_game_assets = [](GameAssetIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i) {
                tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
            }
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };
        build_game_assets(game_assets_bp_, "GameAssets/BP");
        build_game_assets(game_assets_rp_, "GameAssets/RP");
    }
};

} // namespace mcdk
