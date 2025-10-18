#pragma once

#include "lock_file_manager.h"
#include "mpmc_queue.h"
#include "external_message.h"

#include <memory>
#include <thread>

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
    pMpmcQueue_{nullptr},bIsCreateSharedMemory_{true},workerType_{WorkerType::Consumer} {
    }

    ~QueueManager() {
        Release();
    }

    bool Init(StrategyMessageManager* pStrategyMessageManager, WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory = true);

    bool Init(UteMessageManager* pUteMessageManager, WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory = true);


    bool AttachShareMemory(const char* cstrSharedMemName);

    bool CreateShareMemory(const char* cstrSharedMemName);

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
    WorkerType                   workerType_;            // 锁文件句柄;

    StrategyMessageManager       *pStrategyMessageManager_;      // 策略进程对应的策略消息管理器;
    UteMessageManager             *pUteMessageManager_;            // UTE进程对应的策略消息管理器;

    std::shared_ptr<std::thread>    shptrConsumerThread_;           // 策略进程对应的锁文件;
};