#include "strategy_message_manager.h"

#include "share_comm_util.h"

namespace share_common 
{

/// @brief 策略进程发送请求，是发送到共享内存中，所以使用SendMsgShare;
/// @param iMsgID 
/// @param pMsgBuf 
/// @param iMsgLen 
/// @return 
bool StrategyMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // TODO: Implement the message sending logic here.

    if (m_bRspQueueInit) { 
        m_ReqQueueManager.SendMsg(iMsgID, pMsgBuf, iMsgLen, m_StrategyKey); // 用于回报的共享内存已经初始化成功，可以发送任何请求;
    } else if (m_bUteInit && iMsgID == kPktLoginReq) {
        
        m_ReqQueueManager.SendMsg(iMsgID, pMsgBuf, iMsgLen, m_StrategyKey);

        StartWaitUte();  // todo 先不测试这个。
    } else {
        LOG_WARN("UTE Not Ready!");
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

    if (!m_LockFileManager.Init(cstrUTESysName, m_StrategyKey, this, m_iEventSleepSec)) { 
        LOG_ERROR("Failed to initialize lock file manager.");
        return false;
    }

    // 初始化请求相关的无锁队列 以及 对应的 共享内存 -- 共享内存是UTE进程创建好， 这里只需要attach即可;
    if (!m_ReqQueueManager.Init(this, Producer, GetQueueName(cstrUTESysName).c_str(),  false)){
        LOG_ERROR("Failed to initialize request queue manager.");
        return false;
    }

    return true;
}


/// @brief  策略进程是从 共享内存取出数据,所以用 TryPopShare
/// @param msg 
/// @return 
bool StrategyMessageManager::TryPopMsg(UteMsg& msg) {
    return m_ReqQueueManager.TryPop(msg);
}

void StrategyMessageManager::StartWaitUte() {
    if (!m_bStartWaitUteRspQueue) {
        LOG_DEBUG("------ Stratgy Start Wait Ute To Init RspQueue {} !",  GetQueueName(m_StrategyKey));
        m_bStartWaitUteRspQueue = true;

        std::thread waitThread([this]() {
            long lStartWaitSec = SecTime();
            
            while (true) {
                UteMsg msg;
                if (m_RspQueueManager.Init(this, Consumer, GetQueueName(m_StrategyKey).c_str(), false)) {
                    m_bRspQueueInit = true;
                    LOG_INFO("Stratgy {} Attach Rsp Queue {} SUCCESS", m_StrategyKey, GetQueueName(m_StrategyKey));
                    break; // 成功;
                }

                if (SecTime() - lStartWaitSec > m_iWaitUteSec) {
                    LOG_ERROR("Wait Too long For Ute To Init RspQueue {} !",  GetQueueName(m_StrategyKey));
                    m_pfnOnEvent(kUteNotInited, "Wait Too long For Ute To Init RspQueue!");
                    return;
                }

                sleep(2); // todo 测试专用;
                }
            }); //

        if (waitThread.joinable()) {
            waitThread.join();
        }

    }
}

} // namespace share_common 