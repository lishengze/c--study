#pragma once

#include <cstring>
#include <array>
#include <functional>
#include <string>

namespace share_common 
{

#pragma pack(push, 1)

const unsigned int kSuccess = 0; //成功
const unsigned int kWriteFailed = 1; //Write进程终止;
const unsigned int kWriteNotInited = 2; //Write进程终止;
const unsigned int kPktReadInit = 8; // Read初始化消息;


const unsigned int kTickData = 1001; // TickData;

struct TickData {
    char strSymbol[32]; // StockName;
    unsigned long long dTickTime; // 时间戳
    double dPrice; // 价格
    double dVolume; // 成交量
    double dTurnover; // 成交额

    TickData(const TickData& other) {
        memcpy(strSymbol, other.strSymbol, sizeof(strSymbol));
        dTickTime = other.dTickTime;
        dPrice = other.dPrice;
        dVolume = other.dVolume;
        dTurnover = other.dTurnover;
    }

    TickData& operator =(const TickData& other) {
        if (this == &other) {
            return *this;
        }
        memcpy(strSymbol, other.strSymbol, sizeof(strSymbol));
        dTickTime = other.dTickTime;
        dPrice = other.dPrice;
        dVolume = other.dVolume;
        dTurnover = other.dTurnover;
        return *this;
    }

    TickData() = default;
    ~TickData() = default;


    std::string str() {
        return std::string("TickData: dTickTime=") + std::to_string(dTickTime) +
            ", dPrice=" + std::to_string(dPrice) +
            ", dVolume=" + std::to_string(dVolume) +
            ", dTurnover=" + std::to_string(dTurnover);
    }
};


struct ReadKey {
    unsigned long long ulReadKey; //ReadID
};

/// @brief 通用请求消息结构体
struct WriteMsg {
    WriteMsg() : iMsgID(0), iMsgSrcType(0), pMsgHander(nullptr), iMsgLen(0), ulReadKey(0) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));
    }

    WriteMsg(int iMsgID, unsigned int iMsgLen, unsigned long long ulReadKey, const char* pMsgBuf) :
         iMsgID(iMsgID), iMsgSrcType(0), pMsgHander(nullptr), iMsgLen(iMsgLen), ulReadKey(ulReadKey) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));

        // LOG_DEBUG("***** Default Constructor!");

        if (iMsgLen > 0 && iMsgLen <= sizeof(strMsgBuf)) {
            memcpy(strMsgBuf, pMsgBuf, iMsgLen);            
        }

    }

    WriteMsg(int iMsgID, unsigned int iMsgLen, int iMsgSrcType, void* pMsgHander, const char* pMsgBuf) :
         iMsgID(iMsgID), iMsgSrcType(iMsgSrcType), pMsgHander(pMsgHander), iMsgLen(iMsgLen), ulReadKey(0) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));
        if (iMsgLen > 0 && iMsgLen <= sizeof(strMsgBuf)) {
            memcpy(strMsgBuf, pMsgBuf, iMsgLen);            
        }
    }    

    WriteMsg(const WriteMsg&& other) :
        iMsgID(other.iMsgID),iMsgSrcType(other.iMsgSrcType), 
        pMsgHander(other.pMsgHander), iMsgLen(other.iMsgLen), ulReadKey(other.ulReadKey) {
        memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);         
    }

    WriteMsg& operator=(const WriteMsg&& other)
    {
        if (this == &other ) return *this;
        iMsgID = other.iMsgID;
        iMsgLen = other.iMsgLen;
        ulReadKey = other.ulReadKey;
        iMsgSrcType = other.iMsgSrcType;
        pMsgHander = other.pMsgHander;
        memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);
        return *this;
    }

     WriteMsg(const WriteMsg& other)
    {
        if (this == &other ) return;
        iMsgID = other.iMsgID;
        iMsgLen = other.iMsgLen;
        ulReadKey = other.ulReadKey;
        iMsgSrcType = other.iMsgSrcType;
        pMsgHander = other.pMsgHander;
        memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);
        return;
    }    

    int  iMsgID;            // 消息类型
    int  iMsgSrcType;       // 消息来源类型
    void* pMsgHander;       // 消息处理句柄
    unsigned int iMsgLen;  // 拷贝消息缓冲区的真实长度;
    unsigned long long ulReadKey; // 由strategyID 和 bachID 拼接的key;
    char strMsgBuf[2048]; // 增加编译宏判断;
};

// 用于Read进程和Write进程之间的消息通信;
// 1. iMsgID: 消息ID;
// 2. pMsgBuf: 消息缓冲区;
// 3. ulMsgKey: ReadID;;

using WriteGetReadReqCallBackFuncType = std::function<void(int , const char* , unsigned long long)>;


// 用于转发API请求和交易所回报 到 Write 业务线程的回调接口;
// 1. iMsgID: 消息ID;
// 2. pMsgBuf: 消息缓冲区;
// 3. iMsgLen: 消息长度;
// 4. iMsgSrcType: 消息来源类型;
// 5. pMsgHandler: 消息处理句柄;
using WriteGetInnerReqCallBackFuncType = std::function<void(int , const char* , int , int, void*)>;

// Read接收Write Event 事件的回调接口类型;
// 1. iErrCode: 错误码;
// 2. pErrDesc: 错误信息;
using ReadGetRspCallbackEventFuncType = std::function<void(int , const char*)>;


// Read接收Write 回报消息的 事件的回调接口类型;
// 1. iMsgID: 消息ID;
// 2. pMsgBuf: 消息缓冲区;
// 3. iMsgLen: 消息长度;
using ReadGetRspCallbackMessageFuncType = std::function<void(int , const char*, const int)>;


#ifdef __GNUC__
/**
 * @brief      生成有利于分支预测的代码,告知编译器条件表达式"x"为false的概率较高
 *
 * @param      x     条件表达式
 *
 * @return     条件表达式的布尔值
 */
#define SHARE_COMM_LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define SHARE_COMM_LIKELY(x) (x)
#endif

} // namespace share_common

#pragma pack(pop)