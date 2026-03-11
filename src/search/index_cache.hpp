#pragma once

#include "search/bm25.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unordered_map>

namespace mcdk {

// 索引缓存文件格式 (v5):
// [magic: 8B] [version: 4B] [fingerprint] [data...]
// v5: 倒排表存储 (doc_id, tf) 对而非仅 doc_id，搜索时 O(1) 取 TF
// GameAssets: entries 只存 rel_path，content 直接从 fragments 引用

class IndexCache {
public:
    static constexpr char     MAGIC[8] = {'M','C','D','K','I','D','X','\0'};
    static constexpr uint32_t VERSION  = 5;

    // ── Fingerprint：只读几个子目录的 mtime，< 1ms ──
    static std::string compute_fingerprint(const std::string& knowledge_dir) {
        namespace fs = std::filesystem;
        static const char* WATCH[] = {
            "ModAPI", "BedrockWiki", "NeteaseGuide", "QuModDocs",
            "GameAssets/behavior_packs", "GameAssets/resource_packs"
        };
        uint64_t hash = 14695981039346656037ULL;
        for (const auto* sub : WATCH) {
            fs::path p = fs::path(knowledge_dir) / sub;
            std::string s = std::string(sub) + ":";
            if (fs::exists(p)) {
                s += std::to_string(fs::last_write_time(p).time_since_epoch().count());
            } else {
                s += "0";
            }
            for (char c : s) { hash ^= static_cast<uint8_t>(c); hash *= 1099511628211ULL; }
        }
        char buf[17];
        snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(hash));
        return std::string(buf);
    }

    // ── 数据结构 ──
    struct BM25State {
        std::vector<int>                                                doc_lengths;
        double                                                         avg_dl = 0.0;
        std::unordered_map<std::string, double>                        idf;
        std::unordered_map<std::string, std::vector<BM25Engine::Posting>> inverted_index;
    };

    struct CatData {
        std::vector<DocFragment>                 fragments;
        std::vector<std::vector<std::string>>    tokenized_docs;
        BM25State                                bm25;
    };

    // GameAssets: entries 只有 rel_path（content 与 fragments[i].content 共享）
    struct GameAssetData {
        std::vector<std::string>               rel_paths;     // entries 的 key
        std::vector<DocFragment>               fragments;
        std::vector<std::vector<std::string>>  tokenized_docs;
        BM25State                              bm25;
    };

    struct CacheData {
        std::string                fingerprint;
        std::vector<CatData>       categories;   // api, event, enum, wiki, qumod, netease_guide
        std::vector<GameAssetData> game_assets;  // bp, rp
    };

    // ── 保存接口 ──
    struct CatIndexRef {
        const std::vector<DocFragment>*                 fragments;
        const std::vector<std::vector<std::string>>*    tokenized_docs;
        const BM25Engine*                               engine;
    };
    struct GameIndexRef {
        const std::vector<std::string>*                 rel_paths;   // 只需 rel_path
        const std::vector<DocFragment>*                 fragments;   // content 在这里
        const std::vector<std::vector<std::string>>*    tokenized_docs;
        const BM25Engine*                               engine;
    };

    static bool save(const std::string& cache_path,
                     const std::string& fingerprint,
                     const std::vector<CatIndexRef>&  cat_refs,
                     const std::vector<GameIndexRef>& ga_refs)
    {
        // 先写入内存 buffer，再一次性 fwrite（减少磁盘 I/O 次数）
        std::vector<char> buf;
        buf.reserve(256 * 1024 * 1024);  // 预留 256MB

        // Header
        append_raw(buf, MAGIC, 8);
        append_u32(buf, VERSION);
        append_string(buf, fingerprint);

        // Categories
        append_u32(buf, static_cast<uint32_t>(cat_refs.size()));
        for (const auto& r : cat_refs) {
            append_fragments(buf, *r.fragments);
            append_tokenized(buf, *r.tokenized_docs);
            append_bm25(buf, *r.engine);
        }

        // GameAssets（只存 rel_path，不再存 content）
        append_u32(buf, static_cast<uint32_t>(ga_refs.size()));
        for (const auto& r : ga_refs) {
            append_u32(buf, static_cast<uint32_t>(r.rel_paths->size()));
            for (const auto& rp : *r.rel_paths)
                append_string(buf, rp);
            append_fragments(buf, *r.fragments);
            append_tokenized(buf, *r.tokenized_docs);
            append_bm25(buf, *r.engine);
        }

        // 一次性写入文件
        std::ofstream ofs(cache_path, std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "[MCDK] cache: cannot open for writing: " << cache_path << std::endl;
            return false;
        }
        ofs.write(buf.data(), static_cast<std::streamsize>(buf.size()));
        ofs.flush();
        if (ofs.good()) {
            std::cout << "[MCDK] cache: saved " << (buf.size() / 1024 / 1024)
                      << " MB to " << cache_path << std::endl;
            return true;
        }
        std::cerr << "[MCDK] cache: write error" << std::endl;
        return false;
    }

    // ── 加载：整块读入后指针解析 ──
    // skip_fingerprint_check: 仅缓存模式时跳过 fingerprint 校验
    static bool load(const std::string& cache_path,
                     const std::string& expected_fp,
                     CacheData& out,
                     bool skip_fingerprint_check = false)
    {
        // 整块读入
        std::ifstream ifs(cache_path, std::ios::binary | std::ios::ate);
        if (!ifs.is_open()) return false;
        std::streamsize fsize = ifs.tellg();
        if (fsize < 16) return false;
        ifs.seekg(0);

        std::vector<char> buf(static_cast<size_t>(fsize));
        ifs.read(buf.data(), fsize);
        if (!ifs.good()) return false;

        const char* p   = buf.data();
        const char* end = p + buf.size();

        // Magic
        if (end - p < 8 || std::memcmp(p, MAGIC, 8) != 0) {
            std::cerr << "[MCDK] cache: bad magic" << std::endl;
            return false;
        }
        p += 8;

        // Version
        uint32_t ver = read_u32(p, end);
        if (ver != VERSION) {
            std::cout << "[MCDK] cache: version " << ver << " != " << VERSION << ", rebuilding" << std::endl;
            return false;
        }

        // Fingerprint
        out.fingerprint = read_string(p, end);
        if (!skip_fingerprint_check && out.fingerprint != expected_fp) {
            std::cout << "[MCDK] cache: fingerprint mismatch, rebuilding" << std::endl;
            return false;
        }

        // Categories
        uint32_t cat_n = read_u32(p, end);
        out.categories.resize(cat_n);
        for (uint32_t i = 0; i < cat_n; ++i) {
            read_fragments(p, end, out.categories[i].fragments);
            read_tokenized(p, end, out.categories[i].tokenized_docs);
            read_bm25(p, end, out.categories[i].bm25);
            if (p > end) return false;
        }

        // GameAssets
        uint32_t ga_n = read_u32(p, end);
        out.game_assets.resize(ga_n);
        for (uint32_t i = 0; i < ga_n; ++i) {
            uint32_t rp_n = read_u32(p, end);
            out.game_assets[i].rel_paths.resize(rp_n);
            for (uint32_t j = 0; j < rp_n; ++j)
                out.game_assets[i].rel_paths[j] = read_string(p, end);
            read_fragments(p, end, out.game_assets[i].fragments);
            read_tokenized(p, end, out.game_assets[i].tokenized_docs);
            read_bm25(p, end, out.game_assets[i].bm25);
            if (p > end) return false;
        }

        return true;
    }

private:
    // ── 写入辅助（向 vector<char> append）──

    static void append_raw(std::vector<char>& buf, const void* data, size_t n) {
        const char* d = static_cast<const char*>(data);
        buf.insert(buf.end(), d, d + n);
    }
    static void append_u32(std::vector<char>& buf, uint32_t v) {
        append_raw(buf, &v, 4);
    }
    static void append_u64(std::vector<char>& buf, uint64_t v) {
        append_raw(buf, &v, 8);
    }
    static void append_double(std::vector<char>& buf, double v) {
        append_raw(buf, &v, 8);
    }
    static void append_string(std::vector<char>& buf, const std::string& s) {
        append_u32(buf, static_cast<uint32_t>(s.size()));
        if (!s.empty()) append_raw(buf, s.data(), s.size());
    }

    static void append_fragments(std::vector<char>& buf, const std::vector<DocFragment>& frags) {
        append_u32(buf, static_cast<uint32_t>(frags.size()));
        for (const auto& f : frags) {
            append_string(buf, f.content);
            append_string(buf, f.file);
            append_u32(buf, static_cast<uint32_t>(f.line_start));
            append_u32(buf, static_cast<uint32_t>(f.line_end));
        }
    }

    static void append_tokenized(std::vector<char>& buf, const std::vector<std::vector<std::string>>& docs) {
        append_u32(buf, static_cast<uint32_t>(docs.size()));
        for (const auto& doc : docs) {
            append_u32(buf, static_cast<uint32_t>(doc.size()));
            for (const auto& tok : doc) append_string(buf, tok);
        }
    }

    static void append_bm25(std::vector<char>& buf, const BM25Engine& e) {
        const auto& dl = e.doc_lengths();
        append_u32(buf, static_cast<uint32_t>(dl.size()));
        for (int v : dl) append_u32(buf, static_cast<uint32_t>(v));

        append_double(buf, e.avg_dl());

        const auto& idf = e.idf();
        append_u32(buf, static_cast<uint32_t>(idf.size()));
        for (const auto& [term, val] : idf) {
            append_string(buf, term);
            append_double(buf, val);
        }

        const auto& inv = e.inverted_index();
        append_u32(buf, static_cast<uint32_t>(inv.size()));
        for (const auto& [term, postings] : inv) {
            append_string(buf, term);
            append_u32(buf, static_cast<uint32_t>(postings.size()));
            for (const auto& posting : postings) {
                append_u64(buf, static_cast<uint64_t>(posting.doc_id));
                append_u32(buf, static_cast<uint32_t>(posting.tf));
            }
        }
    }

    // ── 读取辅助（从内存指针解析，移动 p）──

    static uint32_t read_u32(const char*& p, const char* end) {
        if (p + 4 > end) return 0;
        uint32_t v; std::memcpy(&v, p, 4); p += 4; return v;
    }
    static uint64_t read_u64(const char*& p, const char* end) {
        if (p + 8 > end) return 0;
        uint64_t v; std::memcpy(&v, p, 8); p += 8; return v;
    }
    static double read_double(const char*& p, const char* end) {
        if (p + 8 > end) return 0.0;
        double v; std::memcpy(&v, p, 8); p += 8; return v;
    }
    static std::string read_string(const char*& p, const char* end) {
        uint32_t len = read_u32(p, end);
        if (len == 0 || p + len > end) { p += len; return {}; }
        std::string s(p, len); p += len; return s;
    }

    static void read_fragments(const char*& p, const char* end, std::vector<DocFragment>& frags) {
        uint32_t n = read_u32(p, end);
        frags.resize(n);
        for (uint32_t i = 0; i < n; ++i) {
            frags[i].content    = read_string(p, end);
            frags[i].file       = read_string(p, end);
            frags[i].line_start = static_cast<int>(read_u32(p, end));
            frags[i].line_end   = static_cast<int>(read_u32(p, end));
        }
    }

    static void read_tokenized(const char*& p, const char* end,
                                std::vector<std::vector<std::string>>& docs) {
        uint32_t n = read_u32(p, end);
        docs.resize(n);
        for (uint32_t i = 0; i < n; ++i) {
            uint32_t m = read_u32(p, end);
            docs[i].resize(m);
            for (uint32_t j = 0; j < m; ++j)
                docs[i][j] = read_string(p, end);
        }
    }

    static void read_bm25(const char*& p, const char* end, BM25State& s) {
        uint32_t dl_n = read_u32(p, end);
        s.doc_lengths.resize(dl_n);
        for (uint32_t i = 0; i < dl_n; ++i)
            s.doc_lengths[i] = static_cast<int>(read_u32(p, end));

        s.avg_dl = read_double(p, end);

        uint32_t idf_n = read_u32(p, end);
        s.idf.reserve(idf_n);
        for (uint32_t i = 0; i < idf_n; ++i) {
            std::string term = read_string(p, end);
            double val = read_double(p, end);
            s.idf[std::move(term)] = val;
        }

        uint32_t inv_n = read_u32(p, end);
        s.inverted_index.reserve(inv_n);
        for (uint32_t i = 0; i < inv_n; ++i) {
            std::string term = read_string(p, end);
            uint32_t post_n = read_u32(p, end);
            std::vector<BM25Engine::Posting> postings(post_n);
            for (uint32_t j = 0; j < post_n; ++j) {
                postings[j].doc_id = static_cast<size_t>(read_u64(p, end));
                postings[j].tf     = static_cast<int>(read_u32(p, end));
            }
            s.inverted_index[std::move(term)] = std::move(postings);
        }
    }
};

} // namespace mcdk
