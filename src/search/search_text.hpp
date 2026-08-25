#pragma once

#include <cppjieba/QuerySegment.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace mcdk::search_text {

inline std::unique_ptr<cppjieba::QuerySegment> make_query_segment(const std::filesystem::path& dicts_dir) {
    return std::make_unique<cppjieba::QuerySegment>(
        (dicts_dir / "jieba.dict.utf8").string(),
        (dicts_dir / "hmm_model.utf8").string(),
        (dicts_dir / "user.dict.utf8").string()
    );
}

inline std::unordered_set<std::string> load_stop_words(const std::filesystem::path& path) {
    std::unordered_set<std::string> stop_words;
    std::ifstream ifs(path);
    std::string line;
    while (std::getline(ifs, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) stop_words.insert(line);
    }
    return stop_words;
}

inline void tokenize_zh(cppjieba::QuerySegment& segment,
                        const std::unordered_set<std::string>& stop_words,
                        const std::string& text,
                        std::vector<std::string>& tokens) {
    std::vector<std::string> raw;
    segment.Cut(text, raw);
    tokens.clear();
    for (auto& w : raw) {
        if (w.empty() || w == " " || w == "\t" || w == "\n") continue;
        if (stop_words.count(w)) continue;
        tokens.push_back(std::move(w));
    }
}

inline void tokenize_en(const std::string& text, std::vector<std::string>& tokens) {
    tokens.clear();
    std::string word;
    for (unsigned char c : text) {
        if (std::isalnum(c) || c == '_') {
            word.push_back(static_cast<char>(std::tolower(c)));
        } else if (!word.empty()) {
            tokens.push_back(std::move(word));
            word.clear();
        }
    }
    if (!word.empty()) tokens.push_back(std::move(word));
}

inline bool looks_english_query(const std::string& text) {
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

inline void normalize_tokens(std::vector<std::string>& tokens) {
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

} // namespace mcdk::search_text
