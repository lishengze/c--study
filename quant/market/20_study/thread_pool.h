#include <vector>
#include <thread>
#include <functional>
#include "thread_safe_queue.h"

// 简单的线程池类
class ThreadPool {
private:
    std::vector<std::thread> threads_;
    ThreadSafeQueue<std::function<void()>> task_queue_;
    bool stop_ = false;

public:
    explicit ThreadPool(size_t thread_count = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < thread_count; ++i) {
            threads_.emplace_back([this]() {
                while (true) {
                    auto task = task_queue_.pop();
                    if (!task) {
                        break;
                    }
                    (*task)();
                }
            });
        }
    }

    ~ThreadPool() {
        task_queue_.stop();
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template <typename F>
    void enqueue(F&& f) {
        task_queue_.push(std::forward<F>(f));
    }
};

// 全局线程池（按CPU核心数初始化）
ThreadPool g_calc_pool;

// 指标计算函数（示例：计算简单移动平均SMA）
void calculate_indicator(const MarketData& data) {
    // 模拟指标计算（实际包含均线、MACD、截面排序等计算）
    static std::unordered_map<std::string, std::vector<double>> price_history; // 价格历史
    price_history[data.stock_code].push_back(data.price);

    // 计算5日SMA
    if (price_history[data.stock_code].size() >= 5) {
        double sma = 0.0;
        for (int i = 0; i < 5; ++i) {
            sma += price_history[data.stock_code][price_history[data.stock_code].size() - 1 - i];
        }
        sma /= 5.0;
        std::cout << "指标结果：" << data.stock_code << " SMA5: " << sma << std::endl;
    }

    // 若为截面运算（如全市场股票的因子排序），可积累数据后批量计算
    // 示例：每100ms触发一次截面排序（需额外的定时器协程）
}

// 启动计算任务分发器：从队列读取数据，提交到线程池
void start_calc_dispatcher() {
    std::thread dispatcher_thread([]() {
        while (true) {
            auto data = g_data_queue.pop();
            if (!data) {
                break;
            }
            // 将计算任务提交到线程池（多线程并行计算）
            g_calc_pool.enqueue([data = std::move(*data)]() {
                calculate_indicator(data);
            });
        }
    });
    dispatcher_thread.detach();
}