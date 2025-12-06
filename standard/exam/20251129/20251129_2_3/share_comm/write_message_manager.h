#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "queue_manager.h"
#include "lock_file_manager.h"
#include "share_comm_util.h"
#include "logger.h"
#include "singleton.h"
#include "share_comm_external_message.h"

using namespace tech;

namespace share_common 
{

using TcpInnnerFuncType = std::function<void()>;

/// @brief 用于管理Read进程和Write进程之间的消息通信;
/// 问题: 
/// 1. 是否要增加专门存储 stratey_id 和 batch_id 的结构体, 这两者合起来可以唯一确定一个Read进程;
/// 2. Write 进程向Read进程发送消息的接口, 是否需要增加参数, 用于指定发送给哪个Read进程;
/// 3. 
class WriteMessageManager {
public:
    WriteMessageManager() : m_WriteSysName(""), 
                          shptrConsumerThread_{nullptr}, bIsRunning_{true} {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_iHeartbeatSec = 1;
    }

    ~WriteMessageManager() {
        Reset();

    }

    /// @brief 设置事件回调函数, 告知Read进程, Write进程是否正常运行;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, WriteGetReadReqCallBackFuncType pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; m_iHeartbeatSec = iEventSleepSec;}

    /// @brief 设置消息回调函数, Write进程向Read进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(WriteGetReadReqCallBackFuncType pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }


    /// @brief 初始化消息管理器, 设置Read进程的系统ID和Write进程的系统ID;
    ///        在Init 会校验, OnMessage,OnEvent 是否设置;
    ///        如果未设置, 会返回false;
    ///        所以需要在调用 Init 之前调用 SetOnMessage, SetOnEvent;
    /// @param cstrWriteSysName Write进程的系统ID;
    /// 入参是否要增加共享队列相关参数？
    bool Init(const char* cstrWriteSysName);


    /// @brief 创建一个Read进程接收回报的共享内存队列管理器; 
    /// @param strReadKey Read进程的系统ID;
    QueueManager* CreateReadRspQueue(unsigned long long strReadKey);

    /// @brief 
    /// 按照配置比例监听 Read进程的请求队列和转发API请求到共享队列的线程;
    void StartListenQueue();

    void Reset();

    /// 外部设置的参数;
    WriteGetReadReqCallBackFuncType m_pfnOnMessage;       // 消息回调函数,通知UTE请求相关信息;
    WriteGetReadReqCallBackFuncType m_pfnOnEvent;           // 事件回调函数,告知Write进程, 某个Read进程是否正常运行;

private:


    std::string m_WriteSysName;       // Write进程的系统ID;


    LockFileManager m_LockFileManager; // 锁文件管理器;
    int m_iHeartbeatSec;

    QueueManager m_TickDataQueue;                           // Read Tick Data From Queue;

    std::unordered_map<unsigned long long, QueueManager*> m_mapRspQueue;     // Read进程接收回报的共享内存队列管理器

    std::shared_ptr<std::thread>    shptrConsumerThread_;           // Read进程对应的锁文件;
    bool bIsRunning_;
};  

} // namespace share_common