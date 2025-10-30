/*
版本信息: 1.1.0
更新时间: 2025-10-15
更新内容: 1. UTESysID 由 UINT64 改为 字符串 类型;
        2. StrategySysID 由 UINT64 改为 UINT32 类型;

版本信息: 1.2.0
更新时间: 2025年10月24日
更新内容: 1. Init 接口增加超时等待时间入参;
         2. 完善相关注释;
         
特殊说明：
1. 关于日志: 策略端需要使用自己的  logger.h 文件,否则会按照 StrategyMessageManager 构造函数中的名字独立创建日志文件,
            并且打日志的形式需要符合当前库的模式,否则可能报错。
2. 策略端的编译，只需要包含 当前目录的 多个头文件和相关的源文件即可;
*/

#pragma once
#include <cstring>
#include <thread>
#include <memory>

#include "queue_manager.h"
#include "lock_file_manager.h"
#include "logger.h"
#include "share_comm_util.h"
#include "share_comm_external_message.h"

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
        m_bRspQueueInit = false;
        memset(m_UteName, 0, sizeof(m_UteName));

        std::string loggerName = "startegy_message_manager_"+ SecTimeStr("%Y%m%d") + ".log";
        logger::init(loggerName);
        logger::set_level(spdlog::level::debug);
    }

    /// @brief 设置失败事件回调函数, 当前用于告知策略进程, UTE进程未初始化或突然终结;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, OnEvent pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; m_iEventSleepSec = iEventSleepSec;}

    /// @brief 设置消息回调函数, UTE进程向策略进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(OnMessage pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }

    /// @brief 初始化消息管理器, 设置策略进程的系统ID和UTE进程的系统ID;
    ///        1、校验, OnMessage,OnEvent 是否设置, 如果未设置, 会返回false;
    ///        2、创建策略进程对应的锁文件，并加锁 -- 用于UTE判断策略进程是否存活;
    ///        3、阻塞式的校验UTE锁文件是否创建并加锁来判断UTE进程是否启动, 若是校验超时,报错并 return false;
    ///        4、发送策略进程启动消息给UTE, 并开始阻塞式的等待UTE创建好用于回报的共享内存无锁队列, 若是超时，报错并 return false;
    ///        5、以上都成功,开启心跳线程监听 UTE 的心跳锁文件;
    /// @param StrategySysID 策略进程的系统ID;
    /// @param UteName UTE进程的系统名称;
    /// @param iWaitUteSec 等待 UTE启动和创建好用于回报的共享内存的超时时间;
    bool Init(const char* UteName, unsigned int StrategySysID, int iWaitUteSec);

    /// @brief 策略进程发送消息给UTE进程;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 用于;
    bool SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen);

    /// @brief 策略进程从队列中获取消息;
    /// @param iMsgID 消息ID;
    bool TryPopMsg(UteMsg& msg);

    // 客户登录后，UTE端收到请求便开始创建用于回报的共享内存以及相关队列， 策略端这开始 attach 这块共享内存，直到超时或是成功;
    // 若是成功，将 m_bRspQueueInit 设置为 true;
    // 若是超时，通过 m_pfnOnEvent 通知 策略；
    // lStartWaitSec:起始时间;
    bool StartWaitUteQueue(long lStartWaitSec);
    

    /// @brief 初始化发送请求到UTE的共享内存队列;
    /// @param cstrUTESysName  -- 请求内存的共享内存名;
    /// @return 
    bool InitReqQueue(const char* cstrUTESysName);

    /// @brief 处理UTE终止消息的句柄；
    ///        1、将 m_bRspQueueInit 设置为 false -- 表示无法再发送请求;
    ///        2、通过  m_pfnOnEvent 通知 策略客户;
    /// @param iErrCode 
    /// @param pErrDesc 
    void ProcessUteEnd(int iErrCode, const char* pErrDesc);


    
private:

    /// 外部设置的参数;

    unsigned int  m_iEventSleepSec;             // 检测UTE进程是否正常运行的心跳间隔时间, 单位秒;
    unsigned int  m_iWaitUteSec;                // 等待UTE进程启动时间;

    char m_UteName[32];                         // UTE进程的系统ID,是字符串形式;
    unsigned long long m_StrategyKey;           // 策略进程的系统ID, 由策略ID+批次号组成;

    QueueManager m_NonReqOrderQueueManager; // 请求队列管理器;
    QueueManager m_ReqOrderQueueManager; // 请求队列管理器;
    
    QueueManager m_RspQueueManager; // 响应队列管理器;

    LockFileManager m_LockFileManager; // 锁文件管理器;

    OnMessage m_pfnOnMessage;         // 消息回调函数,策略进程 收到 UTE进程发送消息的接口;
    OnEvent   m_pfnOnEvent;           // 事件回调函数,告知策略进程, UTE进程是否正常运行;        

    bool m_bRspQueueInit;              // 策略端是否已经登录成功 - 用于回报的共享内存已经被UTE创建好,并且已经被策略端 Attach成功;    
};  

}