#include "strategy_client.h"

#include "strategy_message_manager.h"

#include "json_struct.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

#include <thread>
#include <mutex>
#include <iostream>


#include <thread>
#include <mutex>
#include <iostream>

#include <iostream>
#include <cstdio>
#include <termios.h>  // 用于修改终端模式（关闭缓冲）
#include <unistd.h>

#include "json_struct.h"

using namespace share_common;

int gTestCount = 10000;

StrategyMessageManager gStStrategyMessageManager;
std::string gStrSrcJsonFileName = "test_data.json";
JsonStructHelper gJsonStructHelper;
JsonMeta gJsonMeta;


void StrategyOnEvent(int iErrCode, const char* pErrDesc) {
    LOG_WARN("StrategyOnEvent: iErrCode={}, pErrDesc={}", iErrCode, pErrDesc);
}

void StrategyOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    LOG_INFO("StrategyOnMessage: iMsgID={}, iMsgLen={}", iMsgID, iMsgLen);

    switch (iMsgID) {
        case kPktLoginAns:
        {
            LogOnAns* pStData = (LogOnAns*)(pMsgBuf);
            LOG_INFO("LogOnAns: {}", gJsonStructHelper.LogOnAnsStr(*pStData), gStrSrcJsonFileName);
            break;
        }

        case kPktLogoutAns:
        {
            LogOutAns* pStData = (LogOutAns*)(pMsgBuf);
            LOG_INFO("LogOnAns: {}", gJsonStructHelper.LogOutAnsStr(*pStData), gStrSrcJsonFileName);
            break;
        }

        case kPktOrderAns:
        {
            TradeOrderER* pStData = (TradeOrderER*)(pMsgBuf);
            LOG_INFO("TradeOrderER: {}", gJsonStructHelper.TradeOrderERStr(*pStData), gStrSrcJsonFileName);
            break;
        }
        case kPktCancelOrderAns:
        {
            TradeOrderER* pStData = (TradeOrderER*)(pMsgBuf);
            LOG_INFO("TradeOrderER: {}", gJsonStructHelper.TradeOrderERStr(*pStData), gStrSrcJsonFileName);
            break;
        }
        case kPktOrderMatch:
        {
            TradeOrderER* pStData = (TradeOrderER*)(pMsgBuf);
            LOG_INFO("TradeOrderER: {}", gJsonStructHelper.TradeOrderERStr(*pStData), gStrSrcJsonFileName);
            break;
        }
        case kPktRejectMsg:
        {
            RejectMsg* pStData = (RejectMsg*)(pMsgBuf);
            LOG_INFO("LogOnAns: {}", gJsonStructHelper.RejectMsgStr(*pStData), gStrSrcJsonFileName);
            break;
        }
        default:
            break;
    }    

}

// 举例进行登录请求设置
void DoLoginReq(StrategyMessageManager& stragegyMsgManager) {
    // LOG_INFO("DoLoginReq");

    LogOnReq req = {0};
    req.heart_bt_int = 3;
    req.trade_order_user.agw_seq_id = 10001;
    // strcpy(req.password, "XXXXXX");
    // strcpy(req.trade_order_user.fund_account_id, "XXXXXX");
    // strcpy(req.trade_order_user.branch_id, "XXXXXX");
    // strcpy(req.trade_order_user.account_id, "XXXXXX");
    // strcpy(req.trade_order_user.cust_id, "TestLogin");
    req.trade_order_user.client_seq_id = 1;

    stragegyMsgManager.SendMsg(kPktLoginReq, (char*)&req, sizeof(req));
}

void DoTradeOrderReq(StrategyMessageManager& stragegyMsgManager) {
    // LOG_INFO("DoTradeOrderReq");
    TradeOrderReq req;
    // strcpy(req.trade_order_user.fund_account_id, "Client");
    // strcpy(req.trade_order_user.branch_id, "XXXXXX");
    // strcpy(req.trade_order_user.account_id, "XXXXXX");
    // strcpy(req.trade_order_user.cust_id, "TestOrderTime");
    req.trade_order_user.client_seq_id = 10001;
    req.trade_order_info.order_qty = 800;
    req.trade_order_info.side = kBuy;
    req.trade_order_info.order_type = kLimited;
    req.trade_order_info.market_id = kShangHai;
    // strcpy(req.trade_order_info.security_id, "XXXXXX");
  
    req.ulStrategyKey = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();

    stragegyMsgManager.SendMsg(kPktOrderReq, (char*)&req, sizeof(req));
}

void TestStrategyClient() {
    StrategyMessageManager stStrategyMessageManager;

    std::string strUteName = "test_ute";
    unsigned int uiStrategySysID = 1;
    int iSleepSec = 5;

    stStrategyMessageManager.SetOnEvent(iSleepSec, StrategyOnEvent);
    stStrategyMessageManager.SetOnMessage(StrategyOnMessage);

    stStrategyMessageManager.Init(strUteName.c_str(), uiStrategySysID, 100);

    sleep(8);


    for (int i = 0; i < gTestCount; ++i) {
        DoTradeOrderReq(stStrategyMessageManager);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }


    // sleep(20);

    // DoTradeOrderReq(stStrategyMessageManager);

    // while(true) {
    //     sleep(3);
    // }

    // UteMsg testPopMsg;
    // if (stStrategyMessageManager.TryPopMsg(testPopMsg)) {
    //     LOG_DEBUG("[SUCCESS] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // } else {
    //     LOG_ERROR("[FAILED] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // }

    // sleep(1);

    // UteMsg msg;
    // while (stStrategyMessageManager.TryPopMsg(msg)) {
    //     LOG_INFO("TryPopMsg: iMsgID={}, iMsgLen={}", msg.iMsgID, msg.iMsgLen);
    // }

    // DoTradeOrderReq(stStrategyMessageManager);

    // stStrategyMessageManager.Init();
}

template<typename T>
bool AttachShareQueue(const char * cstrSharedMemName , mpmc_queue<T>* & pMpmcQueue, element_slot<T, false>*&  pMpmcShareSlots) {
    int shm_fd = shm_open(cstrSharedMemName, O_RDWR, 0);
    if (shm_fd == -1) {
        LOG_WARN("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    
    // 获取共享内存大小
    struct stat stat_buf;
    if (fstat(shm_fd, &stat_buf) == -1) {
        LOG_WARN("fstat {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    int uiMemorySize_ = stat_buf.st_size; // 记录共享内存大小
    void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_WARN("mmap {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }

    // LOG_DEBUG("pUteMsgMpmcQueue_ attach uiMemorySize_: {}", uiMemorySize_);
    
    close(shm_fd);
    pMpmcQueue = static_cast<mpmc_queue<UteMsg>*>(addr);   

    if (!pMpmcQueue) {
        LOG_WARN("pUteMsgMpmcQueue_ is null");
        return false;
    }

    
     // 手动将slot 映射到外部的内存地址中 -- 共享内存版本,这一步导致了很多的问题，导致无法进行服务端对slot 的解锁出错了。
    pMpmcQueue->slot_attach(pMpmcShareSlots, static_cast<void*>((char*)addr + sizeof(mpmc_queue<UteMsg>) + 128));

    return true;

}

void TestStrategyTime() {
    const char * cstrSharedMemName = "test_time.queue";

    mpmc_queue<UteMsg>* pMpmcQueue = nullptr;
    element_slot<UteMsg, false>*  pMpmcShareSlots = nullptr;

    if (!AttachShareQueue<UteMsg>(cstrSharedMemName, pMpmcQueue, pMpmcShareSlots)) {
        LOG_ERROR("Attach cstrSharedMemName:{}, Failed!", cstrSharedMemName);
        return;
    }

    int iMsgLen = sizeof(TradeOrderReq);
    TradeOrderReq tmpObj;
    unsigned long long ulPushTime;
    for (int i = 0; i < gTestCount; ++i) {
        ulPushTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();
        pMpmcQueue->push_share(pMpmcShareSlots, kPktOrderReq, iMsgLen, ulPushTime,(char*)(&tmpObj));
        usleep(1);
    }
}


// 举例进行登录请求设置
void JsonDoLoginReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoLoginReq");

    LogOnReq req = {0};

    gJsonStructHelper.ParseLogOnReq(req, gStrSrcJsonFileName);

    LOG_INFO("LogOnReq: {}",  gJsonStructHelper.LogOnReqStr(req), gStrSrcJsonFileName); 

    stragegyMsgManager.SendMsg(kPktLoginReq, (char*)&req, sizeof(req));
}

// 举例进行注销请求设置
void JsonDoLogoutReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoLogoutReq");
    LogOutReq req = {0};
    gJsonStructHelper.ParseLogOutReq(req, gStrSrcJsonFileName); 

    LOG_INFO("LogOutReq: {}" , gJsonStructHelper.LogOutReqStr(req), gStrSrcJsonFileName); 

    stragegyMsgManager.SendMsg(kPktLogoutReq, (char*)&req, sizeof(req));
}

// 举例进行订单请求设置
void JsonDoTradeOrderReq(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoTradeOrderReq");
    TradeOrderReq req;
    gJsonStructHelper.ParseTradeOrderReq(req, gStrSrcJsonFileName);

    LOG_INFO("DoTradeOrderReq: {}" , gJsonStructHelper.TradeOrderReqStr(req), gStrSrcJsonFileName); 
  
    stragegyMsgManager.SendMsg(kPktOrderReq, (char*)&req, sizeof(req));
}

// 举例进行取消订单请求设置
void JsonDoCancelOrder(StrategyMessageManager& stragegyMsgManager) {
    LOG_INFO("DoCancelOrder");
    CancelOrderReq req = {0};
    gJsonStructHelper.ParseCancelOrderReq(req, gStrSrcJsonFileName);  

    LOG_INFO("DoCancelOrder: {}" , gJsonStructHelper.CancelOrderReqStr(req), gStrSrcJsonFileName); 

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
    // disableInputBuffering(); // 关闭缓冲，即时读取

    LOG_INFO("Start Waiting Command");

    std::thread testThread([](){
        char input;
        while(true) {
            input = getchar();
            LOG_INFO("input char{}, int{}", input, (int)(input));

            switch (input) {
            case 'h':
                std::cout << GetHelpInfo() << std::endl;
                break;
            case 'q':
                exit(0);
                break;
            case '1':
                JsonDoLoginReq(gStStrategyMessageManager);
                break;
            case '2':
                JsonDoLogoutReq(gStStrategyMessageManager);
                break;
            case '3':
                JsonDoTradeOrderReq(gStStrategyMessageManager);
                break;
            case '4':
                JsonDoCancelOrder(gStStrategyMessageManager);
                break;
            default:
                break;
            }

            sleep(1);
        }

    });

    testThread.join();

    // restoreInputBuffering(); // 退出前恢复终端模式
}


void TestJsonStrategyClient() {

    std::string strUteName = "test_ute";
    unsigned int uiStrategySysID = 1;
    int iSleepSec = 3;

    gStStrategyMessageManager.SetOnEvent(gJsonMeta.uiHeartbeatSec_, StrategyOnEvent);
    gStStrategyMessageManager.SetOnMessage(StrategyOnMessage);
    gStStrategyMessageManager.Init(gJsonMeta.strUteName_.c_str(), gJsonMeta.uiStrategyKey_, gJsonMeta.uiWaitSec_);

    sleep(8);

    StartWaitReqCommand();
}

void TestStrategyMain() {
    const string strSrcJsonFileName = "test_data.json";
    gJsonStructHelper.Init(strSrcJsonFileName);    

    std::string strMetaInfo = gJsonMeta.Init("meta_data.json");
    LOG_INFO("MetaInfo:\n{}", strMetaInfo);

    // TestStrategyClient();

    // TestStrategyTime();

    TestJsonStrategyClient();

}