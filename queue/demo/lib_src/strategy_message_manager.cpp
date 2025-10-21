#include "strategy_message_manager.h"

namespace share_common 
{

bool StrategyMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // TODO: Implement the message sending logic here.
    m_ReqQueueManager.SendMsg(iMsgID, pMsgBuf, iMsgLen, m_StrategyKey);
    return true;
}

bool StrategyMessageManager::Init(const char* UteName, unsigned int uiStrategySysID) {
    LOG_INFO("StrategyMessageManager::Init begin UteName={}, uiStrategySysID={}", UteName, uiStrategySysID);

    if (!m_pfnOnMessage) {
        LOG_ERROR("OnMessage callback function is not set.");
        return false;
    }

    if (!m_pfnOnEvent) {
        LOG_ERROR("OnEvent callback function is not set.");
        return false;
    }


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

    if (!m_LockFileManager.Init(UteName, m_StrategyKey, this, m_iEventSleepSec)) { 
        LOG_ERROR("Failed to initialize lock file manager.");
        return false;
    }

    // 初始化请求相关的无锁队列 以及 对应的 共享内存 -- 共享内存是UTE进程创建好， 这里只需要attach即可;
    if (!m_ReqQueueManager.Init(this, Producer, GetQueueName(UteName).c_str(),  false)){
        LOG_ERROR("Failed to initialize request queue manager.");
        return false;
    }

    // 初始化响应相关的无锁队列 以及 对应的 共享内存 -- 共享内存是策略进程创建好， 这里需要创建和attach;
    // 创建好后，便开始监听队列消息;
    if (!m_RspQueueManager.Init(this, Consumer, GetQueueName(m_StrategyKey).c_str(), true)) {
        LOG_ERROR("Failed to initialize response queue manager.");
        return false;
    }

    return true;
}


bool StrategyMessageManager::TryPopMsg(UteMsg& msg) {
    return m_ReqQueueManager.trypop(msg);
}

} // namespace share_common 