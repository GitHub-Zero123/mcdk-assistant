# zstd (vendored)

- 上游: https://github.com/facebook/zstd
- 版本: v1.5.7 (commit f8745da6ff1ad1e7bab384bd1f9d742439278e99)
- 许可: BSD-3-Clause / GPLv2 双许可，见 LICENSE

## 裁剪内容

只保留上游 `lib/` 下编译 libzstd 核心所需的部分，目录结构与上游 `lib/` 保持一致
（源码内部使用 `../zstd.h` 这类相对包含，改动目录会破坏编译）。

已移除:

- `legacy/`、`deprecated/`、`dictBuilder/`、`dll/` —— 本项目不使用
- `decompress/huf_decompress_amd64.S` —— Huffman 解码的汇编加速路径，改用
  `ZSTD_DISABLE_ASM=1` 走 C 实现，避免引入 ASM 语言支持

  该文件是 GAS 语法，上游只在 GNU 兼容编译器上启用（见 `common/portability_macros.h`
  中 `ZSTD_ASM_SUPPORTED` 的判定）。因此在 MSVC 构建上它本来就不会被启用，移除它
  没有任何代价；只有 Linux / macOS 的 GCC/Clang 构建会因此少掉这条加速路径。
  实测该路径在本项目的索引数据上只占解压耗时的很小一部分（解压本身约占单段加载
  耗时的四分之一），故按简化构建取舍。

## 升级方式

    git clone --depth 1 --branch <tag> https://github.com/facebook/zstd.git
    # 用 zstd/lib 下的 zstd.h zstd_errors.h common/ compress/ decompress/ 覆盖本目录
    # 删除 decompress/*.S，保留本文件与 LICENSE
