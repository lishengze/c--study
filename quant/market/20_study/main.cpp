#include "task.h"
#include "thread_pool.h"
#include "thread_safe_queue.h"

int main() {
    // 待订阅的证券列表（多交易所、多证券）
    std::vector<std::pair<std::string, std::string>> stocks = {
        {"SH", "600519"}, // 贵州茅台（上交所）
        {"SH", "601318"}, // 中国平安（上交所）
        {"SZ", "000001"}, // 平安银行（深交所）
        {"SZ", "300750"}  // 宁德时代（深交所）
    };

    // 1. 初始化asio的IO上下文（协程运行的核心）
    asio::io_context io_context;

    // 2. 启动行情订阅协程组（处理IO密集任务）
    init_subscription(io_context, stocks);

    // 3. 启动计算任务分发器，初始化线程池（处理计算密集任务）
    start_calc_dispatcher();

    // 主线程保持运行
    std::this_thread::sleep_for(std::chrono::hours(1));

    return 0;
}