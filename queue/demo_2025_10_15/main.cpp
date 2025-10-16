#include <iostream>
#include "strategy_message_manager.h"
#include <unistd.h>
#include "external_message.h"

void StragegyOnEvent(int iErrCode, const char* pErrDesc)
{
	//UTE系统异常处理
}

void StragegyOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen)
{
	//举例进行登陆回报处理
    if (kPktLoginAns == iMsgID) {
        LogOnAns* ans = (LogOnAns*)pMsgBuf;
        if (0 == ans->error_code) {
            std::cout << "登录成功" << std::endl;
        } else {
            std::cout << "登录失败，错误码：" << ans->error_code << std::endl;
        }
    }
    else if(kPktOrderAns == iMsgID)
    {
        TradeOrderER* ans  = (TradeOrderER*)pMsgBuf;
        //处理委托回报
    }
    else if(kPktOrderMatch == iMsgID)
    {
        TradeOrderER* ans = (TradeOrderER*)pMsgBuf;
        //处理成交回报
    }

}

// 举例进行登录请求设置
void DoLoginReq(StrategyMessageManager& stragegyMsgManager) {
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

int main()
{
	StrategyMessageManager MessageManager;
	//设置OnEvent函数回调信息
	MessageManager.SetOnEvent(3, StragegyOnEvent);
	//设置OnMessage函数回调信息
	MessageManager.SetOnMessage(StragegyOnMessage);

    DoLoginReq(MessageManager);

    DoTradeOrderReq(MessageManager);

	//初始化
	if(!MessageManager.Init("UTE_11_111_111", 11))
	{
		// LOG("ERROR")；
		exit(-1);
	}

	
    DoLoginReq(MessageManager);


	//应答处理完才可发起正常委托
    DoTradeOrderReq(MessageManager);
	
	while(1)
	{
		sleep(1);
	}
}