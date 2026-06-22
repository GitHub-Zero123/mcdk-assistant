#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Reproduce stdio MCP failures under concurrent large responses.

The script starts a fresh MCP stdio process per case, initializes it, writes a
burst of JSON-RPC tools/call requests, then records how many responses arrive
before timeout or process exit.
"""

import argparse
import json
import queue
import subprocess
import threading
import time
from pathlib import Path


DEFAULT_KEYWORDS = [
    "minecraft:entity",
    "minecraft:block",
    "texture",
    "animation",
    "render_controllers",
    "materials",
]


def pump_lines(stream, out_queue, name):
    while True:
        line = stream.readline()
        if not line:
            break
        out_queue.put((name, line))


def send_json(proc, obj):
    proc.stdin.write(json.dumps(obj, ensure_ascii=False, separators=(",", ":")) + "\n")
    proc.stdin.flush()


def start_stdio_server(exe, startup_timeout):
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
    threading.Thread(target=pump_lines, args=(proc.stdout, out_queue, "stdout"), daemon=True).start()
    threading.Thread(target=pump_lines, args=(proc.stderr, out_queue, "stderr"), daemon=True).start()

    send_json(
        proc,
        {
            "jsonrpc": "2.0",
            "id": 1,
            "method": "initialize",
            "params": {
                "protocolVersion": "2025-03-26",
                "capabilities": {},
                "clientInfo": {"name": "stdio-concurrency-repro", "version": "1"},
            },
        },
    )

    deadline = time.time() + startup_timeout
    stderr_lines = []
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(f"server exited during initialize: {proc.returncode}")

        try:
            src, line = out_queue.get(timeout=0.2)
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
            send_json(proc, {"jsonrpc": "2.0", "method": "notifications/initialized", "params": {}})
            return proc, out_queue, stderr_lines

    raise TimeoutError("initialize response timed out")


def run_case(exe, keywords, top_k, timeout, startup_timeout, mode):
    proc = None
    try:
        proc, out_queue, stderr_lines = start_stdio_server(exe, startup_timeout)
        expected_ids = set(range(100, 100 + len(keywords)))
        lengths = {}
        parse_errors = 0

        for offset, keyword in enumerate(keywords):
            send_json(
                proc,
                {
                    "jsonrpc": "2.0",
                    "id": 100 + offset,
                    "method": "tools/call",
                    "params": {
                        "name": "search_game_assets",
                        "arguments": {"keyword": keyword, "scope": 0, "top_k": top_k},
                    },
                },
            )

            if mode == "serial":
                expected_id = 100 + offset
                deadline = time.time() + timeout
                while time.time() < deadline:
                    if proc.poll() is not None:
                        break
                    try:
                        src, line = out_queue.get(timeout=0.2)
                    except queue.Empty:
                        continue
                    if src == "stderr":
                        stderr_lines.append(line.rstrip())
                        continue
                    try:
                        msg = json.loads(line)
                    except json.JSONDecodeError:
                        parse_errors += 1
                        continue
                    if msg.get("id") == expected_id:
                        lengths[expected_id] = len(line)
                        break

        deadline = time.time() + timeout

        while time.time() < deadline and len(lengths) < len(expected_ids):
            returncode = proc.poll()
            if returncode is not None:
                break

            try:
                src, line = out_queue.get(timeout=0.2)
            except queue.Empty:
                continue

            if src == "stderr":
                stderr_lines.append(line.rstrip())
                continue

            try:
                msg = json.loads(line)
            except json.JSONDecodeError:
                parse_errors += 1
                continue

            msg_id = msg.get("id")
            if msg_id in expected_ids:
                lengths[msg_id] = len(line)

        returncode = proc.poll()
        missing = sorted(expected_ids.difference(lengths))
        return {
            "top_k": top_k,
            "ok": len(lengths),
            "expected": len(expected_ids),
            "missing": missing,
            "lengths": dict(sorted(lengths.items())),
            "returncode": returncode,
            "parse_errors": parse_errors,
            "stderr_tail": stderr_lines[-8:],
        }
    finally:
        if proc is not None:
            try:
                proc.stdin.close()
            except Exception:
                pass
            try:
                proc.terminate()
                proc.wait(timeout=2)
            except Exception:
                proc.kill()


def parse_top_k_values(text):
    values = []
    for item in text.split(","):
        item = item.strip()
        if item:
            values.append(int(item))
    if not values:
        raise argparse.ArgumentTypeError("at least one top_k value is required")
    return values


def result_ok(result):
    return (
        result["ok"] == result["expected"]
        and not result["missing"]
        and result["returncode"] is None
        and result["parse_errors"] == 0
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--exe",
        type=Path,
        default=Path("build/x64-msvc-release/mcdk-asst-lite.exe"),
        help="Path to MCP executable.",
    )
    parser.add_argument(
        "--top-k",
        type=parse_top_k_values,
        default=parse_top_k_values("1,2,3,5,8,10,12,15,20"),
        help="Comma-separated top_k values to test.",
    )
    parser.add_argument("--concurrency", type=int, default=6, help="Number of concurrent requests per case.")
    parser.add_argument(
        "--mode",
        choices=("burst", "serial"),
        default="burst",
        help="burst writes all requests at once; serial waits for each response before sending the next request.",
    )
    parser.add_argument("--timeout", type=float, default=12.0, help="Seconds to wait for each burst.")
    parser.add_argument("--startup-timeout", type=float, default=25.0, help="Seconds to wait for initialize.")
    parser.add_argument(
        "--keywords",
        default=",".join(DEFAULT_KEYWORDS),
        help="Comma-separated search keywords. The first concurrency values are used.",
    )
    parser.add_argument(
        "--repeat-keyword",
        action="store_true",
        help="Use the first keyword for every concurrent request. This isolates concurrency from result diversity.",
    )
    parser.add_argument(
        "--cycle-keywords",
        action="store_true",
        help="Cycle the provided keywords until concurrency is reached.",
    )
    parser.add_argument("--iterations", type=int, default=1, help="Repeat the full top_k matrix this many times.")
    parser.add_argument("--fail-fast", action="store_true", help="Stop immediately when a case fails.")
    parser.add_argument(
        "--summary-only",
        action="store_true",
        help="Print compact successful case summaries and full failed cases.",
    )
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        raise SystemExit(f"executable not found: {exe}")

    keywords = [item.strip() for item in args.keywords.split(",") if item.strip()]
    if args.repeat_keyword and keywords:
        keywords = [keywords[0]] * args.concurrency
    elif args.cycle_keywords and keywords:
        keywords = [keywords[i % len(keywords)] for i in range(args.concurrency)]
    elif len(keywords) < args.concurrency:
        raise SystemExit(f"need at least {args.concurrency} keywords, got {len(keywords)}")
    else:
        keywords = keywords[: args.concurrency]

    print(f"exe={exe}")
    print(f"concurrency={args.concurrency} mode={args.mode} keywords={keywords}")
    print(f"repeat_keyword={args.repeat_keyword}")
    print(f"cycle_keywords={args.cycle_keywords}")
    print(f"iterations={args.iterations}")
    print()

    total = 0
    failures = 0
    for iteration in range(1, args.iterations + 1):
        if args.iterations > 1:
            print(f"iteration={iteration}/{args.iterations}", flush=True)
        for top_k in args.top_k:
            result = run_case(exe, keywords, top_k, args.timeout, args.startup_timeout, args.mode)
            ok = result_ok(result)
            result["case_ok"] = ok
            result["iteration"] = iteration
            total += 1
            if not ok:
                failures += 1
            if args.summary_only and ok:
                lengths = result["lengths"].values()
                compact = {
                    "iteration": iteration,
                    "top_k": top_k,
                    "ok": result["ok"],
                    "expected": result["expected"],
                    "bytes_min": min(lengths) if lengths else 0,
                    "bytes_max": max(lengths) if lengths else 0,
                    "case_ok": True,
                }
                print(json.dumps(compact, ensure_ascii=False), flush=True)
            else:
                print(json.dumps(result, ensure_ascii=False), flush=True)
            if args.fail_fast and not ok:
                print(f"summary total={total} failures={failures}", flush=True)
                raise SystemExit(1)

    print(f"summary total={total} failures={failures}", flush=True)
    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
