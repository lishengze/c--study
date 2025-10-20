#include "strategy_client.h"

#include "strategy_message_manager.h"

using namespace share_common;

void StrategyOnEvent(int iErrCode, const char* pErrDesc) {
    LOG_WARN("StrategyOnEvent: iErrCode={}, pErrDesc={}", iErrCode, pErrDesc);
}

void StrategyOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    LOG_INFO("StrategyOnMessage: iMsgID={}, iMsgLen={}", iMsgID, iMsgLen);
}


// 举例进行登录请求设置
void DoLoginReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoLoginReq");

    LogOnReq req = {0};
    req.heart_bt_int = 3;
    req.trade_order_user.agw_seq_id = 10001;
    strcpy(req.password, "XXXXXX");
    strcpy(req.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(req.trade_order_user.branch_id, "XXXXXX");
    strcpy(req.trade_order_user.account_id, "XXXXXX");
    strcpy(req.trade_order_user.cust_id, "XXXXXX");
    req.trade_order_user.client_seq_id = 1;

    stragegyMsgManager.SendMsg(kPktLoginReq, (char*)&req, sizeof(req));
}

void DoTradeOrderReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoTradeOrderReq");
    TradeOrderReq req = {0};
    strcpy(req.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(req.trade_order_user.branch_id, "XXXXXX");
    strcpy(req.trade_order_user.account_id, "XXXXXX");
    strcpy(req.trade_order_user.cust_id, "XXXXXX");
    req.trade_order_user.client_seq_id = 10001;
    req.trade_order_info.order_qty = 800;
    req.trade_order_info.side = kBuy;
    req.trade_order_info.order_type = kLimited;
    req.trade_order_info.market_id = kShangHai;
    strcpy(req.trade_order_info.security_id, "XXXXXX");
  
    stragegyMsgManager.SendMsg(kPktOrderReq, (char*)&req, sizeof(req));
}

void TestStrategyClient() {
    StrategyMessageManager stStrategyMessageManager;

    std::string strUteName = "test_ute";
    unsigned int uiStrategySysID = 1;
    int iSleepSec = 3;

    stStrategyMessageManager.SetOnEvent(iSleepSec, StrategyOnEvent);
    stStrategyMessageManager.SetOnMessage(StrategyOnMessage);

    stStrategyMessageManager.Init(strUteName.c_str(), uiStrategySysID);

    DoLoginReq(stStrategyMessageManager);

    // sleep(5);

    // DoTradeOrderReq(stStrategyMessageManager);

    // stStrategyMessageManager.Init();
}