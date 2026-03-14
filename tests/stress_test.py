#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MCP SSE 并发压测脚本（2024-11-05 legacy SSE 协议）
完整握手流程：
  1. GET /sse  → 读 endpoint 事件，拿到 session_url
  2. POST initialize → 从 SSE 流等 initialize 响应
  3. POST notifications/initialized → session 进入 ready 状态
  4. 循环 POST tools/call

修复：使用单一 SSEReader 后台线程解析全部 SSE 事件（包含 endpoint），
     避免主线程 iter_lines 迭代器与后台线程竞争同一个 socket。

依赖：pip install requests
用法：python tests/stress_test.py
"""

import threading
import time
import json
import random
import queue
import requests
from collections import defaultdict

# ── 配置 ─────────────────────────────────────────────────────────
HOST            = "http://127.0.0.1:18766"
SSE_ENDPOINT    = HOST + "/sse"
NUM_CLIENTS     = 100      # 并发客户端数
CALLS_PER_SEC   = 2       # 每个客户端每秒调用次数
TEST_DURATION   = 8      # 压测持续秒数
CONNECT_STAGGER = 0.05     # 客户端启动错峰间隔（秒）
INIT_TIMEOUT    = 10.0    # initialize 响应等待超时（秒）

KEYWORDS = [
    "玩家血量", "实体速度", "方块破坏", "物品栏", "音效播放",
    "粒子效果", "玩家传送", "世界坐标", "附魔", "经验值",
    "事件监听", "组件系统", "UI界面", "摄像机", "天气",
    "方块放置", "实体生成", "伤害计算", "物品栏操作", "背包",
]

# ── 统计 ─────────────────────────────────────────────────────────
stats_lock = threading.Lock()
stats = defaultdict(int)

def inc(key):
    with stats_lock:
        stats[key] += 1


# ── SSE 流读取器（后台线程）────────────────────────────────────────

class SSEReader:
    """
    在后台线程里持续消费 SSE 流。
    负责解析全部事件类型：
      - endpoint: 放入 endpoint_q（只会出现一次）
      - message:  放入 msg_queue
    不再由主线程单独迭代 iter_lines，避免迭代器竞争。
    """
    def __init__(self, resp):
        self.resp        = resp
        self.endpoint_q  = queue.Queue()   # endpoint 事件 data（session URL）
        self.msg_queue   = queue.Queue()   # message 事件的 data
        self.stop_event  = threading.Event()
        self._t = threading.Thread(target=self._run, daemon=True)
        self._t.start()

    def _run(self):
        event_type = None
        try:
            for line in self.resp.iter_lines(decode_unicode=True):
                if self.stop_event.is_set():
                    break
                if not line:
                    event_type = None
                    continue
                line = line.strip()
                if line.startswith("event:"):
                    event_type = line[len("event:"):].strip()
                elif line.startswith("data:"):
                    data = line[len("data:"):].strip()
                    if event_type == "endpoint":
                        self.endpoint_q.put(data)
                    elif event_type == "message":
                        self.msg_queue.put(data)
                    # heartbeat 忽略
        except Exception:
            pass

    def get_endpoint(self, timeout=8.0):
        """阻塞等待 endpoint 事件，返回 session URL 字符串，超时返回 None"""
        try:
            return self.endpoint_q.get(timeout=timeout)
        except queue.Empty:
            return None

    def get_next_message(self, timeout=5.0):
        """阻塞等待下一条 message 事件，返回 data 字符串，超时返回 None"""
        try:
            return self.msg_queue.get(timeout=timeout)
        except queue.Empty:
            return None

    def stop(self):
        self.stop_event.set()
        try:
            self.resp.close()
        except Exception:
            pass


# ── 单个客户端逻辑 ────────────────────────────────────────────────

def post_jsonrpc(msg_url, method, params, req_id, timeout=10):
    payload = {
        "jsonrpc": "2.0",
        "id":      req_id,
        "method":  method,
        "params":  params,
    }
    r = requests.post(msg_url, json=payload, timeout=timeout,
                      headers={"Content-Type": "application/json"})
    return r.status_code


def post_notify(msg_url, method, params, timeout=5):
    payload = {"jsonrpc": "2.0", "method": method, "params": params}
    requests.post(msg_url, json=payload, timeout=timeout,
                  headers={"Content-Type": "application/json"})


def run_client(client_id: int, stop_event: threading.Event):
    """完整 SSE legacy 握手 + tool 调用"""

    # ── 1. 建立 SSE 连接，同时启动 SSEReader ──────────────────────
    try:
        resp = requests.get(SSE_ENDPOINT, stream=True, timeout=10,
                            headers={"Accept": "text/event-stream"})
        resp.raise_for_status()
    except Exception as e:
        inc("connect_fail")
        print(f"[C{client_id:02d}] SSE 连接失败: {e}", flush=True)
        return

    # SSEReader 立即启动，在后台解析全部事件（包括 endpoint）
    sse = SSEReader(resp)

    # ── 2. 等待 endpoint 事件（由 SSEReader 后台线程解析）──────────
    endpoint_url = sse.get_endpoint(timeout=8.0)
    if not endpoint_url:
        inc("connect_fail")
        print(f"[C{client_id:02d}] 未收到 endpoint 事件", flush=True)
        sse.stop()
        return

    msg_url = (HOST + endpoint_url) if endpoint_url.startswith("/") else endpoint_url
    inc("connected")

    req_id = client_id * 100000

    # ── 3. 发送 initialize ────────────────────────────────────────
    try:
        status = post_jsonrpc(msg_url, "initialize", {
            "protocolVersion": "2024-11-05",
            "capabilities":    {},
            "clientInfo":      {"name": f"stress-{client_id:02d}", "version": "1.0"},
        }, req_id)
        req_id += 1
        if status != 202:
            inc("init_fail")
            print(f"[C{client_id:02d}] initialize POST 失败: {status}", flush=True)
            sse.stop()
            return
    except Exception as e:
        inc("init_fail")
        print(f"[C{client_id:02d}] initialize 异常: {e}", flush=True)
        sse.stop()
        return

    # ── 4. 等待 SSE 流里的 initialize 响应 ───────────────────────
    init_resp = sse.get_next_message(timeout=INIT_TIMEOUT)
    if init_resp is None:
        inc("init_fail")
        print(f"[C{client_id:02d}] 等待 initialize 响应超时", flush=True)
        sse.stop()
        return

    # 解析确认是 initialize 结果（result 里有 protocolVersion）
    try:
        resp_json = json.loads(init_resp)
        if "error" in resp_json:
            inc("init_fail")
            print(f"[C{client_id:02d}] initialize 返回错误: {resp_json['error']}", flush=True)
            sse.stop()
            return
    except Exception:
        pass  # 解析失败也继续

    # ── 5. 发送 notifications/initialized ─────────────────────────
    try:
        post_notify(msg_url, "notifications/initialized", {})
        req_id += 1
    except Exception:
        pass

    inc("initialized")
    print(f"[C{client_id:02d}] ready", flush=True)

    # ── 6. 主循环：定频 tool 调用 ─────────────────────────────────
    interval = 1.0 / CALLS_PER_SEC

    while not stop_event.is_set():
        keyword = random.choice(KEYWORDS)
        t0 = time.time()
        try:
            status = post_jsonrpc(msg_url, "tools/call", {
                "name":      "search_all",
                "arguments": {"keyword": keyword, "top_k": 1},
            }, req_id)
            req_id += 1

            if status == 202:
                inc("ok")
            else:
                inc("err")
                print(f"[C{client_id:02d}] 非 202: {status}", flush=True)

        except requests.exceptions.Timeout:
            inc("timeout")
        except Exception as e:
            inc("err")
            print(f"[C{client_id:02d}] 调用异常: {e}", flush=True)

        sleep_t = interval - (time.time() - t0)
        if sleep_t > 0:
            time.sleep(sleep_t)

    sse.stop()


# ── 主程序 ────────────────────────────────────────────────────────

def main():
    print(f"=== MCP SSE 并发压测（2024-11-05 legacy SSE 协议）===")
    print(f"服务: {HOST}")
    print(f"客户端数: {NUM_CLIENTS}  调用频率: {CALLS_PER_SEC}/s/client")
    print(f"持续时间: {TEST_DURATION}s  预期总调用: {NUM_CLIENTS * CALLS_PER_SEC * TEST_DURATION}")
    print()

    stop_event = threading.Event()
    threads    = []

    for i in range(NUM_CLIENTS):
        t = threading.Thread(target=run_client, args=(i, stop_event), daemon=True)
        t.start()
        threads.append(t)
        time.sleep(CONNECT_STAGGER)

    print(f"所有客户端启动中，压测 {TEST_DURATION}s...", flush=True)
    t_start = time.time()

    try:
        while time.time() - t_start < TEST_DURATION:
            elapsed = time.time() - t_start
            with stats_lock:
                s = dict(stats)
            conn  = s.get("connected", 0)
            init  = s.get("initialized", 0)
            cfail = s.get("connect_fail", 0)
            ifail = s.get("init_fail", 0)
            ok    = s.get("ok", 0)
            err   = s.get("err", 0) + s.get("timeout", 0)
            rate  = ok / elapsed if elapsed > 0 else 0
            print(f"[{elapsed:5.1f}s] 连接={conn}  就绪={init}/{NUM_CLIENTS}  失败={cfail+ifail}"
                  f"  成功请求={ok}  失败请求={err}  速率={rate:.1f} req/s", flush=True)
            time.sleep(3)
    except KeyboardInterrupt:
        print("\n用户中断")

    stop_event.set()
    for t in threads:
        t.join(timeout=5)

    total_elapsed = time.time() - t_start
    with stats_lock:
        s = dict(stats)

    ok      = s.get("ok", 0)
    err     = s.get("err", 0)
    timeout = s.get("timeout", 0)
    conn    = s.get("connected", 0)
    init    = s.get("initialized", 0)
    cfail   = s.get("connect_fail", 0)
    ifail   = s.get("init_fail", 0)
    total   = ok + err + timeout
    rate    = ok / total_elapsed if total_elapsed > 0 else 0

    print(f"\n=== 压测结果 ===")
    print(f"持续时间    : {total_elapsed:.1f}s")
    print(f"SSE 连接    : 成功={conn}  失败={cfail}")
    print(f"initialize  : 成功={init}  失败={ifail}")
    print(f"tool 调用   : 总={total}  成功={ok}  失败={err}  超时={timeout}")
    print(f"成功率      : {ok/total*100:.1f}%" if total else "成功率: N/A")
    print(f"平均吞吐    : {rate:.1f} req/s")

if __name__ == "__main__":
    main()