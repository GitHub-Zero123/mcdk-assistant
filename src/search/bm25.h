#pragma once

#include <string>
#include <vector>
#include <unordered_map>
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

    void build_index(const std::vector<DocFragment>& fragments,
                     const std::vector<std::vector<std::string>>& tokenized_docs) {
        fragments_ = &fragments;
        doc_tokens_ = &tokenized_docs;
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
        for (size_t i = 0; i < n; ++i) {
            std::unordered_map<std::string, bool> seen;
            for (const auto& token : tokenized_docs[i]) {
                if (!seen[token]) {
                    seen[token] = true;
                    inverted_index_[token].push_back(i);
                }
            }
        }

        for (const auto& [term, doc_ids] : inverted_index_) {
            double df = static_cast<double>(doc_ids.size());
            idf_[term] = std::log((num_docs_ - df + 0.5) / (df + 0.5) + 1.0);
        }
    }

    std::vector<SearchResult> search(const std::vector<std::string>& query_tokens,
                                     int top_k = -1) const {
        if (!fragments_ || num_docs_ == 0) return {};

        std::vector<double> scores(num_docs_, 0.0);

        for (const auto& qt : query_tokens) {
            auto idf_it = idf_.find(qt);
            if (idf_it == idf_.end()) continue;
            double idf_val = idf_it->second;

            auto idx_it = inverted_index_.find(qt);
            if (idx_it == inverted_index_.end()) continue;

            for (size_t doc_id : idx_it->second) {
                int tf = 0;
                for (const auto& t : (*doc_tokens_)[doc_id]) {
                    if (t == qt) ++tf;
                }
                double dl = static_cast<double>(doc_lengths_[doc_id]);
                double tf_norm = (static_cast<double>(tf) * (K1 + 1.0))
                    / (static_cast<double>(tf) + K1 * (1.0 - B + B * dl / avg_dl_));
                scores[doc_id] += idf_val * tf_norm;
            }
        }

        std::vector<SearchResult> results;
        for (size_t i = 0; i < num_docs_; ++i) {
            if (scores[i] > 0.0) {
                results.push_back({&(*fragments_)[i], scores[i]});
            }
        }

        std::sort(results.begin(), results.end(),
            [](const SearchResult& a, const SearchResult& b) {
                return a.score > b.score;
            });

        if (top_k > 0 && static_cast<size_t>(top_k) < results.size()) {
            results.resize(top_k);
        }

        return results;
    }

    size_t doc_count() const { return num_docs_; }

private:
    const std::vector<DocFragment>*              fragments_ = nullptr;
    const std::vector<std::vector<std::string>>* doc_tokens_ = nullptr;
    std::vector<int>                             doc_lengths_;
    double                                       avg_dl_ = 0;
    size_t                                       num_docs_ = 0;
    std::unordered_map<std::string, double>                idf_;
    std::unordered_map<std::string, std::vector<size_t>>   inverted_index_;
};

} // namespace mcdk
