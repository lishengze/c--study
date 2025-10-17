#pragma once

#include "mpmc_queue.h"
#include "external_message.h"


/*
主要负责三个功能：
1。 创建请求所用无所队列
2。 将UTE创建的共享内存映射到无所队列中;
3。 创建当前策略进程对应的 锁文件，并监听UTE进程的锁文件;
*/
class ReqQueueManager {    
public:
    ReqQueueManager():uiQueueBlockCount_{10000},uiMemorySize_{1024*1024*10}, pReqQueue_{nullptr} {
    }

    ~ReqQueueManager() {
        Release();
    }


    bool Init(const char* cstrSharedMemName);

    void SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulStrategyKey);

    

    /// @brief 解除内存映射;
    void Release();


private:
    tech::mpmc_queue<UteMsg>*    pReqQueue_;         // 策略进程发送请求的无锁队列;
    unsigned int                 uiMemorySize_;      // 无锁队列占用的共享内存大小;
    unsigned int                 uiQueueBlockCount_; // 无锁队列的块数;
};