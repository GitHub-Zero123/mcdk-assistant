#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Reproduce concurrent asset searches over MCP stdio.

The default case keeps one MCP process alive, releases multiple sender threads
through a barrier, and routes stdout responses back to callers by JSON-RPC ID.
Use --processes to model Codex sub-agents, where every agent owns a separate
stdio MCP process and all processes initialize and search concurrently.
"""

import argparse
import json
import queue
import subprocess
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path


DEFAULT_KEYWORDS = [
    "minecraft:entity",
    "minecraft:block",
    "texture",
    "animation",
    "render_controllers",
    "materials",
]


class StdioClient:
    def __init__(self, exe, index):
        self.exe = exe
        self.index = index
        self.proc = None
        self.stderr_lines = []
        self.parse_errors = 0
        self.startup_seconds = None
        self._pending = {}
        self._pending_lock = threading.Lock()
        self._write_lock = threading.Lock()

    def start(self, timeout, start_barrier=None):
        if start_barrier is not None:
            start_barrier.wait(timeout=timeout)

        started = time.perf_counter()
        self.proc = subprocess.Popen(
            [str(self.exe), "--stdio"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="replace",
            bufsize=1,
        )
        threading.Thread(target=self._pump_stdout, daemon=True).start()
        threading.Thread(target=self._pump_stderr, daemon=True).start()

        response_queue = self.send_request(
            1,
            "initialize",
            {
                "protocolVersion": "2025-06-18",
                "capabilities": {},
                "clientInfo": {"name": "stdio-concurrency-repro", "version": "2"},
            },
        )
        msg, _line, _received_at = self.wait_response(1, response_queue, timeout)
        if msg is None:
            code = self.proc.poll()
            raise TimeoutError(f"client {self.index}: initialize timed out, returncode={code}")
        if "error" in msg:
            raise RuntimeError(f"client {self.index}: initialize failed: {msg['error']}")

        self.send_notification("notifications/initialized", {})
        self.startup_seconds = time.perf_counter() - started
        return self.startup_seconds

    def _pump_stdout(self):
        for line in self.proc.stdout:
            try:
                msg = json.loads(line)
            except json.JSONDecodeError:
                self.parse_errors += 1
                continue

            msg_id = msg.get("id")
            with self._pending_lock:
                response_queue = self._pending.get(msg_id)
            if response_queue is not None:
                response_queue.put((msg, line, time.perf_counter()))

        with self._pending_lock:
            queues = list(self._pending.values())
        for response_queue in queues:
            response_queue.put((None, None, time.perf_counter()))

    def _pump_stderr(self):
        for line in self.proc.stderr:
            self.stderr_lines.append(line.rstrip())
            if len(self.stderr_lines) > 200:
                del self.stderr_lines[:100]

    def _write_json(self, obj):
        payload = json.dumps(obj, ensure_ascii=False, separators=(",", ":")) + "\n"
        with self._write_lock:
            self.proc.stdin.write(payload)
            self.proc.stdin.flush()

    def send_notification(self, method, params):
        self._write_json({"jsonrpc": "2.0", "method": method, "params": params})

    def send_request(self, request_id, method, params):
        response_queue = queue.Queue(maxsize=1)
        with self._pending_lock:
            self._pending[request_id] = response_queue
        try:
            self._write_json(
                {"jsonrpc": "2.0", "id": request_id, "method": method, "params": params}
            )
        except Exception:
            with self._pending_lock:
                self._pending.pop(request_id, None)
            raise
        return response_queue

    def wait_response(self, request_id, response_queue, timeout):
        try:
            return response_queue.get(timeout=timeout)
        except queue.Empty:
            return None, None, time.perf_counter()
        finally:
            with self._pending_lock:
                self._pending.pop(request_id, None)

    def call_command(self, request_id, command, timeout, send_barrier=None):
        ready_at = time.perf_counter()
        if send_barrier is not None:
            send_barrier.wait(timeout=timeout)
        sent_at = time.perf_counter()
        response_queue = self.send_request(
            request_id,
            "tools/call",
            {
                "name": "minecraft_docs",
                "arguments": {"command": command},
            },
        )
        msg, line, completed_at = self.wait_response(request_id, response_queue, timeout)
        return make_call_result(
            self.index,
            request_id,
            command,
            msg,
            line,
            ready_at,
            sent_at,
            completed_at,
            self.proc.poll(),
        )

    def close(self):
        if self.proc is None:
            return
        try:
            self.proc.stdin.close()
        except Exception:
            pass
        try:
            self.proc.wait(timeout=2)
        except subprocess.TimeoutExpired:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait(timeout=2)


def make_call_result(
    client_index,
    request_id,
    command,
    msg,
    line,
    ready_at,
    sent_at,
    completed_at,
    returncode,
):
    request_error = None
    if msg is not None:
        if "error" in msg:
            request_error = msg["error"]
        else:
            result = msg.get("result")
            if isinstance(result, dict) and result.get("isError"):
                request_error = result

    return {
        "client": client_index,
        "id": request_id,
        "command": command_summary(command),
        "ok": msg is not None and request_error is None,
        "timed_out": msg is None and returncode is None,
        "returncode": returncode,
        "bytes": len(line.encode("utf-8")) if line else 0,
        "barrier_wait_seconds": round(sent_at - ready_at, 6),
        "response_seconds": round(completed_at - sent_at, 6),
        "request_error": request_error,
    }


def start_clients(exe, process_count, timeout):
    clients = [StdioClient(exe, i) for i in range(process_count)]
    barrier = threading.Barrier(process_count) if process_count > 1 else None
    errors = []
    with ThreadPoolExecutor(max_workers=process_count) as executor:
        futures = {executor.submit(client.start, timeout, barrier): client for client in clients}
        for future in as_completed(futures):
            try:
                future.result()
            except Exception as exc:
                errors.append(str(exc))

    if errors:
        for client in clients:
            client.close()
        raise RuntimeError("; ".join(errors))
    return clients


def run_case(clients, commands, requests_per_process, timeout, mode, id_base):
    specs = []
    offset = 0
    for client in clients:
        for _ in range(requests_per_process):
            specs.append((client, id_base + offset, commands[offset]))
            offset += 1

    if mode == "serial":
        return [
            client.call_command(request_id, command, timeout)
            for client, request_id, command in specs
        ]

    if mode == "burst":
        pending = []
        for client, request_id, command in specs:
            sent_at = time.perf_counter()
            response_queue = client.send_request(
                request_id,
                "tools/call",
                {
                    "name": "minecraft_docs",
                    "arguments": {"command": command},
                },
            )
            pending.append((client, request_id, command, response_queue, sent_at))

        results = []
        for client, request_id, command, response_queue, sent_at in pending:
            remaining = max(0.0, timeout - (time.perf_counter() - sent_at))
            msg, line, completed_at = client.wait_response(
                request_id, response_queue, remaining
            )
            results.append(
                make_call_result(
                    client.index,
                    request_id,
                    command,
                    msg,
                    line,
                    sent_at,
                    sent_at,
                    completed_at,
                    client.proc.poll(),
                )
            )
        return results

    send_barrier = threading.Barrier(len(specs)) if len(specs) > 1 else None
    with ThreadPoolExecutor(max_workers=len(specs)) as executor:
        futures = [
            executor.submit(
                client.call_command,
                request_id,
                command,
                timeout,
                send_barrier,
            )
            for client, request_id, command in specs
        ]
        return [future.result() for future in futures]


def parse_top_k_values(text):
    values = [int(item.strip()) for item in text.split(",") if item.strip()]
    if not values:
        raise argparse.ArgumentTypeError("at least one top_k value is required")
    return values


def percentile(values, fraction):
    if not values:
        return 0.0
    ordered = sorted(values)
    return ordered[int((len(ordered) - 1) * fraction)]


def command_summary(command):
    preview_limit = 120
    return {
        "chars": len(command),
        "preview": command
        if len(command) <= preview_limit
        else command[:preview_limit] + "...",
    }


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
    parser.add_argument(
        "--concurrency",
        type=int,
        default=6,
        help="Concurrent requests per MCP process.",
    )
    parser.add_argument(
        "--processes",
        type=int,
        default=1,
        help="Concurrent MCP processes. Codex sub-agents normally own separate processes.",
    )
    parser.add_argument(
        "--mode",
        choices=("threaded", "burst", "serial"),
        default="threaded",
        help="How requests are submitted inside a case.",
    )
    parser.add_argument("--timeout", type=float, default=30.0)
    parser.add_argument("--startup-timeout", type=float, default=30.0)
    parser.add_argument(
        "--keywords",
        default=",".join(DEFAULT_KEYWORDS),
        help="Comma-separated search keywords.",
    )
    parser.add_argument(
        "--command",
        dest="commands",
        action="append",
        help=(
            "Raw minecraft_docs command. Repeat for mixed-command cases; "
            "takes precedence over --keywords and --top-k."
        ),
    )
    parser.add_argument(
        "--repeat-token-count",
        type=int,
        default=0,
        help="Prepend a generated assets command containing this many repeated tokens.",
    )
    parser.add_argument(
        "--repeat-token",
        default="a",
        help="Token used by --repeat-token-count.",
    )
    parser.add_argument("--repeat-keyword", action="store_true")
    parser.add_argument("--cycle-keywords", action="store_true")
    parser.add_argument("--iterations", type=int, default=1)
    parser.add_argument("--fail-fast", action="store_true")
    parser.add_argument("--summary-only", action="store_true")
    args = parser.parse_args()

    if args.concurrency < 1 or args.processes < 1:
        raise SystemExit("concurrency and processes must both be positive")

    exe = args.exe.resolve()
    if not exe.exists():
        raise SystemExit(f"executable not found: {exe}")

    request_count = args.concurrency * args.processes
    if args.repeat_token_count < 0:
        raise SystemExit("repeat-token-count must not be negative")
    if args.repeat_token_count and (not args.repeat_token or any(c.isspace() for c in args.repeat_token)):
        raise SystemExit("repeat-token must be one non-empty token without whitespace")

    source_commands = [item.strip() for item in (args.commands or []) if item.strip()]
    if args.repeat_token_count:
        generated = "assets " + (args.repeat_token + " ") * args.repeat_token_count + "--top 5"
        source_commands.insert(0, generated)
    source_keywords = [item.strip() for item in args.keywords.split(",") if item.strip()]
    if not source_commands and not source_keywords:
        raise SystemExit("at least one command or keyword is required")

    print(
        json.dumps(
            {
                "exe": str(exe),
                "processes": args.processes,
                "requests_per_process": args.concurrency,
                "requests_per_case": request_count,
                "mode": args.mode,
                "iterations": args.iterations,
                "commands": [command_summary(command) for command in source_commands] or None,
                "keywords": None if source_commands else source_keywords,
            },
            ensure_ascii=False,
        ),
        flush=True,
    )

    clients = []
    failures = 0
    total = 0
    try:
        clients = start_clients(exe, args.processes, args.startup_timeout)
        print(
            json.dumps(
                {
                    "startup_seconds": [round(client.startup_seconds, 6) for client in clients],
                    "startup_max_seconds": round(max(client.startup_seconds for client in clients), 6),
                }
            ),
            flush=True,
        )

        id_base = 100
        for iteration in range(1, args.iterations + 1):
            for top_k in args.top_k:
                if source_commands:
                    if args.repeat_keyword:
                        commands = [source_commands[0]] * request_count
                    elif args.cycle_keywords:
                        commands = [source_commands[i % len(source_commands)] for i in range(request_count)]
                    elif len(source_commands) < request_count:
                        raise SystemExit(
                            f"need at least {request_count} commands, got {len(source_commands)}"
                        )
                    else:
                        commands = source_commands[:request_count]
                else:
                    if args.repeat_keyword:
                        keywords = [source_keywords[0]] * request_count
                    elif args.cycle_keywords:
                        keywords = [source_keywords[i % len(source_keywords)] for i in range(request_count)]
                    elif len(source_keywords) < request_count:
                        raise SystemExit(
                            f"need at least {request_count} keywords, got {len(source_keywords)}"
                        )
                    else:
                        keywords = source_keywords[:request_count]
                    commands = [f"assets {keyword} --top {top_k}" for keyword in keywords]

                case_started = time.perf_counter()
                calls = run_case(
                    clients,
                    commands,
                    args.concurrency,
                    args.timeout,
                    args.mode,
                    id_base,
                )
                id_base += request_count
                total += 1
                ok = sum(call["ok"] for call in calls)
                timed_out = sum(call["timed_out"] for call in calls)
                case_ok = ok == request_count
                if not case_ok:
                    failures += 1
                response_times = [call["response_seconds"] for call in calls]
                result = {
                    "iteration": iteration,
                    "top_k": top_k,
                    "ok": ok,
                    "expected": request_count,
                    "timed_out": timed_out,
                    "elapsed_seconds": round(time.perf_counter() - case_started, 6),
                    "response_p50_seconds": percentile(response_times, 0.50),
                    "response_p95_seconds": percentile(response_times, 0.95),
                    "response_max_seconds": max(response_times),
                    "bytes_min": min(call["bytes"] for call in calls),
                    "bytes_max": max(call["bytes"] for call in calls),
                    "parse_errors": sum(client.parse_errors for client in clients),
                    "case_ok": case_ok,
                }
                if not args.summary_only or not case_ok:
                    result["calls"] = calls
                    result["stderr_tail"] = {
                        client.index: client.stderr_lines[-8:] for client in clients
                    }
                print(json.dumps(result, ensure_ascii=False), flush=True)
                if args.fail_fast and not case_ok:
                    raise SystemExit(1)
    finally:
        for client in clients:
            client.close()

    print(f"summary total={total} failures={failures}", flush=True)
    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
