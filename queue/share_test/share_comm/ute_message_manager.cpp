#include "ute_message_manager.h"

namespace share_common {

SINGLETON_IMPL(UteMessageManager, common::SingletonType::kUplinkBiz);

bool UteMessageManager::Init (const char* cstrUTESysName, int iApiReqProcessCount, int iStrategyReqProcessCount,
                int iReqShareQueueDataNum, int iRspShareQueueDataNum, int iApiQueueDataNum)
{
    LOG_INFO("Init UTE message manager, utesysname:{}, api req process count:{}, strategy req process count:{}, iReqShareQueueDataNum:{}, iRspShareQueueDataNum:{}, iApiQueueDataNum:{}",
                     cstrUTESysName, iApiReqProcessCount, iStrategyReqProcessCount,
                     iReqShareQueueDataNum, iRspShareQueueDataNum,  iApiQueueDataNum);

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

    ///锁文件相关初始化;
    // 初始化UTE进程的锁文件管理器;
    if (!m_LockFileManager.Init(GetLockFileName(cstrUTESysName).c_str(), this, 5)) {
        LOG_ERROR("Init lock file {} manager failed.", cstrUTESysName);
        return false;
    }

    // 初始化请求相关的无锁队列 以及 对应的 共享内存 - 共享内存是UTE进程创建好， 这里只需要创建和attach即可;
    m_pStrategyReqQueue.Init(Consumer, GetQueueName(cstrUTESysName).c_str(),  true);

    // 创建内存中的API请求队列， 这里不需要创建和attach共享内存， 直接创建即可;
    m_pApiQueue.Init(Consumer, "",  false); 

    StartListenQueue(); // todo 启动监听队列线程;
    
    return true;
}

bool UteMessageManager::WriteMsg(int iMsgID, const char* pMsgBuf, unsigned int iMsgLen, int iMsgSrcType, void* pMsgHandler) {
    m_pApiQueue.SendMsg(iMsgID, pMsgBuf, iMsgLen, iMsgSrcType, pMsgHandler); //API 转发的请求和回报， 策略ID为0;
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
    if (!pStrategyRspQueue->Init(Producer, GetQueueName(ulStrategyKey).c_str(), true, ulStrategyKey)) {
        LOG_ERROR("Init strategy rsp queue {} failed.", ulStrategyKey);
        return nullptr;
    }

    m_mapRspQueue[ulStrategyKey] = pStrategyRspQueue;

    m_LockFileManager.AddListenLockFile(ulStrategyKey); // 监听策略进程的锁文件;

    return pStrategyRspQueue;
}

void UteMessageManager::StartListenQueue() {

    LOG_INFO("Start listen API queue And strategy queue!");

    shptrConsumerThread_ = std::make_shared<std::thread>([this]() {
        while (true) {

            // m_pStrategyReqQueue.TestCharValue();
            
            /// 先尝试处理 策略请求队列 m_iStrategyReqProcessCount 个请求;
            for (int i = 0; i < m_iStrategyReqProcessCount; i++) {
                UteMsg uteMsg;
                if (m_pStrategyReqQueue.TryPop(uteMsg)) {
                    LOG_DEBUG("Get strategy req msg, msgid:{}, strategykey:{}", uteMsg.iMsgID, uteMsg.ulStrategyKey);
                    m_pfnOnMessage(uteMsg.iMsgID, uteMsg.strMsgBuf, uteMsg.ulStrategyKey);
                } else {
                    break;  // 没有数据直接结束
                }
            }

            /// 再尝试处理 API 和交易所回报 请求队列 m_iApiReqProcessCount 个请求;
            for (int i = 0; i < m_iApiReqProcessCount; i++) {
                UteMsg uteMsg;
                if (m_pApiQueue.TryPop(uteMsg)) {
                    LOG_DEBUG("Get api req msg, msgid:{}, src type:{},", uteMsg.iMsgID, uteMsg.iMsgSrcType);
                    m_pfnOnInnerMessage(uteMsg.iMsgID, uteMsg.strMsgBuf, uteMsg.iMsgSrcType, uteMsg.iMsgLen, uteMsg.pMsgHander); // api 请求和交易所回报 过来的消息；
                } else {
                    break; // 没有数据直接结束
                }
            }

            sleep(6); //todo 测试专用;
        }
    });

    if (!shptrConsumerThread_) {
        LOG_ERROR("Create UTE consumer thread failed.");
        return;
    }
}

void UteMessageManager::Reset() {
    if (shptrConsumerThread_ && shptrConsumerThread_->joinable()) {
        shptrConsumerThread_->join();
    }    

    for (auto it : m_mapRspQueue) {
        if (it.second)
            delete it.second;
    }
}


} // namespace share_common