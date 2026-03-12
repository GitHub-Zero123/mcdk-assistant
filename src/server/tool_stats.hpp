#pragma once
/**
 * @file tool_stats.hpp
 * @brief 工具调用次数统计（仅 MCDK_SERVER 模式启用）
 *
 * 设计：
 *  - 热路径：increment(name) 使用 std::atomic<uint64_t> 无锁计数，首次出现时自动注册
 *  - 后台线程每 FLUSH_INTERVAL_SEC 秒将内存增量 flush 到磁盘 JSON 文件
 *  - 磁盘文件位于 <exe_dir>/datas/tool_stats.json，持久化累计次数
 *  - 程序正常退出时（析构）执行最后一次 flush，防止数据丢失
 *  - get_snapshot() 返回当前所有工具的累计调用次数
 *
 * 使用方式（与 mcp::server 解耦）：
 *   auto stats = std::make_shared<mcdk::server::ToolStats>(datas_dir);
 *   srv.set_tool_call_handler([stats](const std::string& name) {
 *       stats->increment(name);
 *   });
 */

#ifdef MCDK_SERVER

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace mcdk {
namespace server {

using json = nlohmann::json;

// ─── 常量 ─────────────────────────────────────────────────────────────────────
static constexpr int         FLUSH_INTERVAL_SEC = 1800; // 每 30 分钟写盘一次
static constexpr const char* STATS_FILE         = "tool_stats.json";
static constexpr const char* DATAS_DIR          = "datas";

// ─── ToolStats ────────────────────────────────────────────────────────────────
class ToolStats {
public:
    /**
     * @param datas_dir  <exe_dir>/datas 目录的绝对路径
     */
    explicit ToolStats(std::filesystem::path datas_dir)
        : datas_dir_(std::move(datas_dir))
        , stats_file_(datas_dir_ / STATS_FILE) {
        std::filesystem::create_directories(datas_dir_);
        load_from_disk();
        flush_thread_ = std::thread(&ToolStats::flush_loop, this);
    }

    ~ToolStats() {
        {
            std::unique_lock<std::mutex> lk(cv_mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        if (flush_thread_.joinable()) flush_thread_.join();
        flush_to_disk(); // 最后一次写盘，确保数据不丢失
    }

    // ── 热路径：工具调用时递增（线程安全，首次自动注册）─────────────────────
    void increment(const std::string& name) {
        // 快速路径：已注册的工具
        {
            std::shared_lock<std::shared_mutex> rl(map_mtx_);
            auto it = counters_.find(name);
            if (it != counters_.end()) {
                it->second.fetch_add(1, std::memory_order_relaxed);
                return;
            }
        }
        // 慢速路径：首次出现，插入新条目
        {
            std::unique_lock<std::shared_mutex> wl(map_mtx_);
            // double-check
            auto it = counters_.find(name);
            if (it == counters_.end()) {
                counters_.emplace(name, 1ULL);
            } else {
                it->second.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }

    // ── 获取当前快照 ──────────────────────────────────────────────────────────
    struct Snapshot {
        std::string tool;
        uint64_t    total; // 历史累计（磁盘 base + 内存未写盘的增量）
    };

    std::vector<Snapshot> get_snapshot() const {
        std::shared_lock<std::shared_mutex> rl(map_mtx_);
        std::lock_guard<std::mutex>         bl(base_mtx_);
        std::vector<Snapshot> result;
        result.reserve(counters_.size());
        for (const auto& [name, atom] : counters_) {
            uint64_t mem  = atom.load(std::memory_order_relaxed);
            uint64_t base = 0;
            auto     it   = base_map_.find(name);
            if (it != base_map_.end()) base = it->second;
            result.push_back({name, base + mem});
        }
        return result;
    }

    // ── 立即写盘（可由外部触发，如信号处理）──────────────────────────────────
    void flush_to_disk() {
        std::shared_lock<std::shared_mutex> rl(map_mtx_);
        std::lock_guard<std::mutex>         bl(base_mtx_);
        flush_locked();
    }

private:
    std::filesystem::path datas_dir_;
    std::filesystem::path stats_file_;

    // 工具名 -> 自上次写盘以来的增量（atomic，无锁热路径）
    mutable std::shared_mutex                               map_mtx_;
    std::unordered_map<std::string, std::atomic<uint64_t>> counters_;

    // 磁盘持久化的基准值
    mutable std::mutex                          base_mtx_;
    std::unordered_map<std::string, uint64_t>  base_map_;

    // flush 线程
    std::thread             flush_thread_;
    std::mutex              cv_mtx_;
    std::condition_variable cv_;
    bool                    stop_ = false;

    // ── 从磁盘加载历史数据 ────────────────────────────────────────────────────
    void load_from_disk() {
        std::lock_guard<std::mutex> bl(base_mtx_);
        if (!std::filesystem::is_regular_file(stats_file_)) return;
        try {
            std::ifstream ifs(stats_file_);
            if (!ifs.is_open()) return;
            json j;
            ifs >> j;
            if (!j.is_object()) return;
            for (auto& [k, v] : j.items()) {
                if (v.is_number_unsigned())
                    base_map_[k] = v.get<uint64_t>();
                else if (v.is_number_integer())
                    base_map_[k] = static_cast<uint64_t>(std::max(int64_t(0), v.get<int64_t>()));
            }
        } catch (...) {}
    }

    // ── 写盘（调用者需持有 map_mtx_（shared）和 base_mtx_）──────────────────
    void flush_locked() {
        try {
            json j = json::object();
            for (auto& [name, atom] : counters_) {
                uint64_t delta    = atom.exchange(0, std::memory_order_relaxed);
                base_map_[name]  += delta;
                j[name]           = base_map_[name];
            }
            // 写到 .tmp 再 rename，防止写一半崩溃损坏文件
            auto tmp = stats_file_;
            tmp += ".tmp";
            {
                std::ofstream ofs(tmp, std::ios::trunc);
                if (!ofs.is_open()) return;
                ofs << j.dump(2) << '\n';
            }
            std::filesystem::rename(tmp, stats_file_);
        } catch (...) {}
    }

    // ── 后台 flush 线程 ───────────────────────────────────────────────────────
    void flush_loop() {
        while (true) {
            {
                std::unique_lock<std::mutex> lk(cv_mtx_);
                cv_.wait_for(lk, std::chrono::seconds(FLUSH_INTERVAL_SEC),
                             [this] { return stop_; });
            }
            {
                std::shared_lock<std::shared_mutex> rl(map_mtx_);
                std::lock_guard<std::mutex>         bl(base_mtx_);
                flush_locked();
            }
            {
                std::lock_guard<std::mutex> lk(cv_mtx_);
                if (stop_) break;
            }
        }
    }
};

} // namespace server
} // namespace mcdk

#endif // MCDK_SERVER
