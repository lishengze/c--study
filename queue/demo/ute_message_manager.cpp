#include "ute_message_manager.h"

QueueManager* UteMessageManager::CreateRspQueueManager(unsigned long long  strStrategyKey){
    return nullptr;
}

bool UteMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {

    return true;
}

bool UteMessageManager::Init(const char* cstrUTESysName) {

    ///锁文件相关初始化;
    if (!m_LockFileManager.Init(cstrUTESysName, this, 5)) {
        // todo 增加日志信息;
        return false;
    }

    // m_pStrategyReqQueue.Init(this， ,cstrUTESysName,  QUEUE_TYPE_REQ);   

    // 初始化请求相关的无锁队列 以及 对应的 共享内存 - 共享内存是UTE进程创建好， 这里只需要创建和attach即可;
    m_pStrategyReqQueue.Init(this, Consumer, (std::string(cstrUTESysName)+".queue").c_str(),  true);

    m_pApiQueue.Init(this, Consumer, "",  false);
    
    // m_pReqQueue = new QueueManager();
    // if (!m_pReqQueue->Init(cstrUTESysName, QUEUE_TYPE_REQ)) {
    //     // todo 增加日志信息;
    //     return false;
    // }
    return true;
}

bool UteMessageManager::WriteMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    return true;
}

//
std::shared_ptr<QueueManager> UteMessageManager::CreateStrategyRspQueue(unsigned long long strStrategyKey) {

    return nullptr;
}
