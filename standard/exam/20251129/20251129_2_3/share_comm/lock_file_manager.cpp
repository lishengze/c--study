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
        LOG_ERROR("Create Write lock file {} failed, errinfo [{}]", cstrLockFileName, strerror(errno));
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

bool LockFileManager::Init(const char* cstrWriteName, unsigned long long ulReadKey, 
           ReadGetRspCallbackEventFuncType ReadOnEventFunc,
           long lStartSec,  
           int iEventSleepSec, int iWaitWriteSec) {

    ReadOnEventFunc_  = ReadOnEventFunc;

    iHeartBeatSec_ = iEventSleepSec;

    if (!ReadOnEventFunc_ ) {
        // todo 增加日志信息;
        LOG_ERROR("Init: ReadOnEventFunc_ is NULL");
        return false;
    }    
    
    // 创建策略进程本身的心跳锁文件;
    std::string strReadLockFile = GetLockFileName(ulReadKey);
    int iReadFd = CreateLockFile(strReadLockFile.c_str());
    if (iReadFd < 0)
    {
        return false;
    } else {
        LOG_INFO("Create Read lock file: {} SUCCESS", strReadLockFile);
    }
    mapNonListenLockFileFd_[std::to_string(ulReadKey)] = iReadFd;

    LOG_INFO("Init Read Lock File:[{}]  SUCESS", GetLockFileName(ulReadKey));

    bool isWriteReady = false;
    int iWriteFd = -1;

    std::string strWriteName = GetLockFileName(cstrWriteName);
    do {

        LOG_DEBUG("Start Waiting Write LockFile:[{}]", strWriteName);

        if (iWriteFd < 0) {
            iWriteFd = shm_open(strWriteName.c_str(), O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
        }       
        if (iWriteFd > 0) {            
            bool opposite_alive = (lockf(iWriteFd, F_TEST, 0) != 0);
            if (opposite_alive) {
                LOG_INFO("Write Lock file {} already exists and locked, opposite process is alive", strWriteName);
                isWriteReady = true;
                break; // UTE进程已启动
            }
        }     
        
        sleep(1);

        if (SecTime() - lStartSec > iWaitWriteSec) {
            return false;
        }

    } while(!isWriteReady);

    LOG_INFO("Check Write lock file [{}] , fd: {} SUCCESS", strWriteName, iWriteFd);

    {
        std::lock_guard<std::mutex> lock(mtxHeartbeat_);
        mapListenLockFileFd_[strWriteName] = iWriteFd; // 监听 UWrite进程的锁文件描述符;
    }      

    return true;
}


/// @brief UTE进程端初始化接口;
/// 1、创建 UTE进程对应的锁文件，并且加锁；
/// @param cstrWriteName UTE进程名，也是锁文件名；
/// @param pWriteMessageManager UTE进程的消息管理器；
/// @param iEventSleepSec 事件监听线程的睡眠时间间隔，默认为5秒；
bool LockFileManager::Init(const char* cstrWriteName,  WriteGetReadReqCallBackFuncType WriteOnEventFunc, int iEventSleepSec) {
    LOG_INFO("Init Write lock file:[{}], iEventSleepSec:{} Starting!", cstrWriteName, iEventSleepSec);

    WriteOnEventFunc_ = WriteOnEventFunc;

    if (!WriteOnEventFunc) {
        LOG_ERROR("Init: WriteOnEventFunc is NULL");
        return false;
    }

    iHeartBeatSec_ = iEventSleepSec;

    int iWriteFd = CreateLockFile(cstrWriteName);
    if (iWriteFd < 0)
    {
        LOG_ERROR("Create Write lock file {} failed, iWriteFd [{}]", cstrWriteName, iWriteFd);
        return false;
    } 

    LOG_INFO("Create Write lock file:[{}] SUCCESS!", cstrWriteName);

    mapNonListenLockFileFd_[cstrWriteName] = iWriteFd;
    
    StartHeartbeatThread();

    return true;
}            


unsigned int LockFileManager::GetSetReadBatchID(unsigned int ReadSysID) {
    std::string strBatchIDFileName = std::to_string(ReadSysID) + ".batchid";
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
                        if (ReadOnEventFunc_) {
                            ReadOnEventFunc_(kWriteFailed, (iter.first + + " is Dead!").c_str());
                        } else if (WriteOnEventFunc_) {
                            WriteOnEventFunc_(kWriteFailed, (iter.first + " is Dead!").c_str(), std::stoull(iter.first)); // 检测到 某个策略进程终止;
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

    // 策略进程的锁文件描述符，由策略进程创建，并由 UTE 进程监听；Write
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