#include <iostream>
#include "strategy_message_manager.h"
#include <unistd.h>

void StragegyOnEvent(int iErrCode, const char* pErrDesc)
{
	//UTE系统异常处理
}

void StragegyOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen)
{
	//举例进行订单回报处理


}

// 举例进行登录请求设置
void SetLoginReq(StrategyMessageManager& stragegyMsgManager) {
    
}

int main()
{
	StrategyMessageManager MessageManager;
	//设置OnEvent函数回调信息
	MessageManager.SetOnEvent(3, StragegyOnEvent);
	//设置OnMessage函数回调信息
	MessageManager.SetOnMessage(StragegyOnMessage);


	//初始化
	if(!MessageManager.Init("XXXX_1", 11))
	{
		// LOG("ERROR")；
		exit(-1);
	}
	
	while(1)
	{
		sleep(1);
	}
}