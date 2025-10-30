#include "ute_server.h"

#include "ute_message_manager.h"
#include "share_comm_external_message.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

#include <vector>
#include <algorithm>

#include "json_struct.h"

using namespace share_common;

int gTestCount = 10000;
int gTestIndex = 0;
int gTestMode = 0; // 0-正常测试；1-压力性能测试;
std::vector<unsigned long long> gTestTimeVec;

UteMessageManager gUteMessageManager;
// JsonStructHelper  JSON_HELPER;
std::string gStrSrcJsonFileName = "test_data.json";
JsonMeta          gJsonMeta;

void SendLogAns(QueueManager* pQueueManager) {
    LOG_INFO("SendLogAns");

    LogOnAns stLogOnAns;

    strcpy(stLogOnAns.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.branch_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.account_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.cust_id, "LogInSucess");

    // gUteMessageManager.

    pQueueManager->SendMsg(kPktLoginAns, (char*)&stLogOnAns, sizeof(LogOutAns));
}

void SendOrderRsp(QueueManager* pQueueManager) {
    LOG_INFO("SendOrderRsp");
    
    TradeOrderER stTradeOrderERP;

    strcpy(stTradeOrderERP.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.branch_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.account_id, "XXXXXX");
    strcpy(stTradeOrderERP.trade_order_user.cust_id, "XXXXXX");    

    pQueueManager->SendMsg(kPktOrderAns, (char*)&stTradeOrderERP, sizeof(TradeOrderER));
}

void UteOnEvent(int iMsgID, const char* pMsgBuf,  unsigned long long ulStrategyKey) {
    LOG_INFO("UteOnEvent: iMsgID={}, ulStrategyKey={}", iMsgID, ulStrategyKey);
}

void AnaTestResult(std::vector<unsigned long long>& vecTime) {
    if (vecTime.size() == 0) {
        LOG_DEBUG("Empty!");
        return;
    }
    unsigned long long ulMin = 0; 
    unsigned long long ulMax = 0;
    unsigned long long ulAve = 0;    
    std::sort(vecTime.begin(), vecTime.end());

    ulMin = vecTime[0];
    ulMax = vecTime[vecTime.size()-1];

    unsigned long long ul25 = vecTime[std::floor(vecTime.size()*25/100)];
    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];

    double dSum = 0;
    for (auto time:vecTime) {
        dSum += time;
    }
    double dAve = dSum / vecTime.size();
    
    std::string sDelayTimeInfo = "\n--------- dataCount: " + std::to_string(vecTime.size()) 
                        +  ", min=" + std::to_string(ulMin) + ", max=" + std::to_string(ulMax)
                        + ", 25%=" + std::to_string(ul25) + ", 50%=" + std::to_string(ul50) 
                        + ", 75%=" + std::to_string(ul75) + ", 90%=" + std::to_string(ul90)
                        + ", ave="+ std::to_string(dAve);

    LOG_DEBUG("{}", sDelayTimeInfo);
}

void UteOnMessage(int iMsgID, const char* pMsgBuf, unsigned long long ulStrategyKey)  {

    if (gTestMode == 0) {
        switch (iMsgID) {
            case kPktLoginReq:
            {
                LogOnReq* pStData = (LogOnReq*)(pMsgBuf);
                LOG_INFO("LogOnReq: {}", JSON_HELPER->LogOnReqStr(*pStData));
                QueueManager* pQueueManager = gUteMessageManager.GetStrategyRspQueue(ulStrategyKey);
                if (nullptr == pQueueManager) {
                    LOG_ERROR("StrategyKey:{}, Get QueueManager Failed");
                } else {
                    LogOnAns rsp;
                    JSON_HELPER->ParseLogOnAns(rsp, gStrSrcJsonFileName);
                    LOG_INFO("LogOnAns: {}", JSON_HELPER->LogOnAnsStr(rsp));
                    pQueueManager->SendMsg(kPktLoginAns, (char*)(&rsp), sizeof(LogOnAns), ulStrategyKey);
                }

                break;
            }

            case kPktLogoutReq:
            {
                LogOutReq* pStData = (LogOutReq*)(pMsgBuf);
                LOG_INFO("LogOutReq: {}", JSON_HELPER->LogOutReqStr(*pStData));
                QueueManager* pQueueManager = gUteMessageManager.GetStrategyRspQueue(ulStrategyKey);
                if (nullptr == pQueueManager) {
                    LOG_ERROR("StrategyKey:{}, Get QueueManager Failed");
                } else {
                    LogOutAns rsp;
                    JSON_HELPER->ParseLogOutAns(rsp, gStrSrcJsonFileName);
                    pQueueManager->SendMsg(kPktLogoutAns, (char*)(&rsp), sizeof(LogOutAns), ulStrategyKey);
                }
                                
                break;
            }

            case kPktOrderReq:
            {
                TradeOrderReq* pStData = (TradeOrderReq*)(pMsgBuf);
                LOG_INFO("TradeOrderReq: {}", JSON_HELPER->TradeOrderReqStr(*pStData));
                QueueManager* pQueueManager = gUteMessageManager.GetStrategyRspQueue(ulStrategyKey);
                if (nullptr == pQueueManager) {
                    LOG_ERROR("StrategyKey:{}, Get QueueManager Failed");
                } else {
                    TradeOrderER rsp;
                    JSON_HELPER->ParseTradeOrderER(rsp, gStrSrcJsonFileName, "OrderReqRsp");
                    pQueueManager->SendMsg(kPktOrderAns, (char*)(&rsp), sizeof(TradeOrderER), ulStrategyKey);
                }
                                
                break;
            }
            case kPktCancelOrderReq:
            {
                CancelOrderReq* pStData = (CancelOrderReq*)(pMsgBuf);
                LOG_INFO("CancelOrderReq: {}", JSON_HELPER->CancelOrderReqStr(*pStData));
                QueueManager* pQueueManager = gUteMessageManager.GetStrategyRspQueue(ulStrategyKey);
                if (nullptr == pQueueManager) {
                    LOG_ERROR("StrategyKey:{}, Get QueueManager Failed");
                } else {
                    TradeOrderER rsp;
                    JSON_HELPER->ParseTradeOrderER(rsp, gStrSrcJsonFileName, "CancelOrderReqRsp");
                    pQueueManager->SendMsg(kPktCancelOrderAns, (char*)(&rsp), sizeof(TradeOrderER), ulStrategyKey);
                }
                                
                break;
            }
            default:
                break;
        }   

    } else if (gTestMode == 1) {

        if (iMsgID == kPktLoginReq) {
            LogOnReq* pReq = (LogOnReq*)(pMsgBuf);

            LOG_DEBUG("CustID: {}", pReq->trade_order_user.cust_id);

            QueueManager* pQueueManager = gUteMessageManager.CreateStrategyRspQueue(ulStrategyKey);

            if (pQueueManager) {
                SendLogAns(pQueueManager);

                // sleep(3);

                // SendOrderRsp(pQueueManager);
            }
        } else if (iMsgID == kPktOrderReq ) {

            if (gTestIndex < gTestCount) {
                TradeOrderReq* pOrderReq = (TradeOrderReq*)pMsgBuf;
                // LOG_DEBUG("CUST:{} ", pOrderReq->trade_order_user.cust_id);

                unsigned long long ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();
                gTestTimeVec[gTestIndex++] = ulPopTime - pOrderReq->ulStrategyKey;

                if (gTestIndex == gTestCount) {
                    AnaTestResult(gTestTimeVec);
                    // exit(0);                
                }
            } 
        }
    }
    // LOG_DEBUG("iMsgID = {}, ulStrategyKey = {}", iMsgID, ulStrategyKey);
}


void UteOnInnerMessage(int iMsgID, const char* pMsgBuf, int iMsgLen, int iMsgSrcType,   void* pMsgHandler)  {
    LOG_DEBUG("iMsgID = {},  iMsgLen: = {}, iMsgSrcType = {}", iMsgID, iMsgLen, iMsgSrcType);

    if (iMsgID == kPktLoginAns) {
        LogOnAns* pLogOnAns = (LogOnAns*)pMsgBuf;
        LOG_DEBUG("From API Request CustID:{}", pLogOnAns->trade_order_user.cust_id);
    }

}

void ApiMessageHandler() {

}

void SendApiMessage() {
    LOG_INFO("SendLogAns");

    LogOnAns stLogOnAns;

    strcpy(stLogOnAns.trade_order_user.fund_account_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.branch_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.account_id, "XXXXXX");
    strcpy(stLogOnAns.trade_order_user.cust_id, "TestApiRequest");

    // gUteMessageManager.

    gUteMessageManager.WriteMsg(kPktLoginAns, (char*)&stLogOnAns, sizeof(LogOutAns), 99, (void*)(&ApiMessageHandler));    
}

void TestUteServer() {

        

    gTestMode = 0;


    gTestTimeVec.reserve(gTestCount);
    for (int i = 0; i<gTestCount; ++i) {
        gTestTimeVec.push_back(0);
    }
    
    std::string strUteName = "test_ute";

    int iEventSleepSec = 5;

    gUteMessageManager.SetOnEvent(iEventSleepSec, UteOnEvent);
    gUteMessageManager.SetOnMessage(UteOnMessage);
    gUteMessageManager.SetOnInnerMessage(UteOnInnerMessage);

    gUteMessageManager.Init(strUteName.c_str(), 1, 1, 4000, 4000, 4000);


    // sleep(5);

    // SendApiMessage();

    // sleep(1);

    // SendApiMessage();    

}

template<typename T>
bool CreateShareQueue(const char * cstrSharedMemName , unsigned int uiQueueBlockCount_, mpmc_queue<T>* & pMpmcQueue, element_slot<T, false>*&  pMpmcShareSlots) {
    int shm_fd = shm_open(cstrSharedMemName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    
    unsigned int uiElementSlotBlocksSize = (tech::roundup_pow_of_two(uiQueueBlockCount_) + 2) * sizeof( element_slot<UteMsg, false>) ;

    // 设置共享内存大小
    int uiMemorySize_ = sizeof(mpmc_queue<UteMsg>) + uiElementSlotBlocksSize + 1024; // 给共享内存留足够的空间;
    if (ftruncate(shm_fd, uiMemorySize_) == -1) {
        LOG_ERROR("ftruncate {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    void* addr = mmap(NULL, uiMemorySize_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_ERROR("mmap {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }

    // LOG_DEBUG("pMpmcQueue create uiMemorySize_: {}", uiMemorySize_);
    
    // 在共享内存中构造队列对象
    pMpmcQueue = new (addr) mpmc_queue<T>();

    if (!pMpmcQueue->create_shared(uiQueueBlockCount_, pMpmcShareSlots, static_cast<void*>((char*)addr + sizeof(mpmc_queue<UteMsg>) + 128))) { // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；
        LOG_ERROR("queue create_shared  failed");
        return false;
    }
    close(shm_fd);

    return true;
}


void TestUteTime() {
    gTestMode =  1;
    const char * cstrSharedMemName = "test_time.queue";
    unsigned int uiQueueBlockCount = 4000;

    mpmc_queue<UteMsg>* pMpmcQueue = nullptr;
    element_slot<UteMsg, false>*  pMpmcShareSlots = nullptr;

    if (!CreateShareQueue<UteMsg>(cstrSharedMemName, uiQueueBlockCount, pMpmcQueue, pMpmcShareSlots)) {
        LOG_ERROR("Create cstrSharedMemName:{}, uiQueueBlockCount: {}, Failed!", cstrSharedMemName, uiQueueBlockCount);
        return;
    }

    gTestTimeVec.reserve(gTestCount);
    for (int i = 0; i<gTestCount; ++i) {
        gTestTimeVec.push_back(0);
    }    

    LOG_INFO("------- Start Test Struct Size: {}, TradeOrderReq.size: {}", sizeof(UteMsg), sizeof(TradeOrderReq));

    UteMsg tmpObj;
    unsigned long long ulPoptime;
    for (int i = 0; i < gTestCount; ++i) {
        pMpmcQueue->pop_share(pMpmcShareSlots, tmpObj);

        ulPoptime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();

        gTestTimeVec[i] = ulPoptime - tmpObj.ulStrategyKey;
    }

    AnaTestResult(gTestTimeVec);
}

__attribute__((destructor)) __attribute__((used)) void after_main() {
    LOG_INFO("TEST OVER---------\n\n\n");
}

void TestUteMain() {
    const string strSrcJsonFileName = "test_data.json";
    JSON_HELPER->Init(strSrcJsonFileName);

    std::string strMetaInfo = gJsonMeta.Init("meta_data.json");
    LOG_INFO("MetaInfo:\n{}", strMetaInfo);    

    // TestUteTime();

    TestUteServer();
}