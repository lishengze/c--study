#include <boost/asio.hpp>
#include <coroutine>
#include <string>
#include <vector>
#include <unordered_map>
#include "thread_safe_queue.h" // 自定义线程安全队列

namespace asio = boost::asio;
using asio::ip::tcp;

// 行情数据结构
struct MarketData {
    std::string exchange; // 交易所（SH/SZ）
    std::string stock_code; // 证券代码
    double price; // 最新价
    double volume; // 成交量
    uint64_t timestamp; // 时间戳（纳秒）
};

// 全局线程安全队列：行情数据缓冲（协程→线程池）
ThreadSafeQueue<MarketData> g_data_queue;

// C++20协程的异步任务返回类型（基于asio）
template <typename T>
struct AsyncTask {
    struct promise_type {
        T value;
        std::exception_ptr ex;

        AsyncTask get_return_object() {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_value(T v) { value = std::move(v); }
        void unhandled_exception() { ex = std::current_exception(); }
    };

    std::coroutine_handle<promise_type> h;
    ~AsyncTask() { if (h) h.destroy(); }

    // 协程等待接口（简化版，实际需结合asio的异步等待）
    T get() {
        if (h.promise().ex) std::rethrow_exception(h.promise().ex);
        return std::move(h.promise().value);
    }
};

// 协程：订阅单个证券的行情数据（异步接收推送）
AsyncTask<void> subscribe_stock_data(
    asio::io_context& io_context,
    const std::string& exchange,
    const std::string& stock_code,
    const std::string& server_ip,
    uint16_t port
) {
    try {
        tcp::socket socket(io_context);
        // 异步连接行情服务器（协程挂起，直到连接完成）
        co_await asio::async_connect(socket, tcp::resolver(io_context).resolve(server_ip, std::to_string(port)),
            [](std::coroutine_handle<> h) { h.resume(); }); // 简化的协程回调

        std::cout << "订阅成功：" << exchange << "-" << stock_code << std::endl;

        char buffer[1024];
        while (true) {
            // 异步读取行情数据（协程挂起，直到数据到达）
            size_t len = co_await asio::async_read(socket, asio::buffer(buffer),
                [](std::coroutine_handle<> h) { h.resume(); });

            // 解包数据（简化版，实际需按交易所协议解包）
            MarketData data;
            data.exchange = exchange;
            data.stock_code = stock_code;
            data.price = std::atof(buffer); // 示例：实际需解析二进制协议
            data.volume = std::atof(buffer + 32);
            data.timestamp = std::stoull(buffer + 64);

            // 将数据放入缓冲队列，交给计算线程池处理
            g_data_queue.push(std::move(data));
        }
    } catch (const std::exception& e) {
        std::cerr << "订阅失败：" << e.what() << std::endl;
    }
    co_return;
}

// 初始化行情订阅协程组
void init_subscription(asio::io_context& io_context, const std::vector<std::pair<std::string, std::string>>& stocks) {
    // 按交易所分组，共享IO线程（减少线程数）
    std::unordered_map<std::string, std::string> exchange_servers = {
        {"SH", "192.168.1.100"}, // 上交所行情服务器
        {"SZ", "192.168.1.101"}  // 深交所行情服务器
    };
    uint16_t port = 8888; // 行情端口

    for (const auto& [exchange, stock_code] : stocks) {
        // 为每个证券创建协程（运行在同一个io_context的线程中）
        auto task = subscribe_stock_data(io_context, exchange, stock_code, exchange_servers[exchange], port);
    }

    // 运行asio的IO线程（单个线程处理所有协程的异步IO）
    std::thread io_thread([&io_context]() { io_context.run(); });
    io_thread.detach();
}