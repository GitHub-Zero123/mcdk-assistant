#pragma once

#include "search/bm25.hpp"
#include "common/path_utils.hpp"
#include <zstd.h>
#include <algorithm>
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

// 索引缓存文件格式 (v7):
// [magic: 8B] [version: 4B] [fingerprint] [section count: 4B] [section table] [section data...]
// 每个分类都是独立 section，运行时先读目录，再按需 seek 并恢复对应索引。
// tokenized_docs 只在构建期使用，不写入运行时缓存。
//
// v7 相比 v6：section 表增加 codec / level / raw_length 三个字段，段数据按
// COMPRESS_BLOCK 切块，每块压成一个独立 zstd 帧。因为 zstd 帧是自描述的，解码侧
// 并不需要知道 level —— 它只作为诊断信息记录，所以后续调整压缩等级不必提升
// VERSION；将来更换编码方式也只需分配一个新的 codec 值，旧版本会按「未知 codec」
// 拒绝该文件，而不是误读。
class IndexCache {
public:
    static constexpr char     MAGIC[8] = {'M','C','D','K','I','D','X','\0'};
    static constexpr uint32_t VERSION  = 7;

    enum SectionCodec : uint32_t {
        CODEC_RAW  = 0,  // 段数据原样存储
        CODEC_ZSTD = 1,  // 段数据按块压缩，每块为 [comp_size u32][raw_size u32][zstd frame]
    };

    // 写入侧参数，已记录进 section 表，调整时不需要动 VERSION。
    // 等级 12 是体积与编译耗时的平衡点：比等级 3 再小约 25%，比等级 19 只大约 20%，
    // 但编译耗时低一个数量级。解压速度与等级基本无关。
    static constexpr int    COMPRESS_LEVEL = 12;
    // 分块压缩：解析时只需常驻一块解压缓冲，避免整段解压推高内存峰值。
    static constexpr size_t COMPRESS_BLOCK = 4u * 1024 * 1024;
    // 单段解压上限，避免损坏或伪造的文件触发超大分配。
    static constexpr uint64_t MAX_SECTION_RAW = 4ull * 1024 * 1024 * 1024;

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
        uint64_t offset = 0;      // 段数据在文件中的起始偏移
        uint64_t length = 0;      // 段数据在文件中占用的字节数（编码后）
        uint64_t item_count = 0;
        uint32_t codec = CODEC_RAW;
        uint32_t level = 0;       // 写入时使用的压缩等级，仅供诊断
        uint64_t raw_length = 0;  // 解码后的字节数
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

private:
    // 段数据写入器：先攒够一块再落盘，压缩模式下每块单独压成一个 zstd 帧。
    class BlockWriter {
    public:
        BlockWriter(FILE* fp, uint32_t codec, int level)
            : fp_(fp), codec_(codec), level_(level) {
            stage_.reserve(COMPRESS_BLOCK);
            if (codec_ == CODEC_ZSTD) comp_.resize(ZSTD_compressBound(COMPRESS_BLOCK));
        }

        void write(const void* src, size_t n) {
            const char* p = static_cast<const char*>(src);
            while (n > 0) {
                const size_t room = COMPRESS_BLOCK - stage_.size();
                const size_t take = n < room ? n : room;
                stage_.insert(stage_.end(), p, p + take);
                p += take;
                n -= take;
                if (stage_.size() == COMPRESS_BLOCK) flush();
            }
        }

        void u32(uint32_t value) { write(&value, 4); }
        void u64(uint64_t value) { write(&value, 8); }
        void f64(double value)   { write(&value, 8); }

        void str(const std::string& value) {
            u32(static_cast<uint32_t>(value.size()));
            if (!value.empty()) write(value.data(), value.size());
        }

        bool finish() {
            if (!stage_.empty()) flush();
            return ok_ && std::ferror(fp_) == 0;
        }

        uint64_t raw_bytes() const { return raw_total_; }

    private:
        void flush() {
            raw_total_ += stage_.size();
            if (codec_ == CODEC_RAW) {
                ok_ = ok_ && std::fwrite(stage_.data(), 1, stage_.size(), fp_) == stage_.size();
            } else {
                const size_t got = ZSTD_compress(comp_.data(), comp_.size(),
                                                 stage_.data(), stage_.size(), level_);
                if (ZSTD_isError(got)) {
                    std::cerr << "[MCDK] cache: zstd compress failed: "
                              << ZSTD_getErrorName(got) << std::endl;
                    ok_ = false;
                } else {
                    const uint32_t comp_size = static_cast<uint32_t>(got);
                    const uint32_t raw_size  = static_cast<uint32_t>(stage_.size());
                    ok_ = ok_ && std::fwrite(&comp_size, 4, 1, fp_) == 1
                              && std::fwrite(&raw_size, 4, 1, fp_) == 1
                              && std::fwrite(comp_.data(), 1, got, fp_) == got;
                }
            }
            stage_.clear();
        }

        FILE*             fp_;
        uint32_t          codec_;
        int               level_;
        std::vector<char> stage_;
        std::vector<char> comp_;
        uint64_t          raw_total_ = 0;
        bool              ok_ = true;
    };

    // 段数据读取器：按块解码到内存后再逐字段取值。
    // 相比 v6 的逐字段 fread，省掉了每个字段一次 stdio 调用的开销。
    //
    // 取值走 cur_/end_ 两个裸指针：字段完整落在当前块内时（绝大多数情况）直接定长
    // memcpy，只有跨块的字段才回退到 read() 的循环。这条快路径是必须的 —— 一个
    // GA_RP 段有近两百万次字段读取，每次都走带循环和边界判断的通用路径会比 v6 的
    // fread 还慢一倍。
    class BlockReader {
    public:
        BlockReader(FILE* fp, const SectionInfo& section) : fp_(fp), section_(&section) {
            // 复用同一个 DCtx：ZSTD_decompress() 这个简易 API 每次调用都会内部
            // 新建并销毁一个上下文，分段解压时这笔开销会乘以块数。
            if (section.codec == CODEC_ZSTD) dctx_ = ZSTD_createDCtx();
        }

        ~BlockReader() { if (dctx_) ZSTD_freeDCtx(dctx_); }

        BlockReader(const BlockReader&) = delete;
        BlockReader& operator=(const BlockReader&) = delete;

        bool read(void* dst, size_t n) {
            char* out = static_cast<char*>(dst);
            while (n > 0) {
                if (cur_ == end_ && !refill()) return false;
                const size_t avail = static_cast<size_t>(end_ - cur_);
                const size_t take = n < avail ? n : avail;
                std::memcpy(out, cur_, take);
                out += take;
                cur_ += take;
                n -= take;
            }
            return true;
        }

        uint32_t u32() { uint32_t v = 0; take<4>(&v); return v; }
        uint64_t u64() { uint64_t v = 0; take<8>(&v); return v; }
        double   f64() { double   v = 0; take<8>(&v); return v; }

        bool str(std::string& out) {
            const uint32_t length = u32();
            if (!ok_) return false;
            if (length > remaining()) return fail();
            if (static_cast<size_t>(end_ - cur_) >= length) {
                out.assign(cur_, length);   // 单次拷贝，省掉先补零再覆写
                cur_ += length;
                return true;
            }
            out.assign(length, '\0');
            if (length == 0) return true;
            if (!read(&out[0], length)) return fail();
            return true;
        }

        bool ok() const { return ok_; }

        // 本段尚未取出的解码后字节数，用于给各处 count 设上界。
        uint64_t remaining() const {
            return (section_->raw_length - consumed_) + static_cast<uint64_t>(end_ - cur_);
        }

        // 是否恰好把整段读完，等价于 v6 里 file_tell == offset + length 的校验。
        bool consumed() const {
            return ok_ && consumed_ == section_->raw_length && cur_ == end_;
        }

    private:
        bool fail() { ok_ = false; return false; }

        // 按字段大小特化，保证 memcpy 长度是编译期常量、能内联成一条指令 ——
        // 一个 GA_RP 段有近两百万次定长字段读取，走运行时长度的 memcpy 调用
        // 会在这里堆出几十毫秒。
        template <size_t N>
        void take(void* dst) {
            if (static_cast<size_t>(end_ - cur_) >= N) {
                std::memcpy(dst, cur_, N);
                cur_ += N;
                return;
            }
            if (!read(dst, N)) fail();
        }

        bool refill() {
            if (!ok_ || consumed_ >= section_->raw_length) return false;
            if (section_->codec == CODEC_RAW) {
                const uint64_t remain = section_->raw_length - consumed_;
                const size_t want = static_cast<size_t>(std::min<uint64_t>(remain, COMPRESS_BLOCK));
                raw_.resize(want);
                if (std::fread(raw_.data(), 1, want, fp_) != want) return fail();
            } else {
                uint32_t comp_size = 0;
                uint32_t raw_size = 0;
                if (std::fread(&comp_size, 4, 1, fp_) != 1 ||
                    std::fread(&raw_size, 4, 1, fp_) != 1) return fail();
                if (raw_size == 0 || raw_size > COMPRESS_BLOCK) return fail();
                if (comp_size == 0 || comp_size > section_->length) return fail();
                if (consumed_ + raw_size > section_->raw_length) return fail();
                if (!dctx_) return fail();
                comp_.resize(comp_size);
                if (std::fread(comp_.data(), 1, comp_size, fp_) != comp_size) return fail();
                raw_.resize(raw_size);
                const size_t got = ZSTD_decompressDCtx(dctx_, raw_.data(), raw_size,
                                                       comp_.data(), comp_size);
                if (ZSTD_isError(got) || got != raw_size) {
                    std::cerr << "[MCDK] cache: zstd decompress failed in section "
                              << section_->kind << std::endl;
                    return fail();
                }
            }
            consumed_ += raw_.size();
            cur_ = raw_.data();
            end_ = cur_ + raw_.size();
            return true;
        }

        FILE*              fp_;
        const SectionInfo* section_;
        ZSTD_DCtx*         dctx_ = nullptr;
        std::vector<char>  raw_;
        std::vector<char>  comp_;
        const char*        cur_ = nullptr;
        const char*        end_ = nullptr;
        uint64_t           consumed_ = 0;
        bool               ok_ = true;
    };

public:
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
            SectionInfo info;
            info.kind = SECTION_API + i;
            info.item_count = cat_refs[i].fragments->size();
            sections.push_back(info);
        }
        for (uint32_t i = 0; i < ga_refs.size(); ++i) {
            SectionInfo info;
            info.kind = SECTION_GAME_ASSETS_BP + i;
            info.item_count = ga_refs[i].rel_paths->size();
            sections.push_back(info);
        }

        write_u32(fp, static_cast<uint32_t>(sections.size()));
        const uint64_t table_offset = file_tell(fp);
        for (const auto& section : sections) write_section_info(fp, section);

        bool encode_ok = true;
        uint64_t raw_total = 0;
        size_t section_index = 0;
        for (const auto& ref : cat_refs) {
            auto& section = sections[section_index++];
            section.offset = file_tell(fp);
            {
                BlockWriter writer(fp, CODEC_ZSTD, COMPRESS_LEVEL);
                write_fragments(writer, *ref.fragments);
                write_bm25(writer, *ref.engine);
                encode_ok = writer.finish() && encode_ok;
                section.raw_length = writer.raw_bytes();
            }
            section.codec = CODEC_ZSTD;
            section.level = static_cast<uint32_t>(COMPRESS_LEVEL);
            section.length = file_tell(fp) - section.offset;
            raw_total += section.raw_length;
        }
        for (const auto& ref : ga_refs) {
            auto& section = sections[section_index++];
            section.offset = file_tell(fp);
            {
                BlockWriter writer(fp, CODEC_ZSTD, COMPRESS_LEVEL);
                writer.u32(static_cast<uint32_t>(ref.rel_paths->size()));
                for (const auto& rel_path : *ref.rel_paths) writer.str(rel_path);
                write_fragments(writer, *ref.fragments);
                write_bm25(writer, *ref.engine);
                encode_ok = writer.finish() && encode_ok;
                section.raw_length = writer.raw_bytes();
            }
            section.codec = CODEC_ZSTD;
            section.level = static_cast<uint32_t>(COMPRESS_LEVEL);
            section.length = file_tell(fp) - section.offset;
            raw_total += section.raw_length;
        }

        const uint64_t file_size = file_tell(fp);
        bool ok = encode_ok && file_seek(fp, table_offset);
        for (const auto& section : sections) write_section_info(fp, section);
        ok = ok && std::fflush(fp) == 0 && std::ferror(fp) == 0;
        std::fclose(fp);

        if (ok && file_size > 0) {
            const double ratio = static_cast<double>(raw_total) / static_cast<double>(file_size);
            std::cerr << "[MCDK] cache: saved " << (file_size / 1024 / 1024)
                      << " MB to " << cache_path_text
                      << " (zstd-" << COMPRESS_LEVEL
                      << ", raw " << (raw_total / 1024 / 1024) << " MB, "
                      << ratio << "x)" << std::endl;
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
            if (section.codec != CODEC_RAW && section.codec != CODEC_ZSTD) {
                std::cerr << "[MCDK] cache: unknown codec " << section.codec
                          << " in section " << section.kind << ", rebuilding" << std::endl;
                std::fclose(fp);
                return false;
            }
            // raw_length 不再受文件大小约束，必须单独设上限，
            // 否则损坏或伪造的文件可以诱导一次超大分配。
            if (section.raw_length > MAX_SECTION_RAW ||
                (section.codec == CODEC_RAW && section.raw_length != section.length)) {
                std::cerr << "[MCDK] cache: invalid raw length in section "
                          << section.kind << std::endl;
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
        BlockReader reader(fp, *section);
        CatData data;
        const bool ok = read_fragments(reader, data.fragments) &&
                        read_bm25(reader, data.bm25) &&
                        reader.consumed() &&
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
        BlockReader reader(fp, *section);
        GameAssetData data;
        const uint32_t path_count = reader.u32();
        bool ok = reader.ok() && static_cast<uint64_t>(path_count) * 4ull <= reader.remaining();
        if (ok) {
            data.rel_paths.resize(path_count);
            for (auto& rel_path : data.rel_paths) {
                if (!reader.str(rel_path)) { ok = false; break; }
            }
        }
        ok = ok &&
             read_fragments(reader, data.fragments) &&
             read_bm25(reader, data.bm25) &&
             reader.consumed() &&
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

    static void write_u32(FILE* fp, uint32_t value) {
        std::fwrite(&value, 4, 1, fp);
    }

    static void write_u64(FILE* fp, uint64_t value) {
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
        write_u32(fp, section.codec);
        write_u32(fp, section.level);
        write_u64(fp, section.raw_length);
    }

    static void write_fragments(BlockWriter& writer, const std::vector<DocFragment>& fragments) {
        writer.u32(static_cast<uint32_t>(fragments.size()));
        for (const auto& fragment : fragments) {
            writer.str(fragment.content);
            writer.str(fragment.file);
            writer.u32(static_cast<uint32_t>(fragment.line_start));
            writer.u32(static_cast<uint32_t>(fragment.line_end));
        }
    }

    static void write_bm25(BlockWriter& writer, const BM25Engine& engine) {
        const auto& doc_lengths = engine.doc_lengths();
        writer.u32(static_cast<uint32_t>(doc_lengths.size()));
        for (int value : doc_lengths) writer.u32(static_cast<uint32_t>(value));

        writer.f64(engine.avg_dl());

        const auto& idf = engine.idf();
        writer.u32(static_cast<uint32_t>(idf.size()));
        for (const auto& [term, value] : idf) {
            writer.str(term);
            writer.f64(value);
        }

        const auto& inverted_index = engine.inverted_index();
        writer.u32(static_cast<uint32_t>(inverted_index.size()));
        for (const auto& [term, postings] : inverted_index) {
            writer.str(term);
            writer.u32(static_cast<uint32_t>(postings.size()));
            for (const auto& posting : postings) {
                writer.u64(static_cast<uint64_t>(posting.doc_id));
                writer.u32(static_cast<uint32_t>(posting.tf));
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

    static std::string fread_string(FILE* fp) {
        const uint32_t length = fread_u32(fp);
        if (length == 0) return {};
        std::string value(length, '\0');
        if (std::fread(&value[0], 1, length, fp) != length) return {};
        return value;
    }

    static SectionInfo fread_section_info(FILE* fp) {
        SectionInfo section;
        section.kind = fread_u32(fp);
        section.offset = fread_u64(fp);
        section.length = fread_u64(fp);
        section.item_count = fread_u64(fp);
        section.codec = fread_u32(fp);
        section.level = fread_u32(fp);
        section.raw_length = fread_u64(fp);
        return section;
    }

    static bool read_fragments(BlockReader& reader, std::vector<DocFragment>& fragments) {
        const uint32_t count = reader.u32();
        if (!reader.ok()) return false;
        // 每条 fragment 至少占 4+4+4+4 字节，据此拒绝明显越界的条目数。
        if (static_cast<uint64_t>(count) * 16ull > reader.remaining()) return false;
        fragments.resize(count);
        for (auto& fragment : fragments) {
            if (!reader.str(fragment.content)) return false;
            if (!reader.str(fragment.file)) return false;
            fragment.line_start = static_cast<int>(reader.u32());
            fragment.line_end = static_cast<int>(reader.u32());
        }
        return reader.ok();
    }

    static bool read_bm25(BlockReader& reader, BM25State& state) {
        const uint32_t doc_length_count = reader.u32();
        if (!reader.ok()) return false;
        if (static_cast<uint64_t>(doc_length_count) * 4ull > reader.remaining()) return false;
        state.doc_lengths.resize(doc_length_count);
        for (auto& value : state.doc_lengths) value = static_cast<int>(reader.u32());

        state.avg_dl = reader.f64();
        if (!reader.ok()) return false;

        const uint32_t idf_count = reader.u32();
        if (!reader.ok()) return false;
        // 每条 idf 至少占 4（词长）+ 8（权重）字节。
        if (static_cast<uint64_t>(idf_count) * 12ull > reader.remaining()) return false;
        state.idf.reserve(idf_count);
        for (uint32_t i = 0; i < idf_count; ++i) {
            std::string term;
            if (!reader.str(term)) return false;
            const double value = reader.f64();
            state.idf[std::move(term)] = value;
        }

        const uint32_t inverted_count = reader.u32();
        if (!reader.ok()) return false;
        // 每条倒排至少占 4（词长）+ 4（postings 数）字节。
        if (static_cast<uint64_t>(inverted_count) * 8ull > reader.remaining()) return false;
        state.inverted_index.reserve(inverted_count);
        for (uint32_t i = 0; i < inverted_count; ++i) {
            std::string term;
            if (!reader.str(term)) return false;
            const uint32_t posting_count = reader.u32();
            if (!reader.ok()) return false;
            if (static_cast<uint64_t>(posting_count) * 12ull > reader.remaining()) return false;
            std::vector<BM25Engine::Posting> postings(posting_count);
            for (auto& posting : postings) {
                posting.doc_id = static_cast<size_t>(reader.u64());
                posting.tf = static_cast<int>(reader.u32());
            }
            if (!reader.ok()) return false;
            state.inverted_index[std::move(term)] = std::move(postings);
        }
        return reader.ok();
    }
};

} // namespace mcdk
