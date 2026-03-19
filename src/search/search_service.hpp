#pragma once

#include "common/path_utils.hpp"
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

enum class DocCategory { Unknown, API, Event, Enum, Beta, Wiki, QuMod, NeteaseGuide };

class SearchService {
public:
    // 正常模式：词典、知识库、缓存都来自磁盘目录。
    SearchService(const std::filesystem::path& dicts_dir,
                  const std::filesystem::path& knowledge_dir,
                  const std::filesystem::path& cache_path = {})
        : jieba_(std::make_unique<cppjieba::Jieba>(
            mcdk::path::to_utf8(dicts_dir / "jieba.dict.utf8"),
            mcdk::path::to_utf8(dicts_dir / "hmm_model.utf8"),
            mcdk::path::to_utf8(dicts_dir / "user.dict.utf8"),
            mcdk::path::to_utf8(dicts_dir / "idf.utf8"),
            mcdk::path::to_utf8(dicts_dir / "stop_words.utf8")
          ))
        , knowledge_dir_(knowledge_dir)
        , cache_path_(cache_path)
        , cache_only_mode_(false)
    {
        this->load_stop_words(dicts_dir / "stop_words.utf8");
        init_indices();
    }

    // cache-only 模式保留搜索能力，但不再要求 knowledge 目录存在。
    SearchService(const std::filesystem::path& dicts_dir,
                  const std::filesystem::path& cache_path,
                  bool cache_only)
        : jieba_(std::make_unique<cppjieba::Jieba>(
            mcdk::path::to_utf8(dicts_dir / "jieba.dict.utf8"),
            mcdk::path::to_utf8(dicts_dir / "hmm_model.utf8"),
            mcdk::path::to_utf8(dicts_dir / "user.dict.utf8"),
            mcdk::path::to_utf8(dicts_dir / "idf.utf8"),
            mcdk::path::to_utf8(dicts_dir / "stop_words.utf8")
          ))
        , cache_path_(cache_path)
        , cache_only_mode_(cache_only)
    {
        this->load_stop_words(dicts_dir / "stop_words.utf8");
        init_indices();
    }

    bool is_cache_only_mode() const { return cache_only_mode_; }

    std::vector<SearchResult> search_api(const std::string& keyword, int top_k = -1) const {
        return search_category_flexible(api_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_event(const std::string& keyword, int top_k = -1) const {
        if (looks_english_query(keyword) && !is_precise_identifier_query(keyword)) {
            const int limited_top_k = top_k > 0 ? std::min(top_k, 8) : 8;
            return search_event_keyword_index(keyword, limited_top_k);
        }
        return search_category_flexible(event_index_, keyword, top_k, SearchMode::PreferEnglishFallback);
    }
    std::vector<SearchResult> search_enum(const std::string& keyword, int top_k = -1) const {
        return search_category_flexible(enum_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_netease_guide(const std::string& keyword, int top_k = -1) const {
        return search_category_flexible(netease_guide_index_, keyword, top_k, SearchMode::Auto);
    }
    std::vector<SearchResult> search_all(const std::string& keyword, int top_k = -1) const {
        // 聚合搜索必须限制每个分区的候选数，否则高频词会把所有分区全量结果拼接，
        // 导致响应体过大甚至表现为无响应。
        const int per_bucket_k = top_k > 0 ? std::max(top_k * 3, 30) : 60;

        auto a = search_api(keyword, per_bucket_k);
        auto b = search_event(keyword, per_bucket_k);
        auto c = search_enum(keyword, per_bucket_k);
        auto d = search_wiki(keyword, per_bucket_k);
        auto e = search_qumod(keyword, per_bucket_k);
        auto f = search_netease_guide(keyword, per_bucket_k);

        std::vector<SearchResult> merged;
        merged.reserve(a.size() + b.size() + c.size() + d.size() + e.size() + f.size());
        merged.insert(merged.end(), a.begin(), a.end());
        merged.insert(merged.end(), b.begin(), b.end());
        merged.insert(merged.end(), c.begin(), c.end());
        merged.insert(merged.end(), d.begin(), d.end());
        merged.insert(merged.end(), e.begin(), e.end());
        merged.insert(merged.end(), f.begin(), f.end());

        std::sort(merged.begin(), merged.end(), [](const SearchResult& x, const SearchResult& y) {
            return x.score != y.score ? x.score > y.score : x.fragment->file < y.fragment->file;
        });

        if (top_k > 0 && static_cast<size_t>(top_k) < merged.size()) merged.resize(static_cast<size_t>(top_k));
        return merged;
    }
    std::vector<SearchResult> search_wiki(const std::string& keyword, int top_k = -1) const {
        return search_category_en(wiki_index_, keyword, top_k);
    }
    std::vector<SearchResult> search_qumod(const std::string& keyword, int top_k = -1) const {
        return search_category_flexible(qumod_index_, keyword, top_k, SearchMode::Auto);
    }

    size_t doc_count() const {
        return api_index_.engine.doc_count() + event_index_.engine.doc_count()
             + enum_index_.engine.doc_count() + wiki_index_.engine.doc_count()
             + qumod_index_.engine.doc_count() + netease_guide_index_.engine.doc_count();
    }

    struct FileReadResult {
        std::string content;
        int         total_lines = 0;
        bool        found       = false;
    };

    FileReadResult read_cached_file(const std::string& rel_path, int line_start = 1, int line_end = INT_MAX) const {
        FileReadResult result;
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

    struct ListResult {
        std::vector<std::string> dirs;
        std::vector<std::string> files;
        bool                     found = false;
    };

    ListResult list_cached_files(const std::string& rel_path = "") const {
        ListResult result;
        std::set<std::string> dir_set, file_set;

        std::string prefix = rel_path;
        if (!prefix.empty()) {
            for (auto& c : prefix) if (c == '\\') c = '/';
            if (prefix.back() != '/') prefix += '/';
        }

        auto collect = [&](const std::vector<DocFragment>& frags) {
            for (const auto& frag : frags) {
                const std::string& file = frag.file;
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

        collect(api_index_.fragments);
        collect(event_index_.fragments);
        collect(enum_index_.fragments);
        collect(wiki_index_.fragments);
        collect(qumod_index_.fragments);
        collect(netease_guide_index_.fragments);
        collect(game_assets_bp_.fragments);
        collect(game_assets_rp_.fragments);

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

    static constexpr int DEFAULT_GAME_ASSET_TOP_K = 200;

    std::vector<AssetResult> search_game_assets(const std::string& keyword, int scope, int top_k = -1) const {
        const int effective_top_k = (top_k > 0) ? top_k : DEFAULT_GAME_ASSET_TOP_K;

        std::vector<std::string> tokens;
        tokenize_en(keyword, tokens);
        if (tokens.empty()) {
            tokenize(keyword, tokens);
            normalize_tokens(tokens);
        }
        if (tokens.empty()) return {};

        std::string kw_lower = keyword;
        for (auto& c : kw_lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        const GameAssetIndex* idx_bp = (scope == 0 || scope == 1) ? &game_assets_bp_ : nullptr;
        const GameAssetIndex* idx_rp = (scope == 0 || scope == 2) ? &game_assets_rp_ : nullptr;

        std::unordered_map<std::string, double>      score_map;
        std::unordered_map<std::string, std::string> snippet_map;
        score_map.reserve(512);
        snippet_map.reserve(512);

        auto collect_bm25 = [&](const GameAssetIndex& idx) {
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

        auto add_path_score = [&](const GameAssetIndex& idx) {
            for (const auto& entry : idx.path_entries) {
                const std::string& rel       = entry.first;
                const std::string& rel_lower = entry.second;

                double path_score = 0.0;
                if (!kw_lower.empty() && rel_lower.find(kw_lower) != std::string::npos)
                    path_score += 10.0;
                for (const auto& tok : tokens) {
                    if (!tok.empty() && rel_lower.find(tok) != std::string::npos)
                        path_score += 3.0;
                }
                if (path_score > 0.0) {
                    score_map[rel] += path_score;
                    if (snippet_map.find(rel) == snippet_map.end()) {
                        snippet_map.emplace(rel, make_asset_path_snippet(rel));
                    }
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
    enum class SearchMode {
        ChineseOnly,
        EnglishOnly,
        Auto,
        PreferEnglishFallback,
    };

    struct CategoryIndex {
        std::vector<DocFragment>              fragments;
        std::vector<std::vector<std::string>> tokenized_docs;
        BM25Engine                            engine;
    };

    struct GameAssetIndex {
        std::vector<std::pair<std::string,std::string>> path_entries;
        std::vector<DocFragment>                        fragments;
        std::vector<std::vector<std::string>>           tokenized_docs;
        BM25Engine                                      engine;
    };

    struct EventKeywordEntry {
        size_t      fragment_index;
        std::string identifier_lower;
    };

    std::unique_ptr<cppjieba::Jieba> jieba_;
    std::filesystem::path           knowledge_dir_;
    std::filesystem::path           cache_path_;
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
    std::vector<EventKeywordEntry>  event_keyword_entries_;

    void init_indices() {
        if (cache_only_mode_) {
            if (cache_path_.empty()) {
                std::cerr << "[MCDK] cache-only mode: no cache path provided" << std::endl;
                return;
            }
            IndexCache::CacheData cached;
            if (IndexCache::load(cache_path_, "", cached, /*skip_fingerprint_check=*/true)) {
                std::cout << "[MCDK] 缓存模式：从缓存文件恢复索引..." << std::endl;
                restore_from_cache(std::move(cached));
            } else {
                std::cerr << "[MCDK] 缓存模式：缓存文件加载失败: " << mcdk::path::to_utf8(cache_path_) << std::endl;
            }
            return;
        }

        if (!cache_path_.empty()) {
            // 有缓存时优先走恢复路径，避免每次启动都重新构建 BM25。
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
            save_cache();  // save_cache() 内部已释放 tokenized_docs
        } else {
            auto free_td = [](CategoryIndex& idx) {
                std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
            };
            free_td(api_index_);
            free_td(event_index_);
            free_td(enum_index_);
            free_td(wiki_index_);
            free_td(qumod_index_);
            free_td(netease_guide_index_);
            auto free_td_ga = [](GameAssetIndex& idx) {
                std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
            };
            free_td_ga(game_assets_bp_);
            free_td_ga(game_assets_rp_);
            std::cout << "[MCDK] tokenized_docs released (no-cache mode)" << std::endl;
        }
    }

    void restore_from_cache(IndexCache::CacheData&& cached) {
        auto restore_cat = [](CategoryIndex& idx, IndexCache::CatData& d) {
            idx.fragments = std::move(d.fragments);
            idx.engine.restore_index(
                idx.fragments, d.tokenized_docs,
                std::move(d.bm25.doc_lengths), d.bm25.avg_dl,
                std::move(d.bm25.idf), std::move(d.bm25.inverted_index)
            );
            std::vector<std::vector<std::string>>().swap(d.tokenized_docs);
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
                idx.fragments = std::move(d.fragments);
                idx.path_entries.resize(d.rel_paths.size());
                for (size_t i = 0; i < d.rel_paths.size(); ++i) {
                    std::string lower = d.rel_paths[i];
                    for (auto& c : lower)
                        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                    idx.path_entries[i].first  = std::move(d.rel_paths[i]);
                    idx.path_entries[i].second = std::move(lower);
                }
                idx.engine.restore_index(
                    idx.fragments, d.tokenized_docs,
                    std::move(d.bm25.doc_lengths), d.bm25.avg_dl,
                    std::move(d.bm25.idf), std::move(d.bm25.inverted_index)
                );
                // restore 完成后立即释放分词数据
                std::vector<std::vector<std::string>>().swap(d.tokenized_docs);
            };
            restore_ga(game_assets_bp_, cached.game_assets[0]);
            restore_ga(game_assets_rp_, cached.game_assets[1]);
        }

        rebuild_event_keyword_entries();

        // CacheData 析构时 categories/game_assets 里剩余的临时内存自动释放
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

        // ── 修复①：缓存写完后立即释放 tokenized_docs ──
        // 运行时搜索完全不读 tokenized_docs，只有构建索引时用到。
        // 200MB 原文 → tokenized_docs 约 300MB，3GB 原文对应约 4.5GB，全部可释放。
        auto free_tokenized = [](CategoryIndex& idx) {
            std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
        };
        free_tokenized(api_index_);
        free_tokenized(event_index_);
        free_tokenized(enum_index_);
        free_tokenized(wiki_index_);
        free_tokenized(qumod_index_);
        free_tokenized(netease_guide_index_);

        auto free_tokenized_ga = [](GameAssetIndex& idx) {
            std::vector<std::vector<std::string>>().swap(idx.tokenized_docs);
        };
        free_tokenized_ga(game_assets_bp_);
        free_tokenized_ga(game_assets_rp_);

        std::cout << "[MCDK] tokenized_docs released after cache save" << std::endl;
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

    std::vector<SearchResult> search_category_flexible(const CategoryIndex& idx,
                                                       const std::string& keyword,
                                                       int top_k,
                                                       SearchMode mode) const {
        std::vector<std::string> primary_tokens = make_query_tokens(keyword, mode, false);
        auto results = idx.engine.search(primary_tokens, top_k);
        if (!results.empty()) return results;

        if (mode == SearchMode::ChineseOnly || mode == SearchMode::EnglishOnly) {
            return results;
        }

        std::vector<std::string> fallback_tokens = make_query_tokens(keyword, mode, true);
        if (fallback_tokens == primary_tokens) return results;
        return idx.engine.search(fallback_tokens, top_k);
    }

    std::vector<SearchResult> search_event_keyword_index(const std::string& keyword, int top_k) const {
        if (top_k <= 0) top_k = 8;

        std::string keyword_lower = to_ascii_lower(keyword);
        std::vector<SearchResult> exact_prefix_matches;
        std::vector<SearchResult> loose_matches;
        exact_prefix_matches.reserve(static_cast<size_t>(top_k));
        loose_matches.reserve(static_cast<size_t>(top_k));

        auto try_push = [&](std::vector<SearchResult>& out, const DocFragment& frag, double score) {
            for (const auto& item : out) {
                if (item.fragment == &frag) return;
            }
            out.push_back({&frag, score});
        };

        for (const auto& entry : event_keyword_entries_) {
            const auto& frag = event_index_.fragments[entry.fragment_index];
            const std::string& best_lower = entry.identifier_lower;

            if (best_lower.find(keyword_lower) == 0) {
                double score = 1000.0 - static_cast<double>(best_lower.size() - keyword_lower.size());
                try_push(exact_prefix_matches, frag, score);
                continue;
            }
            if (best_lower.find(keyword_lower) != std::string::npos) {
                double score = 600.0 - static_cast<double>(best_lower.find(keyword_lower));
                try_push(loose_matches, frag, score);
            }
        }

        auto sorter = [](const SearchResult& a, const SearchResult& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.fragment->file < b.fragment->file;
        };
        std::sort(exact_prefix_matches.begin(), exact_prefix_matches.end(), sorter);
        std::sort(loose_matches.begin(), loose_matches.end(), sorter);

        std::vector<SearchResult> merged;
        merged.reserve(static_cast<size_t>(top_k));
        for (const auto& item : exact_prefix_matches) {
            if (merged.size() >= static_cast<size_t>(top_k)) break;
            merged.push_back(item);
        }
        for (const auto& item : loose_matches) {
            if (merged.size() >= static_cast<size_t>(top_k)) break;
            merged.push_back(item);
        }

        if (!merged.empty()) return merged;
        return search_category_flexible(event_index_, keyword, top_k, SearchMode::PreferEnglishFallback);
    }

    std::vector<std::string> make_query_tokens(const std::string& text,
                                               SearchMode mode,
                                               bool fallback_phase) const {
        std::vector<std::string> tokens;

        const bool prefer_english = mode == SearchMode::EnglishOnly
            || (mode == SearchMode::PreferEnglishFallback && fallback_phase)
            || (mode == SearchMode::Auto && looks_english_query(text));

        const bool prefer_chinese = mode == SearchMode::ChineseOnly
            || (mode == SearchMode::PreferEnglishFallback && !fallback_phase)
            || (mode == SearchMode::Auto && !prefer_english);

        if (prefer_chinese) {
            tokenize(text, tokens);
            if (!tokens.empty()) return tokens;
            if (mode == SearchMode::ChineseOnly) return tokens;
        }

        if (prefer_english) {
            tokenize_en(text, tokens);
            return tokens;
        }

        tokenize_en(text, tokens);
        return tokens;
    }

    static bool looks_english_query(const std::string& text) {
        bool has_ascii_alnum = false;
        bool has_non_ascii = false;
        for (unsigned char c : text) {
            if (c >= 128) {
                has_non_ascii = true;
                continue;
            }
            if (std::isalnum(c) || c == '_') has_ascii_alnum = true;
        }
        return has_ascii_alnum && !has_non_ascii;
    }

    static bool is_precise_identifier_query(const std::string& text) {
        if (text.empty()) return false;
        bool has_upper = false;
        bool has_lower = false;
        size_t alpha_count = 0;
        for (unsigned char c : text) {
            if (!std::isalnum(c) && c != '_') return false;
            if (std::isalpha(c)) {
                ++alpha_count;
                if (std::isupper(c)) has_upper = true;
                if (std::islower(c)) has_lower = true;
            }
        }
        return alpha_count >= 12 && has_upper && has_lower;
    }

    static std::string to_ascii_lower(const std::string& text) {
        std::string out = text;
        for (auto& c : out) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return out;
    }

    static std::string extract_best_event_identifier(const std::string& content) {
        std::string best;
        std::string word;
        auto flush = [&]() {
            if (word.empty()) return;
            if (std::isupper(static_cast<unsigned char>(word[0])) && word.find("Event") != std::string::npos) {
                if (word.size() > best.size()) best = word;
            }
            word.clear();
        };

        for (unsigned char c : content) {
            if (std::isalnum(c) || c == '_') {
                word.push_back(static_cast<char>(c));
            } else {
                flush();
            }
        }
        flush();
        return best;
    }

    void rebuild_event_keyword_entries() {
        event_keyword_entries_.clear();
        event_keyword_entries_.reserve(event_index_.fragments.size());
        for (size_t i = 0; i < event_index_.fragments.size(); ++i) {
            std::string best = extract_best_event_identifier(event_index_.fragments[i].content);
            if (best.empty()) continue;
            event_keyword_entries_.push_back({i, to_ascii_lower(best)});
        }
    }

    static void normalize_tokens(std::vector<std::string>& tokens) {
        std::vector<std::string> normalized;
        normalized.reserve(tokens.size());
        std::unordered_set<std::string> seen;
        for (auto& token : tokens) {
            if (token.empty()) continue;
            std::string lowered;
            lowered.reserve(token.size());
            for (unsigned char c : token) {
                if (c < 128) lowered.push_back(static_cast<char>(std::tolower(c)));
                else lowered.push_back(static_cast<char>(c));
            }
            if (seen.insert(lowered).second) {
                normalized.push_back(std::move(lowered));
            }
        }
        tokens.swap(normalized);
    }

    static std::string make_asset_path_snippet(const std::string& rel_path) {
        return std::string("[PATH MATCH] ") + rel_path;
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
    void load_stop_words(const std::filesystem::path& path) {
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

    void load_knowledge_parallel() {
        namespace fs = std::filesystem;
        if (!fs::exists(knowledge_dir_)) {
            std::cerr << "[MCDK] knowledge dir not found: " << mcdk::path::to_utf8(knowledge_dir_) << std::endl;
            return;
        }

        std::vector<std::pair<fs::path, std::string>> md_files;
        std::vector<std::pair<fs::path, std::string>> ga_files;

        // 先完成路径扫描，再按 Markdown / GameAssets 分开处理，便于后续控制并发策略。
        for (const auto& entry : fs::recursive_directory_iterator(knowledge_dir_)) {
            if (!entry.is_regular_file()) continue;
            std::string rel_path = mcdk::path::to_utf8(fs::relative(entry.path(), knowledge_dir_));

            if (rel_path.find("GameAssets/") == 0 || rel_path.find("GameAssets\\") == 0) {
                std::string assets_rel = rel_path.substr(std::string("GameAssets/").size());
                for (auto& c : assets_rel) if (c == '\\') c = '/';
                if (assets_rel.find("behavior_packs/") == 0 || assets_rel.find("resource_packs/") == 0)
                    ga_files.push_back({entry.path(), assets_rel});
                continue;
            }
            auto ext = mcdk::path::to_utf8(entry.path().extension());
            if (ext == ".md" || ext == ".MD")
                md_files.push_back({entry.path(), rel_path});
        }

        std::cout << "[MCDK] 扫描完成: " << md_files.size() << " md, "
                  << ga_files.size() << " game asset 文件" << std::endl;

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

        // jieba 相关分词阶段仍保持单线程，减少额外同步复杂度。
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
        rebuild_event_keyword_entries();
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
