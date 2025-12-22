#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>

// libgo协程库头文件
#include <libgo/coroutine.h>
#include <libgo/timer.h>
#include <libgo/sync/co_mutex.h>

// 命名空间简化
using namespace std;
using namespace co;

// ===================== 1. 定义行情数据结构 =====================
struct MarketData {
    string stock_code;  // 证券代码
    double price;       // 最新价格
    uint64_t timestamp; // 时间戳（微秒）
    uint64_t volume;    // 成交量

    // 打印行情数据
    friend ostream& operator<<(ostream& os, const MarketData& data) {
        os << "[" << data.timestamp << "] " << data.stock_code
           << " | 价格：" << data.price << " | 成交量：" << data.volume;
        return os;
    }
};

// ===================== 2. 订阅管理器（核心模块） =====================
class SubscriptionManager {
public:
    // 回调函数类型：接收行情数据（libgo协程函数可直接作为回调）
    using CallbackFunc = function<void(const MarketData&)>;
    // 订阅ID（用于取消订阅）
    using SubscriptionId = size_t;

    SubscriptionManager() : next_sub_id_(1) {}

    /**
     * @brief 订阅行情
     * @param stock_code 证券代码（空字符串表示订阅全部）
     * @param callback 回调函数（可是libgo协程函数）
     * @return 订阅ID（用于取消订阅）
     */
    SubscriptionId subscribe(const string& stock_code, CallbackFunc callback) {
        co_mutex::scoped_lock lock(mtx_); // libgo的协程锁（比std::mutex更高效，不阻塞线程）
        SubscriptionId sub_id = next_sub_id_++;
        // 存储订阅关系：证券代码 → （订阅ID，回调函数）
        subs_[stock_code].emplace_back(sub_id, move(callback));
        // 全局订阅（空字符串）：接收所有行情
        if (!stock_code.empty()) {
            subs_[""].emplace_back(sub_id, [stock_code, callback](const MarketData& data) {
                if (data.stock_code == stock_code) {
                    callback(data);
                }
            });
        }
        return sub_id;
    }

    /**
     * @brief 取消订阅
     * @param sub_id 订阅ID
     */
    void unsubscribe(SubscriptionId sub_id) {
        co_mutex::scoped_lock lock(mtx_);
        // 遍历所有证券代码的订阅列表，移除对应订阅ID
        for (auto& [code, sub_list] : subs_) {
            auto it = find_if(sub_list.begin(), sub_list.end(),
                [sub_id](const auto& pair) { return pair.first == sub_id; });
            if (it != sub_list.end()) {
                sub_list.erase(it);
                break; // 订阅ID唯一，找到后退出
            }
        }
    }

    /**
     * @brief 推送行情数据，触发回调（在独立协程中执行回调）
     * @param data 行情数据
     */
    void push_market_data(const MarketData& data) {
        co_mutex::scoped_lock lock(mtx_);
        // 1. 触发该证券代码的订阅回调
        if (subs_.count(data.stock_code)) {
            for (auto& [sub_id, callback] : subs_[data.stock_code]) {
                // libgo：启动新协程执行回调（无阻塞）
                go [callback, data]() {
                    callback(data);
                };
            }
        }
        // 2. 触发全局订阅的回调（空字符串）
        if (subs_.count("")) {
            for (auto& [sub_id, callback] : subs_[""]) {
                go [callback, data]() {
                    callback(data);
                };
            }
        }
    }

private:
    co_mutex mtx_; // libgo的协程互斥锁（适配协程的锁，避免线程阻塞）
    atomic<SubscriptionId> next_sub_id_; // 原子自增的订阅ID（线程安全）
    // 订阅关系：证券代码 → （订阅ID，回调函数）
    unordered_map<string, vector<pair<SubscriptionId, CallbackFunc>>> subs_;
};

// ===================== 3. 行情生产者协程（模拟行情数据生成） =====================
/**
 * @brief 模拟行情数据生成（运行在libgo协程中）
 * @param sub_mgr 订阅管理器
 * @param stock_codes 要生成的证券代码列表
 * @param interval_ms 行情推送间隔（毫秒）
 */
void market_data_producer(SubscriptionManager& sub_mgr,
                          const vector<string>& stock_codes,
                          int interval_ms = 100) {
    // 随机数生成器：模拟价格波动
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> price_dist(0.0, 10.0);
    uniform_int_distribution<uint64_t> vol_dist(100, 10000);

    // 存储每个股票的初始价格
    unordered_map<string, double> init_prices;
    for (const auto& code : stock_codes) {
        init_prices[code] = 100.0 + price_dist(gen);
    }

    uint64_t timestamp = 0;
    while (true) {
        // libgo定时器：等待interval_ms毫秒（协程挂起，不阻塞线程）
        co_sleep(chrono::milliseconds(interval_ms));

        // 生成时间戳（微秒）
        timestamp += interval_ms * 1000;

        // 为每个股票生成行情数据
        for (const auto& code : stock_codes) {
            MarketData data;
            data.stock_code = code;
            data.timestamp = timestamp;
            // 模拟价格小幅波动
            data.price = init_prices[code] + price_dist(gen) - 5.0;
            data.volume = vol_dist(gen);

            // 推送行情数据到订阅管理器
            sub_mgr.push_market_data(data);
        }
    }
}

// ===================== 4. 示例回调函数（用户自定义，支持协程逻辑） =====================
/**
 * @brief 回调函数1：打印沪深300股票的行情（可包含协程逻辑）
 */
void print_hs300_data(const MarketData& data) {
    cout << "[回调1] " << data << endl;
    // 模拟回调处理耗时（比如写入内存数据库，使用co_sleep挂起协程，不阻塞线程）
    co_sleep(chrono::milliseconds(10)); // 模拟10ms耗时
}

/**
 * @brief 回调函数2：监控股价超过阈值的股票（简单逻辑）
 */
void monitor_high_price(const MarketData& data) {
    if (data.price > 105.0) {
        cout << "[回调2] 预警：" << data.stock_code << " 价格超过105，当前价格：" << data.price << endl;
    }
}

// ===================== 主函数：启动整个系统 =====================
int test_libgo() {
    // 1. 配置libgo调度器：绑定4个线程（多核并发，默认是单线程）
    co_sched.GetOptions().worker_threads = 4;
    // 启动libgo的事件循环（后台运行）
    co_sched.Start();

    // 2. 创建订阅管理器
    SubscriptionManager sub_mgr;

    // 3. 订阅行情
    // 订阅"000300"（沪深300）的行情，回调函数为print_hs300_data
    auto sub_id1 = sub_mgr.subscribe("000300", print_hs300_data);
    // 订阅全部股票的行情，回调函数为monitor_high_price
    auto sub_id2 = sub_mgr.subscribe("", monitor_high_price);

    // 4. 启动行情生产者协程（libgo的go关键字创建协程）
    vector<string> stock_codes = {"000300", "600519", "000001"};
    go [&sub_mgr, &stock_codes]() {
        market_data_producer(sub_mgr, stock_codes, 100); // 每100ms推送一次行情
    };

    // 5. 运行5秒后，取消订阅sub_id1
    this_thread::sleep_for(chrono::seconds(5));
    cout << "\n===== 取消订阅sub_id1 =====" << endl;
    sub_mgr.unsubscribe(sub_id1);

    // 6. 再运行5秒后，停止libgo调度器并退出
    this_thread::sleep_for(chrono::seconds(5));
    co_sched.Stop(); // 停止协程调度器
    co_sched.Join(); // 等待所有协程执行完毕

    return 0;
}