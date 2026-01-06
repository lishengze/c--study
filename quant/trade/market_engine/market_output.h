#pragma once

#include "share_comm_external_message.h"
#include "share_comm_mpmc_queue.h"

#include "logger.h"

using namespace share_common;

#include <string>

class MarketOutput {
public:
    MarketOutput():iQueueSize_{4096},
                  strSharedMemName_ {"share_market_data_queue"},     
                    ptr_share_market_data_queue_{nullptr},
                    ptr_share_market_data_queue_slot_{nullptr}
    {
        
    }

    bool Init();

    bool Start();

    bool Stop();

    bool InitShareMarketDataQueue();

    void OutputMarketData(const KlineAtom& market_data);

    void SendKlineAtom(const KlineAtomSharedPtr& market_data);

    void OutputVecKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc);

    
    virtual ~MarketOutput() {}

private:

    unsigned int iQueueSize_;  // 队列大小
    std::string strSharedMemName_;  // 共享内存名称

    mpmc_queue<KlineAtom>* ptr_share_market_data_queue_;  // 处理后的市场数据队列 与 交易进程进行交互
    element_slot<KlineAtom, false>*  ptr_share_market_data_queue_slot_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；
};