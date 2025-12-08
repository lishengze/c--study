#include "write_message_manager.h"
#include <fstream>

namespace share_common {

bool WriteMessageManager::Init (const char* cstrWriteSysName)
{
    LOG_INFO("Init Write message manager, cstrWriteSysName:{}" ,cstrWriteSysName);

    if (cstrWriteSysName == nullptr || strcmp(cstrWriteSysName, "") == 0) {
        LOG_ERROR("Input cstrWriteSysName is Error");
        return false;
    }
    

    if (!m_pfnOnEvent) {
        LOG_ERROR("OnEvent callback function is not set.");
        return false;
    }

    if (!m_pfnOnMessage) {
        LOG_ERROR("OnMessage callback function is not set.");
        return false;
    }


    ///锁文件相关初始化;
    // 初始化UTE进程的锁文件管理器;
    if (!m_LockFileManager.Init(GetLockFileName(cstrWriteSysName).c_str(), m_pfnOnEvent, m_iHeartbeatSec)) {
        LOG_ERROR("Init lock file {} manager failed.", cstrWriteSysName);
        return false;
    }

    // 初始化策略非委托请求相关的无锁队列 以及 对应的 共享内存
    if (!m_TickDataQueue.Init(Consumer, GetNonReqOrderQueueName(cstrWriteSysName).c_str(),  true, 0, false)) {
        LOG_ERROR("Init ReqQueueManager:{}, Failed!", GetNonReqOrderQueueName(cstrWriteSysName).c_str());
        return false;
    } else {
        LOG_INFO("Init ReqQueueManager:{}, SUCCESS!", GetNonReqOrderQueueName(cstrWriteSysName).c_str());
    }

    StartListenQueue(); // todo 启动监听队列线程;
    
    return true;
}


//
QueueManager* WriteMessageManager::CreateReadRspQueue(unsigned long long ulReadKey) {

    if (m_mapRspQueue.find(ulReadKey)!= m_mapRspQueue.end()) {
        LOG_WARN("ulReadKey:{} Has Been Created!", ulReadKey);
        return m_mapRspQueue[ulReadKey];
    }

    QueueManager* pReadRspQueue = new QueueManager();

    if (!pReadRspQueue) {
        LOG_ERROR("Create strategy rsp queue {} failed.", ulReadKey);
        return nullptr;
    }

    // 初始化策略进程接收回报的共享内存队列管理器，这块共享内存由服务端创建好, 此时策略端一直在等待
    if (!pReadRspQueue->Init(Producer, GetQueueName(ulReadKey).c_str(), true, ulReadKey, false)) {
        LOG_ERROR("Init strategy rsp queue {} failed.", ulReadKey);
        return nullptr;
    }

    m_mapRspQueue[ulReadKey] = pReadRspQueue;

    m_LockFileManager.AddListenLockFile(ulReadKey); // 监听策略进程的锁文件;

    // 写入新的batchID；    
    std::ofstream oFile("ReadKey.hist", std::ios::app);
    if (!oFile.is_open()) {
        LOG_WARN("write strategy batchid file failed: ReadKey.hist");
    } else {
        oFile << SecTimeStr("%Y-%m-%d %H-%M-%S") << "    " << ulReadKey << "\n";
        oFile.close();    
    }

    return pReadRspQueue;
}


void WriteMessageManager::StartListenQueue() {

    LOG_INFO("Start listen API queue And strategy queue!");

    shptrConsumerThread_ = std::make_shared<std::thread>([this]() {

        while (bIsRunning_) {

            /// 先尝试处理 策略请求队列 m_iStrateReadcessCount 个请求;
            WriteMsg uteMsg;

            if (m_TickDataQueue.TryPopShare(uteMsg)) {
                if (SHARE_COMM_LIKELY(kPktReadInit != uteMsg.iMsgID)) {  
                    LOG_DEBUG("NoOrderReq, iMsgID:{}, ulReadKey:{},len:{}",uteMsg.iMsgID, uteMsg.ulReadKey, uteMsg.iMsgLen); //todo 测试使用
                    m_pfnOnMessage(uteMsg.iMsgID, uteMsg.strMsgBuf, uteMsg.ulReadKey);
                } else {
                    // Read 进程初始化时会调用 - 创建对应回报共享内存以及相关无锁队列;
                    CreateReadRspQueue(uteMsg.ulReadKey);
                }
            } 
           
            // sleep(1); //todo 测试专用;
        }

        LOG_INFO("Listen Data Over");
        m_TickDataQueue.Release();;

    });

    if (!shptrConsumerThread_) {
        LOG_ERROR("Create Write consumer thread failed.");
        return;
    }

}

void WriteMessageManager::Reset() {
 
    // 等待事件循环;
    if (shptrConsumerThread_ && shptrConsumerThread_->joinable()) {
        shptrConsumerThread_->join();
    }   

    bIsRunning_ = false;
 

    for (auto it : m_mapRspQueue) {
        if (it.second)
            delete it.second;
    }
}


} // namespace share_common