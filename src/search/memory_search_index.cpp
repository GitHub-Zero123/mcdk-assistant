#include "search/memory_search_index.hpp"

#include "common/path_utils.hpp"
#include "search/search_text.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace mcdk {

namespace {

std::string lower_ascii(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return value;
}

std::string extension_of_pattern(const std::string& pattern) {
    const auto dot = pattern.find_last_of('.');
    if (dot == std::string::npos) return {};
    std::string ext = pattern.substr(dot);
    ext.erase(std::remove(ext.begin(), ext.end(), '*'), ext.end());
    return lower_ascii(ext);
}

} // namespace

MemorySearchIndex::MemorySearchIndex(std::filesystem::path dicts_dir, std::string mode)
    : dicts_dir_(std::move(dicts_dir))
    , mode_(std::move(mode)) {
    stop_words_ = search_text::load_stop_words(dicts_dir_ / "stop_words.utf8");
}

void MemorySearchIndex::add_doc(std::string id, std::string text, std::string file, int line_start) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file.empty()) file = id;
    fragments_.push_back({std::move(text), std::move(file), line_start, line_start});
    state_ = State::Dirty;
}

void MemorySearchIndex::set_dir_source(DirSource source) {
    std::lock_guard<std::mutex> lock(mutex_);
    dir_source_ = std::make_unique<DirSource>(std::move(source));
    state_ = State::Created;
}

void MemorySearchIndex::build() {
    std::lock_guard<std::mutex> lock(mutex_);
    build_locked();
}

void MemorySearchIndex::invalidate() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = State::Dirty;
}

std::vector<SearchResult> MemorySearchIndex::search(const std::string& keyword, int top_k) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != State::Ready) {
        build_locked();
    }

    std::vector<std::string> tokens;
    tokenize_text(keyword, tokens);
    if (tokens.empty()) return {};
    return engine_.search(tokens, top_k);
}

size_t MemorySearchIndex::doc_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return fragments_.size();
}

void MemorySearchIndex::ensure_jieba() {
    if (jieba_) return;
    jieba_ = search_text::make_jieba(dicts_dir_);
}

void MemorySearchIndex::build_locked() {
    state_ = State::Building;
    try {
        if (dir_source_ && (state_ == State::Building || fragments_.empty())) {
            fragments_.clear();
            load_dir_source_locked();
        }

        tokenized_docs_.clear();
        tokenized_docs_.resize(fragments_.size());
        for (size_t i = 0; i < fragments_.size(); ++i) {
            tokenize_text(fragments_[i].content, tokenized_docs_[i]);
        }
        engine_.build_index(fragments_, tokenized_docs_);
        state_ = State::Ready;
        last_error_.clear();
    } catch (const std::exception& e) {
        state_ = State::Failed;
        last_error_ = e.what();
        throw;
    }
}

void MemorySearchIndex::load_dir_source_locked() {
    if (!dir_source_) return;
    namespace fs = std::filesystem;
    if (!fs::exists(dir_source_->root)) return;

    for (const auto& entry : fs::recursive_directory_iterator(dir_source_->root)) {
        if (!entry.is_regular_file()) continue;
        std::string rel = mcdk::path::to_utf8(fs::relative(entry.path(), dir_source_->root));
        for (char& c : rel) {
            if (c == '\\') c = '/';
        }
        if (!path_matches_globs(rel, dir_source_->globs)) continue;

        std::ifstream ifs(entry.path(), std::ios::binary);
        if (!ifs.is_open()) continue;
        std::ostringstream ss;
        ss << ifs.rdbuf();
        add_file_fragments_locked(entry.path(), rel, ss.str());
    }
}

void MemorySearchIndex::add_file_fragments_locked(const std::filesystem::path&,
                                                  const std::string& rel_path,
                                                  const std::string& content) {
    std::istringstream iss(content);
    std::ostringstream current;
    std::string line;
    int line_num = 0;
    int fragment_start = 1;
    bool has_content = false;

    auto flush = [&]() {
        std::string text = current.str();
        if (!text.empty() && has_content) {
            fragments_.push_back({std::move(text), rel_path, fragment_start, line_num});
        }
        current.str("");
        current.clear();
        has_content = false;
        fragment_start = line_num + 1;
    };

    while (std::getline(iss, line)) {
        ++line_num;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && line[0] == '#') {
            size_t level = 0;
            while (level < line.size() && line[level] == '#') ++level;
            if (level >= 2 && level <= 4) {
                flush();
                fragment_start = line_num;
            }
        }
        current << line << '\n';
        if (!line.empty()) has_content = true;
    }
    flush();
}

void MemorySearchIndex::tokenize_text(const std::string& text, std::vector<std::string>& tokens) const {
    const bool english = mode_ == "en" || (mode_ == "auto" && search_text::looks_english_query(text));
    if (english) {
        search_text::tokenize_en(text, tokens);
    } else {
        const_cast<MemorySearchIndex*>(this)->ensure_jieba();
        search_text::tokenize_zh(*jieba_, stop_words_, text, tokens);
    }
}

bool MemorySearchIndex::path_matches_globs(const std::string& rel, const std::vector<std::string>& globs) {
    if (globs.empty()) {
        std::string lower = lower_ascii(rel);
        return lower.ends_with(".md") || lower.ends_with(".txt");
    }
    std::string lower = lower_ascii(rel);
    for (const auto& glob : globs) {
        std::string ext = extension_of_pattern(glob);
        if (!ext.empty() && lower.ends_with(ext)) return true;
        std::string needle = lower_ascii(glob);
        needle.erase(std::remove(needle.begin(), needle.end(), '*'), needle.end());
        if (!needle.empty() && lower.find(needle) != std::string::npos) return true;
    }
    return false;
}

} // namespace mcdk
