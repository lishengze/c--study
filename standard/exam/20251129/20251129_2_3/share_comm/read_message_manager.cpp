#include "read_message_manager.h"
#include <utility>
#include "share_comm_util.h"

namespace share_common 
{

/// @brief Read进程发送请求，是发送到共享内存中，所以使用SendMsgShare;
/// @param iMsgID 
/// @param pMsgBuf 
/// @param iMsgLen 
/// @return 
bool ReadMessageManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen) {
    // 理论上，m_bRspQueueInit 会在其他线程修改这个值, 就会出现判断失败的情况，但是这种场景较少发生，并且即便判断失败也没有太大影响，共享内存并未被回收，
    // 如果使用多线程编程模式，增加开销得不偿失;
    // m_bRspQueueInit 赋值为true, 是一定在 SendMsg 前实现，所以不会有问题;
    if (SHARE_COMM_LIKELY(m_bRspQueueInit)) { 
            m_ReqQueueManager.SendMsgShare(iMsgID, pMsgBuf, iMsgLen, m_ReadKey); // 用于回报的共享内存已经初始化成功，可以发送任何请求;
        
    } else {
        LOG_ERROR("Share Memory To Write is Not Init Or Over!");
    }

    return true;
}

bool ReadMessageManager::Init(const char* cstrWriteSysName, unsigned int uiReadSysID, int iWaitWriteSec) {
    LOG_INFO("ReadMessageManager::Init begin cstrWriteSysName={}, uiReadSysID={}", cstrWriteSysName, uiReadSysID);

    if (!m_pfnOnMessage) {
        LOG_ERROR("OnMessage callback function is not set.");
        return false;
    }

    if (!m_pfnOnEvent) {
        LOG_ERROR("OnEvent callback function is not set.");
        return false;
    }

    if (!iWaitWriteSec) {
        LOG_ERROR("iWaitWriteSec is 0");
        return false;
    }   
    m_iWaitWriteSec = iWaitWriteSec;

	    // 从19700101到当前的毫秒数;
    unsigned long long  ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now().time_since_epoch()).count();

    // 再取低32位，这32位的毫秒数，能表示一年多的时间;
    unsigned long long low_part = 0x00000000FFFFFFFF & ms;

    unsigned long long high_part = static_cast<unsigned long long>(uiReadSysID) << 32;


    // 高32位为ReadID, 低32位为时间戳, 理论上能保证不重复;
    m_ReadKey = (high_part | low_part);

    LOG_INFO("uiReadSysID={}, uiBatchID={}, m_ReadKey={}", uiReadSysID, low_part, m_ReadKey);


    long lStartWaitSec = SecTime();

    // 创建好Read的锁文件后，开始监听UTE是否创建, 超时通过OnEvent通知， 成功，调用 InitReqQueue();

    ReadGetRspCallbackEventFuncType ReadOnEventFunc = std::bind(&ReadMessageManager::ProcessWriteEnd, this, std::placeholders::_1, std::placeholders::_2);
    if (!m_LockFileManager.Init(cstrWriteSysName, m_ReadKey, ReadOnEventFunc, lStartWaitSec, m_iEventSleepSec, m_iWaitWriteSec)) { 
        LOG_ERROR("Failed to initialize lock file manager.");
        return false;
    } 

    if (!InitReqQueue(cstrWriteSysName)) {
        return false;
    }

    if (!StartWaitWriteQueue(lStartWaitSec)) {
        return false;
    } else {
        LOG_INFO("Read {} Attach Rsp Queue {} SUCCESS", m_ReadKey, GetQueueName(m_ReadKey));        
    }

    // 回报的共享内存Attach成功后，再开启心跳线程 检Write进程的锁文件;
    m_LockFileManager.StartHeartbeatThread(); 

    return true;
}

bool ReadMessageManager::InitReqQueue(const char* cstrWriteSysName) {
    // 初始化请求相关的无锁队列 以及 对应的 共享内存 -- 共享内存是Write进程创建好， 这里只需要attach即可;
    if (!m_ReqQueueManager.Init(m_pfnOnMessage, Producer, GetNonReqOrderQueueName(cstrWriteSysName).c_str(),  false, false)){
        LOG_ERROR("Failed to initialize request queue manager.");
        m_pfnOnEvent(kWriteNotInited, "Failed to initialize request queue manager.");
        return false;
    } else {
        LOG_INFO("Attach Write Share Memory:{} [PASSED]", GetNonReqOrderQueueName(cstrWriteSysName));
    }

    return true;
}


/// @brief  Read进程是从 共享内存取出数据,所以用 TryPopShare
/// @param msg 
/// @return 
bool ReadMessageManager::TryPopMsg(WriteMsg& msg) {
    return m_ReqQueueManager.TryPop(msg);
}

bool ReadMessageManager::StartWaitWriteQueue(long lStartWaitSec) {

    ReadKey obj;
    obj.ulReadKey = m_ReadKey;

    //  发送StragegyKey 给服务端， 服务端由此创建一对一的回报共享内存队列;
    m_ReqQueueManager.SendMsgShare(kPktReadInit, (char*)(&obj), sizeof(ReadKey), m_ReadKey); 

    while (true) {
        LOG_INFO("Start Waiting Write Create Share Mermory:{}", GetQueueName(m_ReadKey));

        WriteMsg msg;
        if (m_RspQueueManager.Init(m_pfnOnMessage, Consumer, GetQueueName(m_ReadKey).c_str(), false, false)) {
            m_bRspQueueInit = true;            
            break; // 成功;
        }

        if (SecTime() - lStartWaitSec > m_iWaitWriteSec) {            
            m_pfnOnEvent(kWriteNotInited, "Wait Too long For Write To Init RspQueue!");
            return false;
        }

        sleep(1); // todo 测试专用;
    }
    
    return true;
}

void ReadMessageManager::ProcessWriteEnd(int iErrCode, const char* pErrDesc) {

    LOG_WARN("iErrCode:{}, pErrDesc:{}", iErrCode, pErrDesc );

    m_bRspQueueInit = false;  // 告知无法再发送消息即可，无需立刻释放 Queue资源;

    m_pfnOnEvent(iErrCode, pErrDesc);
}



} // namespace share_common 