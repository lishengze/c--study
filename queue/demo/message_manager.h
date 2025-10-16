#pragma once
typedef void (*OnEvent)(int iErrCode, const char* pErrDesc);
typedef void (*OnMessage)(int iMsgID, const char* pMsgBuf, const int iMsgLen);

class CMessageManager {
public:

    CMessageManager() {
        m_pfnOnEvent = nullptr;
        m_pfnOnMessage = nullptr;
        m_iEventSleepSec = 0;
        m_StrategySysID = 0;
        m_UTESysID =0;
    }
	
	~CMessageManager(){}

    /// @brief 设置事件回调函数，告知策略进程，UTE进程是否正常运行；
    /// @param iSleepSec
    /// @param pfnOnEvent
    void SetOnEvent(int iEventSleepSec, OnEvent pfnOnEvent) { m_pfnOnEvent = pfnOnEvent; }

    /// @brief 设置消息回调函数，UTE进程向策略进程发送消息的接口;
    /// @param pfnOnMessage
    void SetOnMessage(OnMessage pfnOnMessage) { m_pfnOnMessage = pfnOnMessage;}

    /// @brief 初始化消息管理器，设置策略进程的系统ID和UTE进程的系统ID;
    ///在Init 会校验，OnMessage,OnEvent 是否设置;
    ///如果未设置，会返回false：
    ///所以需要在调用 Init 之前调用 SetOnMessage, SetOnEvent;
    /// @param StrategySysID 策略进程的系统ID;
    /// @param UTESysID UTE进程的系统ID;
    bool Init(UINT64 StrategySysID, UINT64 UTESysID);

    /// @brief 策略进程发送消息给UTE进程;
    /// @param iMsgID 消息ID;
    /// @param pMsgBuf 消息缓冲区;
    /// @param iMsgLen 消息长度;
    /// @return true 发送成功;
    /// @return false 发送失败 -- 未正确初始化;
    bool SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen);

private:
    /// 外部设置的参数;
    OnMessage m_pfnOnMessage;    // 消息回调函数,UTE进程向策略进程发送消
    OnEvent m_pfnOnEvent;        // 事件回调函数，告知策略进程，UTE进程是否正常运行；
    int m_iEventSleepSec;        // 事件回调函数的睡眠时间，单位：秒；
    char m_StrategySysID[64];    // 策略进程的系统ID;
    char m_UTESysID[64];         // UTE进程的系统ID;

    /// 内部运行的参数；
    bool m_bInit; // 是否初始化成功;

};