#include "strategy_message_manager.h"

namespace share_common 
{

bool StrategyMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // TODO: Implement the message sending logic here.
    return true;
}

bool StrategyMessageManager::Init(const char* UteName, unsigned int uiStrategySysID) {

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
    unsigned long long m_StrategyKey = (uiStrategySysID << 32) | uiBatchID;

    if (!m_LockFileManager.Init(UteName, m_StrategyKey, this, m_iEventSleepSec)) {
        LOG_ERROR("Failed to initialize lock file manager.");
        return false;
    }

    // 初始化请求相关的无锁队列 以及 对应的 共享内存 -- 共享内存是UTE进程创建好， 这里只需要attach即可;
    m_ReqQueueManager.Init(this, Consumer, GetQueueName(UteName).c_str(),  false);

    // 初始化响应相关的无锁队列 以及 对应的 共享内存 -- 共享内存是策略进程创建好， 这里需要创建和attach;
    m_RspQueueManager.Init(this, Producer, GetQueueName(m_StrategyKey).c_str(), true);

    return true;
}

void StrategyMessageManager::ProcessMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {

    // if (kPktLoginAns == iMsgID) {
    //     LogOnAns* pLogOnAns = (LogOnAns*)pMsgBuf;
    //     if (kSuccess == pLogOnAns->error_code) {
    //         // 登录成功, 说明
    //         m_LockFileManager.SetLoginSuccess();
    //     } 
    // }

    // // 处理消息
    // m_pfnOnMessage(iMsgID, pMsgBuf, iMsgLen);
}

} // namespace share_common 