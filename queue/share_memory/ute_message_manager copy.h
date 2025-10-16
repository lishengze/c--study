#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "req_queue.h"
#include "rsp_queue.h"

using std::vector;

typedef void (*OnEvent)(int iErrCode, const char* pErrDesc, const char* strStrategyKey);
typedef void (*OnMessage)(int iMsgID, const char* pMsgBuf, const char* strStrategyKey);

using UINT64 = unsigned long long;


/// @brief 用于管理策略进程和UTE进程之间的消息通信;
/// 问题: 
/// 1. 是否要增加专门存储 stratey_id 和 batch_id 的结构体, 这两者合起来可以唯一确定一个策略进程;
/// 2. UTE 进程向策略进程发送消息的接口, 是否需要增加参数, 用于指定发送给哪个策略进程;
/// 3. 
class UteMessageManager {
public:
    UteMessageManager() {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_UTESysName.clear();
    }

    /// @brief 设置事件回调函数, 告知策略进程, UTE进程是否正常运行;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, OnEvent pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; }

    /// @brief 设置消息回调函数, UTE进程向策略进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(OnMessage pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }

    /// @brief 初始化消息管理器, 设置策略进程的系统ID和UTE进程的系统ID;
    ///        在Init 会校验, OnMessage,OnEvent 是否设置;
    ///        如果未设置, 会返回false;
    ///        所以需要在调用 Init 之前调用 SetOnMessage, SetOnEvent;
    /// @param StrategySysID 策略进程的系统ID;
    /// @param UTESysID UTE进程的系统ID;
    /// 入参是否要增加共享队列相关参数？
    bool Init(UINT64 UTESysID);

    RspQueueManager* CreateRspQueueManager(const char* strStrategyKey);


    /// @brief 策略进程发送消息给UTE进程;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 未正确初始化;
    bool SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen);

private:

    /// 外部设置的参数;
    OnMessage m_pfnOnMessage;       // 消息回调函数,通知UTE请求相关信息;
    OnEvent m_pfnOnEvent;           // 事件回调函数,告知UTE进程, 某个策略进程是否正常运行;

    std::string m_UTESysName;       // UTE进程的系统ID;

    /// 内部运行的参数;
    bool m_bInit;                   // 是否初始化成功;

    ReqQueueManager* m_pReqQueue;                           // 请求队列管理器;
    std::unordered_map<std::string, RspQueueManager*> m_mapRspQueue;     // 响应队列管理器;

    // 策略进程对应lock文件map, 用于存储策略进程的key和心跳文件路径;
    std::unordered_map<std::string, std::string> m_mapStrategyHeartbeatFile;
};  