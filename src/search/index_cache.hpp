#pragma once

#include "search/bm25.hpp"
#include "common/path_utils.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#ifndef _WIN32
#include <sys/types.h>
#endif

namespace mcdk {

inline std::string fs_native_narrow(const std::filesystem::path& path) {
    return path.string();
}

// 索引缓存文件格式 (v6):
// [magic: 8B] [version: 4B] [fingerprint] [section table] [section data...]
// 每个分类都是独立 section，运行时先读目录，再按需 seek 并恢复对应索引。
// tokenized_docs 只在构建期使用，不写入运行时缓存。
class IndexCache {
public:
    static constexpr char     MAGIC[8] = {'M','C','D','K','I','D','X','\0'};
    static constexpr uint32_t VERSION  = 6;

    enum SectionKind : uint32_t {
        SECTION_API = 1,
        SECTION_EVENT,
        SECTION_ENUM,
        SECTION_WIKI,
        SECTION_QUMOD,
        SECTION_NETEASE_GUIDE,
        SECTION_BEDROCK_DEV,
        SECTION_GAME_ASSETS_BP,
        SECTION_GAME_ASSETS_RP,
    };

    static constexpr uint32_t CATEGORY_SECTION_COUNT = 7;
    static constexpr uint32_t GAME_ASSET_SECTION_COUNT = 2;

    struct SectionInfo {
        uint32_t kind = 0;
        uint64_t offset = 0;
        uint64_t length = 0;
        uint64_t item_count = 0;
    };

    struct Manifest {
        std::string fingerprint;
        uint64_t file_size = 0;
        std::vector<SectionInfo> sections;

        const SectionInfo* find(uint32_t kind) const {
            for (const auto& section : sections) {
                if (section.kind == kind) return &section;
            }
            return nullptr;
        }
    };

    // 指纹只看几个高价值目录的 mtime，避免每次启动全量扫描文件元数据。
    static std::string compute_fingerprint(const std::filesystem::path& knowledge_dir) {
        namespace fs = std::filesystem;
        static const char* WATCH[] = {
            "ModAPI", "BedrockWiki", "BedrockDev", "NeteaseGuide", "QuModDocs",
            "GameAssets/behavior_packs", "GameAssets/resource_packs"
        };
        uint64_t hash = 14695981039346656037ULL;
        for (const auto* sub : WATCH) {
            fs::path p = knowledge_dir / sub;
            std::string s = std::string(sub) + ":";
            if (fs::exists(p)) {
                const auto ticks = static_cast<int64_t>(fs::last_write_time(p).time_since_epoch().count());
                s += std::to_string(ticks);
            } else {
                s += "0";
            }
            for (char c : s) { hash ^= static_cast<uint8_t>(c); hash *= 1099511628211ULL; }
        }
        char buf[17];
        snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(hash));
        return std::string(buf);
    }

    struct BM25State {
        std::vector<int>                                                   doc_lengths;
        double                                                             avg_dl = 0.0;
        std::unordered_map<std::string, double>                            idf;
        std::unordered_map<std::string, std::vector<BM25Engine::Posting>> inverted_index;
    };

    struct CatData {
        std::vector<DocFragment> fragments;
        BM25State                bm25;
    };

    struct GameAssetData {
        std::vector<std::string> rel_paths;
        std::vector<DocFragment> fragments;
        BM25State                bm25;
    };

    struct CatIndexRef {
        const std::vector<DocFragment>* fragments;
        const BM25Engine*               engine;
    };

    struct GameIndexRef {
        const std::vector<std::string>* rel_paths;
        const std::vector<DocFragment>* fragments;
        const BM25Engine*               engine;
    };

    static bool save(const std::filesystem::path& cache_path,
                     const std::string& fingerprint,
                     const std::vector<CatIndexRef>& cat_refs,
                     const std::vector<GameIndexRef>& ga_refs) {
        if (cat_refs.size() != CATEGORY_SECTION_COUNT ||
            ga_refs.size() != GAME_ASSET_SECTION_COUNT) {
            std::cerr << "[MCDK] cache: unexpected section count" << std::endl;
            return false;
        }

        std::string cache_path_text = fs_native_narrow(cache_path);
        FILE* fp = std::fopen(cache_path_text.c_str(), "wb+");
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

        std::vector<SectionInfo> sections;
        sections.reserve(cat_refs.size() + ga_refs.size());
        for (uint32_t i = 0; i < cat_refs.size(); ++i) {
            sections.push_back({SECTION_API + i, 0, 0, cat_refs[i].fragments->size()});
        }
        for (uint32_t i = 0; i < ga_refs.size(); ++i) {
            sections.push_back({SECTION_GAME_ASSETS_BP + i, 0, 0, ga_refs[i].rel_paths->size()});
        }

        write_u32(fp, static_cast<uint32_t>(sections.size()));
        const uint64_t table_offset = file_tell(fp);
        for (const auto& section : sections) write_section_info(fp, section);

        size_t section_index = 0;
        for (const auto& ref : cat_refs) {
            auto& section = sections[section_index++];
            section.offset = file_tell(fp);
            write_fragments(fp, *ref.fragments);
            write_bm25(fp, *ref.engine);
            section.length = file_tell(fp) - section.offset;
        }
        for (const auto& ref : ga_refs) {
            auto& section = sections[section_index++];
            section.offset = file_tell(fp);
            write_u32(fp, static_cast<uint32_t>(ref.rel_paths->size()));
            for (const auto& rel_path : *ref.rel_paths) write_string(fp, rel_path);
            write_fragments(fp, *ref.fragments);
            write_bm25(fp, *ref.engine);
            section.length = file_tell(fp) - section.offset;
        }

        const uint64_t file_size = file_tell(fp);
        bool ok = file_seek(fp, table_offset);
        for (const auto& section : sections) write_section_info(fp, section);
        ok = ok && std::fflush(fp) == 0 && std::ferror(fp) == 0;
        std::fclose(fp);

        if (ok && file_size > 0) {
            std::cerr << "[MCDK] cache: saved " << (file_size / 1024 / 1024)
                      << " MB to " << cache_path_text << std::endl;
            return true;
        }
        std::cerr << "[MCDK] cache: write error" << std::endl;
        return false;
    }

    static bool load_manifest(const std::filesystem::path& cache_path,
                              const std::string& expected_fp,
                              Manifest& out,
                              bool skip_fingerprint_check = false) {
        std::string cache_path_text = fs_native_narrow(cache_path);
        FILE* fp = std::fopen(cache_path_text.c_str(), "rb");
        if (!fp) return false;

        static constexpr size_t READ_BUF = 64 * 1024;
        char rbuf[READ_BUF];
        std::setvbuf(fp, rbuf, _IOFBF, READ_BUF);

        if (!file_seek_end(fp)) { std::fclose(fp); return false; }
        const uint64_t file_size = file_tell(fp);
        if (file_size < 16 || !file_seek(fp, 0)) { std::fclose(fp); return false; }

        char magic[8];
        if (std::fread(magic, 1, 8, fp) != 8 || std::memcmp(magic, MAGIC, 8) != 0) {
            std::cerr << "[MCDK] cache: bad magic" << std::endl;
            std::fclose(fp);
            return false;
        }

        const uint32_t version = fread_u32(fp);
        if (version != VERSION) {
            std::cerr << "[MCDK] cache: version " << version << " != " << VERSION
                      << ", rebuilding" << std::endl;
            std::fclose(fp);
            return false;
        }

        Manifest manifest;
        manifest.file_size = file_size;
        manifest.fingerprint = fread_string(fp);
        if (!skip_fingerprint_check && manifest.fingerprint != expected_fp) {
            std::cerr << "[MCDK] cache: fingerprint mismatch, rebuilding" << std::endl;
            std::fclose(fp);
            return false;
        }

        const uint32_t section_count = fread_u32(fp);
        if (section_count != CATEGORY_SECTION_COUNT + GAME_ASSET_SECTION_COUNT) {
            std::cerr << "[MCDK] cache: unexpected section count " << section_count << std::endl;
            std::fclose(fp);
            return false;
        }

        manifest.sections.resize(section_count);
        std::unordered_set<uint32_t> seen_kinds;
        for (auto& section : manifest.sections) {
            section = fread_section_info(fp);
            if (section.kind < SECTION_API || section.kind > SECTION_GAME_ASSETS_RP ||
                !seen_kinds.insert(section.kind).second ||
                section.offset > file_size || section.length > file_size - section.offset) {
                std::cerr << "[MCDK] cache: invalid section table" << std::endl;
                std::fclose(fp);
                return false;
            }
        }

        const uint64_t data_begin = file_tell(fp);
        for (const auto& section : manifest.sections) {
            if (section.offset < data_begin) {
                std::cerr << "[MCDK] cache: section overlaps header" << std::endl;
                std::fclose(fp);
                return false;
            }
        }

        const bool ok = std::ferror(fp) == 0;
        std::fclose(fp);
        if (ok) out = std::move(manifest);
        return ok;
    }

    static bool load_category(const std::filesystem::path& cache_path,
                              const Manifest& manifest,
                              uint32_t kind,
                              CatData& out) {
        const SectionInfo* section = manifest.find(kind);
        if (!section || kind < SECTION_API || kind > SECTION_BEDROCK_DEV) return false;

        FILE* fp = open_section(cache_path, manifest, *section);
        if (!fp) return false;
        CatData data;
        const bool ok = fread_fragments(fp, data.fragments) &&
                        fread_bm25(fp, data.bm25) &&
                        section_consumed(fp, *section) &&
                        data.fragments.size() == section->item_count;
        std::fclose(fp);
        if (ok) out = std::move(data);
        return ok;
    }

    static bool load_game_assets(const std::filesystem::path& cache_path,
                                 const Manifest& manifest,
                                 uint32_t kind,
                                 GameAssetData& out) {
        const SectionInfo* section = manifest.find(kind);
        if (!section || kind < SECTION_GAME_ASSETS_BP || kind > SECTION_GAME_ASSETS_RP) return false;

        FILE* fp = open_section(cache_path, manifest, *section);
        if (!fp) return false;
        GameAssetData data;
        const uint32_t path_count = fread_u32(fp);
        data.rel_paths.resize(path_count);
        for (auto& rel_path : data.rel_paths) rel_path = fread_string(fp);
        const bool ok = std::ferror(fp) == 0 &&
                        fread_fragments(fp, data.fragments) &&
                        fread_bm25(fp, data.bm25) &&
                        section_consumed(fp, *section) &&
                        data.rel_paths.size() == data.fragments.size() &&
                        data.rel_paths.size() == section->item_count;
        std::fclose(fp);
        if (ok) out = std::move(data);
        return ok;
    }

private:
    static bool file_seek(FILE* fp, uint64_t offset) {
#ifdef _WIN32
        return _fseeki64(fp, static_cast<__int64>(offset), SEEK_SET) == 0;
#else
        return fseeko(fp, static_cast<off_t>(offset), SEEK_SET) == 0;
#endif
    }

    static bool file_seek_end(FILE* fp) {
#ifdef _WIN32
        return _fseeki64(fp, 0, SEEK_END) == 0;
#else
        return fseeko(fp, 0, SEEK_END) == 0;
#endif
    }

    static uint64_t file_tell(FILE* fp) {
#ifdef _WIN32
        const auto position = _ftelli64(fp);
#else
        const auto position = ftello(fp);
#endif
        return position < 0 ? 0 : static_cast<uint64_t>(position);
    }

    static FILE* open_section(const std::filesystem::path& cache_path,
                              const Manifest& manifest,
                              const SectionInfo& section) {
        FILE* fp = std::fopen(fs_native_narrow(cache_path).c_str(), "rb");
        if (!fp) return nullptr;
        if (!file_seek_end(fp) || file_tell(fp) != manifest.file_size ||
            !file_seek(fp, section.offset)) {
            std::fclose(fp);
            return nullptr;
        }
        return fp;
    }

    static bool section_consumed(FILE* fp, const SectionInfo& section) {
        return std::ferror(fp) == 0 && file_tell(fp) == section.offset + section.length;
    }

    static void write_u32(FILE* fp, uint32_t value) {
        std::fwrite(&value, 4, 1, fp);
    }

    static void write_u64(FILE* fp, uint64_t value) {
        std::fwrite(&value, 8, 1, fp);
    }

    static void write_double(FILE* fp, double value) {
        std::fwrite(&value, 8, 1, fp);
    }

    static void write_string(FILE* fp, const std::string& value) {
        write_u32(fp, static_cast<uint32_t>(value.size()));
        if (!value.empty()) std::fwrite(value.data(), 1, value.size(), fp);
    }

    static void write_section_info(FILE* fp, const SectionInfo& section) {
        write_u32(fp, section.kind);
        write_u64(fp, section.offset);
        write_u64(fp, section.length);
        write_u64(fp, section.item_count);
    }

    static void write_fragments(FILE* fp, const std::vector<DocFragment>& fragments) {
        write_u32(fp, static_cast<uint32_t>(fragments.size()));
        for (const auto& fragment : fragments) {
            write_string(fp, fragment.content);
            write_string(fp, fragment.file);
            write_u32(fp, static_cast<uint32_t>(fragment.line_start));
            write_u32(fp, static_cast<uint32_t>(fragment.line_end));
        }
    }

    static void write_bm25(FILE* fp, const BM25Engine& engine) {
        const auto& doc_lengths = engine.doc_lengths();
        write_u32(fp, static_cast<uint32_t>(doc_lengths.size()));
        for (int value : doc_lengths) write_u32(fp, static_cast<uint32_t>(value));

        write_double(fp, engine.avg_dl());

        const auto& idf = engine.idf();
        write_u32(fp, static_cast<uint32_t>(idf.size()));
        for (const auto& [term, value] : idf) {
            write_string(fp, term);
            write_double(fp, value);
        }

        const auto& inverted_index = engine.inverted_index();
        write_u32(fp, static_cast<uint32_t>(inverted_index.size()));
        for (const auto& [term, postings] : inverted_index) {
            write_string(fp, term);
            write_u32(fp, static_cast<uint32_t>(postings.size()));
            for (const auto& posting : postings) {
                write_u64(fp, static_cast<uint64_t>(posting.doc_id));
                write_u32(fp, static_cast<uint32_t>(posting.tf));
            }
        }
    }

    static uint32_t fread_u32(FILE* fp) {
        uint32_t value = 0;
        std::fread(&value, 4, 1, fp);
        return value;
    }

    static uint64_t fread_u64(FILE* fp) {
        uint64_t value = 0;
        std::fread(&value, 8, 1, fp);
        return value;
    }

    static double fread_double(FILE* fp) {
        double value = 0.0;
        std::fread(&value, 8, 1, fp);
        return value;
    }

    static std::string fread_string(FILE* fp) {
        const uint32_t length = fread_u32(fp);
        if (length == 0) return {};
        std::string value(length, '\0');
        if (std::fread(value.data(), 1, length, fp) != length) return {};
        return value;
    }

    static SectionInfo fread_section_info(FILE* fp) {
        SectionInfo section;
        section.kind = fread_u32(fp);
        section.offset = fread_u64(fp);
        section.length = fread_u64(fp);
        section.item_count = fread_u64(fp);
        return section;
    }

    static bool fread_fragments(FILE* fp, std::vector<DocFragment>& fragments) {
        const uint32_t count = fread_u32(fp);
        fragments.resize(count);
        for (auto& fragment : fragments) {
            fragment.content = fread_string(fp);
            fragment.file = fread_string(fp);
            fragment.line_start = static_cast<int>(fread_u32(fp));
            fragment.line_end = static_cast<int>(fread_u32(fp));
        }
        return std::ferror(fp) == 0;
    }

    static bool fread_bm25(FILE* fp, BM25State& state) {
        const uint32_t doc_length_count = fread_u32(fp);
        state.doc_lengths.resize(doc_length_count);
        for (auto& value : state.doc_lengths) value = static_cast<int>(fread_u32(fp));

        state.avg_dl = fread_double(fp);

        const uint32_t idf_count = fread_u32(fp);
        state.idf.reserve(idf_count);
        for (uint32_t i = 0; i < idf_count; ++i) {
            std::string term = fread_string(fp);
            const double value = fread_double(fp);
            state.idf[std::move(term)] = value;
        }

        const uint32_t inverted_count = fread_u32(fp);
        state.inverted_index.reserve(inverted_count);
        for (uint32_t i = 0; i < inverted_count; ++i) {
            std::string term = fread_string(fp);
            const uint32_t posting_count = fread_u32(fp);
            std::vector<BM25Engine::Posting> postings(posting_count);
            for (auto& posting : postings) {
                posting.doc_id = static_cast<size_t>(fread_u64(fp));
                posting.tf = static_cast<int>(fread_u32(fp));
            }
            state.inverted_index[std::move(term)] = std::move(postings);
        }
        return std::ferror(fp) == 0;
    }
};

} // namespace mcdk
