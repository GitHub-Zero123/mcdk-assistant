#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>

namespace mcdk {

struct DocFragment {
    std::string content;
    std::string file;
    int         line_start;
    int         line_end;
};

struct SearchResult {
    const DocFragment* fragment;
    double             score;
};

class BM25Engine {
public:
    static constexpr double K1 = 1.5;
    static constexpr double B  = 0.75;

    // 倒排表条目：doc_id + 预计算的 TF
    struct Posting {
        size_t doc_id;
        int    tf;
    };

    void build_index(const std::vector<DocFragment>& fragments,
                     const std::vector<std::vector<std::string>>& tokenized_docs) {
        fragments_ = &fragments;
        size_t n = fragments.size();
        if (n == 0) return;

        doc_lengths_.resize(n);
        double total_len = 0;
        for (size_t i = 0; i < n; ++i) {
            doc_lengths_[i] = static_cast<int>(tokenized_docs[i].size());
            total_len += doc_lengths_[i];
        }
        avg_dl_ = total_len / static_cast<double>(n);
        num_docs_ = n;

        idf_.clear();
        inverted_index_.clear();

        // 构建倒排索引，同时统计 TF
        for (size_t i = 0; i < n; ++i) {
            // 统计当前文档中每个 token 的频率
            std::unordered_map<std::string, int> tf_map;
            for (const auto& token : tokenized_docs[i]) {
                ++tf_map[token];
            }
            // 写入倒排表
            for (const auto& [term, tf] : tf_map) {
                inverted_index_[term].push_back({i, tf});
            }
        }

        for (const auto& [term, postings] : inverted_index_) {
            double df = static_cast<double>(postings.size());
            idf_[term] = std::log((num_docs_ - df + 0.5) / (df + 0.5) + 1.0);
        }
    }

    // 直接恢复已有的倒排索引，跳过重新计算（tokenized_docs 不再需要，传参仅为兼容旧调用）
    void restore_index(const std::vector<DocFragment>& fragments,
                       const std::vector<std::vector<std::string>>& /*tokenized_docs*/,
                       std::vector<int>&& doc_lengths,
                       double avg_dl,
                       std::unordered_map<std::string, double>&& idf,
                       std::unordered_map<std::string, std::vector<Posting>>&& inverted_index) {
        fragments_ = &fragments;
        num_docs_ = fragments.size();
        doc_lengths_ = std::move(doc_lengths);
        avg_dl_ = avg_dl;
        idf_ = std::move(idf);
        inverted_index_ = std::move(inverted_index);
    }

    std::vector<SearchResult> search(const std::vector<std::string>& query_tokens,
                                     int top_k = -1) const {
        if (!fragments_ || num_docs_ == 0) return {};

        std::vector<std::string> unique_tokens;
        unique_tokens.reserve(query_tokens.size());
        std::unordered_set<std::string> seen_tokens;
        for (const auto& token : query_tokens) {
            if (token.empty()) continue;
            if (seen_tokens.insert(token).second) {
                unique_tokens.push_back(token);
            }
        }
        if (unique_tokens.empty()) return {};

        // 稀疏累积：只对命中文档累加得分，避免全量数组初始化+遍历
        std::unordered_map<size_t, double> score_map;
        score_map.reserve(std::min(num_docs_, size_t(4096)));

        const bool has_top_k = top_k > 0;
        const size_t desired_results = has_top_k ? static_cast<size_t>(top_k) : size_t(64);
        const size_t max_accumulate_docs = std::max<size_t>(desired_results * 48, 2048);

        for (const auto& qt : unique_tokens) {
            auto idf_it = idf_.find(qt);
            if (idf_it == idf_.end()) continue;
            double idf_val = idf_it->second;

            auto idx_it = inverted_index_.find(qt);
            if (idx_it == inverted_index_.end()) continue;

            // 对超长 posting list 限制处理量（高频词 IDF 已经很低，继续扫描收益极小）
            const auto& postings = idx_it->second;
            const bool extremely_common = postings.size() > num_docs_ / 3;
            const bool low_signal_term   = qt.size() <= 3 || idf_val < 0.12;
            const size_t MAX_POSTING = extremely_common
                ? (low_signal_term ? 256 : 1024)
                : (low_signal_term ? 1024 : 4096);
            size_t pcount = std::min(postings.size(), MAX_POSTING);

            for (size_t pi = 0; pi < pcount; ++pi) {
                const auto& posting = postings[pi];
                double dl = static_cast<double>(doc_lengths_[posting.doc_id]);
                double tf = static_cast<double>(posting.tf);
                double tf_norm = (tf * (K1 + 1.0))
                    / (tf + K1 * (1.0 - B + B * dl / avg_dl_));
                score_map[posting.doc_id] += idf_val * tf_norm;

                if (score_map.size() >= max_accumulate_docs && low_signal_term) {
                    break;
                }
            }

            if (score_map.size() >= max_accumulate_docs && low_signal_term) {
                continue;
            }
        }

        std::vector<SearchResult> results;
        results.reserve(score_map.size());
        for (const auto& [doc_id, score] : score_map) {
            results.push_back({&(*fragments_)[doc_id], score});
        }

        if (top_k > 0 && static_cast<size_t>(top_k) < results.size()) {
            // partial_sort 仅排前 top_k 个，比全排序快
            std::partial_sort(results.begin(),
                              results.begin() + static_cast<ptrdiff_t>(top_k),
                              results.end(),
                              [](const SearchResult& a, const SearchResult& b) {
                                  return a.score > b.score;
                              });
            results.resize(static_cast<size_t>(top_k));
        } else {
            std::sort(results.begin(), results.end(),
                [](const SearchResult& a, const SearchResult& b) {
                    return a.score > b.score;
                });
        }

        return results;
    }

    size_t doc_count() const { return num_docs_; }

    // 用于序列化访问
    const std::vector<int>& doc_lengths() const { return doc_lengths_; }
    double avg_dl() const { return avg_dl_; }
    const std::unordered_map<std::string, double>& idf() const { return idf_; }
    const std::unordered_map<std::string, std::vector<Posting>>& inverted_index() const { return inverted_index_; }

private:
    const std::vector<DocFragment>*              fragments_ = nullptr;
    std::vector<int>                             doc_lengths_;
    double                                       avg_dl_ = 0;
    size_t                                       num_docs_ = 0;
    std::unordered_map<std::string, double>                idf_;
    std::unordered_map<std::string, std::vector<Posting>>  inverted_index_;
};

} // namespace mcdk
