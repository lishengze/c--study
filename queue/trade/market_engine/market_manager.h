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

    bool InitShareMarketDataQueue() ;

    bool Start();



    ~MarketManager() {
        
    }

private:
    DataCompute data_compute_;
    MarketOutput market_output_;
    MarketReceiver market_receiver_;

    shared_ptr<mpmc_queue<MarketData>> ptr_src_market_data_queue_;  // 源市场行情数据队列
    shared_ptr<mpmc_queue<MarketData>> ptr_share_market_data_queue_;  // 处理后的市场数据队列
    element_slot<MarketData, false>*  ptr_share_market_data_queue_slot_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；

    int iQueueSize_;  // 队列大小
    string strSharedMemName_;  // 共享内存名称

    std::shared_ptr<std::thread>  shptrGetSrcMarketDataThread_;           // 消费者线程;
};