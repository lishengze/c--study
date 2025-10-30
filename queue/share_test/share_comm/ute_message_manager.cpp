#include "ute_message_manager.h"

namespace share_common {

SINGLETON_IMPL(UteMessageManager, common::SingletonType::kUTEShareMessageManager);

bool UteMessageManager::Init (const char* cstrUTESysName, int iApiReqProcessCount, int iStrategyReqProcessCount,
                int iReqShareQueueDataNum, int iRspShareQueueDataNum, int iApiQueueDataNum,
                int iListenCpuID, int iListenNumaCode)
{
    LOG_INFO("Init UTE message manager, utesysname:{}, api req process count:{}, strategy req process count:{}, iReqShareQueueDataNum:{}, iRspShareQueueDataNum:{}, iApiQueueDataNum:{}, iListenCpuID:{}, iListenNumaCode:{}",
                     cstrUTESysName, iApiReqProcessCount, iStrategyReqProcessCount,
                     iReqShareQueueDataNum, iRspShareQueueDataNum,  iApiQueueDataNum,
                     iListenCpuID, iListenNumaCode);

    if (cstrUTESysName == nullptr || strcmp(cstrUTESysName, "") == 0) {
        LOG_ERROR("Input cstrUTESysName is Error");
        return false;
    }
    
    if (!m_pfnOnInnerMessage) {
        LOG_ERROR("OnInnerMessage callback function is not set.");
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

    if (iApiReqProcessCount <= 0) {
        LOG_ERROR("Input iApiReqProcessCount is {}, Set To Default: {} ", iApiReqProcessCount, m_iApiReqProcessCount);
    } else {
        m_iApiReqProcessCount = iApiReqProcessCount;
    }

    if (iStrategyReqProcessCount <= 0) {
        LOG_ERROR("Input iStrategyReqProcessCount is {}, Set To Default: {} ", iStrategyReqProcessCount, m_iStrategyReqProcessCount);
    } else {
        m_iStrategyReqProcessCount = iStrategyReqProcessCount;
    }

    if (iReqShareQueueDataNum <= 0) {
        LOG_ERROR("Input iReqShareQueueDataNum is {}, Set To Default: {} ", iApiReqProcessCount, m_iReqShareQueueDataNum);
    } else {
        m_iReqShareQueueDataNum = iReqShareQueueDataNum;
    }

    if (iRspShareQueueDataNum <= 0) {
        LOG_ERROR("Input iRspShareQueueDataNum is {}, Set To Default: {} ", iRspShareQueueDataNum, m_iRspShareQueueDataNum);
    } else {
        m_iRspShareQueueDataNum = iRspShareQueueDataNum;
    }

    if (iApiQueueDataNum <= 0) {
        LOG_ERROR("Input iApiQueueDataNum is {}, Set To Default: {} ", iApiQueueDataNum, m_iApiQueueDataNum);
    } else {
        m_iApiQueueDataNum = iApiQueueDataNum;
    }            

    m_iListenCpuID = iListenCpuID;
    m_iListenNumaCode = iListenNumaCode;

    ///锁文件相关初始化;
    // 初始化UTE进程的锁文件管理器;
    if (!m_LockFileManager.Init(GetLockFileName(cstrUTESysName).c_str(), m_pfnOnEvent, 5)) {
        LOG_ERROR("Init lock file {} manager failed.", cstrUTESysName);
        return false;
    }

    // 初始化策略非委托请求相关的无锁队列 以及 对应的 共享内存
    if (!m_StrategyNonReqOrderQueue.Init(Consumer, GetNonReqOrderQueueName(cstrUTESysName).c_str(),  true, 0, false)) {
        LOG_ERROR("Init ReqQueueManager:{}, Failed!", GetNonReqOrderQueueName(cstrUTESysName).c_str());
        return false;
    } else {
        LOG_INFO("Init ReqQueueManager:{}, SUCCESS!", GetNonReqOrderQueueName(cstrUTESysName).c_str());
    }

    // 初始化策略非委托请求相关的无锁队列 以及 对应的 共享内存
    if (!m_StrategyReqOrderQueue.Init(Consumer, GetReqOrderQueueName(cstrUTESysName).c_str(),  true, 0, true)) {
        LOG_ERROR("Init ReqQueueManager:{}, Failed!", GetReqOrderQueueName(cstrUTESysName).c_str());
        return false;
    } else {
        LOG_INFO("Init ReqQueueManager:{}, SUCCESS!", GetReqOrderQueueName(cstrUTESysName).c_str());
    }

    // 创建内存中的API请求队列， 这里不需要创建和attach共享内存， 直接创建即可;
    if (!m_InnerMsgQueue.Init(Consumer, "",  false, 0, false)) {
        LOG_ERROR("Init ReqQueueManager For API Failed");
        return false;
    } else {
        LOG_INFO("Init ReqQueueManager For API SUCCESS!");
    }

    StartListenQueue(); // todo 启动监听队列线程;
    
    return true;
}

bool UteMessageManager::WriteMsg(int iMsgID, const char* pMsgBuf, unsigned int iMsgLen, int iMsgSrcType, void* pMsgHandler) {
    LOG_DEBUG("API Msg: iMsg:{}, iMsgLen:{}, iMsgSrcType:{}", iMsgID, iMsgLen, iMsgSrcType);
    m_InnerMsgQueue.SendMsg(iMsgID, pMsgBuf, iMsgLen, iMsgSrcType, pMsgHandler); //API 转发的请求和回报， 策略ID为0;
    return true;
}

//
QueueManager* UteMessageManager::CreateStrategyRspQueue(unsigned long long ulStrategyKey) {

    if (m_mapRspQueue.find(ulStrategyKey)!= m_mapRspQueue.end()) {
        return m_mapRspQueue[ulStrategyKey];
    }

    QueueManager* pStrategyRspQueue = new QueueManager();

    if (!pStrategyRspQueue) {
        LOG_ERROR("Create strategy rsp queue {} failed.", ulStrategyKey);
        return nullptr;
    }

    // 初始化策略进程接收回报的共享内存队列管理器，这块共享内存由服务端创建好, 此时策略端一直在等待
    if (!pStrategyRspQueue->Init(Producer, GetQueueName(ulStrategyKey).c_str(), true, ulStrategyKey, false)) {
        LOG_ERROR("Init strategy rsp queue {} failed.", ulStrategyKey);
        return nullptr;
    }

    m_mapRspQueue[ulStrategyKey] = pStrategyRspQueue;

    m_LockFileManager.AddListenLockFile(ulStrategyKey); // 监听策略进程的锁文件;

    return pStrategyRspQueue;
}

/// @brief 根据key 获取对应回报共享内存无锁队列句柄;
/// @param strStrategyKey 
/// @return 
QueueManager*  UteMessageManager::GetStrategyRspQueue(unsigned long long strStrategyKey) {
    if (m_mapRspQueue.find(strStrategyKey) != m_mapRspQueue.end()) {
        return m_mapRspQueue[strStrategyKey];
    } else {
        return nullptr;
    }
}

void UteMessageManager::StartListenQueue() {

    LOG_INFO("Start listen API queue And strategy queue!");

    shptrConsumerThread_ = std::make_shared<std::thread>([this]() {

        BindCpuID(m_iListenCpuID, m_iListenNumaCode);

        while (bIsRunning_) {

            // m_StrategyNonReqOrderQueue.TestCharValue();
            
            /// 先尝试处理 策略请求队列 m_iStrategyReqProcessCount 个请求;

            int iStrategyReqCount = 0;
            UteMsg uteMsg;
            TradeOrderReq reqOrder;

            while(true) {
                int iCurDataCount = 0;
                
                if (m_StrategyNonReqOrderQueue.TryPopShare(uteMsg)) {
                    if (SHARE_COMM_LIKELY(kPktStrategyInit != uteMsg.iMsgID)) {  
                        m_pfnOnMessage(uteMsg.iMsgID, uteMsg.strMsgBuf, uteMsg.ulStrategyKey);
                    } else {
                         // 只有某个策略进程初始化时会调用 - 创建对应回报共享内存以及相关无锁队列;
                        CreateStrategyRspQueue(uteMsg.ulStrategyKey);
                    }
                    iCurDataCount++;
                    if (iCurDataCount++ >= m_iStrategyReqProcessCount) break;
                } 

                if (m_StrategyReqOrderQueue.TryPopReqOrder(reqOrder)) {
                    if (reqOrder.ulStrategyKey == 0) continue; // todo 测试首单是否巨大延迟;

                    m_pfnOnMessage(kPktOrderReq, (char*)(&reqOrder), reqOrder.ulStrategyKey);
                    iCurDataCount++;
                    if (iCurDataCount++ >= m_iStrategyReqProcessCount) break;
                }                 

                if (!iCurDataCount) break; // 此轮没有数据处理,直接去处理内部的数据请求队列;
            }

            /// 再尝试处理 API 和交易所回报 请求队列 m_iApiReqProcessCount 个请求;
            for (int i = 0; i < m_iApiReqProcessCount; i++) {
                UteMsg uteMsg;
                if (m_InnerMsgQueue.TryPop(uteMsg)) {
                    LOG_DEBUG("Get api req msg, msgid:{}, src type:{},", uteMsg.iMsgID, uteMsg.iMsgSrcType);
                    m_pfnOnInnerMessage(uteMsg.iMsgID, uteMsg.strMsgBuf, uteMsg.iMsgLen, uteMsg.iMsgSrcType,  uteMsg.pMsgHander); // api 请求和交易所回报 过来的消息；                    
                } else {
                    break; // 没有数据直接结束
                }
            }

           

            // sleep(6); //todo 测试专用;
        }

        LOG_INFO("Listen Data Over");
        m_StrategyNonReqOrderQueue.Release();
        m_StrategyReqOrderQueue.Release();
        m_InnerMsgQueue.Release();

    });

    if (!shptrConsumerThread_) {
        LOG_ERROR("Create UTE consumer thread failed.");
        return;
    }

    // if (shptrConsumerThread_->joinable()) {
    //     LOG_DEBUG("Listen Req Thread Join");
    //     shptrConsumerThread_->join();
    // }    

}

void UteMessageManager::Reset() {
 
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