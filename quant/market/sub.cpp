#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <queue>
#include <random>
#include <chrono>

// Boost.Asio 协程支持（需要C++20）
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

namespace asio = boost::asio;
using namespace std::chrono_literals;

// ===================== 1. 定义行情数据结构 =====================
struct MarketData {
    std::string stock_code;  // 证券代码
    double price;            // 最新价格
    uint64_t timestamp;      // 时间戳（微秒）
    uint64_t volume;         // 成交量

    // 打印行情数据
    friend std::ostream& operator<<(std::ostream& os, const MarketData& data) {
        os << "[" << data.timestamp << "] " << data.stock_code 
           << " | 价格：" << data.price << " | 成交量：" << data.volume;
        return os;
    }
};

// ===================== 2. 订阅管理器（核心模块） =====================
class SubscriptionManager {
public:
    // 回调函数类型：接收行情数据，返回协程（C++20 协程函数）
    using CallbackFunc = std::function<asio::awaitable<void>(const MarketData&)>;
    // 订阅ID（用于取消订阅）
    using SubscriptionId = size_t;

    // 构造函数：传入io_context，用于调度回调协程
    explicit SubscriptionManager(asio::io_context& io_ctx) : io_ctx_(io_ctx), next_sub_id_(1) {}

    /**
     * @brief 订阅行情
     * @param stock_code 证券代码（空字符串表示订阅全部）
     * @param callback 回调函数（协程）
     * @return 订阅ID（用于取消订阅）
     */
    SubscriptionId subscribe(const std::string& stock_code, CallbackFunc callback) {
        std::lock_guard<std::mutex> lock(mtx_);
        SubscriptionId sub_id = next_sub_id_++;
        // 存储订阅关系：证券代码 → （订阅ID，回调函数）
        subs_[stock_code].emplace_back(sub_id, std::move(callback));
        // 全局订阅（空字符串）单独存储，接收所有行情
        if (!stock_code.empty()) {
            subs_[""].emplace_back(sub_id, [stock_code, callback](const MarketData& data) -> asio::awaitable<void> {
                if (data.stock_code == stock_code) {
                    co_await callback(data);
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
        std::lock_guard<std::mutex> lock(mtx_);
        // 遍历所有证券代码的订阅列表，移除对应订阅ID
        for (auto& [code, sub_list] : subs_) {
            auto it = std::find_if(sub_list.begin(), sub_list.end(),
                [sub_id](const auto& pair) { return pair.first == sub_id; });
            if (it != sub_list.end()) {
                sub_list.erase(it);
                break; // 订阅ID唯一，找到后直接退出
            }
        }
    }

    /**
     * @brief 推送行情数据，触发回调
     * @param data 行情数据
     */
    void push_market_data(const MarketData& data) {
        std::lock_guard<std::mutex> lock(mtx_);
        // 1. 触发该证券代码的订阅回调
        if (subs_.contains(data.stock_code)) {
            for (auto& [sub_id, callback] : subs_[data.stock_code]) {
                // 启动协程执行回调，detached表示无需等待协程完成
                asio::co_spawn(io_ctx_, callback(data), asio::detached);
            }
        }
        // 2. 触发全局订阅的回调（空字符串）
        if (subs_.contains("")) {
            for (auto& [sub_id, callback] : subs_[""]) {
                asio::co_spawn(io_ctx_, callback(data), asio::detached);
            }
        }
    }

private:
    asio::io_context& io_ctx_;                // 协程调度的io_context
    std::mutex mtx_;                          // 保护订阅关系的互斥锁
    SubscriptionId next_sub_id_;              // 自增的订阅ID
    // 订阅关系：证券代码 → （订阅ID，回调函数）
    std::unordered_map<std::string, std::vector<std::pair<SubscriptionId, CallbackFunc>>> subs_;
};

// ===================== 3. 行情生产者协程（模拟行情数据生成） =====================
/**
 * @brief 模拟行情数据生成（生产者协程）
 * @param io_ctx 事件循环
 * @param sub_mgr 订阅管理器
 * @param stock_codes 要生成的证券代码列表
 */
asio::awaitable<void> market_data_producer(
    asio::io_context& io_ctx,
    SubscriptionManager& sub_mgr,
    const std::vector<std::string>& stock_codes) {

    // 随机数生成器：模拟价格波动
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> price_dist(0.0, 10.0);
    std::uniform_int_distribution<uint64_t> vol_dist(100, 10000);

    // 存储每个股票的初始价格
    std::unordered_map<std::string, double> init_prices;
    for (const auto& code : stock_codes) {
        init_prices[code] = 100.0 + price_dist(gen);
    }

    // 定时器：每隔100ms生成一次行情
    asio::steady_timer timer(io_ctx);
    uint64_t timestamp = 0;

    while (true) {
        // 重置定时器（100ms后触发）
        timer.expires_after(100ms);
        co_await timer.async_wait(asio::use_awaitable);

        // 生成时间戳（微秒）
        timestamp += 100000; // 100ms = 100000微秒

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

// ===================== 4. 示例回调函数（用户自定义） =====================
/**
 * @brief 回调函数1：打印沪深300股票的行情
 */
asio::awaitable<void> print_hs300_data(const MarketData& data) {
    std::cout << "[回调1] " << data << std::endl;
    // 模拟回调处理耗时（比如写入内存数据库）
    asio::steady_timer timer(co_await asio::this_coro::executor);
    timer.expires_after(10ms); // 模拟10ms耗时
    co_await timer.async_wait(asio::use_awaitable);
}

/**
 * @brief 回调函数2：监控股价超过阈值的股票
 */
asio::awaitable<void> monitor_high_price(const MarketData& data) {
    if (data.price > 105.0) {
        std::cout << "[回调2] 预警：" << data.stock_code << " 价格超过105，当前价格：" << data.price << std::endl;
    }
    co_return;
}

// ===================== 主函数：启动整个系统 =====================
int test() {
    // 1. 创建io_context（事件循环），绑定4个线程（多核调度）
    asio::io_context io_ctx(4);
    auto work_guard = asio::make_work_guard(io_ctx); // 防止io_context退出

    // 2. 启动线程池，运行事件循环
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&io_ctx]() {
            io_ctx.run();
        });
    }

    // 3. 创建订阅管理器
    SubscriptionManager sub_mgr(io_ctx);

    // 4. 订阅行情
    // 订阅"000300"（沪深300）的行情，回调函数为print_hs300_data
    auto sub_id1 = sub_mgr.subscribe("000300", print_hs300_data);
    // 订阅全部股票的行情，回调函数为monitor_high_price
    auto sub_id2 = sub_mgr.subscribe("", monitor_high_price);

    // 5. 启动行情生产者协程
    std::vector<std::string> stock_codes = {"000300", "600519", "000001"};
    asio::co_spawn(io_ctx, market_data_producer(io_ctx, sub_mgr, stock_codes), asio::detached);

    // 6. 运行5秒后，取消订阅sub_id1
    std::this_thread::sleep_for(5s);
    std::cout << "\n===== 取消订阅sub_id1 =====" << std::endl;
    sub_mgr.unsubscribe(sub_id1);

    // 7. 再运行5秒后，退出程序
    std::this_thread::sleep_for(5s);
    work_guard.reset(); // 允许io_context退出

    // 8. 等待线程池结束
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}