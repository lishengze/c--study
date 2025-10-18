#pragma once

#include <fcntl.h>
#include <thread>
#include <memory>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <unordered_map>

class StrategyMessageManager;

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
    LockFileManager():shptrHearbeatThread_{nullptr}, iMyFd_{-1}, iOppositeFd_{-1}, 
        pStrategyMessageManager_{nullptr},iHeartBeatSec_{1},iWaitUteSec_{1} {}
    ~LockFileManager() {
        iMyFd_ > 0 ? close(iMyFd_) : 0;    
        // todo 是否需要删除 策略进程对应的锁文件；
    }


    /// @brief 判断 cstrUteName 对应的锁文件是否存在并且加锁，若是失败，则循环式的继续判断；
    ///        若是上面判断成功，创建策略进程对应的锁文件，并且加锁；
    ///        之后便开启心跳线程，每隔一段时间，检查UTE锁文件是否存在并加锁;
    /// @param cstrUteName UTE进程名，也是锁文件名；
    /// @param cstrStrategyLockFile 策略进程名，也是锁文件名；
    /// @return 
    bool Init(const char* cstrUteName, const char* cstrStrategyLockFile, 
              int iEventSleepSec, StrategyMessageManager* strategy_message_manager);

    // 通过本地以策略ID为名的文件，获取对应的上次的BatchID；
    unsigned int GetSetStrategyBatchID(unsigned int StrategySysID);

    void StartHeartbeatThread() ;

    /// @brief 添加需要监听的锁文件，当锁文件不存在或者加锁失败时，会触发事件；
    /// @param cstrLockFileName 锁文件名；
    /// @param bNeedCreate 是否需要创建锁文件，默认为false；
    /// @return 成功返回FD， 失败返回-1；
    int AddListenLockFile(const char* cstrLockFileName, bool bNeedCreate = false);




private:
    int  iMyFd_;                  // 策略进程对应的锁文件;
    int  iOppositeFd_;            // UTE进程对应的锁文件;

    std::unordered_map<std::string, int> mapListenLockFileFd_;  // 定时监听的锁文件描述符与对应的锁文件名的映射;

    int  iHeartBeatSec_;          // 心跳时间间隔;
    int  iWaitUteSec_;      // 等待UTE启动时间间隔;
    std::string strMyLockFileName_;  // 策略进程对应的锁文件名;
    std::string strOppositeLockFileName_;  // UTE进程对应的锁文件名;
    std::shared_ptr<std::thread> shptrHearbeatThread_;  // 检验UTE进程是否还在运行的线程;

    StrategyMessageManager*   pStrategyMessageManager_;  // 策略进程的消息管理器,心跳线程需要用到;
};