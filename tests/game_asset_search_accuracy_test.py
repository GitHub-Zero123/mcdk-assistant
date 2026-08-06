#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Accuracy and bounded-query regression tests for GameAssets MCP search."""

import argparse
import json
import time
from pathlib import Path

from stdio_concurrency_repro import StdioClient


def call_assets(client, request_id, command, timeout):
    response_queue = client.send_request(
        request_id,
        "tools/call",
        {
            "name": "minecraft_docs",
            "arguments": {"command": command},
        },
    )
    msg, _line, _completed_at = client.wait_response(request_id, response_queue, timeout)
    if msg is None:
        raise AssertionError(f"request timed out: {command[:160]}")
    if "error" in msg:
        raise AssertionError(f"JSON-RPC error for {command[:160]}: {msg['error']}")

    result = msg.get("result", {})
    if result.get("isError"):
        raise AssertionError(f"tool error for {command[:160]}: {result}")
    content = result.get("content", [])
    if not isinstance(content, list):
        raise AssertionError(f"invalid content for {command[:160]}: {content!r}")
    return content


def files(content):
    return [item.get("file", "") for item in content if item.get("file")]


def overlap_ratio(expected, actual):
    expected_set = set(expected)
    if not expected_set:
        return 0.0
    return len(expected_set.intersection(actual)) / len(expected_set)


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--exe",
        type=Path,
        default=Path("build/x64-msvc-release/mcdk-assistant.exe"),
    )
    parser.add_argument("--timeout", type=float, default=15.0)
    args = parser.parse_args()

    exe = args.exe.resolve()
    if not exe.exists():
        raise SystemExit(f"executable not found: {exe}")

    client = StdioClient(exe, 0)
    checks = []
    request_id = 100
    try:
        client.start(args.timeout)

        baseline = files(call_assets(client, request_id, "assets oak_stairs --top 20", args.timeout))
        request_id += 1
        require(baseline, "oak_stairs baseline returned no files")

        tail_query = "assets " + " ".join(f"zznoise{i}" for i in range(40))
        tail_query += " oak_stairs --top 20"
        tail = files(call_assets(client, request_id, tail_query, args.timeout))
        request_id += 1
        tail_overlap = overlap_ratio(baseline, tail)
        require(tail_overlap >= 0.80, f"tail-token recall too low: {tail_overlap:.2%}")
        checks.append({"name": "tail_token_recall", "overlap": tail_overlap, "results": len(tail)})

        large_query = "assets " + " ".join(f"zzunique{i}" for i in range(1024))
        large_query += " oak_stairs --top 20"
        started = time.perf_counter()
        large = files(call_assets(client, request_id, large_query, args.timeout))
        large_seconds = time.perf_counter() - started
        request_id += 1
        large_overlap = overlap_ratio(baseline, large)
        require(large_overlap >= 0.80, f"1024-token recall too low: {large_overlap:.2%}")
        require(large_seconds < 2.0, f"1024-token query too slow: {large_seconds:.3f}s")
        checks.append({
            "name": "large_unique_query",
            "overlap": large_overlap,
            "seconds": round(large_seconds, 6),
        })

        repeated_query = "assets " + " ".join(["oak_stairs"] * 100) + " --top 20"
        repeated = files(call_assets(client, request_id, repeated_query, args.timeout))
        request_id += 1
        repeated_overlap = overlap_ratio(baseline, repeated)
        require(repeated_overlap >= 0.80, f"repeated-token recall too low: {repeated_overlap:.2%}")
        checks.append({"name": "repeated_token_recall", "overlap": repeated_overlap})

        split = files(call_assets(client, request_id, "assets oak stairs --top 20", args.timeout))
        request_id += 1
        split_overlap = overlap_ratio(baseline, split)
        require(split_overlap >= 0.30, f"identifier split-form overlap too low: {split_overlap:.2%}")
        checks.append({"name": "identifier_split_form", "overlap": split_overlap})

        for keyword in ("render_controllers", "format_version"):
            result_files = files(call_assets(
                client, request_id, f"assets {keyword} --top 20", args.timeout
            ))
            request_id += 1
            require(result_files, f"expected results for {keyword}")
            checks.append({"name": keyword, "results": len(result_files)})

        bp = files(call_assets(client, request_id, "assets oak_stairs --bp --top 20", args.timeout))
        request_id += 1
        rp = files(call_assets(client, request_id, "assets oak_stairs --rp --top 20", args.timeout))
        request_id += 1
        require(bp, "BP-scoped oak_stairs returned no files")
        require(rp, "RP-scoped oak_stairs returned no files")
        require(all(path.startswith("GameAssets/behavior_packs/") for path in bp), "BP scope leaked RP files")
        require(all(path.startswith("GameAssets/resource_packs/") for path in rp), "RP scope leaked BP files")
        checks.append({"name": "scope_filtering", "bp": len(bp), "rp": len(rp)})

        nonsense = files(call_assets(
            client, request_id, "assets zzqx_no_such_asset_987654321 --top 20", args.timeout
        ))
        require(not nonsense, f"nonsense query returned {len(nonsense)} files")
        checks.append({"name": "nonsense_zero_results", "results": 0})
    finally:
        client.close()

    print(json.dumps({"ok": True, "checks": checks}, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
