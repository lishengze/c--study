#include "ute_message_manager.h"

QueueManager* UteMessageManager::CreateRspQueueManager(unsigned long long  strStrategyKey){
    return nullptr;
}

bool UteMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {

    return true;
}

bool UteMessageManager::Init(UINT64 UTESysID) {
    return true;
}

bool UteMessageManager::WriteMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    return true;
}
