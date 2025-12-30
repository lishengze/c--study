#pragma once

#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include "logger.h"

class MarketReceiver {
public:
    MarketReceiver():ptr_src_market_data_queue_(nullptr) {}

    bool Init();

    void SetSrcMarketDataQueue(mpmc_queue<MarketData>* ptr_src_market_data_queue) {
        ptr_src_market_data_queue_ = ptr_src_market_data_queue;
    }

    bool Start();

    bool Stop();

    virtual ~MarketReceiver() {}

private:
    
    mpmc_queue<MarketData>* ptr_src_market_data_queue_;  // 源市场行情数据队列

};