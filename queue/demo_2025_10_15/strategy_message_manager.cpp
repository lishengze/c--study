#include "strategy_message_manager.h"

bool StrategyMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // TODO: Implement the message sending logic here.
    return true;
}

bool StrategyMessageManager::Init(const char* UteName, unsigned int uiStrategySysID) {

    /// 锁文件相关初始化;
    unsigned int uiBatchID = m_LockFileManager.GetSetBatchID(uiStrategySysID);
    if (uiBatchID == 0) {
        // todo 增加日志信息;
        return false;
    }
    unsigned long long m_StrategyKey = (uiStrategySysID << 32) | uiBatchID;

    std::string strStrategyKey = std::to_string(m_StrategyKey);
    std::string strUteKey = std::string(UteName);

    if (!m_LockFileManager.Init((strUteKey+".lck").c_str(), (strStrategyKey + ".lck").c_str(), 
                                m_iEventSleepSec, this)) {
        // todo 增加日志信息;
        return false;
    }

    // 初始化请求相关的无锁队列 以及 对应的 共享内存;
    m_ReqQueueManager.Init((strUteKey+".queue").c_str());

    // 初始化响应相关的无锁队列 以及 对应的 共享内存;
    m_RspQueueManager.Init((strStrategyKey+".queue").c_str());

    return true;
}