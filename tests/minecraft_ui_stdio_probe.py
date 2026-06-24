#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
minecraft_ui 单工具 stdio 冒烟测试。

启动 mcdk-assistant.exe --stdio，通过 JSON-RPC 调用 minecraft_ui 工具，
验证 7 个子命令（help/gen/diag/query/tree/search/patch）的分发与参数解析，
并确认旧的 7 个独立 JSON UI 工具不再出现在 tools/list 中。

用法:
    python tests/minecraft_ui_stdio_probe.py
    python tests/minecraft_ui_stdio_probe.py --exe build/x64-msvc-release/mcdk-assistant.exe
"""

import argparse
import json
import os
import queue
import subprocess
import sys
import tempfile
import threading
import time
from pathlib import Path


# ── 测试用 JSON UI 样例（含 namespace/main/子控件/继承/绑定，多层嵌套）──
SAMPLE_UI = json.dumps({
    "namespace": "TEST_UI",
    "main": {
        "type": "screen",
        "size": ["100%", "100%"],
        "controls": [
            {"bg": {"type": "image", "size": ["100%", "100%"], "texture": "textures/ui/bg"}},
            {"panel": {
                "type": "panel",
                "size": ["50%", "50%"],
                "anchor_from": "center", "anchor_to": "center",
                "controls": [
                    {"title": {"type": "label", "text": "Hello", "color": [1, 1, 1], "layer": 2}},
                    {"btn@common.button": {
                        "size": ["40%y", "10%"],
                        "$pressed_button_name": "button.click",
                        "controls": [
                            {"button_label": {"type": "label", "text": "OK", "layer": 3}}
                        ]
                    }}
                ]
            }},
            {"hidden_panel": {"type": "panel", "visible": False, "size": ["10%", "10%"]}}
        ]
    }
}, ensure_ascii=False)

# 故意有问题的 UI（缺 namespace、size 格式错、anchor 不成对）
BAD_UI = json.dumps({
    "main": {
        "type": "screen",
        "size": ["100%"],  # 只有一个元素，size 格式错误
        "anchor_from": "center",  # 缺 anchor_to，不成对
        "controls": [
            {"c1": {"type": "bogus_type"}},  # 无效 type
            {"c1": {"type": "label"}},  # 重复 key c1
        ]
    }
}, ensure_ascii=False)


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
    )
    out_queue = queue.Queue()
    threading.Thread(target=_pump_lines, args=(proc.stdout, out_queue, "stdout"), daemon=True).start()
    threading.Thread(target=_pump_lines, args=(proc.stderr, out_queue, "stderr"), daemon=True).start()

    _send_json(proc, {
        "jsonrpc": "2.0", "id": 1, "method": "initialize",
        "params": {"protocolVersion": "2025-03-26", "capabilities": {},
                   "clientInfo": {"name": "ui-probe", "version": "1"}},
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


class UiClient:
    def __init__(self, proc, out_queue):
        self.proc = proc
        self.out_queue = out_queue
        self.req_id = 100

    def _call(self, command, timeout=15.0):
        self.req_id += 1
        req_id = self.req_id
        _send_json(self.proc, {
            "jsonrpc": "2.0", "id": req_id, "method": "tools/call",
            "params": {"name": "minecraft_ui", "arguments": {"command": command}},
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
        """返回 (text, rpc_error)。注意：一个 tools/call 可能返回多个 content 项，
        这里取所有 text 拼接，便于断言。无 text 时返回 (None, None)。"""
        msg = self._call(command)
        if "error" in msg:
            return None, msg["error"]
        content = msg.get("result", {}).get("content", [])
        texts = [item.get("text", "") for item in content if item.get("type") == "text"]
        if texts:
            return "\n---\n".join(texts), None
        return None, None

    def all_content(self, command):
        """返回完整 content 列表（含 label），用于 gen 这种多段输出。"""
        msg = self._call(command)
        if "error" in msg:
            return None, msg["error"]
        return msg.get("result", {}).get("content", []), None


# ── 测试用例 ────────────────────────────────────────────────

class TestRunner:
    def __init__(self, client):
        self.c = client
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

    def run_text(self, name, command, predicate, detail_fn=None):
        try:
            text, err = self.c.text(command)
        except Exception as e:
            self.check(name, False, f"exception: {e}")
            return None
        if err is not None:
            self.check(name, False, f"rpc error: {err}")
            return None
        ok = predicate(text) if text is not None else False
        detail = "" if ok else (detail_fn(text) if detail_fn and text else f"text={text!r}")
        self.check(name, ok, detail)
        return text


def run_tests(proc, out_queue, tmpdir):
    client = UiClient(proc, out_queue)
    r = TestRunner(client)

    # ── 1. tools/list 验证 ──
    print("\n[1] tools/list 验证")
    try:
        msg = list_tools(proc, out_queue, 200)
        tools = [t["name"] for t in msg.get("result", {}).get("tools", [])]
        r.check("tools/list 包含 minecraft_ui", "minecraft_ui" in tools, f"tools={tools}")
        old_ui = {"get_jsonui_reference", "generate_ui_fullstack", "diagnose_ui",
                  "query_ui_control", "dump_ui_tree", "search_ui_content", "patch_ui_file"}
        leaked = old_ui.intersection(set(tools))
        r.check("旧 7 个 JSON UI 工具不再注册", len(leaked) == 0, f"leaked={sorted(leaked)}")
    except Exception as e:
        r.check("tools/list 可调用", False, f"exception: {e}")

    # ── 2. help / reference ──
    print("\n[2] help / reference")
    r.run_text("help 返回速查手册+用法", "help",
               lambda t: t is not None and "MC JSON UI 全栈速查手册" in t and "minecraft_ui" in t and "tree" in t)
    r.run_text("空 command 等价 help", "",
               lambda t: t is not None and "MC JSON UI 全栈速查手册" in t)
    r.run_text("/help 前导斜杠", "/help",
               lambda t: t is not None and "MC JSON UI 全栈速查手册" in t)
    r.run_text("reference 仅返回速查手册", "reference",
               lambda t: t is not None and "MC JSON UI 全栈速查手册" in t)

    # ── 3. gen 生成模板 ──
    print("\n[3] gen 生成全栈模板")
    try:
        content, err = client.all_content("gen --template screen_basic --ns MY_TEST_UI --mod mytest")
        if err is not None:
            r.check("gen screen_basic 返回内容", False, f"rpc error: {err}")
        else:
            labels = [item.get("label", "") for item in content]
            texts = [item.get("text", "") for item in content]
            has_json_ui = any("MY_TEST_UI" in t and "main" in t for t in texts)
            has_python = any("class" in t.lower() or "ScreenNode" in t for t in texts)
            r.check("gen 返回 json_ui 段", has_json_ui, f"labels={labels}")
            r.check("gen 返回 python_class 段", has_python, f"labels={labels}")
    except Exception as e:
        r.check("gen screen_basic 返回内容", False, f"exception: {e}")
    r.run_text("gen 缺参数报错", "gen --template screen_basic",
               lambda t: t is not None and "--ns" in t and "--mod" in t)

    # ── 4. query 查询控件 ──
    print("\n[4] query 查询控件")
    cmd_content = f"--content '{SAMPLE_UI}'"
    r.run_text("query 无 path 列顶层", f"query {cmd_content}",
               lambda t: t is not None and "main" in t and "顶层控件列表" in t)
    r.run_text("query 指定控件", f"query {cmd_content} --path main",
               lambda t: t is not None and "控件: main" in t and "panel" in t and "screen" in t)
    r.run_text("query 深层路径", f"query {cmd_content} --path main/panel",
               lambda t: t is not None and "控件: main/panel" in t and ("title" in t or "btn" in t))
    r.run_text("query 不存在路径报错", f"query {cmd_content} --path main/nonexistent",
               lambda t: t is not None and "未找到" in t)

    # ── 5. tree 结构树 ──
    print("\n[5] tree 结构树")
    r.run_text("tree 完整树", f"tree {cmd_content}",
               lambda t: t is not None and "namespace: TEST_UI" in t and "main" in t and "panel" in t and "统计:" in t)
    r.run_text("tree depth 限制", f"tree {cmd_content} --depth 1",
               lambda t: t is not None and "depth limit" in t)
    r.run_text("tree root 指定路径", f"tree {cmd_content} --root main/panel",
               lambda t: t is not None and "main/panel" in t and ("title" in t or "btn" in t))
    r.run_text("tree search 匹配", f'tree {cmd_content} --search title',
               lambda t: t is not None and "命中" in t and "title" in t)

    # ── 6. search 内容搜索 ──
    print("\n[6] search 内容搜索")
    r.run_text("search 匹配纹理", f"search {cmd_content} --keyword textures",
               lambda t: t is not None and "textures" in t and "条匹配" in t)
    r.run_text("search 匹配绑定名", f'search {cmd_content} --keyword button.click',
               lambda t: t is not None and "button.click" in t)
    r.run_text("search 无匹配", f"search {cmd_content} --keyword zzz_no_match_zzz",
               lambda t: t is not None and "未找到" in t)
    r.run_text("search 缺 keyword 报错", f"search {cmd_content}",
               lambda t: t is not None and "--keyword" in t)

    # ── 7. diag 诊断 ──
    print("\n[7] diag 诊断")
    good_cmd = f"diag {cmd_content}"
    r.run_text("diag 正常 UI 通过", good_cmd,
               lambda t: t is not None and "未发现问题" in t)
    bad_cmd = f"diag --content '{BAD_UI}'"
    r.run_text("diag 问题 UI 报错", bad_cmd,
               lambda t: t is not None and ("namespace" in t or "anchor" in t or "size" in t or "重复" in t),
               detail_fn=lambda t: f"text={t!r}")

    # ── 8. patch 增量补丁 ──
    print("\n[8] patch 增量补丁")
    # 把 SAMPLE_UI 写到临时文件，用 patch 修改后读回验证
    ui_file = os.path.join(tmpdir, "test_ui.json").replace("\\", "/")
    with open(ui_file, "w", encoding="utf-8") as f:
        f.write(SAMPLE_UI)

    # set_prop: 把 main 的 size 改掉
    # 注意：set_prop 用 "props" 对象，不是 key/value（见 ui_patcher.h 注释）
    patch_ops = json.dumps([
        {"op": "set_prop", "path": "main", "props": {"size": ["50%", "50%"]}}
    ], ensure_ascii=False)
    r.run_text("patch set_prop 成功", f'patch --file {ui_file} --ops \'{patch_ops}\'',
               lambda t: t is not None and "成功应用" in t and "1 个补丁" in t)
    # 读回验证 size 已改
    with open(ui_file, "r", encoding="utf-8") as f:
        patched = json.load(f)
    r.check("patch 后 size 已修改",
            patched["main"]["size"] == ["50%", "50%"],
            f"size={patched['main']['size']}")

    # patch 缺参数
    r.run_text("patch 缺 --file 报错", f"patch --ops '{patch_ops}'",
               lambda t: t is not None and "--file" in t)
    r.run_text("patch 缺 --ops 报错", f"patch --file {ui_file}",
               lambda t: t is not None and "--ops" in t)
    # patch 非法 ops JSON
    r.run_text("patch 非法 ops JSON", f"patch --file {ui_file} --ops 'not_json'",
               lambda t: t is not None and "解析失败" in t)

    # ── 9. 非法子命令 ──
    print("\n[9] 非法子命令容错")
    r.run_text("bogus 返回 help", "bogus",
               lambda t: t is not None and "未知子命令" in t and "minecraft_ui" in t)
    r.run_text("缺 --file/--content 报错", "query --path main",
               lambda t: t is not None and ("--file" in t or "--content" in t))

    return r


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path,
                        default=Path("build/x64-msvc-release/mcdk-assistant.exe"))
    parser.add_argument("--startup-timeout", type=float, default=60.0)
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        print(f"[FATAL] 可执行文件不存在: {exe}", file=sys.stderr)
        return 2

    print(f"exe = {exe}")
    print(f"startup_timeout = {args.startup_timeout}s")

    proc = None
    try:
        with tempfile.TemporaryDirectory(prefix="ui_probe_") as tmpdir:
            print(f"tmpdir = {tmpdir}")
            print("启动 stdio MCP server ...")
            proc, out_queue, stderr_lines = start_server(exe, args.startup_timeout)
            if stderr_lines:
                print(f"server stderr (前几行): {stderr_lines[:3]}")

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
