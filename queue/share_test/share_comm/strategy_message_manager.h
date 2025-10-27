/*
版本信息: 1.1.0
更新时间: 2025-10-15
更新内容: 1. UTESysID 由 UINT64 改为 字符串 类型;
        2. StrategySysID 由 UINT64 改为 UINT32 类型;
*/

#pragma once
#include <cstring>
#include <thread>
#include <memory>

#include "queue_manager.h"
#include "lock_file_manager.h"
#include "logger.h"
#include "share_comm_util.h"

using namespace tech;

namespace share_common 
{


typedef void (*OnEvent)(int iErrCode, const char* pErrDesc);
typedef void (*OnMessage)(int iMsgID, const char* pMsgBuf, const int iMsgLen);

class StrategyMessageManager {
public:
    StrategyMessageManager() {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_iEventSleepSec = 0;
        m_iWaitUteSec = 0;
        m_bUteInit = false;
        m_bRspQueueInit = false;
        m_bStartWaitUteRspQueue =  false;
        memset(m_UteName, 0, sizeof(m_UteName));

        std::string loggerName = "startegy_message_manager_"+ SecTimeStr("%Y%m%d") + ".log";
        logger::init(loggerName);
        logger::set_level(spdlog::level::debug);
    }

    /// @brief 设置事件回调函数, 告知策略进程, UTE进程是否正常运行;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, OnEvent pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; m_iEventSleepSec = iEventSleepSec;}

    /// @brief 设置消息回调函数, UTE进程向策略进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(OnMessage pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }

    /// @brief 初始化消息管理器, 设置策略进程的系统ID和UTE进程的系统ID;
    ///        在Init 会校验, OnMessage,OnEvent 是否设置;
    ///        如果未设置, 会返回false;
    ///        所以需要在调用 Init 之前调用 SetOnMessage, SetOnEvent;
    ///        Init 过程中，会通过阻塞式的校验UTE锁文件是否创建并加锁来判断，UTE进程是否正常运行;
    ///        确认UTE进程正常运行后，会创建策略进程对应的锁文件，并加锁;
    ///        之后会启动心跳线程监听 UTE进程是否正常运行;
    /// @param StrategySysID 策略进程的系统ID;
    /// @param UteName UTE进程的系统名称;
    /// @param iWaitUteSec 等待UTE启动的时间和创建好用于回报的共享内存的时间;
    bool Init(const char* UteName, unsigned int StrategySysID, int iWaitUteSec);

    /// @brief 策略进程发送消息给UTE进程;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 未正确初始化;
    bool SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen);

    /// @brief 策略进程接收消息;
    /// @param iMsgID 消息ID;
    bool TryPopMsg(UteMsg& msg);

    // 客户登录后，UTE端收到请求便开始创建用于回报的共享内存以及相关队列， 策略端这开始 attach 这块共享内存，直到超时或是成功;
    // 若是成功，将 m_bRspQueueInit 设置为 true;
    // 若是超时，通过 m_pfnOnEvent 通知 策略；
    void StartWaitUte();
    
    void SetUteInit(bool bUteInit) {m_bUteInit = bUteInit;}

    unsigned int GetWaitUteSec() {return m_iWaitUteSec;}

    OnMessage m_pfnOnMessage;       // 消息回调函数,策略进程向UTE进程发送消息的接口;
    OnEvent   m_pfnOnEvent;           // 事件回调函数,告知策略进程, UTE进程是否正常运行;    
    
private:

    /// 外部设置的参数;

    unsigned int  m_iEventSleepSec;             // 检测UTE进程是否正常运行的间隔时间, 单位秒;
    unsigned int  m_iWaitUteSec;                 // 等待UTE进程启动时间;

    char m_UteName[32];                // UTE进程的系统ID,是字符串形式;
    unsigned long long m_StrategyKey;              // 策略进程的系统ID, 由策略ID+批次号组成;

    QueueManager m_ReqQueueManager; // 请求队列管理器;

    QueueManager m_RspQueueManager; // 响应队列管理器;

    LockFileManager m_LockFileManager; // 锁文件管理器;

    bool m_bUteInit;                   // UTE是否已经启动;
    bool m_bRspQueueInit;              // 策略端是否已经登录成功 - 用于回报的共享内存已经被UTE创建好,并且已经被策略端 Attach成功;    
    bool m_bStartWaitUteRspQueue;     // 是否已经开始等待 UTE 创建回报共享内存队列；
};  

}