#pragma once

#include "search/bm25.hpp"

#include <cppjieba/Jieba.hpp>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace mcdk {

class MemorySearchIndex {
public:
    struct DirSource {
        std::filesystem::path root;
        std::vector<std::string> globs;
        std::string chunk = "markdown_heading";
    };

    explicit MemorySearchIndex(std::filesystem::path dicts_dir, std::string mode = "zh");

    void add_doc(std::string id, std::string text, std::string file = {}, int line_start = 1);
    void set_dir_source(DirSource source);
    void build();
    void invalidate();

    std::vector<SearchResult> search(const std::string& keyword, int top_k = 6);
    size_t doc_count() const;

private:
    enum class State {
        Created,
        Building,
        Ready,
        Dirty,
        Failed,
    };

    std::filesystem::path dicts_dir_;
    std::string mode_;
    std::unique_ptr<cppjieba::Jieba> jieba_;
    std::unordered_set<std::string> stop_words_;

    mutable std::mutex mutex_;
    State state_ = State::Created;
    std::string last_error_;
    std::vector<DocFragment> fragments_;
    std::vector<std::vector<std::string>> tokenized_docs_;
    BM25Engine engine_;
    std::unique_ptr<DirSource> dir_source_;

    void ensure_jieba();
    void build_locked();
    void load_dir_source_locked();
    void add_file_fragments_locked(const std::filesystem::path& path,
                                   const std::string& rel_path,
                                   const std::string& content);
    void tokenize_text(const std::string& text, std::vector<std::string>& tokens) const;
    static bool path_matches_globs(const std::string& rel, const std::vector<std::string>& globs);
};

} // namespace mcdk
