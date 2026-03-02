# -*- coding: utf-8 -*-
"""
GameAssetsFormat.py
处理 knowledge/GameAssets 路径下的所有文件：
  1. 过滤后缀：不在后缀集合中的文件会被删除
     （若文件名在 KEEP_FILENAMES 中，则无论后缀如何都保留）
  2. 格式化所有 JSON 文件（支持自定义"视为JSON"的后缀集合）
"""

import os
import json

# ── 配置区 ────────────────────────────────────────────────────────────────────

# 要保留的文件后缀（小写）；不在此集合中的文件将被删除
KEEP_EXTENSIONS = {
    ".json",
    ".txt",
    ".fragment",
    ".vertex",
    ".material",
    ".h"
}

# 要保留的特定文件名（大小写敏感）；文件名在此集合中时无论后缀如何都保留
# 例如只保留中文语言文件而丢弃其他语言文件
KEEP_FILENAMES = {
    "zh_CN.lang",
}

# 视为 JSON 内容、需要格式化的后缀（小写）
# 除了 .json 之外，Minecraft 资源包中还有很多非 .json 后缀但内容是 JSON 的文件
JSON_EXTENSIONS = {
    ".json",
    ".material",  # Minecraft material 文件
    ".anim",      # 部分动画文件
}

# 目标目录（相对于本脚本所在位置）
ASSETS_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "knowledge", "GameAssets"
)

# ── 核心逻辑 ──────────────────────────────────────────────────────────────────

def iter_files(root):
    """递归遍历 root 下所有文件，yield 绝对路径。"""
    for dirpath, _dirnames, filenames in os.walk(root):
        for filename in filenames:
            yield os.path.join(dirpath, filename)


def filter_files(root, keep_exts, keep_names):
    """
    删除 root 下不满足保留条件的文件。
    保留条件（满足任意一条即保留）：
      - 文件后缀（小写）在 keep_exts 中
      - 文件名（basename，大小写敏感）在 keep_names 中
    返回 (deleted, kept) 文件路径列表。
    """
    deleted = []
    kept = []
    for filepath in iter_files(root):
        basename = os.path.basename(filepath)
        ext = os.path.splitext(filepath)[1].lower()
        if ext in keep_exts or basename in keep_names:
            kept.append(filepath)
        else:
            try:
                os.remove(filepath)
                deleted.append(filepath)
                print(u"[删除] {}".format(filepath))
            except Exception as e:
                print(u"[删除失败] {} => {}".format(filepath, e))
    return deleted, kept


def format_json_files(filepaths, json_exts):
    """
    对 filepaths 中后缀属于 json_exts 的文件进行 JSON 格式化。
    解析失败则跳过；成功则原地写入格式化内容。
    返回 (formatted, skipped) 计数。
    """
    formatted = 0
    skipped = 0
    for filepath in filepaths:
        ext = os.path.splitext(filepath)[1].lower()
        if ext not in json_exts:
            continue
        try:
            with open(filepath, "r", encoding="utf-8") as f:
                data = json.load(f)
            pretty = json.dumps(data, ensure_ascii=False, indent=4, separators=(",", ": "))
            with open(filepath, "w", encoding="utf-8") as f:
                f.write(pretty)
            formatted += 1
            print(u"[格式化] {}".format(filepath))
        except Exception as e:
            skipped += 1
            print(u"[跳过] {} => {}".format(filepath, e))
    return formatted, skipped


def main():
    if not os.path.isdir(ASSETS_DIR):
        print(u"[错误] 目录不存在: {}".format(ASSETS_DIR))
        return

    print(u"=== GameAssetsFormat 开始 ===")
    print(u"目标目录    : {}".format(ASSETS_DIR))
    print(u"保留后缀    : {}".format(KEEP_EXTENSIONS))
    print(u"保留文件名  : {}".format(KEEP_FILENAMES))
    print(u"JSON后缀    : {}".format(JSON_EXTENSIONS))
    print()

    # 步骤 1：过滤文件
    print(u"--- 步骤 1：过滤非法后缀文件 ---")
    deleted, kept = filter_files(ASSETS_DIR, KEEP_EXTENSIONS, KEEP_FILENAMES)
    print(u"删除 {} 个文件，保留 {} 个文件。\n".format(len(deleted), len(kept)))

    # 步骤 2：格式化 JSON
    print(u"--- 步骤 2：格式化 JSON 文件 ---")
    formatted, skipped = format_json_files(kept, JSON_EXTENSIONS)
    print(u"格式化 {} 个文件，跳过 {} 个文件。\n".format(formatted, skipped))

    print(u"=== GameAssetsFormat 完成 ===")


if __name__ == "__main__":
    main()
