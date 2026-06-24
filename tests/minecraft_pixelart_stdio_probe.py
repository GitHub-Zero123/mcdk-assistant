#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
minecraft_pixelart 单工具 stdio 冒烟测试。

启动 mcdk-assistant.exe --stdio，通过 JSON-RPC 调用 minecraft_pixelart 工具，
验证子命令分发、参数解析、画布操作的正确性，并确认旧的 28 个独立像素画工具
不再出现在 tools/list 中。

用法:
    python tests/minecraft_pixelart_stdio_probe.py
    python tests/minecraft_pixelart_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
"""

import argparse
import base64
import json
import os
import queue
import subprocess
import sys
import tempfile
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
    """启动 stdio MCP server 并完成 initialize 握手。"""
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
            "clientInfo": {"name": "pixelart-probe", "version": "1"},
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


def call_tool(proc, out_queue, command, req_id, timeout=15.0):
    """调用 minecraft_pixelart(command=...), 返回 result 或 error dict。"""
    _send_json(proc, {
        "jsonrpc": "2.0",
        "id": req_id,
        "method": "tools/call",
        "params": {
            "name": "minecraft_pixelart",
            "arguments": {"command": command},
        },
    })

    deadline = time.time() + timeout
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"server exited during call: {proc.returncode}")
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
    raise TimeoutError(f"tools/call timed out for command: {command!r}")


def list_tools(proc, out_queue, req_id, timeout=15.0):
    _send_json(proc, {
        "jsonrpc": "2.0",
        "id": req_id,
        "method": "tools/list",
        "params": {},
    })
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


# ── 结果提取辅助 ────────────────────────────────────────────

def extract_text(result_msg):
    """从 tools/call 响应中提取第一个 text 内容。"""
    if "error" in result_msg:
        return None, result_msg["error"]
    result = result_msg.get("result", {})
    content = result.get("content", [])
    for item in content:
        if item.get("type") == "text":
            return item.get("text", ""), None
    return None, None


def extract_image(result_msg):
    """从 tools/call 响应中提取第一个 image 的 data(base64)。"""
    if "error" in result_msg:
        return None, result_msg["error"]
    result = result_msg.get("result", {})
    content = result.get("content", [])
    for item in content:
        if item.get("type") == "image":
            return item.get("data", ""), None
    return None, None


def is_error_response(result_msg):
    """判断是否是 JSON-RPC error 响应（非 isError 字段，而是协议级 error）。"""
    return "error" in result_msg


# ── 测试用例 ────────────────────────────────────────────────

class TestRunner:
    def __init__(self, proc, out_queue):
        self.proc = proc
        self.out_queue = out_queue
        self.req_id = 100
        self.passed = 0
        self.failed = 0
        self.failures = []

    def _next_id(self):
        self.req_id += 1
        return self.req_id

    def _call(self, command):
        return call_tool(self.proc, self.out_queue, command, self._next_id())

    def check(self, name, condition, detail=""):
        if condition:
            self.passed += 1
            print(f"  [PASS] {name}")
        else:
            self.failed += 1
            self.failures.append(name)
            print(f"  [FAIL] {name}  {detail}")

    def run_text(self, name, command, predicate, detail_get=None):
        """调用命令，提取 text，用 predicate(text)->bool 判断。"""
        try:
            msg = self._call(command)
        except Exception as e:
            self.check(name, False, f"exception: {e}")
            return None
        text, err = extract_text(msg)
        if err is not None:
            self.check(name, False, f"rpc error: {err}")
            return None
        ok = predicate(text) if text is not None else False
        detail = detail_get(text) if detail_get else ("" if ok else f"text={text!r}")
        self.check(name, ok, detail)
        return text

    def run_image(self, name, command, predicate):
        try:
            msg = self._call(command)
        except Exception as e:
            self.check(name, False, f"exception: {e}")
            return None
        data, err = extract_image(msg)
        if err is not None:
            self.check(name, False, f"rpc error: {err}")
            return None
        ok = predicate(data) if data is not None else False
        self.check(name, ok, "" if ok else "no image content")
        return data


def run_tests(proc, out_queue, tmpdir):
    r = TestRunner(proc, out_queue)

    # ── 1. tools/list 验证 ──
    print("\n[1] tools/list 验证")
    try:
        msg = list_tools(proc, out_queue, 200)
        tools = [t["name"] for t in msg.get("result", {}).get("tools", [])]
        r.check("tools/list 包含 minecraft_pixelart", "minecraft_pixelart" in tools, f"tools={tools}")
        old_pa = {
            "canvas_new", "canvas_load", "canvas_save", "canvas_info", "canvas_preview",
            "draw_pixel", "draw_pixels_batch", "draw_line", "draw_rect", "draw_circle",
            "fill_flood", "fill_rect", "fill_gradient",
            "apply_outline", "apply_shadow", "apply_dithering", "apply_palette_quantize",
            "pixelate", "read_pixel", "read_area", "extract_palette", "replace_color",
            "transform_flip", "transform_rotate", "transform_scale", "transform_crop",
        }
        leaked = old_pa.intersection(set(tools))
        r.check("旧 26+ 像素画工具不再注册", len(leaked) == 0, f"leaked={sorted(leaked)}")
    except Exception as e:
        r.check("tools/list 可调用", False, f"exception: {e}")

    # ── 2. help ──
    print("\n[2] help 子命令")
    r.run_text("help 返回非空帮助", "help",
               lambda t: t is not None and "minecraft_pixelart" in t and "new --w" in t)
    r.run_text("空 command 等价 help", "",
               lambda t: t is not None and "minecraft_pixelart" in t)
    r.run_text("/help 前导斜杠等价", "/help",
               lambda t: t is not None and "minecraft_pixelart" in t)

    # ── 3. 画布生命周期 ──
    print("\n[3] 画布生命周期")
    r.run_text("new 4x4", "new --w 4 --h 4", lambda t: t == "Canvas created: 4x4")
    r.run_text("info 显示尺寸", "info", lambda t: "Width=4" in t and "Height=4" in t)
    r.run_text("pixel 画点", "pixel --x 0 --y 0 --color #FF0000FF", lambda t: t == "OK")
    r.run_text("rpixel 读点", "rpixel --x 0 --y 0", lambda t: t == "#FF0000FF")

    # 注意：command_parser 会把 \<char> 当转义序列处理，Windows 反斜杠路径会被吃掉。
    # 测试用正斜杠路径（Windows API 与 stb 均支持正斜杠）。
    png_path = os.path.join(tmpdir, "test_canvas.png").replace("\\", "/")
    r.run_text("save 到 PNG", f"save --path {png_path}", lambda t: "Saved:" in t and png_path in t)
    r.check("PNG 文件实际生成", os.path.exists(png_path), f"path={png_path}")
    r.run_text("load 重新加载", f"load --path {png_path}", lambda t: "Loaded:" in t and "4x4" in t)

    # ── 4. 绘制形状 ──
    print("\n[4] 绘制形状 + rarea")
    r.run_text("new 8x8", "new --w 8 --h 8", lambda t: "8x8" in t)
    r.run_text("rect 实心", "rect --x 1 --y 1 --w 3 --h 3 --color #00FF00FF --filled", lambda t: t == "OK")
    r.run_text("rarea 读取区域", "rarea --x 0 --y 0 --w 3 --h 3",
               lambda t: t is not None and "#00FF00FF" in t,
               detail_get=lambda t: f"text={t[:120]!r}")

    # ── 5. 变换链 (flip) ──
    print("\n[5] 变换链: flip")
    r.run_text("new 2x2", "new --w 2 --h 2", lambda t: "2x2" in t)
    r.run_text("pixel 左上红", "pixel --x 0 --y 0 --color #FF0000FF", lambda t: t == "OK")
    r.run_text("flip H", "flip --axis H", lambda t: t == "Flipped")
    r.run_text("rpixel 右上(变换后)", "rpixel --x 1 --y 0",
               lambda t: t == "#FF0000FF", detail_get=lambda t: f"text={t!r}")

    # ── 6. batch ──
    print("\n[6] batch 批量绘制")
    r.run_text("new 4x4 (batch 前)", "new --w 4 --h 4", lambda t: "4x4" in t)
    batch_json = '[{"x":0,"y":0,"color":"#FF0000FF"},{"x":1,"y":0,"color":"#00FF00FF"},{"x":2,"y":0,"color":"#0000FFFF"}]'
    r.run_text("batch 3 像素", f'batch --pixels \'{batch_json}\'',
               lambda t: t == "Drew 3 pixels")

    # ── 7. 非法输入 ──
    print("\n[7] 非法输入容错")
    r.run_text("bogus 子命令返回 help", "bogus",
               lambda t: t is not None and "未知子命令" in t and "minecraft_pixelart" in t)
    r.run_text("new 缺 --w/--h", "new",
               lambda t: t is not None and ("--w" in t or "--h" in t))
    r.run_text("pixel 缺参数", "pixel",
               lambda t: t is not None and "--color" in t)
    r.run_text("rotate 非法角度", "rotate --angle 45",
               lambda t: t is not None and "90/180/270" in t)

    # ── 8. preview (image 返回) ──
    print("\n[8] preview 返回 image")
    r.run_text("new 2x2 (preview 前)", "new --w 2 --h 2", lambda t: "2x2" in t)
    r.run_image("preview 返回 base64 image", "preview --scale 2",
                lambda data: data is not None and len(data) > 0)

    # ── 9. 其他子命令冒烟 ──
    print("\n[9] 其他子命令冒烟")
    r.run_text("new 4x4 (others 前)", "new --w 4 --h 4 --bg #000000FF", lambda t: "4x4" in t)
    r.run_text("line", "line --x0 0 --y0 0 --x1 3 --y1 3 --color #FFFFFFFF", lambda t: t == "OK")
    r.run_text("circle", "circle --cx 2 --cy 2 --r 1 --color #FFFFFFFF", lambda t: t == "OK")
    r.run_text("flood", "flood --x 0 --y 0 --color #123456FF", lambda t: t == "OK")
    r.run_text("gradient", "gradient --x 0 --y 0 --w 4 --h 4 --color-a #000000FF --color-b #FFFFFFFF",
               lambda t: t == "OK")
    r.run_text("outline", "outline --color #FFFFFFFF --thickness 1", lambda t: "Outline applied" in t)
    r.run_text("shadow", "shadow --opacity 0.5", lambda t: "Shadow applied" in t)
    r.run_text("pixelate", "pixelate --factor 2", lambda t: t == "Pixelated")
    r.run_text("rotate 90", "rotate --angle 90", lambda t: "Rotated 90" in t)
    r.run_text("scale", "scale --w 8 --h 8", lambda t: "Scaled to 8x8" in t)
    r.run_text("crop", "crop --x 0 --y 0 --w 4 --h 4", lambda t: "Cropped to 4x4" in t)
    r.run_text("recolor", "recolor --from #FFFFFFFF --to #AAAAAAFF", lambda t: t == "OK")
    r.run_text("quantize", 'quantize --palette \'["#000000FF","#FFFFFFFF","#FF0000FF"]\'',
               lambda t: "Quantized to 3 colors" in t)
    r.run_text("dither", 'dither --palette \'["#000000FF","#FFFFFFFF"]\'',
               lambda t: "Dithering applied with 2 colors" in t)
    r.run_text("palette 提取", "palette --max 4",
               lambda t: t is not None and t.startswith("["),
               detail_get=lambda t: f"text={t[:80]!r}")

    return r


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--exe", type=Path,
        default=Path("build/x64-msvc-release/mcdk-assistant.exe"),
        help="mcdk-assistant.exe 路径（full 版，含像素画工具）",
    )
    parser.add_argument("--startup-timeout", type=float, default=60.0,
                        help="initialize 超时秒数（full 版需加载索引库，留足）")
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        print(f"[FATAL] 可执行文件不存在: {exe}", file=sys.stderr)
        return 2

    print(f"exe = {exe}")
    print(f"startup_timeout = {args.startup_timeout}s")

    proc = None
    try:
        with tempfile.TemporaryDirectory(prefix="pixelart_probe_") as tmpdir:
            print(f"tmpdir = {tmpdir}")
            print("启动 stdio MCP server ...")
            proc, out_queue, stderr_lines = start_server(exe, args.startup_timeout)
            if stderr_lines:
                print(f"server stderr (前几行): {stderr_lines[:5]}")

            runner = run_tests(proc, out_queue, tmpdir)

        print("\n" + "=" * 60)
        print(f"结果: PASS={runner.passed}  FAIL={runner.failed}")
        if runner.failures:
            print(f"失败用例: {runner.failures}")
            return 1
        print("全部通过 ✓")
        return 0

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
