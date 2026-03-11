#pragma once

#include "search/bm25.hpp"
#include "search/index_cache.hpp"
#include <cppjieba/Jieba.hpp>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <climits>
#include <mutex>
#include <thread>
#include <future>

namespace mcdk {

// 文档分类（基于路径子目录）
enum class DocCategory { Unknown, API, Event, Enum, Beta, Wiki, QuMod, NeteaseGuide };

class SearchService {
public:
    // 完整模式：提供词典目录 + 知识库目录 + 可选缓存路径
    SearchService(const std::string& dicts_dir, const std::string& knowledge_dir,
                  const std::string& cache_path = "")
        : jieba_(std::make_unique<cppjieba::Jieba>(
            dicts_dir + "/jieba.dict.utf8",
            dicts_dir + "/hmm_model.utf8",
            dicts_dir + "/user.dict.utf8",
            dicts_dir + "/idf.utf8",
            dicts_dir + "/stop_words.utf8"
          ))
        , knowledge_dir_(knowledge_dir)
        , cache_path_(cache_path)
        , cache_only_mode_(false)
    {
        load_stop_words(dicts_dir + "/stop_words.utf8");
        init_indices();
    }

    // 仅缓存模式：提供词典目录和缓存文件路径，无需知识库目录
    SearchService(const std::string& dicts_dir, const std::string& cache_path, bool cache_only)
        : jieba_(std::make_unique<cppjieba::Jieba>(
            dicts_dir + "/jieba.dict.utf8",
            dicts_dir + "/hmm_model.utf8",
            dicts_dir + "/user.dict.utf8",
            dicts_dir + "/idf.utf8",
            dicts_dir + "/stop_words.utf8"
          ))
        , cache_path_(cache_path)
        , cache_only_mode_(cache_only)
    {
        load_stop_words(dicts_dir + "/stop_words.utf8");
        init_indices();
    }

    // 是否为仅缓存模式（无磁盘知识库目录）
    bool is_cache_only_mode() const { return cache_only_mode_; }

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

    // ── 从缓存的 fragments 中读取文件内容（供仅缓存模式下的 read_knowledge 使用）──
    // 返回: {content, total_lines}，file 未找到时 content 为空
    struct FileReadResult {
        std::string content;
        int         total_lines = 0;
        bool        found       = false;
    };

    FileReadResult read_cached_file(const std::string& rel_path, int line_start = 1, int line_end = INT_MAX) const {
        FileReadResult result;
        // 合并所有 fragments 的内容（同一 file 可能有多个 fragment）
        // 先收集所有来源
        std::string full_content;
        bool found = false;

        auto collect = [&](const CategoryIndex& idx) {
            for (const auto& frag : idx.fragments) {
                if (frag.file == rel_path) {
                    full_content += frag.content;
                    found = true;
                }
            }
        };

        collect(api_index_);
        collect(event_index_);
        collect(enum_index_);
        collect(wiki_index_);
        collect(qumod_index_);
        collect(netease_guide_index_);

        // 也搜索 GameAssets 的 fragments
        auto collect_ga = [&](const GameAssetIndex& idx) {
            for (const auto& frag : idx.fragments) {
                if (frag.file == rel_path) {
                    full_content += frag.content;
                    found = true;
                }
            }
        };
        collect_ga(game_assets_bp_);
        collect_ga(game_assets_rp_);

        if (!found) return result;

        result.found = true;

        // 按行切割并提取指定范围
        std::istringstream iss(full_content);
        std::string line;
        int cur = 0;
        if (line_start < 1) line_start = 1;
        if (line_end < line_start) line_end = line_start;

        while (std::getline(iss, line)) {
            ++cur;
            if (cur < line_start) continue;
            if (cur > line_end) break;
            result.content += line;
            result.content += '\n';
        }
        result.total_lines = cur;
        return result;
    }

    // ── 从缓存的 fragments 中列出文件/目录（供仅缓存模式下的 list_knowledge 使用）──
    struct ListResult {
        std::vector<std::string> dirs;
        std::vector<std::string> files;
        bool                     found = false;
    };

    ListResult list_cached_files(const std::string& rel_path = "") const {
        ListResult result;
        std::set<std::string> dir_set, file_set;

        // 规范化路径：确保以 / 结尾（用于前缀匹配），空路径表示根目录
        std::string prefix = rel_path;
        if (!prefix.empty()) {
            for (auto& c : prefix) if (c == '\\') c = '/';
            if (prefix.back() != '/') prefix += '/';
        }

        auto collect = [&](const std::vector<DocFragment>& frags) {
            for (const auto& frag : frags) {
                const std::string& file = frag.file;
                // 检查是否在指定目录下
                if (!prefix.empty() && file.find(prefix) != 0) continue;
                // 取相对于 prefix 的部分
                std::string remainder = file.substr(prefix.size());
                // 找第一个 /，判断是子目录还是直接文件
                auto slash_pos = remainder.find('/');
                if (slash_pos != std::string::npos) {
                    dir_set.insert(remainder.substr(0, slash_pos));
                } else if (!remainder.empty()) {
                    file_set.insert(remainder);
                }
            }
        };

        collect(api_index_.fragments);
        collect(event_index_.fragments);
        collect(enum_index_.fragments);
        collect(wiki_index_.fragments);
        collect(qumod_index_.fragments);
        collect(netease_guide_index_.fragments);
        collect(game_assets_bp_.fragments);
        collect(game_assets_rp_.fragments);

        // 也从 GameAssets 的 path_entries 收集（因为 fragments 可能只保存了部分内容）
        auto collect_ga_paths = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.path_entries) {
                const std::string& file = entry.first;
                if (!prefix.empty() && file.find(prefix) != 0) continue;
                std::string remainder = file.substr(prefix.size());
                auto slash_pos = remainder.find('/');
                if (slash_pos != std::string::npos) {
                    dir_set.insert(remainder.substr(0, slash_pos));
                } else if (!remainder.empty()) {
                    file_set.insert(remainder);
                }
            }
        };
        collect_ga_paths(game_assets_bp_);
        collect_ga_paths(game_assets_rp_);

        result.dirs.assign(dir_set.begin(), dir_set.end());
        result.files.assign(file_set.begin(), file_set.end());
        result.found = !result.dirs.empty() || !result.files.empty() || prefix.empty();
        return result;
    }

    struct AssetResult {
        std::string rel_path;
        std::string snippet;
        double      score;
    };

    // 默认 top_k=200，防止 20000 个文件全量返回导致超时
    static constexpr int DEFAULT_GAME_ASSET_TOP_K = 200;

    std::vector<AssetResult> search_game_assets(const std::string& keyword, int scope, int top_k = -1) const {
        // 无上限时使用默认上限，避免大量文件返回导致超时
        const int effective_top_k = (top_k > 0) ? top_k : DEFAULT_GAME_ASSET_TOP_K;

        std::vector<std::string> tokens;
        tokenize_en(keyword, tokens);
        if (tokens.empty()) return {};

        // 预处理：小写关键词和每个 token
        std::string kw_lower = keyword;
        for (auto& c : kw_lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        // tokens 已经是 lowercase（tokenize_en 里做了 tolower）

        const GameAssetIndex* idx_bp = (scope == 0 || scope == 1) ? &game_assets_bp_ : nullptr;
        const GameAssetIndex* idx_rp = (scope == 0 || scope == 2) ? &game_assets_rp_ : nullptr;

        // 用 doc_id 为 key 的稀疏 map，避免 string hash 开销
        // 路径匹配分和 BM25 内容分分开汇总，最后合并
        std::unordered_map<std::string, double>      score_map;
        std::unordered_map<std::string, std::string> snippet_map;
        score_map.reserve(512);
        snippet_map.reserve(512);

        // BM25 内容搜索（倒排，只命中文档才进 score_map）
        auto collect_bm25 = [&](const GameAssetIndex& idx) {
            // 传 effective_top_k*4 做内部预限制，避免无上限全量排序
            auto bm25_results = idx.engine.search(tokens, effective_top_k * 4);
            for (const auto& r : bm25_results) {
                const std::string& path = r.fragment->file;
                score_map[path] += r.score;
                if (snippet_map.find(path) == snippet_map.end()) {
                    const std::string& content = r.fragment->content;
                    snippet_map[path] = content.size() > 300 ? content.substr(0, 300) + "..." : content;
                }
            }
        };

        if (idx_bp) collect_bm25(*idx_bp);
        if (idx_rp) collect_bm25(*idx_rp);

        // 路径名匹配：只对路径字符串做 find，不读取文件内容
        // entries.second 不再使用（content 已在 fragments 中），避免额外内存访问
        auto add_path_score = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.path_entries) {
                // entry 只包含 (rel_path, rel_path_lower)
                const std::string& rel       = entry.first;
                const std::string& rel_lower = entry.second;

                double path_score = 0.0;
                // 精确子串匹配整个关键词（高权重）
                if (!kw_lower.empty() && rel_lower.find(kw_lower) != std::string::npos)
                    path_score += 10.0;
                // 逐 token 匹配（中等权重）
                for (const auto& tok : tokens) {
                    if (!tok.empty() && rel_lower.find(tok) != std::string::npos)
                        path_score += 3.0;
                }
                if (path_score > 0.0) {
                    score_map[rel] += path_score;
                    // snippet 只从已有 BM25 结果中取，路径命中不读内容
                }
            }
        };

        if (idx_bp) add_path_score(*idx_bp);
        if (idx_rp) add_path_score(*idx_rp);

        std::vector<AssetResult> results;
        results.reserve(score_map.size());
        for (auto& [path, score] : score_map) {
            std::string snip;
            auto it = snippet_map.find(path);
            if (it != snippet_map.end()) snip = std::move(it->second);
            results.push_back({path, std::move(snip), score});
        }

        // 用 partial_sort 只排前 effective_top_k 个
        if (static_cast<size_t>(effective_top_k) < results.size()) {
            std::partial_sort(results.begin(),
                              results.begin() + effective_top_k,
                              results.end(),
                              [](const AssetResult& a, const AssetResult& b) {
                                  return a.score != b.score ? a.score > b.score : a.rel_path < b.rel_path;
                              });
            results.resize(static_cast<size_t>(effective_top_k));
        } else {
            std::sort(results.begin(), results.end(), [](const AssetResult& a, const AssetResult& b) {
                return a.score != b.score ? a.score > b.score : a.rel_path < b.rel_path;
            });
        }
        return results;
    }

    size_t game_assets_count() const {
        return game_assets_bp_.path_entries.size() + game_assets_rp_.path_entries.size();
    }

private:
    struct CategoryIndex {
        std::vector<DocFragment>              fragments;
        std::vector<std::vector<std::string>> tokenized_docs;
        BM25Engine                            engine;
    };

    struct GameAssetIndex {
        // 轻量路径表：只存 (rel_path, rel_path_lower)，不再存文件内容副本
        std::vector<std::pair<std::string,std::string>> path_entries;  // (rel_path, rel_path_lower)
        std::vector<DocFragment>                        fragments;
        std::vector<std::vector<std::string>>           tokenized_docs;
        BM25Engine                                      engine;
    };

    std::unique_ptr<cppjieba::Jieba> jieba_;       // 仅缓存模式下为 nullptr
    std::string                     knowledge_dir_;
    std::string                     cache_path_;
    bool                            cache_only_mode_ = false;
    std::unordered_set<std::string> stop_words_;
    CategoryIndex                   api_index_;
    CategoryIndex                   event_index_;
    CategoryIndex                   enum_index_;
    CategoryIndex                   wiki_index_;
    CategoryIndex                   qumod_index_;
    CategoryIndex                   netease_guide_index_;
    GameAssetIndex                  game_assets_bp_;
    GameAssetIndex                  game_assets_rp_;

    // ── 初始化入口 ──
    void init_indices() {
        if (cache_only_mode_) {
            // 仅缓存模式：直接从 .bin 加载，跳过 fingerprint 校验
            if (cache_path_.empty()) {
                std::cerr << "[MCDK] cache-only mode: no cache path provided" << std::endl;
                return;
            }
            IndexCache::CacheData cached;
            if (IndexCache::load(cache_path_, "", cached, /*skip_fingerprint_check=*/true)) {
                std::cout << "[MCDK] 仅缓存模式：从缓存文件恢复索引..." << std::endl;
                restore_from_cache(std::move(cached));
            } else {
                std::cerr << "[MCDK] 仅缓存模式：缓存文件加载失败: " << cache_path_ << std::endl;
            }
            return;
        }

        if (!cache_path_.empty()) {
            std::string fp = IndexCache::compute_fingerprint(knowledge_dir_);
            std::cout << "[MCDK] knowledge fingerprint: " << fp << std::endl;

            IndexCache::CacheData cached;
            if (IndexCache::load(cache_path_, fp, cached)) {
                std::cout << "[MCDK] 命中缓存，恢复索引中..." << std::endl;
                restore_from_cache(std::move(cached));
                return;
            }
        }

        std::cout << "[MCDK] 缓存未命中，开始完整构建索引..." << std::endl;
        load_knowledge_parallel();
        build_indices();

        if (!cache_path_.empty()) {
            save_cache();
        }
    }

    // 从缓存直接恢复，完全跳过 build_index()
    void restore_from_cache(IndexCache::CacheData&& cached) {
        auto restore_cat = [](CategoryIndex& idx, IndexCache::CatData& d) {
            idx.fragments      = std::move(d.fragments);
            idx.tokenized_docs = std::move(d.tokenized_docs);
            idx.engine.restore_index(
                idx.fragments, idx.tokenized_docs,
                std::move(d.bm25.doc_lengths), d.bm25.avg_dl,
                std::move(d.bm25.idf), std::move(d.bm25.inverted_index)
            );
        };

        if (cached.categories.size() >= 6) {
            restore_cat(api_index_,           cached.categories[0]);
            restore_cat(event_index_,         cached.categories[1]);
            restore_cat(enum_index_,          cached.categories[2]);
            restore_cat(wiki_index_,          cached.categories[3]);
            restore_cat(qumod_index_,         cached.categories[4]);
            restore_cat(netease_guide_index_, cached.categories[5]);
        }

        if (cached.game_assets.size() >= 2) {
            auto restore_ga = [](GameAssetIndex& idx, IndexCache::GameAssetData& d) {
                idx.fragments      = std::move(d.fragments);
                idx.tokenized_docs = std::move(d.tokenized_docs);
                // 重建 path_entries：(rel_path, rel_path_lower)，不再保存内容副本
                idx.path_entries.resize(d.rel_paths.size());
                for (size_t i = 0; i < d.rel_paths.size(); ++i) {
                    std::string lower = d.rel_paths[i];
                    for (auto& c : lower)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    idx.path_entries[i].first  = std::move(d.rel_paths[i]);
                    idx.path_entries[i].second = std::move(lower);
                }
                idx.engine.restore_index(
                    idx.fragments, idx.tokenized_docs,
                    std::move(d.bm25.doc_lengths), d.bm25.avg_dl,
                    std::move(d.bm25.idf), std::move(d.bm25.inverted_index)
                );
            };
            restore_ga(game_assets_bp_, cached.game_assets[0]);
            restore_ga(game_assets_rp_, cached.game_assets[1]);
        }

        std::cout << "[MCDK] 缓存恢复完成: " << doc_count() << " fragments, "
                  << game_assets_count() << " game assets" << std::endl;
    }

    // 保存缓存：GameAssets 只存 rel_path，content 通过 fragments 序列化（去掉重复）
    void save_cache() {
        std::string fp = IndexCache::compute_fingerprint(knowledge_dir_);

        // 提取 GA 的 rel_path 列表（从 path_entries 取原始路径）
        std::vector<std::string> bp_rel_paths, rp_rel_paths;
        bp_rel_paths.reserve(game_assets_bp_.path_entries.size());
        rp_rel_paths.reserve(game_assets_rp_.path_entries.size());
        for (const auto& e : game_assets_bp_.path_entries) bp_rel_paths.push_back(e.first);
        for (const auto& e : game_assets_rp_.path_entries) rp_rel_paths.push_back(e.first);

        std::vector<IndexCache::CatIndexRef> cat_refs = {
            {&api_index_.fragments,           &api_index_.tokenized_docs,           &api_index_.engine},
            {&event_index_.fragments,         &event_index_.tokenized_docs,         &event_index_.engine},
            {&enum_index_.fragments,          &enum_index_.tokenized_docs,          &enum_index_.engine},
            {&wiki_index_.fragments,          &wiki_index_.tokenized_docs,          &wiki_index_.engine},
            {&qumod_index_.fragments,         &qumod_index_.tokenized_docs,         &qumod_index_.engine},
            {&netease_guide_index_.fragments, &netease_guide_index_.tokenized_docs, &netease_guide_index_.engine},
        };

        std::vector<IndexCache::GameIndexRef> ga_refs = {
            {&bp_rel_paths, &game_assets_bp_.fragments, &game_assets_bp_.tokenized_docs, &game_assets_bp_.engine},
            {&rp_rel_paths, &game_assets_rp_.fragments, &game_assets_rp_.tokenized_docs, &game_assets_rp_.engine},
        };

        IndexCache::save(cache_path_, fp, cat_refs, ga_refs);
    }

    // ── 搜索辅助 ──
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

    // ── 文件分类 ──
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

    // ── 分词 ──
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
        jieba_->CutForSearch(text, raw);
        tokens.clear();
        for (auto& w : raw) {
            if (w.empty() || w == " " || w == "\t" || w == "\n") continue;
            if (stop_words_.count(w)) continue;
            tokens.push_back(std::move(w));
        }
    }

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

    // ── 并行加载知识库 ──
    void load_knowledge_parallel() {
        namespace fs = std::filesystem;
        if (!fs::exists(knowledge_dir_)) {
            std::cerr << "[MCDK] knowledge dir not found: " << knowledge_dir_ << std::endl;
            return;
        }

        std::vector<std::pair<fs::path, std::string>> md_files;
        std::vector<std::pair<fs::path, std::string>> ga_files;  // (abs, ga_rel)

        for (const auto& entry : fs::recursive_directory_iterator(knowledge_dir_)) {
            if (!entry.is_regular_file()) continue;
            std::string rel_path = path_to_utf8(fs::relative(entry.path(), knowledge_dir_));

            if (rel_path.find("GameAssets/") == 0 || rel_path.find("GameAssets\\") == 0) {
                std::string assets_rel = rel_path.substr(std::string("GameAssets/").size());
                for (auto& c : assets_rel) if (c == '\\') c = '/';
                if (assets_rel.find("behavior_packs/") == 0 || assets_rel.find("resource_packs/") == 0)
                    ga_files.push_back({entry.path(), assets_rel});
                continue;
            }
            auto ext = entry.path().extension().string();
            if (ext == ".md" || ext == ".MD")
                md_files.push_back({entry.path(), rel_path});
        }

        std::cout << "[MCDK] 扫描完成: " << md_files.size() << " md, "
                  << ga_files.size() << " game asset 文件" << std::endl;

        // 多线程并行读取 GameAssets
        unsigned int nthreads = std::max(1u, std::thread::hardware_concurrency());
        std::cout << "[MCDK] 使用 " << nthreads << " 线程读取 GameAssets..." << std::endl;

        size_t ga_total = ga_files.size();
        std::mutex bp_mutex, rp_mutex;

        size_t batch = std::max<size_t>(1, (ga_total + nthreads - 1) / nthreads);
        std::vector<std::future<void>> futures;

        for (size_t start = 0; start < ga_total; start += batch) {
            size_t end = std::min(start + batch, ga_total);
            futures.push_back(std::async(std::launch::async, [&, start, end]() {
                for (size_t i = start; i < end; ++i) {
                    const auto& [abs_path, assets_rel] = ga_files[i];
                    std::ifstream ifs(abs_path, std::ios::binary);
                    std::string content;
                    if (ifs.is_open()) {
                        std::ostringstream ss;
                        ss << ifs.rdbuf();
                        content = ss.str();
                    }
                    std::string ga_path = "GameAssets/" + assets_rel;
                    // 预计算小写路径，路径匹配时直接用，避免搜索时重复转换
                    std::string ga_path_lower = ga_path;
                    for (auto& c : ga_path_lower)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

                    DocFragment frag{std::move(content), ga_path, 1, 0};
                    std::pair<std::string,std::string> pe{ga_path, std::move(ga_path_lower)};

                    if (assets_rel.find("behavior_packs/") == 0) {
                        std::lock_guard<std::mutex> lk(bp_mutex);
                        game_assets_bp_.path_entries.push_back(std::move(pe));
                        game_assets_bp_.fragments.push_back(std::move(frag));
                    } else {
                        std::lock_guard<std::mutex> lk(rp_mutex);
                        game_assets_rp_.path_entries.push_back(std::move(pe));
                        game_assets_rp_.fragments.push_back(std::move(frag));
                    }
                }
            }));
        }
        for (auto& f : futures) f.get();

        // 单线程加载 Markdown（jieba 非线程安全）
        for (const auto& [abs_path, rel_path] : md_files)
            load_markdown_file(abs_path, rel_path);

        std::cout << "[MCDK] loaded " << doc_count() << " md-fragments, "
                  << game_assets_count() << " game assets" << std::endl;
    }

    void load_markdown_file(const std::filesystem::path& abs_path, const std::string& rel_path) {
        std::ifstream ifs(abs_path);
        if (!ifs.is_open()) return;

        DocCategory cat = classify_path(rel_path);
        CategoryIndex* idx = index_for(cat);
        if (!idx) return;

        std::string line;
        std::ostringstream current_content;
        int fragment_start = 1, line_num = 0;
        bool has_content = false;

        auto flush_fragment = [&]() {
            std::string content = current_content.str();
            if (!content.empty() && has_content)
                idx->fragments.push_back({std::move(content), rel_path, fragment_start, line_num});
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
                if (level >= 2 && level <= 4) { flush_fragment(); fragment_start = line_num; }
            }
            current_content << line << "\n";
            if (!line.empty()) has_content = true;
        }
        flush_fragment();
    }

    // ── 构建 BM25 索引 ──
    void build_indices() {
        auto build_cn = [this](CategoryIndex& idx, const char* name) {
            idx.tokenized_docs.resize(idx.fragments.size());
            for (size_t i = 0; i < idx.fragments.size(); ++i)
                tokenize(idx.fragments[i].content, idx.tokenized_docs[i]);
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        auto build_en_parallel = [](CategoryIndex& idx, const char* name) {
            size_t n = idx.fragments.size();
            idx.tokenized_docs.resize(n);
            unsigned int nt = std::max(1u, std::thread::hardware_concurrency());
            size_t batch = std::max<size_t>(1, (n + nt - 1) / nt);
            std::vector<std::future<void>> futs;
            for (size_t s = 0; s < n; s += batch) {
                size_t e = std::min(s + batch, n);
                futs.push_back(std::async(std::launch::async, [&idx, s, e]() {
                    for (size_t i = s; i < e; ++i)
                        tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
                }));
            }
            for (auto& f : futs) f.get();
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        build_cn(api_index_,            "API");
        build_cn(event_index_,          "Event");
        build_cn(enum_index_,           "Enum");
        build_en_parallel(wiki_index_,  "Wiki");
        build_cn(qumod_index_,          "QuMod");
        build_cn(netease_guide_index_,  "NeteaseGuide");

        auto build_ga_parallel = [](GameAssetIndex& idx, const char* name) {
            size_t n = idx.fragments.size();
            idx.tokenized_docs.resize(n);
            unsigned int nt = std::max(1u, std::thread::hardware_concurrency());
            size_t batch = std::max<size_t>(1, (n + nt - 1) / nt);
            std::vector<std::future<void>> futs;
            for (size_t s = 0; s < n; s += batch) {
                size_t e = std::min(s + batch, n);
                futs.push_back(std::async(std::launch::async, [&idx, s, e]() {
                    for (size_t i = s; i < e; ++i)
                        tokenize_en(idx.fragments[i].content, idx.tokenized_docs[i]);
                }));
            }
            for (auto& f : futs) f.get();
            idx.engine.build_index(idx.fragments, idx.tokenized_docs);
            std::cout << "[MCDK] " << name << " index: " << idx.fragments.size() << " docs" << std::endl;
        };

        build_ga_parallel(game_assets_bp_, "GameAssets/BP");
        build_ga_parallel(game_assets_rp_, "GameAssets/RP");
    }
};

} // namespace mcdk
