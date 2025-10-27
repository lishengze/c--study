#include "strategy_client.h"

#include "strategy_message_manager.h"

#include <thread>
#include <mutex>
#include <iostream>

#include <iostream>
#include <cstdio>
#include <termios.h>  // 用于修改终端模式（关闭缓冲）
#include <unistd.h>

#include "json.hpp"
#include "json_util.hpp"
#include "json_struct.h"

using namespace share_common;

StrategyMessageManager gStStrategyMessageManager;

JsonStructHelper gJsonStructHelper;
void StrategyOnEvent(int iErrCode, const char* pErrDesc) {
    LOG_WARN("StrategyOnEvent: iErrCode={}, pErrDesc={}", iErrCode, pErrDesc);
}

void StrategyOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    LOG_INFO("StrategyOnMessage: iMsgID={}, iMsgLen={}", iMsgID, iMsgLen);

    if (kPktLoginAns == iMsgID) {
        LogOnAns* pLogonAns = (LogOnAns*)(pMsgBuf);
        LOG_DEBUG("LogOnAns client: {}", pLogonAns->trade_order_user.cust_id);
    }
}


// 举例进行登录请求设置
void DoLoginReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoLoginReq");

    LogOnReq req = {0};

    gJsonStructHelper.ParseLogOnReq(req);

    stragegyMsgManager.SendMsg(kPktLoginReq, (char*)&req, sizeof(req));
}

// 举例进行注销请求设置
void DoLogoutReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoLogoutReq");
    LogOutReq req = {0};
    gJsonStructHelper.ParseLogOutReq(req);  

    stragegyMsgManager.SendMsg(kPktLogoutReq, (char*)&req, sizeof(req));
}

// 举例进行订单请求设置
void DoTradeOrderReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoTradeOrderReq");
    TradeOrderReq req = {0};
    gJsonStructHelper.ParseTradeOrderReq(req);
  
    stragegyMsgManager.SendMsg(kPktOrderReq, (char*)&req, sizeof(req));
}

// 举例进行取消订单请求设置
void DoCancelOrder(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoCancelOrder");
    CancelOrderReq req = {0};
    gJsonStructHelper.ParseCancelOrderReq(req);  
    stragegyMsgManager.SendMsg(kPktCancelOrderReq, (char*)&req, sizeof(req));
}



// 关闭终端输入缓冲（使 getchar 无需回车即可读取）
void disableInputBuffering() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ICANON;  // 关闭规范模式（无需回车）
    tty.c_lflag &= ~ECHO;    // 可选：关闭输入回显（不显示输入的字符）
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// 恢复终端默认模式
void restoreInputBuffering() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ICANON;   // 恢复规范模式
    tty.c_lflag |= ECHO;     // 恢复回显
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

std::string GetHelpInfo() {
    return "h:help;\nq:quit\n1:login;\n2:logout;\n3:OrderReq;\n4:CancelReq;\n";
}

void StartWaitReqCommand()  {
    disableInputBuffering(); // 关闭缓冲，即时读取

    std::thread testThread([](){
        char input;
        while(true) {
            input = getchar();

            switch (input) {
            case 'h':
                std::cout << GetHelpInfo() << std::endl;
                break;
            case 'q':
                exit(0);
                break;
            case '1':
                DoLoginReq(gStStrategyMessageManager);
                break;
            case '2':
                DoLogoutReq(gStStrategyMessageManager);
                break;
            case '3':
                DoTradeOrderReq(gStStrategyMessageManager);
                break;
            case '4':
                DoCancelOrder(gStStrategyMessageManager);
                break;
            default:
                break;
            }

            sleep(1);
        }

    });

    restoreInputBuffering(); // 退出前恢复终端模式
}

void TestStrategyClient() {
    const string strSrcJsonFileName = "test_data.json";
    gJsonStructHelper.Init(strSrcJsonFileName);
    

    std::string strUteName = "test_ute";
    unsigned int uiStrategySysID = 1;
    int iSleepSec = 3;

    gStStrategyMessageManager.SetOnEvent(iSleepSec, StrategyOnEvent);
    gStStrategyMessageManager.SetOnMessage(StrategyOnMessage);

    gStStrategyMessageManager.Init(strUteName.c_str(), uiStrategySysID, 10);

    sleep(3);


    DoLoginReq(gStStrategyMessageManager);
    

    // UteMsg testPopMsg;
    // if (gStStrategyMessageManager.TryPopMsg(testPopMsg)) {
    //     LOG_DEBUG("[SUCCESS] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // } else {
    //     LOG_ERROR("[FAILED] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // }

    // sleep(1);

    // UteMsg msg;
    // while (gStStrategyMessageManager.TryPopMsg(msg)) {
    //     LOG_INFO("TryPopMsg: iMsgID={}, iMsgLen={}", msg.iMsgID, msg.iMsgLen);
    // }

    // DoTradeOrderReq(gStStrategyMessageManager);

    // gStStrategyMessageManager.Init();
}