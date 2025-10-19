#include "lock_file_manager.h"
#include "strategy_message_manager.h"
#include "ute_message_manager.h"
#include <sys/mman.h>
#include <chrono>
#include <thread>
#include <iostream>

namespace share_common 
{

bool LockFileManager::Init(const char* cstrUteName, unsigned long long ulStrategyKey, 
           StrategyMessageManager* pStrategyMessageManager, int iEventSleepSec) {
    pStrategyMessageManager_ = pStrategyMessageManager;
    iHeartBeatSec_ = iEventSleepSec;

    if (!pStrategyMessageManager_ || !pStrategyMessageManager_->m_pfnOnEvent) {
        // todo 增加日志信息;
        return false;
    }    
    
    int iUteFd = -1;
    std::string strUteName = GetLockFileName(cstrUteName);
    do {
       bool opposite_alive = false;
       iUteFd = shm_open(strUteName.c_str(), O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

       if (iUteFd > 0) {
           opposite_alive = (lockf(iUteFd, F_TEST, 0) != 0);
           if (opposite_alive) {
            // todo 增加日志信息;
                break; // UTE进程已启动
           } else {
                std::this_thread::sleep_for(std::chrono::seconds(iWaitUteSec_));
           }
       }       
    } while(iUteFd < 0);

    mapListenLockFileFd_[std::string(cstrUteName)] = iUteFd; // 监听 UTE 进程的锁文件描述符;

    std::string strStrategyLockFile = GetLockFileName(ulStrategyKey);

    int iStrategyFd = shm_open(strStrategyLockFile.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (iStrategyFd < 0)
    {
        // #ifdef APP_USE_ADK_LOG
        // ADK_LOG_ERROR_TF(100003, "Init info", "{1} iFile open failed, errinfo [{2}]", name + PRODUCER_LOCK_POSTFIX, strerror(errno));
        // #else
        // LOG_ERROR("{} iFile open failed, errinfo [{}]", (name + PRODUCER_LOCK_POSTFIX), strerror(errno));
        // #endif
        // todo 增加日志信息;
        return false;
    } else {
        // todo 增加日志信息;
    }
    mapNonListenLockFileFd_[std::to_string(ulStrategyKey)] = iStrategyFd;
    
    StartHeartbeatThread();

    return true;
}

bool LockFileManager::Init(const char* cstrUteName,  UteMessageManager* pUteMessageManager, int iEventSleepSec) {
    pUteMessageManager_ = pUteMessageManager;

    if (!pUteMessageManager || !pUteMessageManager->m_pfnOnEvent) {
        // todo 增加日志信息;
        return false;
    }

    iHeartBeatSec_ = iEventSleepSec;

    int iUteFd = shm_open(cstrUteName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (iUteFd < 0)
    {
        // #ifdef APP_USE_ADK_LOG
        // ADK_LOG_ERROR_TF(100003, "Init info", "{1} iFile open failed, errinfo [{2}]", name + PRODUCER_LOCK_POSTFIX, strerror(errno));
        // #else
        // LOG_ERROR("{} iFile open failed, errinfo [{}]", (name + PRODUCER_LOCK_POSTFIX), strerror(errno));
        // #endif
        // todo 增加日志信息;
        return false;
    } else {
        // todo 增加日志信息;
    }

    mapNonListenLockFileFd_[cstrUteName] = iUteFd;
    
    StartHeartbeatThread();

    return true;
}            


unsigned int LockFileManager::GetSetStrategyBatchID(unsigned int StrategySysID) {
    std::string strBatchIDFileName = std::to_string(StrategySysID) + ".batchid";
    std::ifstream iFile(strBatchIDFileName);

    unsigned int result = 0;

    unsigned int batchID = 0;


    if (iFile.good()) {
        // 若是文件存在，读取batchID；
        if (!iFile.is_open()) {
            // todo 增加日志信息;
            std::cout << "open file failed:" << StrategySysID<< std::endl;
            return 0;
        }
        
        if (!(iFile >> batchID)) {
            // todo 增加日志信息;
            std::cout << "read file failed:" << StrategySysID<< std::endl;
            return 0;
        }
        iFile.close();

        result = batchID; // 读取到的batchID；

        
    }

    batchID++; // 加1后写入新的batchID；

    // 写入新的batchID；    
    std::ofstream oFile(strBatchIDFileName);
    if (!oFile.is_open()) {
        // todo 增加日志信息;
        std::cout << "write file failed:" << StrategySysID<< std::endl;
        return 0;
    }
    oFile << batchID;
    oFile.close();
    
    return result;
}

void LockFileManager::StartHeartbeatThread() {
    shptrHearbeatThread_ = std::make_shared<std::thread>([this]() {
        while (true) {
            {
                std::lock_guard<std::mutex> lock(mtxHeartbeat_);
                std::vector<std::string> vecInvalidLockFileNames;
                vecInvalidLockFileNames.reserve(mapListenLockFileFd_.size());
                for (auto& iter:mapListenLockFileFd_) {
                    if (!test_lock_file_is_alive(iter.second)) {
                        // 这两个指针在 Init 时已经判空过，这里不用再判空；
                        if (pStrategyMessageManager_) {
                            pStrategyMessageManager_->m_pfnOnEvent(kUteFailed, iter.first.c_str()); // 检测到 UTE 进程终止;
                        } else if (pUteMessageManager_) {
                            pUteMessageManager_->m_pfnOnEvent(kUteFailed, iter.first.c_str(), std::stoull(iter.first)); // 检测到 某个策略进程终止;
                        }
                        
                        vecInvalidLockFileNames.push_back(iter.first);
                    }
                }

                // 移除无效的锁文件描述符 - 已经终止的策略进程;
                for (auto& strLockFileName:vecInvalidLockFileNames) {
                    mapListenLockFileFd_.erase(strLockFileName);
                }
            }

            sleep(iHeartBeatSec_);
        }
    });
}

bool LockFileManager::AddListenLockFile(unsigned long long ulFileKey) {

    // 策略进程的锁文件描述符，由策略进程创建，并由 UTE 进程监听；
    int iFd = shm_open(GetLockFileName(ulFileKey).c_str(), O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (iFd < 0)
    {
        // #ifdef APP_USE_ADK_LOG
        // ADK_LOG_ERROR_TF(100003, "Init info", "{1} iFile open failed, errinfo [{2}]", name + PRODUCER_LOCK_POSTFIX, strerror(errno));
        // #else
        // LOG_ERROR("{} iFile open failed, errinfo [{}]", (name + PRODUCER_LOCK_POSTFIX), strerror(errno));
        // #endif
        // todo 增加日志信息;
        return false;
    } else {
        // todo 增加日志信息;
    }

    std::lock_guard<std::mutex> lock(mtxHeartbeat_);
    mapListenLockFileFd_[std::to_string(ulFileKey)] = iFd;

    return true;
}

}