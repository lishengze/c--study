#pragma once

#include "data_compute.h"
#include "market_output.h"
#include "market_receiver.h"

#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include "logger.h"

#include <memory>
#include <string>

using namespace std;

using namespace share_common;

class MarketManager {
public:
    MarketManager() ;

    bool Init() ;

    bool InitSrcMarketDataQueue() ;

    bool Start();

    /// @brief 启动监听源市场数据队列
    /// @return 
    bool StartListenSrcMarketData();



    ~MarketManager() {
        if (shptrGetSrcMarketDataThread_->joinable()) {
            shptrGetSrcMarketDataThread_->join();
        }
    }

private:
    DataCompute data_compute_;
    MarketOutput market_output_;
    MarketReceiver market_receiver_;

    mpmc_queue<MarketData>* ptr_src_market_data_queue_;  // 源市场行情数据队列

    int iQueueSize_;  // 队列大小

    std::shared_ptr<std::thread>  shptrGetSrcMarketDataThread_;           // 消费者线程;
    bool bIsRunning_;  // 线程运行标志位
};