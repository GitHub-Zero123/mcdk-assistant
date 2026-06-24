#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
minecraft_pixelart 可视化观测脚本。

与 minecraft_pixelart_stdio_probe.py（日志断言）不同，本脚本目的是：
  1. 打印 server 暴露的全部 tool 列表（标注像素画相关）；
  2. 调用 minecraft_pixelart 子命令组合绘制若干样例画布；
  3. 把每个样例保存成 PNG 文件，便于肉眼观测像素画效果是否正确。

每个样例同时走两条保存路径：
  - server 端 save 子命令直接写 PNG（验证文件写入链路）
  - preview 子命令返回 base64，Python 端解码另存一份（验证 image 传输链路）

输出目录默认 ./pixelart_demo_output/，可用 --outdir 覆盖。

用法:
    python tests/minecraft_pixelart_render_demo.py
    python tests/minecraft_pixelart_render_demo.py --exe build/x64-msvc-release/mcdk-assistant.exe
    python tests/minecraft_pixelart_render_demo.py --outdir D:/tmp/pixelart
"""

import argparse
import base64
import json
import os
import queue
import subprocess
import sys
import threading
import time
from pathlib import Path


# ── stdio MCP 客户端 ────────────────────────────────────────

def _pump_lines(stream, out_queue, name):
    while True:
        line = stream.readline()
        if not line:
            break
        out_queue.put((name, line))


def _send_json(proc, obj):
    proc.stdin.write(json.dumps(obj, ensure_ascii=False, separators=(",", ":")) + "\n")
    proc.stdin.flush()


def start_server(exe, startup_timeout):
    proc = subprocess.Popen(
        [str(exe), "--stdio"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding="utf-8",
        errors="replace",
        bufsize=1,
    )
    out_queue = queue.Queue()
    threading.Thread(target=_pump_lines, args=(proc.stdout, out_queue, "stdout"), daemon=True).start()
    threading.Thread(target=_pump_lines, args=(proc.stderr, out_queue, "stderr"), daemon=True).start()

    _send_json(proc, {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "initialize",
        "params": {
            "protocolVersion": "2025-03-26",
            "capabilities": {},
            "clientInfo": {"name": "pixelart-render-demo", "version": "1"},
        },
    })

    deadline = time.time() + startup_timeout
    stderr_lines = []
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"server exited during initialize: {proc.returncode}; stderr={stderr_lines[-20:]}")
        try:
            src, line = out_queue.get(timeout=0.3)
        except queue.Empty:
            continue
        if src == "stderr":
            stderr_lines.append(line.rstrip())
            continue
        try:
            msg = json.loads(line)
        except json.JSONDecodeError:
            continue
        if msg.get("id") == 1 and "result" in msg:
            _send_json(proc, {"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}})
            return proc, out_queue, stderr_lines

    raise TimeoutError(f"initialize timed out; stderr={stderr_lines[-20:]}")


def list_tools(proc, out_queue, req_id, timeout=15.0):
    _send_json(proc, {"jsonrpc": "2.0", "id": req_id, "method": "tools/list", "params": {}})
    deadline = time.time() + timeout
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"server exited during tools/list: {proc.returncode}")
        try:
            src, line = out_queue.get(timeout=0.3)
        except queue.Empty:
            continue
        if src == "stderr":
            continue
        try:
            msg = json.loads(line)
        except json.JSONDecodeError:
            continue
        if msg.get("id") == req_id:
            return msg
    raise TimeoutError("tools/list timed out")


class PixelartClient:
    """对 minecraft_pixelart 的轻量封装，自增 req_id。"""

    def __init__(self, proc, out_queue):
        self.proc = proc
        self.out_queue = out_queue
        self.req_id = 100

    def _next_id(self):
        self.req_id += 1
        return self.req_id

    def call(self, command, timeout=15.0):
        """调用 minecraft_pixelart(command)，返回原始 JSON-RPC 响应。"""
        req_id = self._next_id()
        _send_json(self.proc, {
            "jsonrpc": "2.0",
            "id": req_id,
            "method": "tools/call",
            "params": {"name": "minecraft_pixelart", "arguments": {"command": command}},
        })
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self.proc.poll() is not None:
                raise RuntimeError(f"server exited during call: {self.proc.returncode}")
            try:
                src, line = self.out_queue.get(timeout=0.3)
            except queue.Empty:
                continue
            if src == "stderr":
                continue
            try:
                msg = json.loads(line)
            except json.JSONDecodeError:
                continue
            if msg.get("id") == req_id:
                return msg
        raise TimeoutError(f"tools/call timed out for command: {command!r}")

    def text(self, command):
        """调用并提取首个 text 内容，返回 (text, error)。"""
        msg = self.call(command)
        if "error" in msg:
            return None, msg["error"]
        content = msg.get("result", {}).get("content", [])
        for item in content:
            if item.get("type") == "text":
                return item.get("text", ""), None
        return None, None

    def image_b64(self, command):
        """调用并提取首个 image 的 base64 data，返回 (b64, error)。"""
        msg = self.call(command)
        if "error" in msg:
            return None, msg["error"]
        content = msg.get("result", {}).get("content", [])
        for item in content:
            if item.get("type") == "image":
                return item.get("data", ""), None
        return None, None


# ── 辅助：构造 batch JSON ──────────────────────────────────

def checker_pixels(size, c1, c2):
    """棋盘格像素列表。"""
    px = []
    for y in range(size):
        for x in range(size):
            color = c1 if (x + y) % 2 == 0 else c2
            px.append({"x": x, "y": y, "color": color})
    return px


def pixels_to_json(px_list):
    return json.dumps(px_list, ensure_ascii=False, separators=(",", ":"))


# ── 样例渲染 ────────────────────────────────────────────────

def render_demo(client, outdir):
    """逐个样例：执行命令 → save 写 PNG → preview 拿 base64 解码写 PNG。"""
    results = []  # (name, ok, detail)

    def record(name, ok, detail=""):
        results.append((name, ok, detail))
        mark = "OK  " if ok else "FAIL"
        print(f"  [{mark}] {name}  {detail}")

    def save_pair(command_seq, sample_name):
        """对一组命令执行完后，save + preview 双路径保存。
        command_seq: List[str]  依次执行的命令（最后一条通常是 save 或无）。
        样例 PNG 路径: <outdir>/<sample_name>.png (server save)
                      <outdir>/<sample_name>_preview.png (python 解码 preview)
        """
        server_png = os.path.join(outdir, f"{sample_name}.png").replace("\\", "/")
        preview_png = os.path.join(outdir, f"{sample_name}_preview.png").replace("\\", "/")

        # 执行前置命令
        for cmd in command_seq:
            text, err = client.text(cmd)
            if err is not None:
                record(sample_name, False, f"前置命令失败: {cmd!r} -> {err}")
                return
            if text is not None and ("Failed" in text or "[ERROR]" in text):
                record(sample_name, False, f"前置命令返回错误: {cmd!r} -> {text!r}")
                return

        # server 端 save
        text, err = client.text(f"save --path {server_png}")
        if err is not None or text is None or "Saved:" not in text:
            record(sample_name, False, f"save 失败: err={err} text={text!r}")
            return
        server_ok = os.path.exists(server_png)

        # preview 解码另存
        b64, err = client.image_b64("preview --scale 8")
        if err is not None or not b64:
            record(sample_name, False, f"preview 失败: err={err}")
            return
        try:
            png_bytes = base64.b64decode(b64)
            with open(preview_png, "wb") as f:
                f.write(png_bytes)
            preview_ok = os.path.exists(preview_png) and len(png_bytes) > 0
        except Exception as e:
            record(sample_name, False, f"preview 解码异常: {e}")
            return

        if server_ok and preview_ok:
            record(sample_name, True,
                   f"server={os.path.getsize(server_png)}B  preview={os.path.getsize(preview_png)}B")
        else:
            record(sample_name, False, f"server_ok={server_ok} preview_ok={preview_ok}")

    # ── 样例 1：棋盘格 8x8 ──
    print("\n[样例 1] 棋盘格 8x8 (batch 批量绘制)")
    px = checker_pixels(8, "#FFFFFFFF", "#000000FF")
    save_pair([
        "new --w 8 --h 8 --bg #888888FF",
        f"batch --pixels '{pixels_to_json(px)}'",
    ], "01_checkerboard")

    # ── 样例 2：红圆绿底 16x16 ──
    print("\n[样例 2] 红圆绿底 16x16 (flood 背景 + circle 实心)")
    save_pair([
        "new --w 16 --h 16 --bg #2E7D32FF",          # 绿色背景
        "circle --cx 8 --cy 8 --r 5 --color #F44336FF --filled",  # 红色实心圆
    ], "02_red_circle")

    # ── 样例 3：水平渐变条 32x8 ──
    print("\n[样例 3] 水平渐变条 32x8 (gradient)")
    save_pair([
        "new --w 32 --h 8 --bg #000000FF",
        "gradient --x 0 --y 0 --w 32 --h 8 --color-a #000000FF --color-b #FFFFFFFF --direction H",
    ], "03_gradient_h")

    # ── 样例 4：笑脸 16x16 (组合绘制) ──
    print("\n[样例 4] 笑脸 16x16 (circle + line + pixel 组合)")
    # 脸轮廓：空心圆 (cx=8, cy=8, r=7) 黄色
    # 左眼 (4,6) 右眼 (12,6) 黑色 pixel
    # 嘴：line (5,11)->(11,11) 黑色
    save_pair([
        "new --w 16 --h 16 --bg #FFFFFFFF",
        "circle --cx 8 --cy 8 --r 7 --color #FFEB3BFF",            # 黄色脸轮廓（空心）
        "pixel --x 5 --y 6 --color #000000FF",                     # 左眼
        "pixel --x 6 --y 6 --color #000000FF",
        "pixel --x 10 --y 6 --color #000000FF",                    # 右眼
        "pixel --x 11 --y 6 --color #000000FF",
        "line --x0 5 --y0 11 --x1 11 --y1 11 --color #000000FF",   # 嘴
    ], "04_smiley")

    # ── 样例 5：描边方块 8x8 (outline) ──
    print("\n[样例 5] 白色方块 + 黑色描边 8x8 (outline)")
    save_pair([
        "new --w 8 --h 8 --bg #00000000",                          # 透明背景
        "rect --x 2 --y 2 --w 4 --h 4 --color #FFFFFFFF --filled", # 白色实心方块
        "outline --color #000000FF --thickness 1",                 # 黑色描边
    ], "05_outlined_block")

    # ── 样例 6：像素化 (16x16 彩色 → pixelate factor 4) ──
    print("\n[样例 6] 像素化效果 (16x16 彩色图 → pixelate --factor 4)")
    # 先画一些零散彩色像素
    colorful = [
        {"x": x, "y": y, "color": f"#{x*15:02X}{y*15:02X}{(x+y)*8:02X}FF"}
        for y in range(16) for x in range(16)
    ]
    save_pair([
        "new --w 16 --h 16 --bg #000000FF",
        f"batch --pixels '{pixels_to_json(colorful)}'",
        "pixelate --factor 4",
    ], "06_pixelated")

    # ── 样例 7：阴影效果 (8x8) ──
    print("\n[样例 7] 阴影效果 8x8 (shadow)")
    save_pair([
        "new --w 8 --h 8 --bg #00000000",
        "rect --x 2 --y 2 --w 4 --h 4 --color #4CAF50FF --filled",  # 绿色方块
        "shadow --offset-x 1 --offset-y 1 --color #000000FF --opacity 0.6",
    ], "07_shadow")

    # ── 样例 8：旋转 + 翻转链 (矩形→旋转→翻转) ──
    print("\n[样例 8] 变换链：L 形像素 → rotate 90 → flip H")
    # 画一个 L 形
    l_shape = []
    for y in range(4):
        l_shape.append({"x": 0, "y": y, "color": "#2196F3FF"})  # 竖条
    for x in range(1, 4):
        l_shape.append({"x": x, "y": 3, "color": "#2196F3FF"})  # 底横条
    save_pair([
        "new --w 8 --h 8 --bg #00000000",
        f"batch --pixels '{pixels_to_json(l_shape)}'",
        "rotate --angle 90",
        "flip --axis H",
    ], "08_transform_chain")

    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--exe", type=Path,
        default=Path("build/x64-msvc-release/mcdk-assistant.exe"),
        help="mcdk-assistant.exe 路径（full 版）",
    )
    parser.add_argument(
        "--outdir", type=Path,
        default=Path("pixelart_demo_output"),
        help="PNG 样例输出目录",
    )
    parser.add_argument("--startup-timeout", type=float, default=60.0,
                        help="initialize 超时秒数（full 版需加载索引库）")
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        print(f"[FATAL] 可执行文件不存在: {exe}", file=sys.stderr)
        return 2

    outdir = args.outdir.resolve()
    outdir.mkdir(parents=True, exist_ok=True)

    print(f"exe     = {exe}")
    print(f"outdir  = {outdir}")
    print(f"startup = {args.startup_timeout}s")

    proc = None
    try:
        print("\n启动 stdio MCP server ...")
        proc, out_queue, stderr_lines = start_server(exe, args.startup_timeout)
        if stderr_lines:
            print(f"server stderr (前几行): {stderr_lines[:3]}")

        # ── 1. 打印发现的 tool 列表 ──
        print("\n" + "=" * 60)
        print("[1] 发现的 MCP tool 列表")
        print("=" * 60)
        msg = list_tools(proc, out_queue, 200)
        tools = msg.get("result", {}).get("tools", [])
        print(f"共 {len(tools)} 个 tool:")
        pixelart_related = []
        for t in tools:
            name = t.get("name", "")
            desc = t.get("description", "").replace("\n", " ")
            if len(desc) > 80:
                desc = desc[:77] + "..."
            # 标注像素画相关
            mark = ""
            if name == "minecraft_pixelart":
                mark = "  <-- 像素画入口"
                pixelart_related.append(name)
            elif any(kw in name.lower() for kw in
                     ("canvas", "pixel", "draw", "fill", "outline", "shadow",
                      "transform", "rect", "circle", "line", "flood", "gradient")):
                mark = "  <-- 疑似旧像素画工具泄漏!"
                pixelart_related.append(name)
            print(f"  - {name}: {desc}{mark}")

        if len(pixelart_related) == 1 and pixelart_related[0] == "minecraft_pixelart":
            print(f"\n  [OK] 像素画工具已整合为单入口 minecraft_pixelart，无旧工具泄漏")
        elif len(pixelart_related) > 1:
            print(f"\n  [WARN] 发现多个像素画相关 tool: {pixelart_related}")
        else:
            print(f"\n  [WARN] 未找到 minecraft_pixelart 工具!")

        # ── 2. 渲染样例 ──
        print("\n" + "=" * 60)
        print("[2] 渲染像素画样例（save + preview 双路径保存）")
        print("=" * 60)
        client = PixelartClient(proc, out_queue)
        results = render_demo(client, str(outdir))

        # ── 3. 汇总 ──
        print("\n" + "=" * 60)
        print("[3] 汇总")
        print("=" * 60)
        passed = sum(1 for _, ok, _ in results if ok)
        failed = sum(1 for _, ok, _ in results if not ok)
        print(f"样例: {passed} 成功, {failed} 失败, 共 {len(results)} 个")
        if failed:
            print("失败样例:")
            for name, ok, detail in results:
                if not ok:
                    print(f"  - {name}: {detail}")
        print(f"\nPNG 文件输出在: {outdir}")
        print("请用图像查看器打开观测像素画效果是否正确。")
        print("提示: *_preview.png 是放大 8 倍的版本，肉眼更易分辨像素。")

        # 列出实际生成的文件
        png_files = sorted([f for f in os.listdir(outdir) if f.endswith(".png")])
        if png_files:
            print(f"\n生成的 {len(png_files)} 个 PNG 文件:")
            for f in png_files:
                size = os.path.getsize(os.path.join(outdir, f))
                print(f"  {f}  ({size} bytes)")

        return 1 if failed else 0

    finally:
        if proc is not None:
            try:
                proc.stdin.close()
            except Exception:
                pass
            try:
                proc.terminate()
                proc.wait(timeout=3)
            except Exception:
                try:
                    proc.kill()
                except Exception:
                    pass


if __name__ == "__main__":
    sys.exit(main())
