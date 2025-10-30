#include "strategy_message_manager.h"
#include <utility>
#include "share_comm_util.h"

namespace share_common 
{

/// @brief 策略进程发送请求，是发送到共享内存中，所以使用SendMsgShare;
/// @param iMsgID 
/// @param pMsgBuf 
/// @param iMsgLen 
/// @return 
bool StrategyMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // 理论上，m_bRspQueueInit 会在其他线程修改这个值, 就会出现判断失败的情况，但是这种场景较少发生，并且即便判断失败也没有太大影响，共享内存并未被回收，
    // 如果使用多线程编程模式，增加开销得不偿失;
    // m_bRspQueueInit 赋值为true, 是一定在 SendMsg 前实现，所以不会有问题;
    if (SHARE_COMM_LIKELY(m_bRspQueueInit)) { 

        if (iMsgID == kPktOrderReq) {
            TradeOrderReq* pOrderReq = (TradeOrderReq*)pMsgBuf;
            pOrderReq->ulStrategyKey  = m_StrategyKey; // 将策略KEY发送给服务端;
            m_ReqOrderQueueManager.SendReqOrder(pMsgBuf); // 用于回报的共享内存已经初始化成功，可以发送任何请求;
        } else {
            m_NonReqOrderQueueManager.SendMsgShare(iMsgID, pMsgBuf, iMsgLen, m_StrategyKey); // 用于回报的共享内存已经初始化成功，可以发送任何请求;
        }
    } else {
        LOG_ERROR("Share Memory To UTE is Not Init Or Over!");
    }

    return true;
}

bool StrategyMessageManager::Init(const char* cstrUTESysName, unsigned int uiStrategySysID, int iWaitUteSec) {
    LOG_INFO("StrategyMessageManager::Init begin cstrUTESysName={}, uiStrategySysID={}", cstrUTESysName, uiStrategySysID);

    if (!m_pfnOnMessage) {
        LOG_ERROR("OnMessage callback function is not set.");
        return false;
    }

    if (!m_pfnOnEvent) {
        LOG_ERROR("OnEvent callback function is not set.");
        return false;
    }

    if (!iWaitUteSec) {
        LOG_ERROR("iWaitUteSec is 0");
        return false;
    }   
    m_iWaitUteSec = iWaitUteSec;

    /// 锁文件相关初始化;
    unsigned int uiBatchID = m_LockFileManager.GetSetStrategyBatchID(uiStrategySysID);
    if (uiBatchID == 0) {
        LOG_ERROR("Failed to get or set batch ID.");
        return false;
    }
    unsigned long long high_part = static_cast<unsigned long long>(uiStrategySysID) << 32;
    // 2. 低 32 位：将 low 转换为 64 位（自动填充高 32 位为 0）
    unsigned long long low_part = static_cast<unsigned long long>(uiBatchID);
    m_StrategyKey = (high_part | low_part);

    LOG_INFO("uiStrategySysID={}, uiBatchID={}, m_StrategyKey={}", uiStrategySysID, uiBatchID, m_StrategyKey);


    long lStartWaitSec = SecTime();

    // 创建好策略的锁文件后，开始监听UTE是否创建, 超时通过OnEvent通知， 成功，调用 InitReqQueue();

    StrategyGetRspCallbackEventFuncType StrategyOnEventFunc = std::bind(&StrategyMessageManager::ProcessUteEnd, this, std::placeholders::_1, std::placeholders::_2);
    if (!m_LockFileManager.Init(cstrUTESysName, m_StrategyKey, StrategyOnEventFunc, lStartWaitSec, m_iEventSleepSec, m_iWaitUteSec)) { 
        LOG_ERROR("Failed to initialize lock file manager.");
        return false;
    } 

    if (!InitReqQueue(cstrUTESysName)) {
        return false;
    }

    if (!StartWaitUteQueue(lStartWaitSec)) {
        return false;
    } else {
        LOG_INFO("Strategy {} Attach Rsp Queue {} SUCCESS", m_StrategyKey, GetQueueName(m_StrategyKey));        
    }

    // 回报的共享内存Attach成功后，再开启心跳线程 检UTE进程的锁文件;
    m_LockFileManager.StartHeartbeatThread(); 

    return true;
}

bool StrategyMessageManager::InitReqQueue(const char* cstrUTESysName) {
    // 初始化请求相关的无锁队列 以及 对应的 共享内存 -- 共享内存是UTE进程创建好， 这里只需要attach即可;
    if (!m_NonReqOrderQueueManager.Init(m_pfnOnMessage, Producer, GetNonReqOrderQueueName(cstrUTESysName).c_str(),  false, false)){
        LOG_ERROR("Failed to initialize request queue manager.");
        m_pfnOnEvent(kUteNotInited, "Failed to initialize request queue manager.");
        return false;
    } else {
        LOG_INFO("Attach UTE Share Memory:{} [PASSED]", GetNonReqOrderQueueName(cstrUTESysName));
    }

    if (!m_ReqOrderQueueManager.Init(m_pfnOnMessage, Producer, GetReqOrderQueueName(cstrUTESysName).c_str(),  false, true)){
        LOG_ERROR("Failed to initialize request queue manager.");
        m_pfnOnEvent(kUteNotInited, "Failed to initialize request queue manager.");
        return false;
    } else {
        LOG_INFO("Attach UTE Share Memory:{} [PASSED]", GetReqOrderQueueName(cstrUTESysName));
    }    


    return true;
}


/// @brief  策略进程是从 共享内存取出数据,所以用 TryPopShare
/// @param msg 
/// @return 
bool StrategyMessageManager::TryPopMsg(UteMsg& msg) {
    return m_NonReqOrderQueueManager.TryPop(msg);
}

bool StrategyMessageManager::StartWaitUteQueue(long lStartWaitSec) {

    StrategyKey obj;
    obj.ulStrategyKey = m_StrategyKey;

    //  发送StragegyKey 给服务端， 服务端由此创建一对一的回报共享内存队列;
    m_NonReqOrderQueueManager.SendMsgShare(kPktStrategyInit, (char*)(&obj), sizeof(StrategyKey), m_StrategyKey); 

    while (true) {
        LOG_INFO("Start Waiting UTE Create Share Mermory:{}", GetQueueName(m_StrategyKey));

        UteMsg msg;
        if (m_RspQueueManager.Init(m_pfnOnMessage, Consumer, GetQueueName(m_StrategyKey).c_str(), false, false)) {
            m_bRspQueueInit = true;            
            break; // 成功;
        }

        if (SecTime() - lStartWaitSec > m_iWaitUteSec) {            
            m_pfnOnEvent(kUteNotInited, "Wait Too long For Ute To Init RspQueue!");
            return false;
        }

        sleep(1); // todo 测试专用;
    }
    
    return true;
}

void StrategyMessageManager::ProcessUteEnd(int iErrCode, const char* pErrDesc) {

    LOG_WARN("iErrCode:{}, pErrDesc:{}", iErrCode, pErrDesc );

    m_bRspQueueInit = false;  // 告知无法再发送消息即可，无需立刻释放 Queue资源;

    m_pfnOnEvent(iErrCode, pErrDesc);
}



} // namespace share_common 