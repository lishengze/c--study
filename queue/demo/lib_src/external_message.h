#pragma once

#include <cstring>

namespace share_common 
{

#pragma pack(push, 1)

//msgtype字典
const unsigned int kPktLoginReq = 1001; //登录请求
const unsigned int kPktLoginAns = 2001; //登录应答
const unsigned int kPktLogoutReq = 1002; //登出请求
const unsigned int kPktLogoutAns = 2002; //登出应答
const unsigned int kPktOrderReq = 1003; //委托请求
const unsigned int kPktCancelOrderReq = 1004; //撤单请求
const unsigned int kPktOrderAns = 2003; //委托应答
const unsigned int kPktCancelOrderAns= 2004; //撤单应答
const unsigned int kPktOrderMatch = 2005; //委托成交回报
const unsigned int kPktRejectMsg = 9; //请求拒单
const unsigned int kPktStrategyEnd = 10; //策略结束


//order_staus字典
const unsigned char kNew = 0;                                    //已申报
const unsigned char kPartiallyFilled = 1;                        //部分成交
const unsigned char kFilled = 2;                                 //全部成交
const unsigned char kCancelled = 4;                              //已撤销
const unsigned char kReject = 8;                                 //已拒绝
const unsigned char kPartiallyFilledPartiallyCancelled = 3;      //部分成交部分撤销
const unsigned char kPartiallyCancelled = 5;                     //部分撤销
const unsigned char kUnSend = 9;                                 //未申报
const unsigned char kSended = 10;                                //正报
const unsigned char kWaitCancelled = 11;                         //待撤销
const unsigned char kPartiallyFilledWaitCancelled = 12;          //部成待撤
const unsigned char kProcessed = 13;                             //已处理
const unsigned char kNull = 99;                                  //无

//market_id字典
const unsigned short kShangHai = 101;  ///< 上海
const unsigned short kShenZhen = 102;  ///< 深圳
const unsigned short kHongKong = 103;  ///< 香港
const unsigned short kThird = 109;  ///< 北京
const unsigned short kHKTShangHai = 105;  ///< 港股通-沪市
const unsigned short kHKTShenZhen = 106;  ///< 港股通-深市
const unsigned short kBShangHai = 107;  ///< 上海B股
const unsigned short kBShenZhen = 108;  ///< 深圳B股
const unsigned short kFund = 110;  ///< 基金
const unsigned short kBankShangHai = 111;  ///< 银行间-沪市
const unsigned short kBankShenZhen = 112;  ///< 银行间-深市
const unsigned short kHKT = 199;  ///< 港股通

//side字典
const char kBuy = '1';  ///< 买
const char kSell = '2';  ///< 卖
const char kPurchase = 'D';  ///< 申购
const char kRedeem = 'E';  ///< 赎回
const char kBorrow = 'G';  ///< 借入
const char kLoan = 'F';  ///< 出借

//order_type字典
const char kLimited = '1';    ///< 限价委托
const char kLocalOptimal = '2';   ///< 本方最优
const char kCounterpartyOptimalOrLimited = '3';   ///< 对手方最优剩余转限价
const char kImmediateOrCancel = '4';  ///< 市价立即成交剩余撤销
const char kFillOrKill = '5'; ///< 市价全额成交或撤销
const char kFiveLevelFillOrKill = '6';    ///< 市价最优五档全额成交剩余撤销

// ErrCode

const unsigned int kSuccess = 0; //成功
const unsigned int kUteFailed = 1; //UTE进程终止;

struct TradeOrderUser
{
	char fund_account_id[17]; //资金账号
	char branch_id[11]; //营业部代码
	char account_id[13]; //证券账户
	char cust_id[17]; //客户号
	unsigned long long client_seq_id; //用户系统消息编号
	unsigned long long agw_seq_id; //内部订单编号
};

struct TradeOrderInfo
{
    char security_id[9]; //证券代码
    unsigned short market_id; //市场
    char side; ///< 买卖方向, 1=买; 2=卖; G=借入; F=借出; D=申购; E=赎回
    char order_type; ///< 订单类型, 1=市价; 2=限价; U=本方最优
    long long order_qty; ///< 委托数量, N15(2)
    long long order_price; ///< 委托价格, N13(4)
    long long stop_px; ///< 止损价, N13(4)
};

struct CancelOrderInfo
{
    long long   orig_client_seq_id; //原用户系统消息序号
    long long  orig_clordno; //原客户订单编号
};

struct OrdERInfo
{
    char order_id[17]; ///< 交易所赋予的订单编号, 跨交易日不重复
    char clordid[11]; ///< 申报合同号
    char security_id[9]; ///< 证券代码
    unsigned short market_id; ///< 市场代码
    char exec_type; ///< 执行报告类型, 0=New, 表示新订单; 4=Cancelled, 表示已撤销; 8=Reject, 表示已拒绝; F=Trade, 表示已成交
    unsigned char ord_status; //内部订单状态
    long long price; ///< 价格, N13(4)
    long long order_qty; ///< 委托数量, N15(2)
    long long leaves_qty; ///< 订单剩余数量, N15(2)
    long long cum_qty; ///< 累计执行数量, N15(2)
    char side; ///< 买卖方向, 1=买; 2=卖; G=借入; F=借出; D=申购; E=赎回
    long long transact_time; //时间戳
    char user_info[65]; ///< 用户私有信息, 尽可包含ASCII可显示字符
    char exec_id[17]; ///< 交易所赋予的执行编号, 单个交易日内不重复
    char orig_clordid[11]; //原申报合同号
    char ord_type; ///< 订单类型, 1=市价; 2=限价; U=本方最优
    unsigned short ord_rej_reason;//交易所错误码
    char time_in_force; ///< 订单有效时期类型, 0=当日有效, Day(港股通增强限价盘); 3=即时成交或取消(IOC); 9=At Crossing(港股通竞价限价盘)
    long long last_px; ///< 成交价格, N13(4)
    long long last_qty; ///< 成交数量, N15(2)
    char cash_margin; ///< 融资融券信用标识, 1=Cash, 普通交易; 2=Open, 融资融券开仓; 3=Close, 融资融券平仓
    char cancel_flag; //撤单标志
    long long clordno; ///< 客户订单编号
    long long orig_clordno; ///< 原始客户订单编号
    long long index; //发送至客户端序号
    unsigned short code;//内部错误码
    long long frozen_trade_value;//冻结交易金额
    long long frozen_fee; //冻结费用
    long long fee; // 单笔成交费用
    long long total_value_traded;//成交金额
};	

//客户登录请求
struct LogOnReq
{
	TradeOrderUser trade_order_user; //客户信息
	unsigned int heart_bt_int; ///< 心跳检测时间
	char password[101]; ///< 密码
	char client_feature_code[1025]; //客户端特征码
};

//客户登录应答
struct LogOnAns
{
    TradeOrderUser trade_order_user; //客户信息
    int session_status; ///< 会话状态
    unsigned int error_code; //错误码
};

//客户登出请求
struct LogOutReq
{
	TradeOrderUser trade_order_user; //客户信息
    char password[101]; ///< 密码
};

//客户登出应答
struct LogOutAns
{
    TradeOrderUser trade_order_user; //客户信息
    int session_status; ///< 会话状态
    unsigned int error_code; ///< 错误码
};

//委托请求
struct TradeOrderReq
{
    TradeOrderUser trade_order_user; //客户信息
    TradeOrderInfo trade_order_info; //委托信息
};

//撤单请求
struct CancelOrderReq
{
    TradeOrderUser   trade_order_user; //客户信息
    CancelOrderInfo  cancel_order_info; //撤单信息
};


//业务回报
struct TradeOrderER
{
    TradeOrderUser trade_order_user; //客户信息
    OrdERInfo order_er_info; //订单信息
};

//拒绝返回
struct RejectMsg
{
    TradeOrderUser trade_order_user; //客户信息
    unsigned short reject_reason_code; /// 错误编码
    char cancel_flag; //撤单标识
};

struct StrategyEnd {
    unsigned long long ulStrategyKey; //策略ID
};

/// @brief 通用请求消息结构体
struct UteMsg {
    UteMsg() : iMsgID(0), iMsgLen(0), ulStrategyKey(0), iMsgSrcType(0), pMsgHander(nullptr) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));
    }

    UteMsg(int iMsgID, unsigned int iMsgLen, unsigned long long ulStrategyKey, const char* pMsgBuf) :
         iMsgID(iMsgID), iMsgLen(iMsgLen), ulStrategyKey(ulStrategyKey), iMsgSrcType(0), pMsgHander(nullptr) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));

        if (iMsgLen > 0 && iMsgLen <= sizeof(strMsgBuf)) {
            memcpy(strMsgBuf, pMsgBuf, iMsgLen);            
        }

    }

    UteMsg(int iMsgID, unsigned int iMsgLen, int iMsgSrcType, void* pMsgHander, const char* pMsgBuf) :
         iMsgID(iMsgID), iMsgLen(iMsgLen), ulStrategyKey(0), iMsgSrcType(iMsgSrcType), pMsgHander(pMsgHander) {
        memset(strMsgBuf, 0, sizeof(strMsgBuf));
        if (iMsgLen > 0 && iMsgLen <= sizeof(strMsgBuf)) {
            memcpy(strMsgBuf, pMsgBuf, iMsgLen);            
        }
    }    

    UteMsg(const UteMsg&& other) :
        iMsgID(other.iMsgID), iMsgLen(other.iMsgLen), ulStrategyKey(other.ulStrategyKey), iMsgSrcType(other.iMsgSrcType), pMsgHander(other.pMsgHander) {
        // memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);

        if (iMsgLen > 0 && iMsgLen <= sizeof(strMsgBuf)) {
            memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);           
        }        
    }

    UteMsg& operator=(const UteMsg&& other)
    {
        if (this == &other ) return *this;
        iMsgID = other.iMsgID;
        iMsgLen = other.iMsgLen;
        ulStrategyKey = other.ulStrategyKey;
        iMsgSrcType = other.iMsgSrcType;
        pMsgHander = other.pMsgHander;
        memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);
        return *this;
    }

     UteMsg(const UteMsg& other)
    {
        if (this == &other ) return;
        iMsgID = other.iMsgID;
        iMsgLen = other.iMsgLen;
        ulStrategyKey = other.ulStrategyKey;
        iMsgSrcType = other.iMsgSrcType;
        pMsgHander = other.pMsgHander;
        memcpy(strMsgBuf, other.strMsgBuf, other.iMsgLen);
        return;
    }    

    int  iMsgID;            // 消息类型
    int  iMsgSrcType;       // 消息来源类型
    void* pMsgHander;       // 消息处理句柄
    unsigned int iMsgLen;  // 拷贝消息缓冲区的真实长度;
    unsigned long long ulStrategyKey; // 由strategyID 和 bachID 拼接的key;
    char strMsgBuf[sizeof(LogOnReq)+1]; // 增加编译宏判断;
};

} // namespace share_common

#pragma pack(pop)