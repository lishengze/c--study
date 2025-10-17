#include "lock_file_manager.h"
#include "strategy_message_manager.h"
#include <sys/mman.h>
#include <chrono>
#include <thread>

bool LockFileManager::Init(const char* cstrUteName, const char* cstrStrategyLockFile, 
            int iEventSleepSec, StrategyMessageManager* strategy_message_manager) {
    pStrategyMessageManager_ = strategy_message_manager;
    iHeartBeatSec_ = iEventSleepSec;
    
    do {
       bool opposite_alive = false;
       iOppositeFd_ = shm_open(cstrUteName, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

       if (iOppositeFd_ > 0) {
           opposite_alive = (lockf(iOppositeFd_, F_TEST, 0) != 0);
           if (opposite_alive) {
                break; // UTE进程已启动
           } else {
                std::this_thread::sleep_for(std::chrono::seconds(iWaitUteSec_));
           }
       }       
    } while(iOppositeFd_ < 0);

    iMyFd_ = shm_open(cstrStrategyLockFile, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (iMyFd_ < 0)
    {
        // #ifdef APP_USE_ADK_LOG
        // ADK_LOG_ERROR_TF(100003, "Init info", "{1} iFile open failed, errinfo [{2}]", name + PRODUCER_LOCK_POSTFIX, strerror(errno));
        // #else
        // LOG_ERROR("{} iFile open failed, errinfo [{}]", (name + PRODUCER_LOCK_POSTFIX), strerror(errno));
        // #endif
        // todo 增加日志信息;
        return false;
    }        
    

 

    StartHeartbeatThread();
}

unsigned int LockFileManager::GetSetBatchID(unsigned int StrategySysID) {
    std::string strBatchIDFileName = std::to_string(StrategySysID) + "_heartbeat.lck";
    std::ifstream iFile(strBatchIDFileName);
    if (!iFile.is_open()) {
        // todo 增加日志信息;
        return 0;
    }
    unsigned int batchID = 0;
    if (!(iFile >> batchID)) {
        // todo 增加日志信息;
        return 0;
    }
    iFile.close();


    batchID++;
    std::ofstream oFile(strBatchIDFileName);
    if (!oFile.is_open()) {
        // todo 增加日志信息;
        return false;
    }
    oFile << batchID;
    oFile.close();
    
    return batchID-1;
}

void LockFileManager::StartHeartbeatThread() {
    shptrHearbeatThread_ = std::make_shared<std::thread>([this]() {
        while (true) {
            if (!test_lock_file_is_alive(iOppositeFd_)) {
                // todo 增加日志信息;
                if (!pStrategyMessageManager_ || !pStrategyMessageManager_->m_pfnOnEvent) {
                    pStrategyMessageManager_->m_pfnOnEvent(kUteFailed, (strOppositeLockFileName_+" not locked").c_str());
                } else {
                    // todo 增加日志信息;
                }
                break;
            }
            sleep(iHeartBeatSec_);
        }
    });
}