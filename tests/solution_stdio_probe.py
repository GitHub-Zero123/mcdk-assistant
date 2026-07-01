#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
解决方案层（--solution / solution <id>）stdio 冒烟测试。

启动 mcdk-assistant.exe --stdio，通过 JSON-RPC 调用 minecraft_docs 工具，验证：
  正向（exe 相邻存在 mcdk_solutions_cache.bin 时）:
    - help 文案含【solution】节与 --solution / solution <id> 说明；
    - `api PushScreen --solution` 在正常检索结果后追加"相关解决方案"指针，命中 ui-custom-screen；
    - `solution ui-custom-screen` 返回内嵌的入口正文（index.md）；
    - 不加 --solution 时【默认关闭】不追加指针块；
    - 中文意图词 `api 界面 --solution` 也能触发。
  反向（临时移走 bin，模拟"exe 相邻无解决方案缓存"）:
    - help 不含【solution】节；
    - `--solution` 即便传入也不触发（无指针块）；
    - `solution <id>` 作为未知子命令处理。

用法:
    python tests/solution_stdio_probe.py
    python tests/solution_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
"""

import argparse
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
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True, encoding="utf-8", errors="replace", bufsize=1,
        cwd=str(Path(exe).resolve().parent),
    )
    out_queue = queue.Queue()
    threading.Thread(target=_pump_lines, args=(proc.stdout, out_queue, "stdout"), daemon=True).start()
    threading.Thread(target=_pump_lines, args=(proc.stderr, out_queue, "stderr"), daemon=True).start()

    _send_json(proc, {
        "jsonrpc": "2.0", "id": 1, "method": "initialize",
        "params": {"protocolVersion": "2025-03-26", "capabilities": {},
                   "clientInfo": {"name": "sol-probe", "version": "1"}},
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


def stop_server(proc):
    if proc is None:
        return
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


class DocsClient:
    def __init__(self, proc, out_queue):
        self.proc = proc
        self.out_queue = out_queue
        self.req_id = 100

    def _call(self, command, timeout=20.0):
        self.req_id += 1
        req_id = self.req_id
        _send_json(self.proc, {
            "jsonrpc": "2.0", "id": req_id, "method": "tools/call",
            "params": {"name": "minecraft_docs", "arguments": {"command": command}},
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
        msg = self._call(command)
        if "error" in msg:
            return None, msg["error"]
        content = msg.get("result", {}).get("content", [])
        texts = [item.get("text", "") for item in content if item.get("type") == "text"]
        return ("\n---\n".join(texts) if texts else None), None


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


# ── 测试运行器 ──────────────────────────────────────────────

class TestRunner:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.failures = []

    def check(self, name, condition, detail=""):
        if condition:
            self.passed += 1
            print(f"  [PASS] {name}")
        else:
            self.failed += 1
            self.failures.append(name)
            print(f"  [FAIL] {name}  {detail}")

    def run_text(self, client, name, command, predicate, detail_fn=None):
        try:
            text, err = client.text(command)
        except Exception as e:
            self.check(name, False, f"exception: {e}")
            return None
        if err is not None:
            self.check(name, False, f"rpc error: {err}")
            return None
        ok = predicate(text) if text is not None else False
        detail = "" if ok else (detail_fn(text) if detail_fn and text else f"text={text!r}"[:400])
        self.check(name, ok, detail)
        return text


def run_positive(client, out_queue, r):
    print("\n[1] tools/list 验证")
    try:
        msg = list_tools(client.proc, out_queue, 200)
        tools = [t["name"] for t in msg.get("result", {}).get("tools", [])]
        r.check("tools/list 含 minecraft_docs", "minecraft_docs" in tools, f"tools={tools}")
    except Exception as e:
        r.check("tools/list 可调用", False, f"exception: {e}")

    print("\n[2] help 含解决方案节（bin 已加载）")
    r.run_text(client, "help 含【solution】节", "help",
               lambda t: t is not None and "【solution】" in t and "--solution" in t and "solution <id>" in t)

    print("\n[3] --solution 触发指针块")
    r.run_text(client, "api PushScreen --solution 追加相关解决方案", "api PushScreen --solution",
               lambda t: t is not None and "相关解决方案" in t and "ui-custom-screen" in t
                         and 'solution ui-custom-screen' in t)
    r.run_text(client, "中文意图词 api 界面 --solution 触发", "api 界面 --solution",
               lambda t: t is not None and "相关解决方案" in t and "ui-custom-screen" in t)

    print("\n[4] 默认关闭：不加 --solution 不追加指针")
    r.run_text(client, "api PushScreen（无 flag）不含指针块", "api PushScreen",
               lambda t: t is not None and "相关解决方案" not in t)

    print("\n[4b] tip（踩坑/小技巧）内联、不跳转")
    # SetButtonTouchUpCallback 同时命中 ui-custom-screen(方案) 与 ui-button-mappings-empty(tip)
    r.run_text(client, "tip 内联在「提示/踩坑」栏且不给跳转命令",
               "api SetButtonTouchUpCallback --solution",
               lambda t: t is not None and "提示 / 踩坑" in t and "button_mappings 必须" in t
                         and 'solution ui-button-mappings-empty' not in t)
    r.run_text(client, "同一次命中里普通方案仍以引用+跳转呈现",
               "api SetButtonTouchUpCallback --solution",
               lambda t: t is not None and "相关解决方案" in t
                         and 'solution ui-custom-screen' in t)

    print("\n[5] solution <id> 读取正文")
    r.run_text(client, "solution ui-custom-screen 返回内嵌正文", "solution ui-custom-screen",
               lambda t: t is not None and "自定义 UI 界面" in t and "解决什么问题" in t
                         and ("RegisterUI" in t or "PushScreen" in t))
    r.run_text(client, "solution 未知 id 友好报错", "solution no-such-solution-xyz",
               lambda t: t is not None and "未找到解决方案" in t)
    r.run_text(client, "solution 缺参数报错", "solution",
               lambda t: t is not None and "需要一个 id" in t)


def run_negative(client, r):
    print("\n[6] 运行时闸门：移走 bin 后无解决方案功能")
    r.run_text(client, "help 不含【solution】节", "help",
               lambda t: t is not None and "【solution】" not in t and "minecraft_docs" in t)
    r.run_text(client, "--solution 传入也不触发（无指针块）", "api PushScreen --solution",
               lambda t: t is not None and "相关解决方案" not in t)
    r.run_text(client, "solution 子命令按未知处理", "solution ui-custom-screen",
               lambda t: t is not None and "未知子命令" in t)


def main():
    # Windows 控制台默认 GBK，会让中文输出乱码并在 '✓' 处崩溃 —— 统一切到 UTF-8。
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except Exception:
            pass

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path,
                        default=Path("build/x64-msvc-release/mcdk-assistant.exe"))
    parser.add_argument("--startup-timeout", type=float, default=90.0)
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        print(f"[FATAL] 可执行文件不存在: {exe}", file=sys.stderr)
        return 2

    bin_path = exe.parent / "mcdk_solutions_cache.bin"
    if not bin_path.exists():
        print(f"[FATAL] 解决方案缓存不存在: {bin_path}\n"
              f"        先运行 mcdk-solutions-compiler 生成它。", file=sys.stderr)
        return 2

    print(f"exe = {exe}")
    print(f"solutions bin = {bin_path}")

    r = TestRunner()

    # ── 正向：bin 在位 ──
    proc = None
    try:
        print("\n启动 stdio MCP server（bin 在位）...")
        proc, out_queue, stderr_lines = start_server(exe, args.startup_timeout)
        client = DocsClient(proc, out_queue)
        run_positive(client, out_queue, r)
    finally:
        stop_server(proc)

    # ── 反向：临时移走 bin ──
    bak_path = bin_path.with_suffix(".bin.bak")
    proc = None
    try:
        os.replace(bin_path, bak_path)
        print("\n启动 stdio MCP server（bin 已移走）...")
        proc, out_queue, stderr_lines = start_server(exe, args.startup_timeout)
        client = DocsClient(proc, out_queue)
        run_negative(client, r)
    finally:
        stop_server(proc)
        if bak_path.exists():
            os.replace(bak_path, bin_path)  # 还原

    print("\n" + "=" * 60)
    print(f"结果: PASS={r.passed}  FAIL={r.failed}")
    if r.failures:
        print(f"失败用例: {r.failures}")
        return 1
    print("全部通过 ✓")
    return 0


if __name__ == "__main__":
    sys.exit(main())
