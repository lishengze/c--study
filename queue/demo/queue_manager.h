#pragma once

#include "lock_file_manager.h"
#include "mpmc_queue.h"
#include "external_message.h"

#include <memory>
#include <thread>
#include <string>

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
    QueueManager():uiQueueBlockCount_{10000},uiMemorySize_{1024*1024*10}, 
    pMpmcQueue_{nullptr},bIsCreateSharedMemory_{true},bIsAttachSharedMemory_{true},
    workerType_{WorkerType::Consumer} {
    }

    ~QueueManager() {
        Release();
    }

    bool Init(StrategyMessageManager* pStrategyMessageManager, WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory = true);

    /// @brief ute进程相关队列会使用到的初始化函数;
    /// @param pUteMessageManager 
    /// @param cstrSharedMemName 
    /// @param bIsCreateSharedMemory 
    /// @return ute 有两个消费者队列，需要统一的进行调度监听，所以不直接在 QueueManager 进行事件监听;
    bool Init(UteMessageManager* pUteMessageManager, WorkerType workerType, const char* cstrSharedMemName="",  bool bIsCreateSharedMemory = true);


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
    bool trypop(UteMsg& msg);



    void SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulStrategyKey);

    void StartConsumerThread();

    /// @brief 解除内存映射;
    void Release();


private:
    tech::mpmc_queue<UteMsg>*    pMpmcQueue_;         // 策略进程发送请求的无锁队列;

    unsigned int                 uiMemorySize_;       // 无锁队列占用的共享内存大小;
    unsigned int                 uiQueueBlockCount_;  // 无锁队列的块数;
    std::string                  strSharedMemName_;   // 共享内存名称;

    bool                         bIsCreateSharedMemory_; // 是否创建共享内存;
    bool                         bIsAttachSharedMemory_; // 是否映射共享内存到无锁队列中;
    WorkerType                   workerType_;            // 锁文件句柄;

    StrategyMessageManager       *pStrategyMessageManager_;      // 策略进程对应的策略消息管理器;
    UteMessageManager            *pUteMessageManager_;           // UTE进程对应的策略消息管理器;

    std::shared_ptr<std::thread>    shptrConsumerThread_;           // 策略进程对应的锁文件;
};

inline std::string GetQueueName(unsigned long long ulFileKey) {
    return std::to_string(ulFileKey) + ".queue";
}

inline std::string GetQueueName(const char* cstrFileKey) {
    return std::string(cstrFileKey) + ".queue";
}