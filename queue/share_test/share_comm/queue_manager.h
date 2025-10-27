#pragma once

#include "lock_file_manager.h"
#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include <memory>
#include <thread>
#include <string>
#include "logger.h"

using namespace tech;

namespace share_common 
{


class StrategyMessageManager;
class UteMessageManager;

enum WorkerType {
    Consumer=0,
    Producer=1,
    ConsumerAndProducer=2
};

/*
主要负责三个功能：
1。 创建请求所用无所队列
2。 将UTE创建的共享内存映射到无所队列中;
3。 创建当前策略进程对应的 锁文件，并监听UTE进程的锁文件;
*/
class QueueManager {
public:
    QueueManager():pMpmcQueue_{nullptr},pMpmcShareSlots_{nullptr},
        uiMemorySize_{1024*1024*10}, uiQueueBlockCount_{10000},
        strSharedMemName_{""}, bIsCreateSharedMemory_{false},bIsAttachSharedMemory_{false},bIsInShareMemory_{false},
        workerType_{WorkerType::Consumer},
        pStrategyMessageManager_{nullptr}, pUteMessageManager_{nullptr},
        shptrConsumerThread_{nullptr}, ulFileKey_{0}

     {
    }

    ~QueueManager() {
        if (shptrConsumerThread_ && shptrConsumerThread_->joinable()) {
            shptrConsumerThread_->join();
        }

        Release();
    }

    bool Init(StrategyMessageManager* pStrategyMessageManager, WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory = true);

    /// @brief ute进程相关队列会使用到的初始化函数;
    /// @param pUteMessageManager 
    /// @param cstrSharedMemName 
    /// @param bIsCreateSharedMemory 
    /// @return ute 有两个消费者队列，需要统一的进行调度监听，所以不直接在 QueueManager 进行事件监听;
    bool Init(WorkerType workerType, const char* cstrSharedMemName="",  bool bIsCreateSharedMemory = true, unsigned long long ulFileKey = 0);


    /// @brief 直接初始化无锁队列，不映射到共享内存;
    /// @return 
    bool InitQueueWithoutSharedMemory();

    /// @brief 映射共享内存到无锁队列中;
    /// @param cstrSharedMemName : 共享内存名称;
    /// @return 映射是否成功;
    bool AttachShareMemory(const char* cstrSharedMemName);

    /// @brief 创建共享内存,并将其映射到无锁队列中;
    /// @param cstrSharedMemName : 共享内存名称;
    /// @return 创建是否成功;
    bool CreateShareMemory(const char* cstrSharedMemName);

    /// @brief  外部监听消息队列的接口;
    /// @param msg 
    /// @return 
    bool TryPop(UteMsg& msg);

    /// @brief 发送消息到无锁队列中;
    /// @param iMsgID : 消息ID;
    /// @param pMsgBuf : 消息内容;
    /// @param iMsgLen : 消息长度;
    /// @param ulStrategyKey : 策略进程的唯一标识 - 策略端写入请求时需要填入， UTE端写回报不需要写；
    void SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulStrategyKey =0);

    /// @brief  转发 API和交易所回报消息到队列中
    /// @param iMsgID 
    /// @param pMsgBuf 
    /// @param iMsgLen 
    /// @param iMsgSrcType 
    /// @param pMsgHandler 
    void SendMsg(int iMsgID, const char* pMsgBuf, unsigned int  iMsgLen, int iMsgSrcType, void* pMsgHandler) ;

    void StartConsumerThread();

    unsigned long long GetStrategyKey() {return ulFileKey_;}

    /// @brief 解除内存映射;
    void Release();

    void TestCharValue() {
        // char* testChar = (char*)(pMpmcQueue_) + sizeof(mpmc_queue<UteMsg>) + 16;
        // LOG_DEBUG("TestCharFromAttachValue: {}", *testChar);

        LOG_DEBUG("iTestIndex_ From Attach : {}", pMpmcQueue_->iTestIndex_);

        LOG_DEBUG("From Attach slot[10].iSlotInnerTestIndex_: {} ", pMpmcShareSlots_[10].iSlotInnerTestIndex_);
    }

private:
    mpmc_queue<UteMsg>*    pMpmcQueue_;         // 无锁队列;
    element_slot<UteMsg, false>*  pMpmcShareSlots_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；

    unsigned int                 uiMemorySize_;       // 无锁队列占用的共享内存大小;
    unsigned int                 uiQueueBlockCount_;  // 无锁队列的块数;
    std::string                  strSharedMemName_;   // 共享内存名称;

    bool                         bIsCreateSharedMemory_; // 是否创建共享内存;
    bool                         bIsAttachSharedMemory_; // 是否映射共享内存到无锁队列中;
    bool                         bIsInShareMemory_;      // 此次队列是否在共享内存中;
    WorkerType                   workerType_;            // queue 的工作类型;

    StrategyMessageManager       *pStrategyMessageManager_;      // 策略进程对应的策略消息管理器;
    UteMessageManager            *pUteMessageManager_;           // UTE进程对应的策略消息管理器;

    std::shared_ptr<std::thread>  shptrConsumerThread_;           // 消费者线程;

    unsigned long long           ulFileKey_;                    // 策略进程对应的锁文件名称;
};

/// @brief  UTE发送给策略的回报共享内存;
/// @param ulFileKey 
/// @return 
inline std::string GetQueueName(unsigned long long ulFileKey) {
    return std::string("UTE_Startegy_") + std::to_string(ulFileKey) + ".queue";
}

/// @brief 策略进程发送请求给UTE的共享内存;
/// @param cstrFileKey 
/// @return 
inline std::string GetQueueName(const char* cstrFileKey) {
    return std::string("Strategy_UTE_") + std::string(cstrFileKey) + ".queue";
}

} // namespace share_common 