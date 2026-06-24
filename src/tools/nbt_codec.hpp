#pragma once
// nbt_codec.hpp — Minecraft NBT 二进制编解码核心（无新依赖）
// Reference: jaquadro/NBTExplorer (MIT License), a Minecraft NBT editor.
//   https://github.com/jaquadro/NBTExplorer
// This file keeps an independent C++ implementation for mcdk-assistant while
// following the NBT data model and read/write semantics used by that project.
// 职责: 解析/序列化 NBT，支持基岩 .mcstructure（小端无压缩）、基岩 level.dat（8 字节头）、
//       无压缩 .nbt（大/小端）；提供树状/tagged-JSON 渲染、路径导航与增删改原语。
// 设计要点:
//   - NbtTag 用 std::variant 承载 13 种 NBT 类型；Compound 用有序 vector<pair> 保留键顺序，保证 round-trip 保真。
//   - 字符串按“原始字节”读写（不做 modified-UTF-8 解码），保证字节级往返一致；.mcstructure 内方块名均为 ASCII。
//   - float/double 用 memcpy 还原/写出位模式，避免算术造成的精度漂移。
// ponytail: 仅支持无压缩 NBT。gzip 压缩的 Java .nbt/level.dat 会被显式拒绝；
//           升级路径是引入单文件解压库（如 miniz）后在 load_nbt_file 中先解压再解析。
#include "common/path_utils.hpp"
#include <nlohmann/json.hpp>
#include <cstdint>
#include <cstring>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace mcdk::nbt {

using Byte = uint8_t;

// ── NBT 标签类型编码 ──────────────────────────────────────
enum class TagType : Byte {
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    ByteArray = 7,
    String = 8,
    List = 9,
    Compound = 10,
    IntArray = 11,
    LongArray = 12
};

inline Byte to_byte(TagType t) {
    return static_cast<Byte>(t);
}

inline TagType tag_type_from_byte(Byte raw) {
    switch (raw) {
        case 0: return TagType::End;
        case 1: return TagType::Byte;
        case 2: return TagType::Short;
        case 3: return TagType::Int;
        case 4: return TagType::Long;
        case 5: return TagType::Float;
        case 6: return TagType::Double;
        case 7: return TagType::ByteArray;
        case 8: return TagType::String;
        case 9: return TagType::List;
        case 10: return TagType::Compound;
        case 11: return TagType::IntArray;
        case 12: return TagType::LongArray;
        default: throw std::runtime_error("未知 NBT 标签类型: " + std::to_string(static_cast<int>(raw)));
    }
}

inline uint32_t checked_i32_len(size_t n, const char* what) {
    if (n > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
        throw std::runtime_error(std::string(what) + " too large for NBT length field");
    return static_cast<uint32_t>(n);
}

template <typename Int>
inline Int checked_int_cast(int64_t value, const char* what) {
    if (value < static_cast<int64_t>(std::numeric_limits<Int>::min()) ||
        value > static_cast<int64_t>(std::numeric_limits<Int>::max())) {
        throw std::runtime_error(std::string(what) + " is out of range");
    }
    return static_cast<Int>(value);
}

struct NbtTag;
// 列表标签: 所有元素共享 elem_type
struct NbtList { TagType elem_type = TagType::End; std::vector<NbtTag> items; };
// 复合标签: 有序键值对（保留写入顺序，round-trip 保真）
using NbtCompound = std::vector<std::pair<std::string, NbtTag>>;

// 单个 NBT 标签: type 为权威类型，value 持有对应 variant 分支
struct NbtTag {
    TagType type = TagType::End;
    std::variant<
        std::monostate,            // end / 占位
        int8_t, int16_t, int32_t, int64_t,
        float, double,
        std::vector<int8_t>,       // byte_array
        std::string,               // string（原始字节）
        NbtList,                   // list
        NbtCompound,               // compound
        std::vector<int32_t>,      // int_array
        std::vector<int64_t>       // long_array
    > value;
};

// 一个完整 NBT 文件的解析结果（含端序与 level.dat 头信息，供原格式写回）
struct NbtFile {
    NbtTag root;                       // 根标签（必为 Compound）
    std::string root_name;             // 根名（.mcstructure 通常为空）
    bool little_endian = true;         // true=基岩(小端) false=Java(大端)
    bool has_leveldat_header = false;  // 是否带基岩 level.dat 的 8 字节头
    uint32_t leveldat_version = 0;     // level.dat 头部的版本号（写回时原样保留）
};

// ── 类型名 ↔ 编码 ─────────────────────────────────────────
inline const char* tag_type_name(TagType t) {
    switch (t) {
        case TagType::Byte: return "byte";          case TagType::Short: return "short";
        case TagType::Int: return "int";            case TagType::Long: return "long";
        case TagType::Float: return "float";        case TagType::Double: return "double";
        case TagType::ByteArray: return "byte_array"; case TagType::String: return "string";
        case TagType::List: return "list";          case TagType::Compound: return "compound";
        case TagType::IntArray: return "int_array"; case TagType::LongArray: return "long_array";
        case TagType::End: return "end";            default: return "?";
    }
}
inline TagType tag_type_code(const std::string& n) {
    if (n == "end") return TagType::End;         // 空 list 的 elem_type 常为 end
    if (n == "byte") return TagType::Byte;       if (n == "short") return TagType::Short;
    if (n == "int") return TagType::Int;         if (n == "long") return TagType::Long;
    if (n == "float") return TagType::Float;     if (n == "double") return TagType::Double;
    if (n == "byte_array") return TagType::ByteArray; if (n == "string") return TagType::String;
    if (n == "list") return TagType::List;       if (n == "compound") return TagType::Compound;
    if (n == "int_array") return TagType::IntArray;   if (n == "long_array") return TagType::LongArray;
    throw std::runtime_error("未知 NBT 类型名: " + n);
}
inline bool is_scalar_type(TagType t) {
    return t == TagType::Byte || t == TagType::Short || t == TagType::Int || t == TagType::Long ||
           t == TagType::Float || t == TagType::Double || t == TagType::String;
}

// ── 二进制读取器（按端序逐字节组装，宿主端序无关）────────────
struct NbtReader {
    const Byte* cur; const Byte* end; bool le;
    NbtReader(const std::vector<Byte>& b, size_t off, bool little)
        : cur(b.data() + off), end(b.data() + b.size()), le(little) {
        if (off > b.size()) throw std::runtime_error("NBT reader offset is out of range");
    }
    size_t remaining() const { return static_cast<size_t>(end - cur); }

    // 读 n 字节为无符号整数
    uint64_t u(int n) {
        if (cur + n > end) throw std::runtime_error("NBT 解析越界");
        uint64_t v = 0;
        if (le) for (int i = 0; i < n; ++i) v |= static_cast<uint64_t>(cur[i]) << (8 * i);
        else    for (int i = 0; i < n; ++i) v = (v << 8) | cur[i];
        cur += n; return v;
    }
    std::string str() {
        uint16_t n = static_cast<uint16_t>(u(2));
        if (cur + n > end) throw std::runtime_error("NBT 字符串越界");
        std::string s(reinterpret_cast<const char*>(cur), n);
        cur += n; return s;
    }
    TagType tag_type() {
        return tag_type_from_byte(static_cast<Byte>(u(1)));
    }
    // 数组/列表长度: 校验不超过剩余字节（每元素至少 1 字节），避免错误端序触发巨量分配
    int32_t len() {
        int32_t n = static_cast<int32_t>(u(4));
        if (n < 0 || static_cast<size_t>(n) > remaining())
            throw std::runtime_error("NBT 长度非法: " + std::to_string(n));
        return n;
    }
    NbtTag payload(TagType type);
};

inline NbtTag NbtReader::payload(TagType type) {
    NbtTag t; t.type = type;
    switch (type) {
        case TagType::Byte:  t.value = static_cast<int8_t>(u(1)); break;
        case TagType::Short: t.value = static_cast<int16_t>(u(2)); break;
        case TagType::Int:   t.value = static_cast<int32_t>(u(4)); break;
        case TagType::Long:  t.value = static_cast<int64_t>(u(8)); break;
        case TagType::Float:  { uint32_t b = static_cast<uint32_t>(u(4)); float f;  std::memcpy(&f, &b, 4); t.value = f; } break;
        case TagType::Double: { uint64_t b = u(8);                        double d; std::memcpy(&d, &b, 8); t.value = d; } break;
        case TagType::ByteArray: {
            int32_t n = len(); std::vector<int8_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int8_t>(u(1)));
            t.value = std::move(a);
        } break;
        case TagType::String: t.value = str(); break;
        case TagType::List: {
            TagType et = tag_type(); int32_t n = len();
            NbtList lst; lst.elem_type = et; lst.items.reserve(n);
            for (int32_t i = 0; i < n; ++i) lst.items.push_back(payload(et));
            t.value = std::move(lst);
        } break;
        case TagType::Compound: {
            NbtCompound c;
            while (true) {
                TagType ct = tag_type();
                if (ct == TagType::End) break;
                std::string name = str();
                c.emplace_back(std::move(name), payload(ct));
            }
            t.value = std::move(c);
        } break;
        case TagType::IntArray: {
            int32_t n = len(); std::vector<int32_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int32_t>(u(4)));
            t.value = std::move(a);
        } break;
        case TagType::LongArray: {
            int32_t n = len(); std::vector<int64_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int64_t>(u(8)));
            t.value = std::move(a);
        } break;
        default: throw std::runtime_error("未知 NBT 标签类型: " + std::to_string(static_cast<int>(to_byte(type))));
    }
    return t;
}

// ── 二进制写出器 ──────────────────────────────────────────
struct NbtWriter {
    std::vector<Byte>& out; bool le;
    void u(uint64_t v, int n) {
        if (le) for (int i = 0; i < n; ++i)      out.push_back(static_cast<Byte>((v >> (8 * i)) & 0xff));
        else    for (int i = n - 1; i >= 0; --i) out.push_back(static_cast<Byte>((v >> (8 * i)) & 0xff));
    }
    void str(const std::string& s) {
        if (s.size() > std::numeric_limits<uint16_t>::max())
            throw std::runtime_error("NBT string exceeds 65535 bytes");
        u(static_cast<uint16_t>(s.size()), 2);
        out.insert(out.end(), s.begin(), s.end());
    }
    void payload(const NbtTag& t);
};

inline void NbtWriter::payload(const NbtTag& t) {
    switch (t.type) {
        case TagType::Byte:  u(static_cast<Byte>(std::get<int8_t>(t.value)), 1); break;
        case TagType::Short: u(static_cast<uint16_t>(std::get<int16_t>(t.value)), 2); break;
        case TagType::Int:   u(static_cast<uint32_t>(std::get<int32_t>(t.value)), 4); break;
        case TagType::Long:  u(static_cast<uint64_t>(std::get<int64_t>(t.value)), 8); break;
        case TagType::Float:  { float f = std::get<float>(t.value);  uint32_t b; std::memcpy(&b, &f, 4); u(b, 4); } break;
        case TagType::Double: { double d = std::get<double>(t.value); uint64_t b; std::memcpy(&b, &d, 8); u(b, 8); } break;
        case TagType::ByteArray: {
            auto& a = std::get<std::vector<int8_t>>(t.value);
            u(checked_i32_len(a.size(), "byte_array"), 4);
            for (int8_t x : a) u(static_cast<Byte>(x), 1);
        } break;
        case TagType::String: str(std::get<std::string>(t.value)); break;
        case TagType::List: {
            auto& l = std::get<NbtList>(t.value);
            u(to_byte(l.elem_type), 1); u(checked_i32_len(l.items.size(), "list"), 4);
            for (auto& it : l.items) payload(it);
        } break;
        case TagType::Compound: {
            auto& c = std::get<NbtCompound>(t.value);
            for (auto& kv : c) { u(to_byte(kv.second.type), 1); str(kv.first); payload(kv.second); }
            u(to_byte(TagType::End), 1);
        } break;
        case TagType::IntArray: {
            auto& a = std::get<std::vector<int32_t>>(t.value);
            u(checked_i32_len(a.size(), "int_array"), 4);
            for (int32_t x : a) u(static_cast<uint32_t>(x), 4);
        } break;
        case TagType::LongArray: {
            auto& a = std::get<std::vector<int64_t>>(t.value);
            u(checked_i32_len(a.size(), "long_array"), 4);
            for (int64_t x : a) u(static_cast<uint64_t>(x), 8);
        } break;
        default: throw std::runtime_error("无法写入未知 NBT 类型: " + std::to_string(static_cast<int>(to_byte(t.type))));
    }
}

// ── 文件字节读写（用 from_utf8 走宽字符路径，二进制安全）──────
inline std::vector<Byte> read_file_bytes(const std::string& path_utf8) {
    std::ifstream ifs(mcdk::path::from_utf8(path_utf8), std::ios::binary);
    if (!ifs.is_open()) throw std::runtime_error("无法打开文件: " + path_utf8);
    return std::vector<Byte>((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}
inline void write_file_bytes(const std::string& path_utf8, const std::vector<Byte>& data) {
    std::ofstream ofs(mcdk::path::from_utf8(path_utf8), std::ios::binary);
    if (!ofs.is_open()) throw std::runtime_error("无法写入文件: " + path_utf8);
    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

// 以指定端序完整解析（要求根为 Compound 且恰好消费全部字节，作为端序判定依据）
inline bool try_parse(const std::vector<Byte>& bytes, size_t off, bool le, NbtFile& out) {
    try {
        NbtReader r(bytes, off, le);
        TagType rt = tag_type_from_byte(static_cast<uint8_t>(r.u(1)));
        if (rt != TagType::Compound) return false;
        std::string name = r.str();
        NbtTag root = r.payload(TagType::Compound);
        if (r.remaining() != 0) return false;   // 必须恰好读完
        out.root = std::move(root);
        out.root_name = std::move(name);
        out.little_endian = le;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// 加载并解析一个 NBT 文件
inline NbtFile load_nbt_file(const std::string& path_utf8) {
    std::vector<Byte> bytes = read_file_bytes(path_utf8);
    if (bytes.size() >= 2 && bytes[0] == 0x1f && bytes[1] == 0x8b)
        throw std::runtime_error(
            "暂不支持 gzip 压缩的 NBT 文件（典型为 Java .nbt / level.dat）。"
            "当前仅支持无压缩 NBT：基岩 .mcstructure、基岩 level.dat、无压缩 .nbt。");

    // 基岩 level.dat 头探测: 前 8 字节 = version(i32 LE) + payloadLength(i32 LE)，其后紧跟 NBT(0x0a)
    size_t off = 0; bool header = false; uint32_t ver = 0;
    if (bytes.size() >= 12) {
        uint32_t len_le = static_cast<uint32_t>(bytes[4]) | (static_cast<uint32_t>(bytes[5]) << 8) |
                          (static_cast<uint32_t>(bytes[6]) << 16) | (static_cast<uint32_t>(bytes[7]) << 24);
        if (bytes[8] == to_byte(TagType::Compound) && static_cast<size_t>(len_le) == bytes.size() - 8) {
            header = true; off = 8;
            ver = static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) |
                  (static_cast<uint32_t>(bytes[2]) << 16) | (static_cast<uint32_t>(bytes[3]) << 24);
        }
    }

    NbtFile f;
    // 先小端（基岩），再大端（Java）。两者均以“恰好消费完字节”作为成功判据。
    if (!try_parse(bytes, off, true, f) && !try_parse(bytes, off, false, f))
        throw std::runtime_error("无法解析为有效的无压缩 NBT（端序/格式不符或文件损坏）");
    f.has_leveldat_header = header;
    f.leveldat_version = ver;
    return f;
}

// 按原始端序与 level.dat 头序列化为字节
inline std::vector<Byte> serialize_nbt(const NbtFile& f) {
    std::vector<Byte> body;
    NbtWriter w{body, f.little_endian};
    w.u(to_byte(f.root.type), 1);   // 根类型(0x0a)
    w.str(f.root_name);    // 根名
    w.payload(f.root);
    if (!f.has_leveldat_header) return body;

    // 重新拼回 8 字节头（version + length，均小端）
    std::vector<Byte> outb; outb.reserve(body.size() + 8);
    uint32_t len = checked_i32_len(body.size(), "level.dat body");
    for (int i = 0; i < 4; ++i) outb.push_back(static_cast<uint8_t>((f.leveldat_version >> (8 * i)) & 0xff));
    for (int i = 0; i < 4; ++i) outb.push_back(static_cast<uint8_t>((len >> (8 * i)) & 0xff));
    outb.insert(outb.end(), body.begin(), body.end());
    return outb;
}
inline void save_nbt_file(const std::string& path_utf8, const NbtFile& f) {
    write_file_bytes(path_utf8, serialize_nbt(f));
}

// ── 树状渲染（供 nbt_view，大数组/列表按 max_items 截断）────────
inline std::string fmt_scalar(const NbtTag& t) {
    std::ostringstream o;
    switch (t.type) {
        case TagType::Byte:   o << static_cast<int>(std::get<int8_t>(t.value)); break;
        case TagType::Short:  o << std::get<int16_t>(t.value); break;
        case TagType::Int:    o << std::get<int32_t>(t.value); break;
        case TagType::Long:   o << std::get<int64_t>(t.value); break;
        case TagType::Float:  o << std::get<float>(t.value); break;
        case TagType::Double: o << std::get<double>(t.value); break;
        case TagType::String: o << '"' << std::get<std::string>(t.value) << '"'; break;
        default:         o << '?'; break;
    }
    return o.str();
}
template <typename Vec>
inline std::string fmt_num_array(const Vec& a, int max_items) {
    std::ostringstream o; o << '[';
    int n = static_cast<int>(a.size());
    int show = (max_items > 0 && n > max_items) ? max_items : n;
    for (int i = 0; i < show; ++i) { if (i) o << ", "; o << static_cast<long long>(a[i]); }
    if (show < n) o << ", … (+" << (n - show) << " more)";
    o << ']'; return o.str();
}
inline std::string fmt_scalar_list(const NbtList& l, int max_items) {
    std::ostringstream o; o << '[';
    int n = static_cast<int>(l.items.size());
    int show = (max_items > 0 && n > max_items) ? max_items : n;
    for (int i = 0; i < show; ++i) { if (i) o << ", "; o << fmt_scalar(l.items[i]); }
    if (show < n) o << ", … (+" << (n - show) << " more)";
    o << ']'; return o.str();
}

inline void render_node(std::ostringstream& o, const std::string& name, const NbtTag& t,
                        int indent, int depth, int max_depth, int max_items) {
    std::string pad(static_cast<size_t>(indent) * 2, ' ');
    std::string deeper(static_cast<size_t>(indent + 1) * 2, ' ');
    std::string label = name.empty() ? std::string() : (name + ": ");
    switch (t.type) {
        case TagType::Compound: {
            auto& c = std::get<NbtCompound>(t.value);
            o << pad << label << "(Compound, " << c.size() << " entries)\n";
            if (depth >= max_depth) { if (!c.empty()) o << deeper << "… (depth limit)\n"; break; }
            int shown = 0;
            for (auto& kv : c) {
                if (max_items > 0 && shown++ >= max_items) { o << deeper << "… (+" << (c.size() - max_items) << " more)\n"; break; }
                render_node(o, kv.first, kv.second, indent + 1, depth + 1, max_depth, max_items);
            }
        } break;
        case TagType::List: {
            auto& l = std::get<NbtList>(t.value);
            if (is_scalar_type(l.elem_type)) {
                o << pad << label << "(List<" << tag_type_name(l.elem_type) << ">, " << l.items.size() << ") "
                  << fmt_scalar_list(l, max_items) << "\n";
            } else {
                o << pad << label << "(List<" << tag_type_name(l.elem_type) << ">, " << l.items.size() << ")\n";
                if (depth >= max_depth) { if (!l.items.empty()) o << deeper << "… (depth limit)\n"; break; }
                int shown = 0;
                for (size_t i = 0; i < l.items.size(); ++i) {
                    if (max_items > 0 && shown++ >= max_items) { o << deeper << "… (+" << (l.items.size() - max_items) << " more)\n"; break; }
                    render_node(o, "[" + std::to_string(i) + "]", l.items[i], indent + 1, depth + 1, max_depth, max_items);
                }
            }
        } break;
        case TagType::ByteArray: o << pad << label << "(Byte_Array, " << std::get<std::vector<int8_t>>(t.value).size() << ") "  << fmt_num_array(std::get<std::vector<int8_t>>(t.value), max_items) << "\n"; break;
        case TagType::IntArray:  o << pad << label << "(Int_Array, "  << std::get<std::vector<int32_t>>(t.value).size() << ") " << fmt_num_array(std::get<std::vector<int32_t>>(t.value), max_items) << "\n"; break;
        case TagType::LongArray: o << pad << label << "(Long_Array, " << std::get<std::vector<int64_t>>(t.value).size() << ") " << fmt_num_array(std::get<std::vector<int64_t>>(t.value), max_items) << "\n"; break;
        default: o << pad << label << fmt_scalar(t) << "  (" << tag_type_name(t.type) << ")\n"; break;
    }
}

inline std::string to_tree(const NbtFile& f, int max_depth, int max_items) {
    std::ostringstream o;
    o << "[NBT] endian=" << (f.little_endian ? "little (Bedrock)" : "big (Java)")
      << (f.has_leveldat_header ? "  +level.dat header" : "")
      << "  root_name=\"" << f.root_name << "\"\n";
    render_node(o, "(root)", f.root, 0, 0, max_depth, max_items);
    return o.str();
}

// ── tagged-JSON 互转（无损，用 ordered_json 保序）────────────
using ojson = nlohmann::ordered_json;

inline ojson to_tagged_json(const NbtTag& t) {
    ojson j; j["type"] = tag_type_name(t.type);
    switch (t.type) {
        case TagType::Byte:   j["value"] = static_cast<int>(std::get<int8_t>(t.value)); break;
        case TagType::Short:  j["value"] = std::get<int16_t>(t.value); break;
        case TagType::Int:    j["value"] = std::get<int32_t>(t.value); break;
        case TagType::Long:   j["value"] = std::get<int64_t>(t.value); break;
        case TagType::Float:  j["value"] = std::get<float>(t.value); break;
        case TagType::Double: j["value"] = std::get<double>(t.value); break;
        case TagType::String: j["value"] = std::get<std::string>(t.value); break;
        case TagType::ByteArray: { auto& a = std::get<std::vector<int8_t>>(t.value); ojson arr = ojson::array(); for (int8_t x : a) arr.push_back(static_cast<int>(x)); j["value"] = std::move(arr); } break;
        case TagType::IntArray:  { auto& a = std::get<std::vector<int32_t>>(t.value); ojson arr = ojson::array(); for (int32_t x : a) arr.push_back(x); j["value"] = std::move(arr); } break;
        case TagType::LongArray: { auto& a = std::get<std::vector<int64_t>>(t.value); ojson arr = ojson::array(); for (int64_t x : a) arr.push_back(x); j["value"] = std::move(arr); } break;
        case TagType::List: {
            auto& l = std::get<NbtList>(t.value);
            j["elem_type"] = tag_type_name(l.elem_type);
            ojson arr = ojson::array(); for (auto& it : l.items) arr.push_back(to_tagged_json(it));
            j["value"] = std::move(arr);
        } break;
        case TagType::Compound: {
            auto& c = std::get<NbtCompound>(t.value);
            ojson obj = ojson::object(); for (auto& kv : c) obj[kv.first] = to_tagged_json(kv.second);
            j["value"] = std::move(obj);
        } break;
        default: break;
    }
    return j;
}

inline NbtTag from_tagged_json(const ojson& j) {
    if (!j.contains("type")) throw std::runtime_error("tagged-JSON 缺少 type 字段");
    TagType type = tag_type_code(j.at("type").get<std::string>());
    NbtTag t; t.type = type;
    static const ojson kNull;
    const ojson& v = j.contains("value") ? j.at("value") : kNull;
    switch (type) {
        case TagType::Byte:   t.value = static_cast<int8_t>(v.get<int64_t>()); break;
        case TagType::Short:  t.value = static_cast<int16_t>(v.get<int64_t>()); break;
        case TagType::Int:    t.value = static_cast<int32_t>(v.get<int64_t>()); break;
        case TagType::Long:   t.value = static_cast<int64_t>(v.get<int64_t>()); break;
        case TagType::Float:  t.value = static_cast<float>(v.get<double>()); break;
        case TagType::Double: t.value = v.get<double>(); break;
        case TagType::String: t.value = v.get<std::string>(); break;
        case TagType::ByteArray: { std::vector<int8_t> a;  for (auto& e : v) a.push_back(static_cast<int8_t>(e.get<int64_t>())); t.value = std::move(a); } break;
        case TagType::IntArray:  { std::vector<int32_t> a; for (auto& e : v) a.push_back(static_cast<int32_t>(e.get<int64_t>())); t.value = std::move(a); } break;
        case TagType::LongArray: { std::vector<int64_t> a; for (auto& e : v) a.push_back(e.get<int64_t>()); t.value = std::move(a); } break;
        case TagType::List: {
            NbtList l;
            l.elem_type = j.contains("elem_type") ? tag_type_code(j.at("elem_type").get<std::string>()) : TagType::End;
            for (auto& e : v) {
                NbtTag it = from_tagged_json(e);
                if (l.elem_type == TagType::End) l.elem_type = it.type;
                if (it.type != l.elem_type) throw std::runtime_error("list 元素类型与 elem_type 不一致");
                l.items.push_back(std::move(it));
            }
            t.value = std::move(l);
        } break;
        case TagType::Compound: {
            NbtCompound c;
            for (auto it = v.begin(); it != v.end(); ++it) c.emplace_back(it.key(), from_tagged_json(it.value()));
            t.value = std::move(c);
        } break;
        default: throw std::runtime_error("tagged-JSON 不支持的 type: " + j.at("type").get<std::string>());
    }
    return t;
}

// ── 路径导航与增删改原语 ──────────────────────────────────
// 路径以 '/' 分段: compound 段=键名, list 段=整数下标; 空串/"/"=根。
// ponytail: 键名内含 '/' 无法寻址（已知限制，Minecraft 键名基本不含 '/'）。
inline std::vector<std::string> split_path(const std::string& path) {
    std::vector<std::string> segs; std::string cur;
    for (char ch : path) {
        if (ch == '/') { if (!cur.empty()) { segs.push_back(cur); cur.clear(); } }
        else cur.push_back(ch);
    }
    if (!cur.empty()) segs.push_back(cur);
    return segs;
}
inline int parse_index(const std::string& s) {
    if (s.empty()) throw std::runtime_error("list 路径段需为整数下标，实际为空");
    size_t i = (s[0] == '-') ? 1 : 0;
    for (; i < s.size(); ++i) if (s[i] < '0' || s[i] > '9') throw std::runtime_error("list 路径段需为整数下标: " + s);
    return std::stoi(s);
}

// 按前 count 段导航到目标标签；找不到返回 nullptr
inline NbtTag* navigate(NbtTag& root, const std::vector<std::string>& segs, size_t count) {
    NbtTag* cur = &root;
    for (size_t i = 0; i < count; ++i) {
        const std::string& s = segs[i];
        if (cur->type == TagType::Compound) {
            NbtTag* nxt = nullptr;
            for (auto& kv : std::get<NbtCompound>(cur->value)) if (kv.first == s) { nxt = &kv.second; break; }
            if (!nxt) return nullptr;
            cur = nxt;
        } else if (cur->type == TagType::List) {
            auto& l = std::get<NbtList>(cur->value);
            int idx = parse_index(s);
            if (idx < 0 || static_cast<size_t>(idx) >= l.items.size()) return nullptr;
            cur = &l.items[idx];
        } else {
            return nullptr;   // 标量/数组无法继续向下寻址
        }
    }
    return cur;
}

// 将标量值字符串按目标类型写入标签
inline void assign_scalar(NbtTag& t, TagType type, const std::string& s) {
    t.type = type;
    switch (type) {
        case TagType::Byte:   t.value = checked_int_cast<int8_t>(std::stoll(s), "byte value"); break;
        case TagType::Short:  t.value = checked_int_cast<int16_t>(std::stoll(s), "short value"); break;
        case TagType::Int:    t.value = checked_int_cast<int32_t>(std::stoll(s), "int value"); break;
        case TagType::Long:   t.value = static_cast<int64_t>(std::stoll(s)); break;
        case TagType::Float:  t.value = static_cast<float>(std::stod(s)); break;
        case TagType::Double: t.value = std::stod(s); break;
        case TagType::String: t.value = s; break;
        default: throw std::runtime_error("set 仅支持标量类型(byte/short/int/long/float/double/string)；复杂类型请用 add + json_value");
    }
}

// set: 修改/新建标量。tag_type 为空时沿用原类型；目标不存在时必须给 tag_type。
inline std::string op_set(NbtTag& root, const std::string& path, const std::string& value, const std::string& tag_type) {
    auto segs = split_path(path);
    if (segs.empty()) throw std::runtime_error("set 需要非空 path");
    NbtTag* parent = navigate(root, segs, segs.size() - 1);
    if (!parent) throw std::runtime_error("set: 父路径不存在");
    const std::string& last = segs.back();
    if (parent->type == TagType::Compound) {
        for (auto& kv : std::get<NbtCompound>(parent->value)) {
            if (kv.first == last) {
                TagType type = tag_type.empty() ? kv.second.type : tag_type_code(tag_type);
                assign_scalar(kv.second, type, value);
                return "set 成功: " + path + " = " + value + " (" + tag_type_name(type) + ")";
            }
        }
        if (tag_type.empty()) throw std::runtime_error("set: 目标不存在且未提供 tag_type，无法新建");
        NbtTag t; assign_scalar(t, tag_type_code(tag_type), value);
        std::get<NbtCompound>(parent->value).emplace_back(last, std::move(t));
        return "set 新建: " + path + " = " + value + " (" + tag_type + ")";
    } else if (parent->type == TagType::List) {
        auto& l = std::get<NbtList>(parent->value);
        int idx = parse_index(last);
        if (idx < 0 || static_cast<size_t>(idx) >= l.items.size()) throw std::runtime_error("set: list 下标越界");
        TagType type = tag_type.empty() ? l.items[idx].type : tag_type_code(tag_type);
        if (type != l.elem_type) throw std::runtime_error("set: list 内元素类型必须等于 elem_type(" + std::string(tag_type_name(l.elem_type)) + ")");
        assign_scalar(l.items[idx], type, value);
        return "set 成功: " + path + " = " + value + " (" + tag_type_name(type) + ")";
    }
    throw std::runtime_error("set: 父节点不是 compound/list");
}

inline std::string op_remove(NbtTag& root, const std::string& path) {
    auto segs = split_path(path);
    if (segs.empty()) throw std::runtime_error("remove 需要非空 path");
    NbtTag* parent = navigate(root, segs, segs.size() - 1);
    if (!parent) throw std::runtime_error("remove: 父路径不存在");
    const std::string& last = segs.back();
    if (parent->type == TagType::Compound) {
        auto& c = std::get<NbtCompound>(parent->value);
        for (auto it = c.begin(); it != c.end(); ++it) if (it->first == last) { c.erase(it); return "remove 成功: " + path; }
        throw std::runtime_error("remove: 键不存在: " + last);
    } else if (parent->type == TagType::List) {
        auto& l = std::get<NbtList>(parent->value);
        int idx = parse_index(last);
        if (idx < 0 || static_cast<size_t>(idx) >= l.items.size()) throw std::runtime_error("remove: list 下标越界");
        l.items.erase(l.items.begin() + idx);
        return "remove 成功: " + path;
    }
    throw std::runtime_error("remove: 父节点不是 compound/list");
}

inline std::string op_rename(NbtTag& root, const std::string& path, const std::string& new_name) {
    auto segs = split_path(path);
    if (segs.empty()) throw std::runtime_error("rename 需要非空 path");
    if (new_name.empty()) throw std::runtime_error("rename 需要 new_name");
    NbtTag* parent = navigate(root, segs, segs.size() - 1);
    if (!parent || parent->type != TagType::Compound) throw std::runtime_error("rename: 父节点必须是 compound");
    auto& c = std::get<NbtCompound>(parent->value);
    NbtTag* target = nullptr;
    for (auto& kv : c) {
        if (kv.first == new_name) throw std::runtime_error("rename: 目标键名已存在: " + new_name);
        if (kv.first == segs.back()) target = &kv.second;
    }
    if (!target) throw std::runtime_error("rename: 源键不存在: " + segs.back());
    for (auto& kv : c) if (kv.first == segs.back()) { kv.first = new_name; break; }
    return "rename 成功: " + path + " → " + new_name;
}

// add: 在 path 指向的 compound/list 下新增子标签。
//   标量用 tag_type+value；复杂子树用 json_value(tagged-JSON)。compound 必须给 name。
inline std::string op_add(NbtTag& root, const std::string& path, const std::string& name,
                          const std::string& tag_type, const std::string& value, const std::string& json_value) {
    auto segs = split_path(path);
    NbtTag* container = navigate(root, segs, segs.size());
    if (!container) throw std::runtime_error("add: 目标路径不存在: " + path);

    NbtTag child;
    if (!json_value.empty()) {
        child = from_tagged_json(ojson::parse(json_value));
    } else {
        if (tag_type.empty()) throw std::runtime_error("add: 需要 json_value 或 tag_type+value");
        assign_scalar(child, tag_type_code(tag_type), value);
    }

    if (container->type == TagType::Compound) {
        if (name.empty()) throw std::runtime_error("add: 向 compound 添加需要 name");
        auto& c = std::get<NbtCompound>(container->value);
        for (auto& kv : c) if (kv.first == name) throw std::runtime_error("add: 键已存在: " + name);
        c.emplace_back(name, std::move(child));
        return "add 成功: " + (path.empty() ? "" : path + "/") + name + " (" + tag_type_name(c.back().second.type) + ")";
    } else if (container->type == TagType::List) {
        auto& l = std::get<NbtList>(container->value);
        if (l.items.empty() && l.elem_type == TagType::End) l.elem_type = child.type;
        if (child.type != l.elem_type) throw std::runtime_error("add: list 元素类型必须等于 elem_type(" + std::string(tag_type_name(l.elem_type)) + ")");
        l.items.push_back(std::move(child));
        return "add 成功: " + path + "[" + std::to_string(l.items.size() - 1) + "] (" + tag_type_name(l.elem_type) + ")";
    }
    throw std::runtime_error("add: 目标必须是 compound 或 list");
}

// ── 标签工厂（供从零构建 NBT，如 nbt_create）──────────────
inline NbtTag tag_int(int32_t v)      { NbtTag t; t.type = TagType::Int;    t.value = v; return t; }
inline NbtTag tag_string(std::string v){ NbtTag t; t.type = TagType::String; t.value = std::move(v); return t; }
inline NbtTag tag_compound(NbtCompound c = {}) { NbtTag t; t.type = TagType::Compound; t.value = std::move(c); return t; }
inline NbtTag tag_list(TagType elem_type, std::vector<NbtTag> items = {}) {
    NbtTag t; t.type = TagType::List; NbtList l; l.elem_type = elem_type; l.items = std::move(items); t.value = std::move(l); return t;
}

// 构建一个合法可加载的简易 .mcstructure（小端、无压缩）。
// layer0 全部填 fill（palette 下标），layer1 全部 -1（无方块）。
inline NbtFile build_mcstructure(int sx, int sy, int sz,
                                 const std::vector<std::string>& blocks, int fill, int32_t block_version) {
    if (sx <= 0 || sy <= 0 || sz <= 0) throw std::runtime_error("size 必须为正整数 [x,y,z]");
    if (blocks.empty()) throw std::runtime_error("blocks 至少需要一个方块名");
    if (fill < 0 || fill >= static_cast<int>(blocks.size())) throw std::runtime_error("fill 超出 blocks 索引范围");
    int64_t total64 = static_cast<int64_t>(sx) * static_cast<int64_t>(sy) * static_cast<int64_t>(sz);
    if (total64 > static_cast<int64_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("size 乘积过大，无法构建 mcstructure");
    int total = static_cast<int>(total64);

    // palette.default.block_palette: 每个方块名 → {name, states(空), version}
    std::vector<NbtTag> palette_items;
    for (const auto& name : blocks) {
        NbtCompound bc;
        bc.emplace_back("name", tag_string(name));
        bc.emplace_back("states", tag_compound());
        bc.emplace_back("version", tag_int(block_version));
        palette_items.push_back(tag_compound(std::move(bc)));
    }

    // block_indices: 两层（layer0=fill，layer1=-1），各 total 个 int，ZYX 顺序由调用方后续编辑
    std::vector<NbtTag> layer0, layer1; layer0.reserve(total); layer1.reserve(total);
    for (int i = 0; i < total; ++i) { layer0.push_back(tag_int(fill)); layer1.push_back(tag_int(-1)); }
    NbtTag block_indices = tag_list(TagType::List, { tag_list(TagType::Int, std::move(layer0)), tag_list(TagType::Int, std::move(layer1)) });

    NbtCompound def;
    def.emplace_back("block_palette", tag_list(TagType::Compound, std::move(palette_items)));
    def.emplace_back("block_position_data", tag_compound());
    NbtCompound palette; palette.emplace_back("default", tag_compound(std::move(def)));

    NbtCompound structure;
    structure.emplace_back("block_indices", std::move(block_indices));
    structure.emplace_back("entities", tag_list(TagType::Compound));   // 空实体列表
    structure.emplace_back("palette", tag_compound(std::move(palette)));

    NbtCompound root;
    root.emplace_back("format_version", tag_int(1));
    root.emplace_back("size", tag_list(TagType::Int, { tag_int(sx), tag_int(sy), tag_int(sz) }));
    root.emplace_back("structure", tag_compound(std::move(structure)));
    root.emplace_back("structure_world_origin", tag_list(TagType::Int, { tag_int(0), tag_int(0), tag_int(0) }));

    NbtFile f;
    f.root = tag_compound(std::move(root));
    f.little_endian = true;          // 基岩 .mcstructure 固定小端
    f.has_leveldat_header = false;
    return f;
}

} // namespace mcdk::nbt
