#include "external_message.h"
#include "message_manager.h"

void OnEvent(int iErrCode, const char* pErrDesc)
{
	//UTE系统异常处理
}

void OnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen)
{
	//订单回报处理
}

int main()
{
	CMessageManager MessageManager;
	//设置OnEvent函数回调信息
	MessageManager.SetOnEvent(3, OnEvent);
	//设置OnMessage函数回调信息
	MessageManager.SetOnEvent(OnMessage);
	//初始化
	if(!MessageManager.Init("XXXX_1", "UTE_11_11_11"))
	{
		LOG("ERROR")；
		exit(-1)
	}
	
	while(1)
	{
		sleep(1);
	}
}