// nbt_test.cpp — NBT 编解码核心的最小自检
// 验证: 真实 .mcstructure 的字节级 round-trip、tagged-JSON 往返、树渲染非空。
// 通过 MCDK_KNOWLEDGE_DIR 定位真实夹具文件。
#include "tools/nbt_codec.hpp"
#include "tools/register_minecraft_nbt.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifndef MCDK_KNOWLEDGE_DIR
#define MCDK_KNOWLEDGE_DIR "."
#endif

using namespace mcdk;

static int g_failed = 0;
#define CHECK(cond, msg) do { if (!(cond)) { std::cerr << "[FAIL] " << (msg) << "\n"; ++g_failed; } \
                              else { std::cout << "[ok]   " << (msg) << "\n"; } } while (0)

// 对单个文件做完整往返校验
static void test_file(const std::string& rel) {
    std::string path = std::string(MCDK_KNOWLEDGE_DIR) + "/" + rel;
    std::cerr << "--- " << rel << " ---" << std::endl;

    auto original = nbt::read_file_bytes(path);
    CHECK(!original.empty(), "读取原始字节非空");

    // 1. 解析 → 序列化 → 字节完全一致
    nbt::NbtFile f = nbt::load_nbt_file(path);
    auto reser = nbt::serialize_nbt(f);
    CHECK(reser == original, "read_nbt → write_nbt 字节级一致");

    // 2. tagged-JSON 往返 → 重新序列化仍字节一致
    nbt::ojson tj = nbt::to_tagged_json(f.root);
    nbt::NbtTag root2 = nbt::from_tagged_json(tj);
    nbt::NbtFile f2 = f; f2.root = root2;
    auto reser2 = nbt::serialize_nbt(f2);
    CHECK(reser2 == original, "tagged-JSON 往返后字节级一致");

    // 3. 树渲染非空
    std::string tree = nbt::to_tree(f, 32, 256);
    CHECK(tree.find("Compound") != std::string::npos, "to_tree 输出含 Compound");
}

static std::string result_text(const mcp::json& r) {
    return r.at("content").at(0).at("text").get<std::string>();
}

static void test_command_tool() {
    namespace fs = std::filesystem;
    fs::path out_dir = fs::path(MCDK_KNOWLEDGE_DIR) / ".." / ".codex_tmp" / "nbt_test";
    fs::create_directories(out_dir);
    fs::path out_file = out_dir / "command_style.mcstructure";
    std::string out = mcdk::path::to_utf8(out_file);

    auto create = minecraft_nbt_detail::dispatch_minecraft_nbt(
        "create --file \"" + out + "\" --template mcstructure --size \"[2,2,1]\" "
        "--blocks \"[\\\"minecraft:stone\\\",\\\"minecraft:dirt\\\"]\" --fill 1 --overwrite");
    CHECK(result_text(create).find("[ERROR]") == std::string::npos, "minecraft_nbt create command");
    CHECK(fs::exists(out_file), "minecraft_nbt create writes file");

    auto view = minecraft_nbt_detail::dispatch_minecraft_nbt(
        "view --file \"" + out + "\" --path structure/palette/default/block_palette/1/name");
    CHECK(result_text(view).find("minecraft:dirt") != std::string::npos, "minecraft_nbt view command path");

    auto edit = minecraft_nbt_detail::dispatch_minecraft_nbt(
        "edit --file \"" + out + "\" --ops '[{\"op\":\"set\",\"path\":\"format_version\",\"value\":\"2\"}]'");
    CHECK(result_text(edit).find("[ERROR]") == std::string::npos, "minecraft_nbt edit command ops");

    nbt::NbtFile edited = nbt::load_nbt_file(out);
    auto segs = nbt::split_path("format_version");
    nbt::NbtTag* format = nbt::navigate(edited.root, segs, segs.size());
    CHECK(format && std::get<int32_t>(format->value) == 2, "minecraft_nbt edit updated file");

    auto help = minecraft_nbt_detail::dispatch_minecraft_nbt("/help");
    std::string help_text = result_text(help);
    CHECK(help_text.find("Usage: minecraft_nbt(command=") != std::string::npos, "minecraft_nbt /help shows command usage");
    CHECK(help_text.find("action=") == std::string::npos, "minecraft_nbt help does not expose legacy action tutorial");
    CHECK(help_text.find("wrapped in quotes") != std::string::npos, "minecraft_nbt help explains quoted arguments");
    CHECK(help_text.find("Without --save this writes back to the original file") != std::string::npos,
          "minecraft_nbt help warns about edit write-back");
}

int main() {
  try {
    test_file("BedrockWiki/public/assets/packs/entities/aec/aec.mcstructure");
    test_file("BedrockWiki/public/assets/packs/structures/customCrafter/customCrafterExample.mcstructure");

    // 4. 路径导航与 set/add 原语（不落盘）
    nbt::NbtFile f = nbt::load_nbt_file(
        std::string(MCDK_KNOWLEDGE_DIR) + "/BedrockWiki/public/assets/packs/entities/aec/aec.mcstructure");
    auto segs = nbt::split_path("size/0");
    nbt::NbtTag* sz0 = nbt::navigate(f.root, segs, segs.size());
    CHECK(sz0 != nullptr && sz0->type == nbt::TagType::Int, "navigate size/0 命中 Int");

    nbt::op_set(f.root, "format_version", "2", "");
    auto segs2 = nbt::split_path("format_version");
    nbt::NbtTag* fv = nbt::navigate(f.root, segs2, segs2.size());
    CHECK(fv && std::get<int32_t>(fv->value) == 2, "op_set 修改 format_version=2");

    nbt::op_add(f.root, "", "test_tag", "string", "hello", "");
    auto segs3 = nbt::split_path("test_tag");
    nbt::NbtTag* tt = nbt::navigate(f.root, segs3, segs3.size());
    CHECK(tt && tt->type == nbt::TagType::String && std::get<std::string>(tt->value) == "hello", "op_add 新增 string 标签");

    // 5. build_mcstructure: 构建 2x2x2 → 序列化 → 重新解析校验
    nbt::NbtFile ms = nbt::build_mcstructure(2, 2, 2, {"minecraft:stone", "minecraft:dirt"}, 1, 18161159);
    auto ms_bytes = nbt::serialize_nbt(ms);
    nbt::NbtReader rr(ms_bytes, 0, true);
    CHECK(static_cast<uint8_t>(rr.u(1)) == nbt::to_byte(nbt::TagType::Compound), "mcstructure 根为 Compound");
    rr.str(); nbt::NbtTag ms_root = rr.payload(nbt::TagType::Compound);
    nbt::NbtFile ms2; ms2.root = ms_root;
    auto seg_idx0 = nbt::split_path("structure/block_indices/0");
    nbt::NbtTag* layer0 = nbt::navigate(ms2.root, seg_idx0, seg_idx0.size());
    CHECK(layer0 && layer0->type == nbt::TagType::List && std::get<nbt::NbtList>(layer0->value).items.size() == 8,
          "mcstructure layer0 含 2*2*2=8 个方块下标");
    auto seg_name = nbt::split_path("structure/palette/default/block_palette/1/name");
    nbt::NbtTag* bname = nbt::navigate(ms2.root, seg_name, seg_name.size());
    CHECK(bname && std::get<std::string>(bname->value) == "minecraft:dirt", "mcstructure palette[1].name = minecraft:dirt");

    test_command_tool();

  } catch (const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return 2;
  }
    std::cout.flush();
    if (g_failed == 0) { std::cerr << "\nALL PASSED" << std::endl; return 0; }
    std::cerr << "\n" << g_failed << " CHECK(S) FAILED" << std::endl;
    return 1;
}
