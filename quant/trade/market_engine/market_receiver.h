#pragma once
#include <thread>
#include <memory>

#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include "logger.h"

using namespace share_common;

class MarketReceiver {
public:
    MarketReceiver():ptr_src_market_data_queue_(nullptr),ptr_thread_(nullptr) {}

    bool Init();

    void SetSrcMarketDataQueue(mpmc_queue<MarketData>* ptr_src_market_data_queue) {
        ptr_src_market_data_queue_ = ptr_src_market_data_queue;
    }

    void SendMarketDataToQueue(const MarketData& market_data);

    mpmc_queue<MarketData>* GetSrcMarketDataQueue() {
        return ptr_src_market_data_queue_;
    }

    bool InitSrcMarketDataQueue();

    bool Start();

    bool Stop();

    virtual ~MarketReceiver() {
        if (ptr_thread_ && ptr_thread_->joinable()) {
            ptr_thread_->join();
        }
    }

private:
    
    mpmc_queue<MarketData>* ptr_src_market_data_queue_;  // 源市场行情数据队列

    std::shared_ptr<std::thread> ptr_thread_;  // 接收线程
};