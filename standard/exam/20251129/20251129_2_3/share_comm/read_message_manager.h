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

class ReadMessageManager {
public:
    ReadMessageManager() {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_iEventSleepSec = 0;
        m_iWaitWriteSec = 0;
        m_bRspQueueInit = false;
        memset(m_WriteName, 0, sizeof(m_WriteName));

        std::string loggerName = "read_message_manager_"+ SecTimeStr("%Y%m%d") + ".log";
        logger::init(loggerName);
        logger::set_level(spdlog::level::debug);
    }

    /// @brief 设置失败事件回调函数, 当前用于告知Read进程, Write进程未初始化或突然终结;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, OnEvent pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; m_iEventSleepSec = iEventSleepSec;}

    /// @brief 设置消息回调函数, Write进程向Read进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(OnMessage pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }

    /// @param ReadSysID Read进程的系统ID;
    /// @param WriteName Write进程的系统名称;
    /// @param iWaitWriteSec 等待 UTE启动和创建好用于回报的共享内存的超时时间;
    bool Init(const char* WriteName, unsigned int ReadSysID, int iWaitWriteSec);

    /// @brief Read进程发送消息给Write进程;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 用于;
    bool SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen);

    /// @brief Read进程从队列中获取消息;
    /// @param iMsgID 消息ID;
    bool TryPopMsg(WriteMsg& msg);

    // 客户登录后，UTE端收到请求便开始创建用于回报的共享内存以及相关队列， Read端这开始 attach 这块共享内存，直到超时或是成功;
    // 若是成功，将 m_bRspQueueInit 设置为 true;
    // 若是超时，通过 m_pfnOnEvent 通知 Read；
    // lStartWaitSec:起始时间;
    bool StartWaitWriteQueue(long lStartWaitSec);
    

    /// @brief 初始化发送请求到UTE的共享内存队列;
    /// @param cstrWriteSysName  -- 请求内存的共享内存名;
    /// @return 
    bool InitReqQueue(const char* cstrWriteSysName);

    /// @brief 处理UTE终止消息的句柄；
    ///        1、将 m_bRspQueueInit 设置为 false -- 表示无法再发送请求;
    ///        2、通过  m_pfnOnEvent 通知 Read客户;
    /// @param iErrCode 
    /// @param pErrDesc 
    void ProcessWriteEnd(int iErrCode, const char* pErrDesc);


    
private:

    /// 外部设置的参数;

    unsigned int  m_iEventSleepSec;             // 检测Write进程是否正常运行的心跳间隔时间, 单位秒;
    unsigned int  m_iWaitWriteSec;                // 等待Write进程启动时间;

    char m_WriteName[32];                         // Write进程的系统ID,是字符串形式;
    unsigned long long m_ReadKey;           // Read进程的系统ID, 由ReadID+批次号组成;

    QueueManager m_ReqQueueManager; // 请求队列管理器;
    
    QueueManager m_RspQueueManager; // 响应队列管理器;

    LockFileManager m_LockFileManager; // 锁文件管理器;

    OnMessage m_pfnOnMessage;         // 消息回调函数,Read进程 收到 Write进程发送消息的接口;
    OnEvent   m_pfnOnEvent;           // 事件回调函数,告知Read进程, Write进程是否正常运行;        

    bool m_bRspQueueInit;              // Read端是否已经登录成功 - 用于回报的共享内存已经被UTE创建好,并且已经被Read端 Attach成功;    
};  

}