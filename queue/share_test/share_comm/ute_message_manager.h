#pragma once

#include <unordered_map>
#include <string>
#include <functional>

#include "queue_manager.h"
#include "lock_file_manager.h"
#include "share_comm_util.h"
#include "logger.h"
#include "singleton.h"

using namespace tech;

namespace share_common 
{

// 用于策略进程和UTE进程之间的消息通信;
// 1. iMsgID: 消息ID;
// 2. pMsgBuf: 消息缓冲区;
// 3. ulMsgKey: 策略ID;
using CallBackFuncType = std::function<void(int , const char* , unsigned long long)>;

// 用于转发API请求和交易所回报 到 UTE 业务线程的回调接口;
// 1. iMsgID: 消息ID;
// 2. pMsgBuf: 消息缓冲区;
// 3. iMsgSrcType: 消息来源类型;
// 4. iMsgLen: 消息长度;
// 5. pMsgHandler: 消息处理句柄;
using InnerCallBackFuncType = std::function<void(int , const char* , int , int, void*)>;


/// @brief 用于管理策略进程和UTE进程之间的消息通信;
/// 问题: 
/// 1. 是否要增加专门存储 stratey_id 和 batch_id 的结构体, 这两者合起来可以唯一确定一个策略进程;
/// 2. UTE 进程向策略进程发送消息的接口, 是否需要增加参数, 用于指定发送给哪个策略进程;
/// 3. 
class UteMessageManager {
public:
    SINGLETON_DEF(UteMessageManager);
    UteMessageManager() : m_UTESysName(""), m_iApiReqProcessCount{1}, m_iStrategyReqProcessCount{5}, 
                          m_iReqShareQueueDataNum{10000}, m_iRspShareQueueDataNum{4000}, m_iApiQueueDataNum{4000},
                          shptrConsumerThread_{nullptr} {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_pfnOnInnerMessage = nullptr;

        std::string loggerName = "ute_message_manager_"+ SecTimeStr("%Y%m%d") + ".log";
        logger::init(loggerName);
        logger::set_level(spdlog::level::debug);        
    }

    ~UteMessageManager() {
        Reset();

    }

    /// @brief 设置事件回调函数, 告知策略进程, UTE进程是否正常运行;
    /// @param iSleepSec 
    /// @param pfnOnEvent 
    void SetOnEvent(int iEventSleepSec, CallBackFuncType pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; }

    /// @brief 设置消息回调函数, UTE进程向策略进程发送消息的接口;
    /// @param pfnOnMessage 
    void SetOnMessage(CallBackFuncType pfnOnMessage) { m_pfnOnMessage = pfnOnMessage; }

    /// @brief 设置内部消息回调函数, 用于转发API请求和交易所回报 到 UTE 业务线程;
    void SetOnInnerMessage(InnerCallBackFuncType pfnOnInnerMessage) { m_pfnOnInnerMessage = pfnOnInnerMessage; }

    /// @brief 初始化消息管理器, 设置策略进程的系统ID和UTE进程的系统ID;
    ///        在Init 会校验, OnMessage,OnEvent 是否设置;
    ///        如果未设置, 会返回false;
    ///        所以需要在调用 Init 之前调用 SetOnMessage, SetOnEvent;
    /// @param cstrUTESysName UTE进程的系统ID;
    /// @param iApiReqProcessCount 一次最多处理的API请求数量;
    /// @param iStrategyReqProcessCount 一次最多处理的策略请求数量;
    /// @param iReqShareQueueDataNum 策略进程发送请求的共享内存无锁队列元素数目;
    /// @param iRspShareQueueDataNum 回报给策略进程的共享内存无锁队列元素数目;
    /// @param iApiQueueDataNum 转发API请求和交易所回报的无锁队列元素数目;
    /// 入参是否要增加共享队列相关参数？
    bool Init(const char* cstrUTESysName, int iApiReqProcessCount, int iStrategyReqProcessCount,
                int iReqShareQueueDataNum, int iRspShareQueueDataNum, int iApiQueueDataNum);


    /// @brief UTE业务处理线程,转发到相关请求到共享内存请求处理线程，进行统一调度处理的接口,写的是内存中的无锁队列，所以使用SendMsg;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 未正确初始化;
    bool WriteMsg(int iMsgID, const char* pMsgBuf, unsigned int  iMsgLen, int iMsgSrcType, void* pMsgHandler) ;


    /// @brief 收到客户登陆请求后，判断客户所属策略进程的回报共享内存通路时候时候存在，
    ///        如果不存在则进行初始化--共享内存是策略端创建，UTE端 Attach即可;
    /// @param strStrategyKey 
    QueueManager* CreateStrategyRspQueue(unsigned long long strStrategyKey);

    /// @brief 
    /// 按照配置比例监听 策略进程的请求队列和转发API请求到共享队列的线程;
    void StartListenQueue();

    void Reset();

    /// 外部设置的参数;
    CallBackFuncType m_pfnOnMessage;       // 消息回调函数,通知UTE请求相关信息;
    CallBackFuncType m_pfnOnEvent;           // 事件回调函数,告知UTE进程, 某个策略进程是否正常运行;
    InnerCallBackFuncType m_pfnOnInnerMessage; // 内部消息回调函数, 用于转发API请求和交易所回报 到 UTE 业务线程;

    std::unordered_map<unsigned long long, QueueManager*>& get_map_rsp_queue() { return m_mapRspQueue; }    // 共

private:


    std::string m_UTESysName;       // UTE进程的系统ID;
    int m_iApiReqProcessCount;      // 一次最多处理的API请求数量;
    int m_iStrategyReqProcessCount; // 一次最多处理的策略请求数量;
    int m_iReqShareQueueDataNum;    // 策略进程发送请求的共享内存无锁队列元素数目;
    int m_iRspShareQueueDataNum;    // 回报给策略进程的共享内存无锁队列元素数目;
    int m_iApiQueueDataNum;         // 转发API请求和交易所回报的无锁队列元素数目;

    LockFileManager m_LockFileManager; // 锁文件管理器;

    QueueManager m_pStrategyReqQueue;                           // 策略请求队列管理器;
    QueueManager m_pApiQueue;                                   // API请求回报队列管理器;

    std::unordered_map<unsigned long long, QueueManager*> m_mapRspQueue;     // 策略进程接收回报的共享内存队列管理器

    std::shared_ptr<std::thread>    shptrConsumerThread_;           // 策略进程对应的锁文件;
};  

} // namespace share_common