#pragma once
// nbt_codec.hpp — Minecraft NBT 二进制编解码核心（无新依赖）
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
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace mcdk::nbt {

// ── NBT 标签类型编码 ──────────────────────────────────────
enum : uint8_t {
    TAG_End = 0, TAG_Byte = 1, TAG_Short = 2, TAG_Int = 3, TAG_Long = 4,
    TAG_Float = 5, TAG_Double = 6, TAG_ByteArray = 7, TAG_String = 8,
    TAG_List = 9, TAG_Compound = 10, TAG_IntArray = 11, TAG_LongArray = 12
};

struct NbtTag;
// 列表标签: 所有元素共享 elem_type
struct NbtList { uint8_t elem_type = TAG_End; std::vector<NbtTag> items; };
// 复合标签: 有序键值对（保留写入顺序，round-trip 保真）
using NbtCompound = std::vector<std::pair<std::string, NbtTag>>;

// 单个 NBT 标签: type 为权威类型，value 持有对应 variant 分支
struct NbtTag {
    uint8_t type = TAG_End;
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
inline const char* tag_type_name(uint8_t t) {
    switch (t) {
        case TAG_Byte: return "byte";          case TAG_Short: return "short";
        case TAG_Int: return "int";            case TAG_Long: return "long";
        case TAG_Float: return "float";        case TAG_Double: return "double";
        case TAG_ByteArray: return "byte_array"; case TAG_String: return "string";
        case TAG_List: return "list";          case TAG_Compound: return "compound";
        case TAG_IntArray: return "int_array"; case TAG_LongArray: return "long_array";
        case TAG_End: return "end";            default: return "?";
    }
}
inline uint8_t tag_type_code(const std::string& n) {
    if (n == "end") return TAG_End;         // 空 list 的 elem_type 常为 end
    if (n == "byte") return TAG_Byte;       if (n == "short") return TAG_Short;
    if (n == "int") return TAG_Int;         if (n == "long") return TAG_Long;
    if (n == "float") return TAG_Float;     if (n == "double") return TAG_Double;
    if (n == "byte_array") return TAG_ByteArray; if (n == "string") return TAG_String;
    if (n == "list") return TAG_List;       if (n == "compound") return TAG_Compound;
    if (n == "int_array") return TAG_IntArray;   if (n == "long_array") return TAG_LongArray;
    throw std::runtime_error("未知 NBT 类型名: " + n);
}
inline bool is_scalar_type(uint8_t t) {
    return t == TAG_Byte || t == TAG_Short || t == TAG_Int || t == TAG_Long ||
           t == TAG_Float || t == TAG_Double || t == TAG_String;
}

// ── 二进制读取器（按端序逐字节组装，宿主端序无关）────────────
struct NbtReader {
    const uint8_t* cur; const uint8_t* end; bool le;
    NbtReader(const std::vector<uint8_t>& b, size_t off, bool little)
        : cur(b.data() + off), end(b.data() + b.size()), le(little) {}
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
    // 数组/列表长度: 校验不超过剩余字节（每元素至少 1 字节），避免错误端序触发巨量分配
    int32_t len() {
        int32_t n = static_cast<int32_t>(u(4));
        if (n < 0 || static_cast<size_t>(n) > remaining())
            throw std::runtime_error("NBT 长度非法: " + std::to_string(n));
        return n;
    }
    NbtTag payload(uint8_t type);
};

inline NbtTag NbtReader::payload(uint8_t type) {
    NbtTag t; t.type = type;
    switch (type) {
        case TAG_Byte:  t.value = static_cast<int8_t>(u(1)); break;
        case TAG_Short: t.value = static_cast<int16_t>(u(2)); break;
        case TAG_Int:   t.value = static_cast<int32_t>(u(4)); break;
        case TAG_Long:  t.value = static_cast<int64_t>(u(8)); break;
        case TAG_Float:  { uint32_t b = static_cast<uint32_t>(u(4)); float f;  std::memcpy(&f, &b, 4); t.value = f; } break;
        case TAG_Double: { uint64_t b = u(8);                        double d; std::memcpy(&d, &b, 8); t.value = d; } break;
        case TAG_ByteArray: {
            int32_t n = len(); std::vector<int8_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int8_t>(u(1)));
            t.value = std::move(a);
        } break;
        case TAG_String: t.value = str(); break;
        case TAG_List: {
            uint8_t et = static_cast<uint8_t>(u(1)); int32_t n = len();
            NbtList lst; lst.elem_type = et; lst.items.reserve(n);
            for (int32_t i = 0; i < n; ++i) lst.items.push_back(payload(et));
            t.value = std::move(lst);
        } break;
        case TAG_Compound: {
            NbtCompound c;
            while (true) {
                uint8_t ct = static_cast<uint8_t>(u(1));
                if (ct == TAG_End) break;
                std::string name = str();
                c.emplace_back(std::move(name), payload(ct));
            }
            t.value = std::move(c);
        } break;
        case TAG_IntArray: {
            int32_t n = len(); std::vector<int32_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int32_t>(u(4)));
            t.value = std::move(a);
        } break;
        case TAG_LongArray: {
            int32_t n = len(); std::vector<int64_t> a; a.reserve(n);
            for (int32_t i = 0; i < n; ++i) a.push_back(static_cast<int64_t>(u(8)));
            t.value = std::move(a);
        } break;
        default: throw std::runtime_error("未知 NBT 标签类型: " + std::to_string(static_cast<int>(type)));
    }
    return t;
}

// ── 二进制写出器 ──────────────────────────────────────────
struct NbtWriter {
    std::vector<uint8_t>& out; bool le;
    void u(uint64_t v, int n) {
        if (le) for (int i = 0; i < n; ++i)      out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xff));
        else    for (int i = n - 1; i >= 0; --i) out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xff));
    }
    void str(const std::string& s) { u(static_cast<uint16_t>(s.size()), 2); out.insert(out.end(), s.begin(), s.end()); }
    void payload(const NbtTag& t);
};

inline void NbtWriter::payload(const NbtTag& t) {
    switch (t.type) {
        case TAG_Byte:  u(static_cast<uint8_t>(std::get<int8_t>(t.value)), 1); break;
        case TAG_Short: u(static_cast<uint16_t>(std::get<int16_t>(t.value)), 2); break;
        case TAG_Int:   u(static_cast<uint32_t>(std::get<int32_t>(t.value)), 4); break;
        case TAG_Long:  u(static_cast<uint64_t>(std::get<int64_t>(t.value)), 8); break;
        case TAG_Float:  { float f = std::get<float>(t.value);  uint32_t b; std::memcpy(&b, &f, 4); u(b, 4); } break;
        case TAG_Double: { double d = std::get<double>(t.value); uint64_t b; std::memcpy(&b, &d, 8); u(b, 8); } break;
        case TAG_ByteArray: {
            auto& a = std::get<std::vector<int8_t>>(t.value);
            u(static_cast<uint32_t>(a.size()), 4);
            for (int8_t x : a) u(static_cast<uint8_t>(x), 1);
        } break;
        case TAG_String: str(std::get<std::string>(t.value)); break;
        case TAG_List: {
            auto& l = std::get<NbtList>(t.value);
            u(l.elem_type, 1); u(static_cast<uint32_t>(l.items.size()), 4);
            for (auto& it : l.items) payload(it);
        } break;
        case TAG_Compound: {
            auto& c = std::get<NbtCompound>(t.value);
            for (auto& kv : c) { u(kv.second.type, 1); str(kv.first); payload(kv.second); }
            u(TAG_End, 1);
        } break;
        case TAG_IntArray: {
            auto& a = std::get<std::vector<int32_t>>(t.value);
            u(static_cast<uint32_t>(a.size()), 4);
            for (int32_t x : a) u(static_cast<uint32_t>(x), 4);
        } break;
        case TAG_LongArray: {
            auto& a = std::get<std::vector<int64_t>>(t.value);
            u(static_cast<uint32_t>(a.size()), 4);
            for (int64_t x : a) u(static_cast<uint64_t>(x), 8);
        } break;
        default: throw std::runtime_error("无法写入未知 NBT 类型: " + std::to_string(static_cast<int>(t.type)));
    }
}

// ── 文件字节读写（用 from_utf8 走宽字符路径，二进制安全）──────
inline std::vector<uint8_t> read_file_bytes(const std::string& path_utf8) {
    std::ifstream ifs(mcdk::path::from_utf8(path_utf8), std::ios::binary);
    if (!ifs.is_open()) throw std::runtime_error("无法打开文件: " + path_utf8);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}
inline void write_file_bytes(const std::string& path_utf8, const std::vector<uint8_t>& data) {
    std::ofstream ofs(mcdk::path::from_utf8(path_utf8), std::ios::binary);
    if (!ofs.is_open()) throw std::runtime_error("无法写入文件: " + path_utf8);
    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

// 以指定端序完整解析（要求根为 Compound 且恰好消费全部字节，作为端序判定依据）
inline bool try_parse(const std::vector<uint8_t>& bytes, size_t off, bool le, NbtFile& out) {
    try {
        NbtReader r(bytes, off, le);
        uint8_t rt = static_cast<uint8_t>(r.u(1));
        if (rt != TAG_Compound) return false;
        std::string name = r.str();
        NbtTag root = r.payload(TAG_Compound);
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
    std::vector<uint8_t> bytes = read_file_bytes(path_utf8);
    if (bytes.size() >= 2 && bytes[0] == 0x1f && bytes[1] == 0x8b)
        throw std::runtime_error(
            "暂不支持 gzip 压缩的 NBT 文件（典型为 Java .nbt / level.dat）。"
            "当前仅支持无压缩 NBT：基岩 .mcstructure、基岩 level.dat、无压缩 .nbt。");

    // 基岩 level.dat 头探测: 前 8 字节 = version(i32 LE) + payloadLength(i32 LE)，其后紧跟 NBT(0x0a)
    size_t off = 0; bool header = false; uint32_t ver = 0;
    if (bytes.size() >= 12) {
        uint32_t len_le = static_cast<uint32_t>(bytes[4]) | (static_cast<uint32_t>(bytes[5]) << 8) |
                          (static_cast<uint32_t>(bytes[6]) << 16) | (static_cast<uint32_t>(bytes[7]) << 24);
        if (bytes[8] == TAG_Compound && static_cast<size_t>(len_le) == bytes.size() - 8) {
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
inline std::vector<uint8_t> serialize_nbt(const NbtFile& f) {
    std::vector<uint8_t> body;
    NbtWriter w{body, f.little_endian};
    w.u(f.root.type, 1);   // 根类型(0x0a)
    w.str(f.root_name);    // 根名
    w.payload(f.root);
    if (!f.has_leveldat_header) return body;

    // 重新拼回 8 字节头（version + length，均小端）
    std::vector<uint8_t> outb; outb.reserve(body.size() + 8);
    uint32_t len = static_cast<uint32_t>(body.size());
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
        case TAG_Byte:   o << static_cast<int>(std::get<int8_t>(t.value)); break;
        case TAG_Short:  o << std::get<int16_t>(t.value); break;
        case TAG_Int:    o << std::get<int32_t>(t.value); break;
        case TAG_Long:   o << std::get<int64_t>(t.value); break;
        case TAG_Float:  o << std::get<float>(t.value); break;
        case TAG_Double: o << std::get<double>(t.value); break;
        case TAG_String: o << '"' << std::get<std::string>(t.value) << '"'; break;
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
        case TAG_Compound: {
            auto& c = std::get<NbtCompound>(t.value);
            o << pad << label << "(Compound, " << c.size() << " entries)\n";
            if (depth >= max_depth) { if (!c.empty()) o << deeper << "… (depth limit)\n"; break; }
            int shown = 0;
            for (auto& kv : c) {
                if (max_items > 0 && shown++ >= max_items) { o << deeper << "… (+" << (c.size() - max_items) << " more)\n"; break; }
                render_node(o, kv.first, kv.second, indent + 1, depth + 1, max_depth, max_items);
            }
        } break;
        case TAG_List: {
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
        case TAG_ByteArray: o << pad << label << "(Byte_Array, " << std::get<std::vector<int8_t>>(t.value).size() << ") "  << fmt_num_array(std::get<std::vector<int8_t>>(t.value), max_items) << "\n"; break;
        case TAG_IntArray:  o << pad << label << "(Int_Array, "  << std::get<std::vector<int32_t>>(t.value).size() << ") " << fmt_num_array(std::get<std::vector<int32_t>>(t.value), max_items) << "\n"; break;
        case TAG_LongArray: o << pad << label << "(Long_Array, " << std::get<std::vector<int64_t>>(t.value).size() << ") " << fmt_num_array(std::get<std::vector<int64_t>>(t.value), max_items) << "\n"; break;
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
        case TAG_Byte:   j["value"] = static_cast<int>(std::get<int8_t>(t.value)); break;
        case TAG_Short:  j["value"] = std::get<int16_t>(t.value); break;
        case TAG_Int:    j["value"] = std::get<int32_t>(t.value); break;
        case TAG_Long:   j["value"] = std::get<int64_t>(t.value); break;
        case TAG_Float:  j["value"] = std::get<float>(t.value); break;
        case TAG_Double: j["value"] = std::get<double>(t.value); break;
        case TAG_String: j["value"] = std::get<std::string>(t.value); break;
        case TAG_ByteArray: { auto& a = std::get<std::vector<int8_t>>(t.value); ojson arr = ojson::array(); for (int8_t x : a) arr.push_back(static_cast<int>(x)); j["value"] = std::move(arr); } break;
        case TAG_IntArray:  { auto& a = std::get<std::vector<int32_t>>(t.value); ojson arr = ojson::array(); for (int32_t x : a) arr.push_back(x); j["value"] = std::move(arr); } break;
        case TAG_LongArray: { auto& a = std::get<std::vector<int64_t>>(t.value); ojson arr = ojson::array(); for (int64_t x : a) arr.push_back(x); j["value"] = std::move(arr); } break;
        case TAG_List: {
            auto& l = std::get<NbtList>(t.value);
            j["elem_type"] = tag_type_name(l.elem_type);
            ojson arr = ojson::array(); for (auto& it : l.items) arr.push_back(to_tagged_json(it));
            j["value"] = std::move(arr);
        } break;
        case TAG_Compound: {
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
    uint8_t type = tag_type_code(j.at("type").get<std::string>());
    NbtTag t; t.type = type;
    static const ojson kNull;
    const ojson& v = j.contains("value") ? j.at("value") : kNull;
    switch (type) {
        case TAG_Byte:   t.value = static_cast<int8_t>(v.get<int64_t>()); break;
        case TAG_Short:  t.value = static_cast<int16_t>(v.get<int64_t>()); break;
        case TAG_Int:    t.value = static_cast<int32_t>(v.get<int64_t>()); break;
        case TAG_Long:   t.value = static_cast<int64_t>(v.get<int64_t>()); break;
        case TAG_Float:  t.value = static_cast<float>(v.get<double>()); break;
        case TAG_Double: t.value = v.get<double>(); break;
        case TAG_String: t.value = v.get<std::string>(); break;
        case TAG_ByteArray: { std::vector<int8_t> a;  for (auto& e : v) a.push_back(static_cast<int8_t>(e.get<int64_t>())); t.value = std::move(a); } break;
        case TAG_IntArray:  { std::vector<int32_t> a; for (auto& e : v) a.push_back(static_cast<int32_t>(e.get<int64_t>())); t.value = std::move(a); } break;
        case TAG_LongArray: { std::vector<int64_t> a; for (auto& e : v) a.push_back(e.get<int64_t>()); t.value = std::move(a); } break;
        case TAG_List: {
            NbtList l;
            l.elem_type = j.contains("elem_type") ? tag_type_code(j.at("elem_type").get<std::string>()) : TAG_End;
            for (auto& e : v) {
                NbtTag it = from_tagged_json(e);
                if (l.elem_type == TAG_End) l.elem_type = it.type;
                if (it.type != l.elem_type) throw std::runtime_error("list 元素类型与 elem_type 不一致");
                l.items.push_back(std::move(it));
            }
            t.value = std::move(l);
        } break;
        case TAG_Compound: {
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
        if (cur->type == TAG_Compound) {
            NbtTag* nxt = nullptr;
            for (auto& kv : std::get<NbtCompound>(cur->value)) if (kv.first == s) { nxt = &kv.second; break; }
            if (!nxt) return nullptr;
            cur = nxt;
        } else if (cur->type == TAG_List) {
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
inline void assign_scalar(NbtTag& t, uint8_t type, const std::string& s) {
    t.type = type;
    switch (type) {
        case TAG_Byte:   t.value = static_cast<int8_t>(std::stoll(s)); break;
        case TAG_Short:  t.value = static_cast<int16_t>(std::stoll(s)); break;
        case TAG_Int:    t.value = static_cast<int32_t>(std::stoll(s)); break;
        case TAG_Long:   t.value = static_cast<int64_t>(std::stoll(s)); break;
        case TAG_Float:  t.value = static_cast<float>(std::stod(s)); break;
        case TAG_Double: t.value = std::stod(s); break;
        case TAG_String: t.value = s; break;
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
    if (parent->type == TAG_Compound) {
        for (auto& kv : std::get<NbtCompound>(parent->value)) {
            if (kv.first == last) {
                uint8_t type = tag_type.empty() ? kv.second.type : tag_type_code(tag_type);
                assign_scalar(kv.second, type, value);
                return "set 成功: " + path + " = " + value + " (" + tag_type_name(type) + ")";
            }
        }
        if (tag_type.empty()) throw std::runtime_error("set: 目标不存在且未提供 tag_type，无法新建");
        NbtTag t; assign_scalar(t, tag_type_code(tag_type), value);
        std::get<NbtCompound>(parent->value).emplace_back(last, std::move(t));
        return "set 新建: " + path + " = " + value + " (" + tag_type + ")";
    } else if (parent->type == TAG_List) {
        auto& l = std::get<NbtList>(parent->value);
        int idx = parse_index(last);
        if (idx < 0 || static_cast<size_t>(idx) >= l.items.size()) throw std::runtime_error("set: list 下标越界");
        uint8_t type = tag_type.empty() ? l.items[idx].type : tag_type_code(tag_type);
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
    if (parent->type == TAG_Compound) {
        auto& c = std::get<NbtCompound>(parent->value);
        for (auto it = c.begin(); it != c.end(); ++it) if (it->first == last) { c.erase(it); return "remove 成功: " + path; }
        throw std::runtime_error("remove: 键不存在: " + last);
    } else if (parent->type == TAG_List) {
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
    if (!parent || parent->type != TAG_Compound) throw std::runtime_error("rename: 父节点必须是 compound");
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

    if (container->type == TAG_Compound) {
        if (name.empty()) throw std::runtime_error("add: 向 compound 添加需要 name");
        auto& c = std::get<NbtCompound>(container->value);
        for (auto& kv : c) if (kv.first == name) throw std::runtime_error("add: 键已存在: " + name);
        c.emplace_back(name, std::move(child));
        return "add 成功: " + (path.empty() ? "" : path + "/") + name + " (" + tag_type_name(c.back().second.type) + ")";
    } else if (container->type == TAG_List) {
        auto& l = std::get<NbtList>(container->value);
        if (l.items.empty() && l.elem_type == TAG_End) l.elem_type = child.type;
        if (child.type != l.elem_type) throw std::runtime_error("add: list 元素类型必须等于 elem_type(" + std::string(tag_type_name(l.elem_type)) + ")");
        l.items.push_back(std::move(child));
        return "add 成功: " + path + "[" + std::to_string(l.items.size() - 1) + "] (" + tag_type_name(l.elem_type) + ")";
    }
    throw std::runtime_error("add: 目标必须是 compound 或 list");
}

// ── 标签工厂（供从零构建 NBT，如 nbt_create）──────────────
inline NbtTag tag_int(int32_t v)      { NbtTag t; t.type = TAG_Int;    t.value = v; return t; }
inline NbtTag tag_string(std::string v){ NbtTag t; t.type = TAG_String; t.value = std::move(v); return t; }
inline NbtTag tag_compound(NbtCompound c = {}) { NbtTag t; t.type = TAG_Compound; t.value = std::move(c); return t; }
inline NbtTag tag_list(uint8_t elem_type, std::vector<NbtTag> items = {}) {
    NbtTag t; t.type = TAG_List; NbtList l; l.elem_type = elem_type; l.items = std::move(items); t.value = std::move(l); return t;
}

// 构建一个合法可加载的简易 .mcstructure（小端、无压缩）。
// layer0 全部填 fill（palette 下标），layer1 全部 -1（无方块）。
inline NbtFile build_mcstructure(int sx, int sy, int sz,
                                 const std::vector<std::string>& blocks, int fill, int32_t block_version) {
    if (sx <= 0 || sy <= 0 || sz <= 0) throw std::runtime_error("size 必须为正整数 [x,y,z]");
    if (blocks.empty()) throw std::runtime_error("blocks 至少需要一个方块名");
    if (fill < 0 || fill >= static_cast<int>(blocks.size())) throw std::runtime_error("fill 超出 blocks 索引范围");
    int total = sx * sy * sz;

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
    NbtTag block_indices = tag_list(TAG_List, { tag_list(TAG_Int, std::move(layer0)), tag_list(TAG_Int, std::move(layer1)) });

    NbtCompound def;
    def.emplace_back("block_palette", tag_list(TAG_Compound, std::move(palette_items)));
    def.emplace_back("block_position_data", tag_compound());
    NbtCompound palette; palette.emplace_back("default", tag_compound(std::move(def)));

    NbtCompound structure;
    structure.emplace_back("block_indices", std::move(block_indices));
    structure.emplace_back("entities", tag_list(TAG_Compound));   // 空实体列表
    structure.emplace_back("palette", tag_compound(std::move(palette)));

    NbtCompound root;
    root.emplace_back("format_version", tag_int(1));
    root.emplace_back("size", tag_list(TAG_Int, { tag_int(sx), tag_int(sy), tag_int(sz) }));
    root.emplace_back("structure", tag_compound(std::move(structure)));
    root.emplace_back("structure_world_origin", tag_list(TAG_Int, { tag_int(0), tag_int(0), tag_int(0) }));

    NbtFile f;
    f.root = tag_compound(std::move(root));
    f.little_endian = true;          // 基岩 .mcstructure 固定小端
    f.has_leveldat_header = false;
    return f;
}

} // namespace mcdk::nbt
