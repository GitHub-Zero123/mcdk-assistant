#pragma once

#include "search/bm25.hpp"
#include "common/path_utils.hpp"
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

inline std::string fs_native_narrow(const std::filesystem::path& path) {
    return path.string();
}

// 索引缓存文件格式 (v5):
// [magic: 8B] [version: 4B] [fingerprint] [data...]
// v5: 倒排表存储 (doc_id, tf) 对而非仅 doc_id，搜索时 O(1) 取 TF
// GameAssets: entries 只存 rel_path，content 直接从 fragments 引用

class IndexCache {
public:
    static constexpr char     MAGIC[8] = {'M','C','D','K','I','D','X','\0'};
    static constexpr uint32_t VERSION  = 5;

    // 指纹只看几个高价值目录的 mtime，避免每次启动全量扫描文件元数据。
    static std::string compute_fingerprint(const std::filesystem::path& knowledge_dir) {
        namespace fs = std::filesystem;
        static const char* WATCH[] = {
            "ModAPI", "BedrockWiki", "NeteaseGuide", "QuModDocs",
            "GameAssets/behavior_packs", "GameAssets/resource_packs"
        };
        uint64_t hash = 14695981039346656037ULL;
        for (const auto* sub : WATCH) {
            fs::path p = knowledge_dir / sub;
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

    static bool save(const std::filesystem::path& cache_path,
                     const std::string& fingerprint,
                     const std::vector<CatIndexRef>&  cat_refs,
                     const std::vector<GameIndexRef>& ga_refs)
    {
        // 这里仍用 C stdio，主要是为了保持序列化实现简单且可控。
        std::string cache_path_text = fs_native_narrow(cache_path);
        FILE* fp = std::fopen(cache_path_text.c_str(), "wb");
        if (!fp) {
            std::cerr << "[MCDK] cache: cannot open for writing: " << cache_path_text << std::endl;
            return false;
        }
        static constexpr size_t WRITE_BUF = 64 * 1024;
        char wbuf[WRITE_BUF];
        std::setvbuf(fp, wbuf, _IOFBF, WRITE_BUF);

        std::fwrite(MAGIC, 1, 8, fp);
        write_u32(fp, VERSION);
        write_string(fp, fingerprint);

        write_u32(fp, static_cast<uint32_t>(cat_refs.size()));
        for (const auto& r : cat_refs) {
            write_fragments(fp, *r.fragments);
            write_tokenized(fp, *r.tokenized_docs);
            write_bm25(fp, *r.engine);
        }

        write_u32(fp, static_cast<uint32_t>(ga_refs.size()));
        for (const auto& r : ga_refs) {
            write_u32(fp, static_cast<uint32_t>(r.rel_paths->size()));
            for (const auto& rp : *r.rel_paths)
                write_string(fp, rp);
            write_fragments(fp, *r.fragments);
            write_tokenized(fp, *r.tokenized_docs);
            write_bm25(fp, *r.engine);
        }

        bool ok = std::fflush(fp) == 0 && std::ferror(fp) == 0;
        long written = std::ftell(fp);
        std::fclose(fp);
        if (ok && written > 0) {
            std::cout << "[MCDK] cache: saved " << (written / 1024 / 1024)
                      << " MB to " << cache_path_text << std::endl;
            return true;
        }
        std::cerr << "[MCDK] cache: write error" << std::endl;
        return false;
    }

    static bool load(const std::filesystem::path& cache_path,
                     const std::string& expected_fp,
                     CacheData& out,
                     bool skip_fingerprint_check = false)
    {
        // cache-only 模式会跳过 fingerprint 校验，其它模式仍要求目录指纹一致。
        std::string cache_path_text = fs_native_narrow(cache_path);
        FILE* fp = std::fopen(cache_path_text.c_str(), "rb");
        if (!fp) return false;

        static constexpr size_t READ_BUF = 64 * 1024;
        char rbuf[READ_BUF];
        std::setvbuf(fp, rbuf, _IOFBF, READ_BUF);

        std::fseek(fp, 0, SEEK_END);
        long fsize = std::ftell(fp);
        std::fseek(fp, 0, SEEK_SET);
        if (fsize < 16) { std::fclose(fp); return false; }

        char magic[8];
        if (std::fread(magic, 1, 8, fp) != 8 || std::memcmp(magic, MAGIC, 8) != 0) {
            std::cerr << "[MCDK] cache: bad magic" << std::endl;
            std::fclose(fp); return false;
        }

        uint32_t ver = fread_u32(fp);
        if (ver != VERSION) {
            std::cout << "[MCDK] cache: version " << ver << " != " << VERSION << ", rebuilding" << std::endl;
            std::fclose(fp); return false;
        }

        out.fingerprint = fread_string(fp);
        if (!skip_fingerprint_check && out.fingerprint != expected_fp) {
            std::cout << "[MCDK] cache: fingerprint mismatch, rebuilding" << std::endl;
            std::fclose(fp); return false;
        }

        uint32_t cat_n = fread_u32(fp);
        out.categories.resize(cat_n);
        for (uint32_t i = 0; i < cat_n; ++i) {
            if (!fread_fragments(fp, out.categories[i].fragments)) { std::fclose(fp); return false; }
            if (!fread_tokenized(fp, out.categories[i].tokenized_docs)) { std::fclose(fp); return false; }
            if (!fread_bm25(fp, out.categories[i].bm25)) { std::fclose(fp); return false; }
        }

        uint32_t ga_n = fread_u32(fp);
        out.game_assets.resize(ga_n);
        for (uint32_t i = 0; i < ga_n; ++i) {
            uint32_t rp_n = fread_u32(fp);
            out.game_assets[i].rel_paths.resize(rp_n);
            for (uint32_t j = 0; j < rp_n; ++j)
                out.game_assets[i].rel_paths[j] = fread_string(fp);
            if (!fread_fragments(fp, out.game_assets[i].fragments)) { std::fclose(fp); return false; }
            if (!fread_tokenized(fp, out.game_assets[i].tokenized_docs)) { std::fclose(fp); return false; }
            if (!fread_bm25(fp, out.game_assets[i].bm25)) { std::fclose(fp); return false; }
        }

        bool ok = std::ferror(fp) == 0;
        std::fclose(fp);
        return ok;
    }

private:
    // ══════════════════════════════════════════════════════════════════
    // ── 流式写入辅助（直接 fwrite，无中间 buffer）──
    // ══════════════════════════════════════════════════════════════════

    static void write_u32(FILE* fp, uint32_t v) {
        std::fwrite(&v, 4, 1, fp);
    }
    static void write_u64(FILE* fp, uint64_t v) {
        std::fwrite(&v, 8, 1, fp);
    }
    static void write_double(FILE* fp, double v) {
        std::fwrite(&v, 8, 1, fp);
    }
    static void write_string(FILE* fp, const std::string& s) {
        uint32_t len = static_cast<uint32_t>(s.size());
        std::fwrite(&len, 4, 1, fp);
        if (len > 0) std::fwrite(s.data(), 1, len, fp);
    }

    static void write_fragments(FILE* fp, const std::vector<DocFragment>& frags) {
        write_u32(fp, static_cast<uint32_t>(frags.size()));
        for (const auto& f : frags) {
            write_string(fp, f.content);
            write_string(fp, f.file);
            write_u32(fp, static_cast<uint32_t>(f.line_start));
            write_u32(fp, static_cast<uint32_t>(f.line_end));
        }
    }

    static void write_tokenized(FILE* fp, const std::vector<std::vector<std::string>>& docs) {
        write_u32(fp, static_cast<uint32_t>(docs.size()));
        for (const auto& doc : docs) {
            write_u32(fp, static_cast<uint32_t>(doc.size()));
            for (const auto& tok : doc) write_string(fp, tok);
        }
    }

    static void write_bm25(FILE* fp, const BM25Engine& e) {
        const auto& dl = e.doc_lengths();
        write_u32(fp, static_cast<uint32_t>(dl.size()));
        for (int v : dl) write_u32(fp, static_cast<uint32_t>(v));

        write_double(fp, e.avg_dl());

        const auto& idf = e.idf();
        write_u32(fp, static_cast<uint32_t>(idf.size()));
        for (const auto& [term, val] : idf) {
            write_string(fp, term);
            write_double(fp, val);
        }

        const auto& inv = e.inverted_index();
        write_u32(fp, static_cast<uint32_t>(inv.size()));
        for (const auto& [term, postings] : inv) {
            write_string(fp, term);
            write_u32(fp, static_cast<uint32_t>(postings.size()));
            for (const auto& posting : postings) {
                write_u64(fp, static_cast<uint64_t>(posting.doc_id));
                write_u32(fp, static_cast<uint32_t>(posting.tf));
            }
        }
    }

    // ══════════════════════════════════════════════════════════════════
    // ── 流式读取辅助（直接 fread，无整块 buffer）──
    // ══════════════════════════════════════════════════════════════════

    static uint32_t fread_u32(FILE* fp) {
        uint32_t v = 0;
        std::fread(&v, 4, 1, fp);
        return v;
    }
    static uint64_t fread_u64(FILE* fp) {
        uint64_t v = 0;
        std::fread(&v, 8, 1, fp);
        return v;
    }
    static double fread_double(FILE* fp) {
        double v = 0.0;
        std::fread(&v, 8, 1, fp);
        return v;
    }
    static std::string fread_string(FILE* fp) {
        uint32_t len = fread_u32(fp);
        if (len == 0) return {};
        std::string s(len, '\0');
        if (std::fread(s.data(), 1, len, fp) != len) return {};
        return s;
    }

    static bool fread_fragments(FILE* fp, std::vector<DocFragment>& frags) {
        uint32_t n = fread_u32(fp);
        frags.resize(n);
        for (uint32_t i = 0; i < n; ++i) {
            frags[i].content    = fread_string(fp);
            frags[i].file       = fread_string(fp);
            frags[i].line_start = static_cast<int>(fread_u32(fp));
            frags[i].line_end   = static_cast<int>(fread_u32(fp));
        }
        return std::ferror(fp) == 0;
    }

    static bool fread_tokenized(FILE* fp, std::vector<std::vector<std::string>>& docs) {
        uint32_t n = fread_u32(fp);
        docs.resize(n);
        for (uint32_t i = 0; i < n; ++i) {
            uint32_t m = fread_u32(fp);
            docs[i].resize(m);
            for (uint32_t j = 0; j < m; ++j)
                docs[i][j] = fread_string(fp);
        }
        return std::ferror(fp) == 0;
    }

    static bool fread_bm25(FILE* fp, BM25State& s) {
        uint32_t dl_n = fread_u32(fp);
        s.doc_lengths.resize(dl_n);
        for (uint32_t i = 0; i < dl_n; ++i)
            s.doc_lengths[i] = static_cast<int>(fread_u32(fp));

        s.avg_dl = fread_double(fp);

        uint32_t idf_n = fread_u32(fp);
        s.idf.reserve(idf_n);
        for (uint32_t i = 0; i < idf_n; ++i) {
            std::string term = fread_string(fp);
            double val = fread_double(fp);
            s.idf[std::move(term)] = val;
        }

        uint32_t inv_n = fread_u32(fp);
        s.inverted_index.reserve(inv_n);
        for (uint32_t i = 0; i < inv_n; ++i) {
            std::string term = fread_string(fp);
            uint32_t post_n = fread_u32(fp);
            std::vector<BM25Engine::Posting> postings(post_n);
            for (uint32_t j = 0; j < post_n; ++j) {
                postings[j].doc_id = static_cast<size_t>(fread_u64(fp));
                postings[j].tf     = static_cast<int>(fread_u32(fp));
            }
            s.inverted_index[std::move(term)] = std::move(postings);
        }
        return std::ferror(fp) == 0;
    }
};

} // namespace mcdk
