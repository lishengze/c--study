#include "lock_file_manager.h"
#include <sys/mman.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "share_comm_util.h"

int CreateLockFile(const char* cstrLockFileName) {
    int iFileFd = shm_open(cstrLockFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (iFileFd < 0)
    {
        LOG_ERROR("Create UTE lock file {} failed, errinfo [{}]", cstrLockFileName, strerror(errno));
        return -1;
    } 

    if (ftruncate(iFileFd, 4096) == -1) {
        LOG_ERROR("ftruncate cstrLockFileName:[{}] iFileFd:[{}] ",cstrLockFileName,  iFileFd);
        return -1;
    }


    lseek(iFileFd, 0, SEEK_SET);
    if (lockf(iFileFd, F_LOCK, 0) == -1) {
        LOG_ERROR("Lock cstrLockFileName:[{}] iFileFd:[{}] ",cstrLockFileName,  iFileFd);
        close(iFileFd);
        shm_unlink(cstrLockFileName);
        return -1;
    }

    return iFileFd;
}

namespace share_common 
{

bool LockFileManager::Init(const char* cstrUteName, unsigned long long ulStrategyKey, 
           StrategyGetRspCallbackEventFuncType StrategyOnEventFunc,
           long lStartSec,  
           int iEventSleepSec, int iWaitUteSec) {

    StrategyOnEventFunc_  = StrategyOnEventFunc;

    iHeartBeatSec_ = iEventSleepSec;

    if (!StrategyOnEventFunc_ ) {
        // todo 增加日志信息;
        LOG_ERROR("Init: StrategyOnEventFunc_ is NULL");
        return false;
    }    
    
    // 创建策略进程本身的心跳锁文件;
    std::string strStrategyLockFile = GetLockFileName(ulStrategyKey);
    int iStrategyFd = CreateLockFile(strStrategyLockFile.c_str());
    if (iStrategyFd < 0)
    {
        return false;
    } else {
        LOG_INFO("Create Strategy lock file: {} SUCCESS", strStrategyLockFile);
    }
    mapNonListenLockFileFd_[std::to_string(ulStrategyKey)] = iStrategyFd;

    LOG_INFO("Init Strategy Lock File:[{}]  SUCESS", GetLockFileName(ulStrategyKey));

    bool isUteReady = false;
    int iUteFd = -1;

    std::string strUteName = GetLockFileName(cstrUteName);
    do {

        LOG_DEBUG("Start Waiting UTE LockFile:[{}]", strUteName);

        if (iUteFd < 0) {
            iUteFd = shm_open(strUteName.c_str(), O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
        }       
        if (iUteFd > 0) {            
            bool opposite_alive = (lockf(iUteFd, F_TEST, 0) != 0);
            if (opposite_alive) {
                LOG_INFO("UTE Lock file {} already exists and locked, opposite process is alive", strUteName);
                isUteReady = true;
                break; // UTE进程已启动
            }
        }     
        
        sleep(1);

        if (SecTime() - lStartSec > iWaitUteSec) {
            return false;
        }

    } while(!isUteReady);

    LOG_INFO("Check UTE lock file [{}] , fd: {} SUCCESS", strUteName, iUteFd);

    {
        std::lock_guard<std::mutex> lock(mtxHeartbeat_);
        mapListenLockFileFd_[strUteName] = iUteFd; // 监听 UTE 进程的锁文件描述符;
    }      

    return true;
}


/// @brief UTE进程端初始化接口;
/// 1、创建 UTE进程对应的锁文件，并且加锁；
/// @param cstrUteName UTE进程名，也是锁文件名；
/// @param pUteMessageManager UTE进程的消息管理器；
/// @param iEventSleepSec 事件监听线程的睡眠时间间隔，默认为5秒；
bool LockFileManager::Init(const char* cstrUteName,  UteGetStrategyReqCallBackFuncType UteOnEventFunc, int iEventSleepSec) {
    LOG_INFO("Init UTE lock file:[{}], iEventSleepSec:{} Starting!", cstrUteName, iEventSleepSec);

    UteOnEventFunc_ = UteOnEventFunc;

    if (!UteOnEventFunc) {
        LOG_ERROR("Init: UteOnEventFunc is NULL");
        return false;
    }

    iHeartBeatSec_ = iEventSleepSec;

    int iUteFd = CreateLockFile(cstrUteName);
    if (iUteFd < 0)
    {
        LOG_ERROR("Create UTE lock file {} failed, iUteFd [{}]", cstrUteName, iUteFd);
        return false;
    } 

    LOG_INFO("Create UTE lock file:[{}] SUCCESS!", cstrUteName);

    mapNonListenLockFileFd_[cstrUteName] = iUteFd;
    
    StartHeartbeatThread();

    return true;
}            


unsigned int LockFileManager::GetSetStrategyBatchID(unsigned int StrategySysID) {
    std::string strBatchIDFileName = std::to_string(StrategySysID) + ".batchid";
    std::ifstream iFile(strBatchIDFileName);

    unsigned int result = 1;

    unsigned int batchID = 1;


    if (iFile.good()) {
        // 若是文件存在，读取batchID；
        if (!iFile.is_open()) {
            LOG_ERROR("open strategy batchid file failed:{}", strBatchIDFileName);
            return 0;
        }
        
        if (!(iFile >> batchID)) {
            LOG_ERROR("read strategy batchid file failed:{}", strBatchIDFileName);
            return 0;
        }

        iFile.close();

        result = batchID; // 读取到的batchID；

        
    }

    batchID++; // 加1后写入新的batchID；

    // 写入新的batchID；    
    std::ofstream oFile(strBatchIDFileName);
    if (!oFile.is_open()) {
        LOG_ERROR("write strategy batchid file failed:{}", strBatchIDFileName);
        return 0;
    }
    oFile << batchID;
    oFile.close();
    
    return result;
}

void LockFileManager::StartHeartbeatThread() {
    LOG_INFO("StartHeartbeatThread Starting, mapListenLockFileFd_ size: {}, iHeartBeatSec_: {}", mapListenLockFileFd_.size(), iHeartBeatSec_);

    shptrHearbeatThread_ = std::make_shared<std::thread>([this]() {
        while (bIsRunning_) {
            {
                std::lock_guard<std::mutex> lock(mtxHeartbeat_);
                std::vector<std::string> vecInvalidLockFileNames;
                vecInvalidLockFileNames.reserve(mapListenLockFileFd_.size());
                for (auto& iter:mapListenLockFileFd_) {
                    if (!test_lock_file_is_alive(iter.second)) {
                        LOG_WARN("Lock file [{}], fd: [{}] is dead, remove it from listen list", iter.first, iter.second);
                        close(iter.second);
                        
                        // 这两个指针在 Init 时已经判空过，这里不用再判空； //todo 暂时有问题;
                        if (StrategyOnEventFunc_) {
                            StrategyOnEventFunc_(kUteFailed, (iter.first + + " is Dead!").c_str());
                        } else if (UteOnEventFunc_) {
                            UteOnEventFunc_(kUteFailed, (iter.first + " is Dead!").c_str(), std::stoull(iter.first)); // 检测到 某个策略进程终止;
                        }
                        
                        vecInvalidLockFileNames.push_back(iter.first);
                    } else {
                        LOG_DEBUG("LockFile [{}], fd: [{}] is still alive!", iter.first, iter.second);
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
        LOG_ERROR("AddListenLockFile: [{}], [{}] OPEN FAILED", ulFileKey, iFd);
        return false;
    } 

    if (lockf(iFd, F_TEST, 0) == 0) {
        LOG_ERROR("AddListenLockFile: [{}], [{}] LOCK FAILED", ulFileKey, iFd);
        return false;
    }

    LOG_INFO("AddListenLockFile: [{}], [{}] SUCCESS", ulFileKey, iFd);

    std::lock_guard<std::mutex> lock(mtxHeartbeat_);
    mapListenLockFileFd_[std::to_string(ulFileKey)] = iFd;

    return true;
}

}