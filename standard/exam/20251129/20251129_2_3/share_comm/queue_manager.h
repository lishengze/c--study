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
    QueueManager():pWriteMsgMpmcQueue_{nullptr},pWriteMsgMpmcShareSlots_{nullptr},
        // pReqOrderMpmcQueue_{nullptr}, pReqOrderMpmcShareSlots_{nullptr},
        uiMemorySize_{1024*1024*10}, uiQueueBlockCount_{10000},
        strSharedMemName_{""}, bIsCreateSharedMemory_{false},bIsAttachSharedMemory_{false},bIsInShareMemory_{false},
        workerType_{WorkerType::Consumer},
        StraegyOnMessageFunc_{nullptr},
        shptrConsumerThread_{nullptr}, bIsReqOrderQueue_{false}, ulFileKey_{0}, 
        bIsRunning_{true}, bIsReleased_{false}

     {
    }

    ~QueueManager() {
        

        Release();
    }

    /// @brief 策略端初始化队列用的Init接口;
    /// @param StraegyOnMessageFunc 
    /// @param workerType 
    /// @param cstrSharedMemName 
    /// @param bIsCreateSharedMemory 
    /// @param bIsReqOrderQueue 
    /// @return 
    bool Init(ReadGetRspCallbackMessageFuncType StraegyOnMessageFunc,
                WorkerType workerType, 
                const char* cstrSharedMemName,  bool bIsCreateSharedMemory, 
                bool bIsReqOrderQueue);

    /// @brief ute进程相关队列会使用到的初始化函数;
    /// @param pWriteMessageManager 
    /// @param cstrSharedMemName 
    /// @param bIsCreateSharedMemory 
    /// @return ute 有两个消费者队列，需要统一的进行调度监听，所以不直接在 QueueManager 进行事件监听;
    bool Init(WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory, 
        unsigned long long ulFileKey, bool bIsReqOrderQueue);


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
    bool TryPop(WriteMsg& msg);

    /// @brief  外部监听消息队列的接口;
    /// @param msg 
    /// @return 
    bool TryPopShare(WriteMsg& msg);    

    void SendReqOrder(const char* pBuffer);

    // bool TryPopReqOrder(TradeOrderReq& msg);    

    /// @brief 发送消息到无锁队列中;
    /// @param iMsgID : 消息ID;
    /// @param pMsgBuf : 消息内容;
    /// @param iMsgLen : 消息长度;
    /// @param ulReadKey : 策略进程的唯一标识 - 策略端写入请求时需要填入， UTE端写回报不需要写；
    void SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulReadKey =0);

    /// @brief 发送消息到无锁队列中;
    /// @param iMsgID : 消息ID;
    /// @param pMsgBuf : 消息内容;
    /// @param iMsgLen : 消息长度;
    /// @param ulReadKey : 策略进程的唯一标识 - 策略端写入请求时需要填入， UTE端写回报不需要写；
    void SendMsgShare(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulReadKey =0);    

    /// @brief  转发 API和交易所回报消息到队列中
    /// @param iMsgID 
    /// @param pMsgBuf 
    /// @param iMsgLen 
    /// @param iMsgSrcType 
    /// @param pMsgHandler 
    void SendMsg(int iMsgID, const char* pMsgBuf, unsigned int  iMsgLen, int iMsgSrcType, void* pMsgHandler) ;

    void StartConsumerThread();

    unsigned long long GetReadKey() {return ulFileKey_;}

    /// @brief 解除内存映射;
    void Release();

    void Stop() { bIsRunning_ = false; }

private:
    mpmc_queue<WriteMsg>*    pWriteMsgMpmcQueue_;         // 无锁队列;
    element_slot<WriteMsg, false>*  pWriteMsgMpmcShareSlots_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；

    // mpmc_queue<TradeOrderReq>*    pReqOrderMpmcQueue_;         // 无锁队列;
    // element_slot<TradeOrderReq, false>*  pReqOrderMpmcShareSlots_;  // 无锁队列存储真正数据元素的起始地址 -- 用于共享内存映射无锁队列时使用；


    unsigned int                 uiMemorySize_;       // 无锁队列占用的共享内存大小;
    unsigned int                 uiQueueBlockCount_;  // 无锁队列的块数;
    std::string                  strSharedMemName_;   // 共享内存名称;

    bool                         bIsCreateSharedMemory_; // 是否创建共享内存;
    bool                         bIsAttachSharedMemory_; // 是否映射共享内存到无锁队列中;
    bool                         bIsInShareMemory_;      // 此次队列是否在共享内存中;
    WorkerType                   workerType_;            // queue 的工作类型;

    ReadGetRspCallbackMessageFuncType StraegyOnMessageFunc_;

    std::shared_ptr<std::thread>  shptrConsumerThread_;           // 消费者线程;

    bool bIsReqOrderQueue_; // 是否是只存储委托请求的队列;

    unsigned long long           ulFileKey_;                    // 策略进程对应的锁文件名称;

    bool                         bIsRunning_;                   // 退出事件循环的判断逻辑;
    bool                         bIsReleased_;                  // 是否已经释放过资源;
};


inline std::string GetQueueName(unsigned long long ulFileKey) {
    return std::string("Write") + std::to_string(ulFileKey) + ".queue";
}


inline std::string GetNonReqOrderQueueName(const char* cstrFileKey) {
    return std::string("Read_") + std::string(cstrFileKey) + ".queue";
}



} // namespace share_common 