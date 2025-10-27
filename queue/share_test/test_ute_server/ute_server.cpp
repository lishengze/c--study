#include "ute_server.h"

#include "ute_message_manager.h"
#include "share_comm_external_message.h"
#include "json_struct.h"

using namespace share_common;

UteMessageManager uteMessageManager;
JsonStructHelper gJsonStructHelper;

map<unsigned long long, QueueManager*> gStrategyRspQueueMap; 

void SendLogAns(QueueManager* pQueueManager) {
    LOG_INFO("SendLogAns");

    LogOnAns stLogOnAns;
    gJsonStructHelper.ParseLogOnAns(stLogOnAns);
    // uteMessageManager.

    pQueueManager->SendMsg(kPktLoginAns, (char*)&stLogOnAns, sizeof(LogOutAns));
}

void SendLogOutAns(QueueManager* pQueueManager) {
    LOG_INFO("SendLogOutAns");

    LogOutAns stLogOutAns;
    gJsonStructHelper.ParseLogOutAns(stLogOutAns);
    pQueueManager->SendMsg(kPktLogoutAns, (char*)&stLogOutAns, sizeof(LogOutAns));
}



void SendOrderRsp(QueueManager* pQueueManager) {
    LOG_INFO("SendOrderRsp");
    
    TradeOrderER stTradeOrderERP;
    gJsonStructHelper.ParseTradeOrderER(stTradeOrderERP);
    pQueueManager->SendMsg(kPktOrderAns, (char*)&stTradeOrderERP, sizeof(TradeOrderER));
}


void SendRejectMsg(QueueManager* pQueueManager, const char* pMsgBuf, int iMsgLen) {
    LOG_INFO("SendRejectMsg");

    RejectMsg stRejectMsg;
    // memcpy(stRejectMsg.msg_buf, pMsgBuf, iMsgLen);
    // stRejectMsg.msg_len = iMsgLen;
    gJsonStructHelper.ParseRejectMsg(stRejectMsg);

    pQueueManager->SendMsg(kPktRejectMsg, (char*)&stRejectMsg, sizeof(RejectMsg));
}


void UteOnEvent(int iMsgID, const char* pMsgBuf,  unsigned long long ulStrategyKey) {
    LOG_INFO("UteOnEvent: iMsgID={}, ulStrategyKey={}", iMsgID, ulStrategyKey);
}

void UteOnMessage(int iMsgID, const char* pMsgBuf, unsigned long long ulStrategyKey)  {
    LOG_DEBUG("iMsgID = {}, ulStrategyKey = {}", iMsgID, ulStrategyKey);

    switch (iMsgID)
    {
            case kPktLoginReq:
            {
                LogOnReq* pReq = (LogOnReq*)(pMsgBuf);

                LOG_DEBUG("From Req CustID: {}", pReq->trade_order_user.cust_id);

                QueueManager* pQueueManager = uteMessageManager.CreateStrategyRspQueue(ulStrategyKey);

                if (!pQueueManager) {
                    LOG_ERROR("CreateStrategyRspQueue failed, ulStrategyKey = {}", ulStrategyKey);
                    return;
                }

                SendLogAns(pQueueManager);

                break;        
            }
            case kPktOrderReq:
            {
                TradeOrderReq* pReq = (TradeOrderReq*)(pMsgBuf);

                LOG_DEBUG("From Req cust_id: {}", pReq->trade_order_user.cust_id);

                QueueManager* pQueueManager = gStrategyRspQueueMap[ulStrategyKey];

                if (!pQueueManager) {
                    LOG_ERROR("GetStrategyRspQueue failed, ulStrategyKey = {}", ulStrategyKey);
                    return;
                }

                SendOrderRsp(pQueueManager);

                break;        
            }



    
    default:
        break;
    }
}


void UteOnInnerMessage(int iMsgID, const char* pMsgBuf, int iMsgSrcType, int iMsgLen,  void* pMsgHandler)  {
    LOG_DEBUG("iMsgID = {},  iMsgSrcType = {}", iMsgID, iMsgSrcType);
}


void TestUteServer() {

    const string strSrcJsonFileName = "test_data.json";
    gJsonStructHelper.Init(strSrcJsonFileName);
    
    
    std::string strUteName = "test_ute";

    int iEventSleepSec = 5;

    uteMessageManager.SetOnEvent(iEventSleepSec, UteOnEvent);
    uteMessageManager.SetOnMessage(UteOnMessage);
    uteMessageManager.SetOnInnerMessage(UteOnInnerMessage);

    uteMessageManager.Init(strUteName.c_str(), 1, 1, 10000, 4000, 4000);

    while(true) {
        sleep(iEventSleepSec);
    }

}