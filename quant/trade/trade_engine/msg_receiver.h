#pragma once

#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include "logger.h"

using namespace share_common;

/// @brief 消息接收者
/// @brief 接受行情，交易数据;
/// @brief 行情数据来源可能是 无锁队列，也可能是 网络 socket, kafaka;
/// @brief 交易数据来源是 交易所的 API；
class MsgReceiver {
public:
    MsgReceiver():ptr_share_market_data_queue_(nullptr), ptr_share_market_data_queue_slot_(nullptr) {}

    virtual ~MsgReceiver() {
        if (shptrGetSrcMarketDataThread_ && shptrGetSrcMarketDataThread_->joinable()) {
            shptrGetSrcMarketDataThread_->join();
        }
    }

    bool Init();

    bool InitMarketDataQueue();

    bool Start();

    bool StartReceiveMarketData();

    bool Stop();

    void SetMarketDataCallbackFunc(MarketDataCallbackFuncType market_data_callback_func) {
        market_data_callback_func_ = market_data_callback_func;
    }

private:
    
    unsigned int iQueueSize_;  // 队列大小
    std::string strSharedMemName_;  // 共享内存名称

    mpmc_queue<MarketData>* ptr_share_market_data_queue_;  // 处理后的市场数据队列 与 交易进程进行交互
    element_slot<MarketData, false>*  ptr_share_market_data_queue_slot_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；

    std::shared_ptr<std::thread>  shptrGetSrcMarketDataThread_;           // 消费者线程;
    bool bIsRunning_;  // 运行状态

    MarketDataCallbackFuncType market_data_callback_func_;    
};