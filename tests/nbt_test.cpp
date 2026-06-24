// nbt_test.cpp — NBT 编解码核心的最小自检
// 验证: 真实 .mcstructure 的字节级 round-trip、tagged-JSON 往返、树渲染非空。
// 通过 MCDK_KNOWLEDGE_DIR 定位真实夹具文件。
#include "tools/nbt_codec.hpp"
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

    std::vector<uint8_t> original = nbt::read_file_bytes(path);
    CHECK(!original.empty(), "读取原始字节非空");

    // 1. 解析 → 序列化 → 字节完全一致
    nbt::NbtFile f = nbt::load_nbt_file(path);
    std::vector<uint8_t> reser = nbt::serialize_nbt(f);
    CHECK(reser == original, "read_nbt → write_nbt 字节级一致");

    // 2. tagged-JSON 往返 → 重新序列化仍字节一致
    nbt::ojson tj = nbt::to_tagged_json(f.root);
    nbt::NbtTag root2 = nbt::from_tagged_json(tj);
    nbt::NbtFile f2 = f; f2.root = root2;
    std::vector<uint8_t> reser2 = nbt::serialize_nbt(f2);
    CHECK(reser2 == original, "tagged-JSON 往返后字节级一致");

    // 3. 树渲染非空
    std::string tree = nbt::to_tree(f, 32, 256);
    CHECK(tree.find("Compound") != std::string::npos, "to_tree 输出含 Compound");
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
    CHECK(sz0 != nullptr && sz0->type == nbt::TAG_Int, "navigate size/0 命中 Int");

    nbt::op_set(f.root, "format_version", "2", "");
    auto segs2 = nbt::split_path("format_version");
    nbt::NbtTag* fv = nbt::navigate(f.root, segs2, segs2.size());
    CHECK(fv && std::get<int32_t>(fv->value) == 2, "op_set 修改 format_version=2");

    nbt::op_add(f.root, "", "test_tag", "string", "hello", "");
    auto segs3 = nbt::split_path("test_tag");
    nbt::NbtTag* tt = nbt::navigate(f.root, segs3, segs3.size());
    CHECK(tt && tt->type == nbt::TAG_String && std::get<std::string>(tt->value) == "hello", "op_add 新增 string 标签");

    // 5. build_mcstructure: 构建 2x2x2 → 序列化 → 重新解析校验
    nbt::NbtFile ms = nbt::build_mcstructure(2, 2, 2, {"minecraft:stone", "minecraft:dirt"}, 1, 18161159);
    std::vector<uint8_t> ms_bytes = nbt::serialize_nbt(ms);
    nbt::NbtReader rr(ms_bytes, 0, true);
    CHECK(rr.u(1) == nbt::TAG_Compound, "mcstructure 根为 Compound");
    rr.str(); nbt::NbtTag ms_root = rr.payload(nbt::TAG_Compound);
    nbt::NbtFile ms2; ms2.root = ms_root;
    auto seg_idx0 = nbt::split_path("structure/block_indices/0");
    nbt::NbtTag* layer0 = nbt::navigate(ms2.root, seg_idx0, seg_idx0.size());
    CHECK(layer0 && layer0->type == nbt::TAG_List && std::get<nbt::NbtList>(layer0->value).items.size() == 8,
          "mcstructure layer0 含 2*2*2=8 个方块下标");
    auto seg_name = nbt::split_path("structure/palette/default/block_palette/1/name");
    nbt::NbtTag* bname = nbt::navigate(ms2.root, seg_name, seg_name.size());
    CHECK(bname && std::get<std::string>(bname->value) == "minecraft:dirt", "mcstructure palette[1].name = minecraft:dirt");

  } catch (const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return 2;
  }
    std::cout.flush();
    if (g_failed == 0) { std::cerr << "\nALL PASSED" << std::endl; return 0; }
    std::cerr << "\n" << g_failed << " CHECK(S) FAILED" << std::endl;
    return 1;
}
