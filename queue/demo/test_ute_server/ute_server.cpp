#include "ute_server.h"

#include "ute_message_manager.h"
#include "external_message.h"

using namespace share_common;

UteMessageManager uteMessageManager;

void SendLogAns(QueueManager* pQueueManager) {
    LOG_INFO("SendLogAns");

    LogOutAns stLogOutAns;

    strcpy(stLogOutAns.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(stLogOutAns.trade_order_user.branch_id, "XXXXXX");
    strcpy(stLogOutAns.trade_order_user.account_id, "XXXXXX");
    strcpy(stLogOutAns.trade_order_user.cust_id, "XXXXXX");

    // uteMessageManager.

    pQueueManager->SendMsg(kPktLogoutAns, (char*)&stLogOutAns, sizeof(stLogOutAns));
}

void SendOrderRsp(QueueManager* pQueueManager) {
    LOG_INFO("SendOrderRsp");
    
    TradeOrderER stTradeOrderERP;

    strcpy(stTradeOrderERP.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.branch_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.account_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.cust_id, "XXXXXX");    

    pQueueManager->SendMsg(kPktOrderAns, (char*)&stTradeOrderERP, sizeof(stTradeOrderERP));
}

void UteOnEvent(int iMsgID, const char* pMsgBuf,  unsigned long long ulStrategyKey) {
    LOG_INFO("UteOnEvent: iMsgID={}, ulStrategyKey={}", iMsgID, ulStrategyKey);
}

void UteOnMessage(int iMsgID, const char* pMsgBuf,  unsigned long long ulStrategyKey) {
    LOG_INFO("UteOnMessage: iMsgID={},  ulStrategyKey={}", iMsgID, ulStrategyKey);



    if (iMsgID == kPktLoginReq) {
        QueueManager* pQueueManager = uteMessageManager.CreateStrategyRspQueue(ulStrategyKey);

        if (pQueueManager) {
            SendLogAns(pQueueManager);

            sleep(3);

            SendOrderRsp(pQueueManager);
        }
    }
}



void TestUteServer() {
    
    std::string strUteName = "test_ute";

    int iEventSleepSec = 3;

    uteMessageManager.SetOnEvent(iEventSleepSec, UteOnEvent);
    uteMessageManager.SetOnMessage(UteOnMessage);

    uteMessageManager.Init(strUteName.c_str(), 5, 1);

}