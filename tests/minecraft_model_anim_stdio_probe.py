#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
minecraft_model + minecraft_animation 单工具 stdio 冒烟测试。

启动 mcdk-assistant.exe --stdio，验证两个整合工具的子命令分发：
  - minecraft_model: help/parse/bone/op/mirror/create
  - minecraft_animation: help/parse/bone/op
并确认旧 10 个独立工具（model_*/animation_*/anim_op/get_*_reference）不再注册。

用法:
    python tests/minecraft_model_anim_stdio_probe.py
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
                   "clientInfo": {"name": "model-anim-probe", "version": "1"}},
    })
    deadline = time.time() + startup_timeout
    stderr_lines = []
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"server exited during initialize: {proc.returncode}")
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
            raise RuntimeError(f"server exited during tools/list")
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


class ToolClient:
    def __init__(self, proc, out_queue, tool_name):
        self.proc = proc
        self.out_queue = out_queue
        self.tool_name = tool_name
        self.req_id = 100

    def text(self, command, timeout=15.0):
        self.req_id += 1
        req_id = self.req_id
        _send_json(self.proc, {
            "jsonrpc": "2.0", "id": req_id, "method": "tools/call",
            "params": {"name": self.tool_name, "arguments": {"command": command}},
        })
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self.proc.poll() is not None:
                raise RuntimeError(f"server exited during call")
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
                texts = [i.get("text", "") for i in content if i.get("type") == "text"]
                if texts:
                    return "\n".join(texts), None
                return None, None
        raise TimeoutError(f"call timed out: {command!r}")


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

    def run(self, name, client, command, predicate, detail_fn=None):
        try:
            text, err = client.text(command)
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
    r = TestRunner()

    # ── 1. tools/list 验证 ──
    print("\n[1] tools/list 验证")
    try:
        msg = list_tools(proc, out_queue, 200)
        tools = [t["name"] for t in msg.get("result", {}).get("tools", [])]
        r.check("tools/list 包含 minecraft_model", "minecraft_model" in tools, f"tools={tools}")
        r.check("tools/list 包含 minecraft_animation", "minecraft_animation" in tools, f"tools={tools}")
        old = {"get_model_reference", "model_parse", "model_get_bone", "model_op",
               "model_mirror_symmetry", "model_create_from_template",
               "get_animation_reference", "animation_parse", "animation_get_bone_channel", "anim_op"}
        leaked = old.intersection(set(tools))
        r.check("旧 10 个模型/动画工具不再注册", len(leaked) == 0, f"leaked={sorted(leaked)}")
    except Exception as e:
        r.check("tools/list 可调用", False, f"exception: {e}")

    # ── 2. 模型测试 ──
    print("\n[2] minecraft_model 测试")
    mc = ToolClient(proc, out_queue, "minecraft_model")

    # create 生成 humanoid 模板
    geo_file = os.path.join(tmpdir, "test.geo.json").replace("\\", "/")
    r.run("create humanoid 模板", mc,
          f'create --save {geo_file} --id geometry.test_model --template humanoid',
          lambda t: t is not None and "OK: created" in t and "humanoid" in t)

    # parse 验证骨骼（humanoid 含 body/head 等）
    r.run("parse 列出骨骼", mc, f'parse --file {geo_file}',
          lambda t: t is not None and ("body" in t or "head" in t or "root" in t))

    # bone 查看详情
    r.run("bone 查看 body 详情", mc, f'bone --file {geo_file} --bone body',
          lambda t: t is not None and "body" in t)

    # op 批量：add_bone + add_cube
    ops = json.dumps([
        {"op": "add_bone", "bone_name": "customBone", "parent": "body", "pivot": "[0,0,0]"},
        {"op": "add_cube", "bone_name": "customBone", "origin": "[0,0,0]", "size": "[4,4,4]", "uv": "[0,0]"}
    ], ensure_ascii=False)
    r.run("op add_bone + add_cube", mc, f"op --file {geo_file} --ops '{ops}'",
          lambda t: t is not None and "已写回文件" in t and "customBone" in t)

    # 验证 op 生效：parse 应含 customBone
    r.run("parse 验证 op 生效", mc, f'parse --file {geo_file}',
          lambda t: t is not None and "customBone" in t)

    # bone 验证 cube 已添加
    r.run("bone 验证 customBone cube", mc, f'bone --file {geo_file} --bone customBone',
          lambda t: t is not None and "customBone" in t)

    # mirror：把 leftArm 镜像到 testRight
    r.run("mirror leftArm → testRight", mc,
          f'mirror --file {geo_file} --src leftArm --dst testRight',
          lambda t: t is not None and ("mirrored" in t or "testRight" in t or "已写回" in t))

    # help
    r.run("model help 含速查手册", mc, "help",
          lambda t: t is not None and "geometry.json" in t and "minecraft_model" in t)
    r.run("model 空 command 等价 help", mc, "",
          lambda t: t is not None and "geometry.json" in t)

    # 非法输入
    r.run("model bogus 返回 help", mc, "bogus",
          lambda t: t is not None and "未知子命令" in t)
    r.run("model parse 缺文件", mc, "parse",
          lambda t: t is not None and ("--file" in t or "--content" in t))
    r.run("model op 缺 --ops", mc, f'op --file {geo_file}',
          lambda t: t is not None and "--ops" in t)
    r.run("model op 非法 ops JSON", mc, f"op --file {geo_file} --ops 'not_json'",
          lambda t: t is not None and "解析失败" in t)
    r.run("model create 缺参数", mc, "create --template humanoid",
          lambda t: t is not None and ("--save" in t or "--id" in t))

    # ── 3. 动画测试 ──
    print("\n[3] minecraft_animation 测试")
    ac = ToolClient(proc, out_queue, "minecraft_animation")

    # 创建空动画文件（用 op add_anim，文件不存在自动创建）
    anim_file = os.path.join(tmpdir, "test.anim.json").replace("\\", "/")

    # op 批量：add_anim + set_keyframe
    anim_ops = json.dumps([
        {"op": "add_anim", "anim_name": "animation.test.walk", "loop": True, "animation_length": 1.0},
        {"op": "set_keyframe", "anim_name": "animation.test.walk", "bone_name": "leftLeg",
         "channel": "rotation", "time": "0.0", "value": "[0,0,0]"},
        {"op": "set_keyframe", "anim_name": "animation.test.walk", "bone_name": "leftLeg",
         "channel": "rotation", "time": "0.5", "value": "[-30,0,0]"},
    ], ensure_ascii=False)
    r.run("anim op add_anim + set_keyframe", ac, f"op --file {anim_file} --ops '{anim_ops}'",
          lambda t: t is not None and "已写回文件" in t and "animation.test.walk" in t)

    # parse 验证动画已创建
    r.run("anim parse 验证", ac, f'parse --file {anim_file}',
          lambda t: t is not None and "animation.test.walk" in t)

    # bone 查看 keyframe
    r.run("anim bone 查看 leftLeg keyframe", ac,
          f'bone --file {anim_file} --anim animation.test.walk --bone leftLeg',
          lambda t: t is not None and ("leftLeg" in t or "0.0" in t or "0.5" in t))

    # bone 指定 channel
    r.run("anim bone 指定 channel", ac,
          f'bone --file {anim_file} --anim animation.test.walk --bone leftLeg --channel rotation',
          lambda t: t is not None and "rotation" in t)

    # help
    r.run("anim help 含速查手册", ac, "help",
          lambda t: t is not None and "animations.json" in t and "minecraft_animation" in t)
    r.run("anim 空 command 等价 help", ac, "",
          lambda t: t is not None and "animations.json" in t)

    # 非法输入
    r.run("anim bogus 返回 help", ac, "bogus",
          lambda t: t is not None and "未知子命令" in t)
    r.run("anim parse 缺文件", ac, "parse",
          lambda t: t is not None and ("--file" in t or "--content" in t))
    r.run("anim op 缺 --ops", ac, f'op --file {anim_file}',
          lambda t: t is not None and "--ops" in t)
    r.run("anim bone 缺参数", ac, f'bone --file {anim_file}',
          lambda t: t is not None and ("--anim" in t or "--bone" in t))

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
    proc = None
    try:
        with tempfile.TemporaryDirectory(prefix="model_anim_probe_") as tmpdir:
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
