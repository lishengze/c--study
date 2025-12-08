#include "strategy_client.h"

#include "read_message_manager.h"

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
#include <cstdlib>   // rand()、srand()
#include <ctime>     // time()（获取系统时间作为种子）

#include "json_struct.h"

using namespace share_common;

int gTestCount = 10000;

ReadMessageManager gStReadMessageManager;
std::string gStrSrcJsonFileName = "test_data.json";
JsonMeta gJsonMeta;


void ReadOnEvent(int iErrCode, const char* pErrDesc) {
    LOG_WARN("ReadOnEvent: iErrCode={}, pErrDesc={}", iErrCode, pErrDesc);
}


void ReadOnMessage(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    LOG_INFO("StrategyOnMessage: iMsgID={}, iMsgLen={}", iMsgID, iMsgLen);

}




void TestJsonReadClient() {

    std::string strWriteName = "test_ute";
    unsigned int uiReadSysID = 1;
    int iSleepSec = 3;

    gStReadMessageManager.SetOnEvent(gJsonMeta.uiHeartbeatSec_, ReadOnEvent);
    gStReadMessageManager.SetOnMessage(ReadOnMessage);
    gStReadMessageManager.Init(gJsonMeta.strWriteName_.c_str(), gJsonMeta.uiReadKey_, gJsonMeta.uiWaitSec_);

    sleep(8);
    TickData tickData;

    srand((unsigned int)time(nullptr));

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        tickData.dTickTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();;
        tickData.dPrice = (rand() % 1000) / 100.0; 
        tickData.dVolume = rand() % 1000;
        tickData.dTurnover = tickData.dPrice * tickData.dVolume;
        strcpy(tickData.strSymbol, "AAPL");

        gStReadMessageManager.SendMsg(kTickData, (const char*)&tickData, sizeof(tickData));

    }



}

void TestReadMain() {    

    std::string strMetaInfo = gJsonMeta.Init("meta_data.json");
    LOG_INFO("MetaInfo:\n{}", strMetaInfo);

    // TestReadClient();

    // TestReadTime();

    TestJsonReadClient();


    while(true) {
        sleep(10);
    }    

}