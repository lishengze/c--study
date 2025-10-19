#pragma once

#include <fcntl.h>
#include <thread>
#include <memory>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <unordered_map>
#include <mutex>
#include "logger.h"

namespace share_common 
{

class StrategyMessageManager;
class UteMessageManager;

inline bool test_lock_file_is_alive(int file_fd)
{
    auto ret = lockf(file_fd, F_TEST, 0);
    if (ret == -1 && (errno == EACCES || errno == EAGAIN))
    {
        return true;
    }
    else
    {
        // todo 增加日志信息;
        // #ifdef APP_USE_ADK_LOG
        // ADK_LOG_ERROR_TF(100003, "Init info", "Consumer may be exited.");
        // #else
        // LOG_ERROR("Consumer may be exited.");
        // #endif
        // is_running_ = false;
        return false;
    }
}

class LockFileManager {
public:
    LockFileManager():shptrHearbeatThread_{nullptr}, pUteMessageManager_{nullptr}, 
        pStrategyMessageManager_{nullptr},iHeartBeatSec_{1},iWaitUteSec_{1} {}
    ~LockFileManager() {

        if (shptrHearbeatThread_ && shptrHearbeatThread_->joinable()) {
            shptrHearbeatThread_->join();
        }
        
        // iMyFd_ > 0 ? close(iMyFd_) : 0;    
        // todo 是否需要删除 策略进程对应的锁文件；
    }


    /// @brief 策略进程端初始化接口;
    /// 1、判断 cstrUteName 对应的锁文件是否存在并且加锁，若是失败，则循环式的继续判断；
    /// 2、若是上面判断成功，创建策略进程对应的锁文件，并且加锁；
    /// 3、之后便开启心跳线程，每隔一段时间，检查UTE锁文件是否存在并加锁;
    /// @param cstrUteName UTE进程名，也是锁文件名；
    /// @param cstrStrategyLockFile 策略进程名，也是锁文件名；
    /// @param pStrategyMessageManager 策略进程的消息管理器；
    /// @param iEventSleepSec 事件监听线程的睡眠时间间隔，默认为5秒；
    /// @return 是否成功初始化
    bool Init(const char* cstrUteName, unsigned long long ulStrategyKey, 
              StrategyMessageManager* pStrategyMessageManager, int iEventSleepSec = 5);

    /// @brief UTE进程端初始化接口;
    /// 1、创建 UTE进程对应的锁文件，并且加锁；
    /// @param cstrUteName UTE进程名，也是锁文件名；
    /// @param pUteMessageManager UTE进程的消息管理器；
    /// @param iEventSleepSec 事件监听线程的睡眠时间间隔，默认为5秒；
    bool Init(const char* cstrUteName,  UteMessageManager* pUteMessageManager, int iEventSleepSec = 5);              

    // 通过本地以策略ID为名的文件，获取对应的上次的BatchID；
    unsigned int GetSetStrategyBatchID(unsigned int StrategySysID);

    void StartHeartbeatThread() ;

    /// @brief 添加需要监听的锁文件，当锁文件不存在或者加锁失败时，会触发事件；
    /// @param cstrLockFileName 锁文件名；
    /// @return 成功返回true，失败返回false；
    bool AddListenLockFile(unsigned long long ulFileKey);

private:

    std::unordered_map<std::string, int> mapListenLockFileFd_;  // 定时监听的锁文件描述符与对应的锁文件名的映射;
    std::unordered_map<std::string, int> mapNonListenLockFileFd_;  // 非监听的锁文件描述符与对应的锁文件名的映射;

    
    int  iHeartBeatSec_;          // 心跳时间间隔;

    std::shared_ptr<std::thread> shptrHearbeatThread_;   // 监听锁文件的心跳线程;
    std::mutex mtxHeartbeat_;  // 心跳线程的互斥锁，用于保护 mapListenLockFileFd_ ；

    StrategyMessageManager*   pStrategyMessageManager_;  // 策略进程的消息管理器,心跳线程需要用到;
    UteMessageManager*        pUteMessageManager_;       // UTE进程的消息管理器,心跳线程需要用到;

    int  iWaitUteSec_;      // 等待UTE启动时间间隔 - 策略进程专用;    
};

inline std::string GetLockFileName(unsigned long long ulFileKey) {
    return std::to_string(ulFileKey) + ".lck";
}

inline std::string GetLockFileName(const char* cstrFileKey) {
    return std::string(cstrFileKey) + ".lck";
}


} // namespace share_common