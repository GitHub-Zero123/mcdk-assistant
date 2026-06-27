#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
minecraft_sapi 工具 stdio 验证脚本。

启动 mcdk-assistant.exe --stdio，通过 JSON-RPC 调用 minecraft_sapi 工具，
验证两个修复：
  1. 枚举成员的“名 + 值”都返回（含每个枚举值的 doc）。
  2. 类/成员的 JSDoc 不再被 `// @ts-ignore` 指令行遮挡，且成员列表带首句文档。

用法:
    python tests/sapi_stdio_probe.py
    python tests/sapi_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
"""

import argparse
import json
import queue
import subprocess
import sys
import threading
import time
from pathlib import Path


def _pump_lines(stream, out_queue, name):
    for line in iter(stream.readline, ""):
        out_queue.put((name, line))


def _send_json(proc, obj):
    proc.stdin.write(json.dumps(obj, ensure_ascii=False, separators=(",", ":")) + "\n")
    proc.stdin.flush()


def start_server(exe, startup_timeout=30.0):
    proc = subprocess.Popen(
        [str(exe), "--stdio"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        text=True, encoding="utf-8", errors="replace", bufsize=1,
    )
    out_queue = queue.Queue()
    threading.Thread(target=_pump_lines, args=(proc.stdout, out_queue, "stdout"), daemon=True).start()
    threading.Thread(target=_pump_lines, args=(proc.stderr, out_queue, "stderr"), daemon=True).start()

    _send_json(proc, {
        "jsonrpc": "2.0", "id": 1, "method": "initialize",
        "params": {"protocolVersion": "2025-03-26", "capabilities": {},
                   "clientInfo": {"name": "sapi-probe", "version": "1"}},
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


class SapiClient:
    def __init__(self, proc, out_queue):
        self.proc = proc
        self.out_queue = out_queue
        self.req_id = 100

    def text(self, command, timeout=20.0):
        self.req_id += 1
        req_id = self.req_id
        _send_json(self.proc, {
            "jsonrpc": "2.0", "id": req_id, "method": "tools/call",
            "params": {"name": "minecraft_sapi", "arguments": {"command": command}},
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
                if "error" in msg:
                    return None, msg["error"]
                content = msg.get("result", {}).get("content", [])
                texts = [c.get("text", "") for c in content if c.get("type") == "text"]
                return "\n".join(texts), None
        raise TimeoutError(f"tools/call timed out for command: {command!r}")


class Runner:
    def __init__(self):
        self.passed = 0
        self.failed = 0

    def check(self, name, cond, detail=""):
        if cond:
            self.passed += 1
            print(f"  [PASS] {name}")
        else:
            self.failed += 1
            print(f"  [FAIL] {name}  {detail}")


def main():
    here = Path(__file__).resolve().parent.parent
    default_exe = here / "build" / "x64-msvc-release" / "mcdk-assistant.exe"
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", default=str(default_exe))
    args = ap.parse_args()

    exe = Path(args.exe)
    if not exe.exists():
        print(f"ERROR: exe not found: {exe}")
        return 2

    proc, out_queue, _ = start_server(exe)
    c = SapiClient(proc, out_queue)
    r = Runner()

    # ── 修复 2：枚举成员名 + 值 + doc ──
    print("[group] 枚举值 (problem 2)")
    enum_txt, err = c.text("symbol AimAssistTargetMode")
    enum_txt = enum_txt or ""
    r.check("enum: 有 members 区块", "members" in enum_txt, enum_txt[:200])
    r.check("enum: Angle 名+值", "Angle = Angle" in enum_txt or "Angle = 'Angle'" in enum_txt,
            "未见 Angle 的值")
    r.check("enum: Distance 名+值", "Distance = Distance" in enum_txt or "Distance = 'Distance'" in enum_txt,
            "未见 Distance 的值")
    r.check("enum: 枚举值带 doc", "based targeting" in enum_txt, "未见枚举成员 doc")

    # 大枚举：值为 minecraft: 命名空间 id
    block_txt, _ = c.text("symbol MinecraftBlockTypes --members 5")
    block_txt = block_txt or ""
    r.check("big-enum: 值含 minecraft: id", "= minecraft:" in block_txt, block_txt[:200])

    # ── 修复 1：JSDoc 不被 // @ts-ignore 遮挡 + 成员 doc 摘要 ──
    print("[group] 文档归属 (problem 1)")
    player_txt, err = c.text("symbol Player --members 6")
    player_txt = player_txt or ""
    r.check("class doc: 取到真实 JSDoc",
            "Represents a player within the world" in player_txt, player_txt[:300])
    r.check("class doc: 不再误取 @ts-ignore",
            "@ts-ignore" not in player_txt, "doc 仍是 @ts-ignore 指令行")
    r.check("member doc: 成员列表带首句文档",
            "The player's Camera" in player_txt, "camera 成员缺 doc 摘要")

    # ── 模块级文档/演示代码（Manifest Details 版本块）可见且可搜 ──
    print("[group] 模块文档/演示代码")
    mod_txt, _ = c.text("module @minecraft/common --limit 2")
    mod_txt = mod_txt or ""
    r.check("module: 顶部显示 package doc",
            "package doc:" in mod_txt and "Manifest Details" in mod_txt, mod_txt[:300])
    r.check("module: 含 json 版本块",
            "module_name" in mod_txt and "version" in mod_txt, "缺版本 json")
    sym_mod, _ = c.text("symbol @minecraft/common")
    sym_mod = sym_mod or ""
    r.check("symbol <模块名>: 返回 package doc",
            "kind: module" in sym_mod and "Manifest Details" in sym_mod, sym_mod[:200])
    srch_mod, _ = c.text("search manifest details --refs 0")
    srch_mod = srch_mod or ""
    r.check("search: 命中模块版本块",
            "kind: module" in srch_mod or "Manifest Details" in srch_mod, srch_mod[:200])

    proc.terminate()
    print(f"\n==== {r.passed} passed, {r.failed} failed ====")
    return 0 if r.failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
