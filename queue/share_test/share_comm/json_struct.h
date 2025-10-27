#include "share_comm_external_message.h"
#include "json_util.hpp"

#include <iostream>
#include "logger.h"

using namespace share_common;

class JsonStructHelper {
public:
    bool Init(std::string strSrcJsonFileName) {
        m_strSrcJsonFileName = strSrcJsonFileName;

        Error error;
        if ((error = GetJsonFromFile(m_jsonData, m_strSrcJsonFileName)).IsFailed()) {
            LOG_ERROR("JsonStructHelper::Init, ParseJsonFile:{} failed, error: {}", strSrcJsonFileName, error.Str());
            return false;
        }
        return true;
    }  

    bool ResetData() {
        return Init(m_strSrcJsonFileName);
    }

    bool ReInit(std::string strSrcJsonFileName) {
        return Init(strSrcJsonFileName);
    }


    // struct TradeOrderUser
    // {
    // 	char fund_account_id[17]; //资金账号
    // 	char branch_id[11]; //营业部代码
    // 	char account_id[13]; //证券账户
    // 	char cust_id[17]; //客户号
    // 	unsigned long long client_seq_id; //用户系统消息编号
    // 	unsigned long long agw_seq_id; //内部订单编号
    // };
    bool ParseTraderOrderUser(njson& reqJsonData, TradeOrderUser& stTraderOrderUser) {
        
        if (ResetData() || !reqJsonData.contains("TradeOrderUser")) {
            std::string sErrMsg;
            njson jsTradeOrderUser = reqJsonData["TradeOrderUser"];
            GetJsonCharStringField(jsTradeOrderUser, "fund_account_id", stTraderOrderUser.fund_account_id, sizeof(stTraderOrderUser.fund_account_id), sErrMsg);  
            GetJsonCharStringField(jsTradeOrderUser, "branch_id", stTraderOrderUser.branch_id, sizeof(stTraderOrderUser.branch_id), sErrMsg);  
            GetJsonCharStringField(jsTradeOrderUser, "account_id", stTraderOrderUser.account_id, sizeof(stTraderOrderUser.account_id), sErrMsg);  
            GetJsonCharStringField(jsTradeOrderUser, "cust_id", stTraderOrderUser.cust_id, sizeof(stTraderOrderUser.cust_id), sErrMsg);   

            GetJsonUnsignedLongLongField(jsTradeOrderUser, "client_seq_id", stTraderOrderUser.client_seq_id, sErrMsg);  
            GetJsonUnsignedLongLongField(jsTradeOrderUser, "agw_seq_id", stTraderOrderUser.agw_seq_id, sErrMsg);  
        } else {
            // 赋值默认参数;
        }

        return true;
    }

    //     struct TradeOrderInfo
    // {
    //     char security_id[9]; //证券代码
    //     unsigned short market_id; //市场
    //     char side; ///< 买卖方向, 1=买; 2=卖; G=借入; F=借出; D=申购; E=赎回
    //     char order_type; ///< 订单类型, 1=市价; 2=限价; U=本方最优
    //     long long order_qty; ///< 委托数量, N15(2)
    //     long long order_price; ///< 委托价格, N13(4)
    //     long long stop_px; ///< 止损价, N13(4)
    // };
    bool ParseTradeOrderInfo(njson& reqJsonData, TradeOrderInfo& stTradeOrderInfo) {
        if (ResetData() || !reqJsonData.contains("TradeOrderInfo")) {
            std::string sErrMsg;
            njson jsTradeOrderInfo = reqJsonData["TradeOrderInfo"];
            GetJsonCharStringField(jsTradeOrderInfo, "security_id", stTradeOrderInfo.security_id, sizeof(stTradeOrderInfo.security_id), sErrMsg);  

            GetJsonUnsignedShortField(jsTradeOrderInfo, "market_id", stTradeOrderInfo.market_id, sErrMsg);  

            GetJsonCharField(jsTradeOrderInfo, "side", stTradeOrderInfo.side, sErrMsg);  
            GetJsonCharField(jsTradeOrderInfo, "order_type", stTradeOrderInfo.order_type, sErrMsg);  

            GetJsonLongLongField(jsTradeOrderInfo, "order_qty", stTradeOrderInfo.order_qty, sErrMsg);  
            GetJsonLongLongField(jsTradeOrderInfo, "order_price", stTradeOrderInfo.order_price, sErrMsg);  
            GetJsonLongLongField(jsTradeOrderInfo, "stop_px", stTradeOrderInfo.stop_px, sErrMsg);  
        } else {
            // 赋值默认参数;
        }
        return true;
    }

    // struct CancelOrderInfo
    // {
    //     long long   orig_client_seq_id; //原用户系统消息序号
    //     long long  orig_clordno; //原客户订单编号
    // };
    bool ParseCancelOrderInfo(njson& reqJsonData, CancelOrderInfo& stCancelOrderInfo) {
        if (ResetData() || !reqJsonData.contains("CancelOrderInfo")) {
            std::string sErrMsg;
            njson jsCancelOrderInfo = reqJsonData["CancelOrderInfo"];
            GetJsonLongLongField(jsCancelOrderInfo, "orig_client_seq_id", stCancelOrderInfo.orig_client_seq_id, sErrMsg);  
            GetJsonLongLongField(jsCancelOrderInfo, "orig_clordno", stCancelOrderInfo.orig_clordno, sErrMsg);  
 
        } else {
            // 赋值默认参数;
        }
        return true;
    }

    // struct OrdERInfo
    // {
    //     char order_id[17]; ///< 交易所赋予的订单编号, 跨交易日不重复
    //     char clordid[11]; ///< 申报合同号
    //     char security_id[9]; ///< 证券代码
    //     unsigned short market_id; ///< 市场代码
    //     char exec_type; ///< 执行报告类型, 0=New, 表示新订单; 4=Cancelled, 表示已撤销; 8=Reject, 表示已拒绝; F=Trade, 表示已成交
    //     unsigned char ord_status; //内部订单状态
    //     long long price; ///< 价格, N13(4)
    //     long long order_qty; ///< 委托数量, N15(2)
    //     long long leaves_qty; ///< 订单剩余数量, N15(2)
    //     long long cum_qty; ///< 累计执行数量, N15(2)
    //     char side; ///< 买卖方向, 1=买; 2=卖; G=借入; F=借出; D=申购; E=赎回
    //     long long transact_time; //时间戳
    //     char user_info[65]; ///< 用户私有信息, 尽可包含ASCII可显示字符
    //     char exec_id[17]; ///< 交易所赋予的执行编号, 单个交易日内不重复
    //     char orig_clordid[11]; //原申报合同号
    //     char ord_type; ///< 订单类型, 1=市价; 2=限价; U=本方最优
    //     unsigned short ord_rej_reason;//交易所错误码
    //     char time_in_force; ///< 订单有效时期类型, 0=当日有效, Day(港股通增强限价盘); 3=即时成交或取消(IOC); 9=At Crossing(港股通竞价限价盘)
    //     long long last_px; ///< 成交价格, N13(4)
    //     long long last_qty; ///< 成交数量, N15(2)
    //     char cash_margin; ///< 融资融券信用标识, 1=Cash, 普通交易; 2=Open, 融资融券开仓; 3=Close, 融资融券平仓
    //     char cancel_flag; //撤单标志
    //     long long clordno; ///< 客户订单编号
    //     long long orig_clordno; ///< 原始客户订单编号
    //     long long index; //发送至客户端序号
    //     unsigned short code;//内部错误码
    //     long long frozen_trade_value;//冻结交易金额
    //     long long frozen_fee; //冻结费用
    //     long long fee; // 单笔成交费用
    //     long long total_value_traded;//成交金额
    // };	    
    bool ParseOrdERInfo(njson& reqJsonData, OrdERInfo& stOrdERInfo) {
        if (ResetData() || !reqJsonData.contains("OrdERInfo")) {
            std::string sErrMsg;
            njson jsOrdERInfo = reqJsonData["OrdERInfo"];
            GetJsonCharStringField(jsOrdERInfo, "order_id", stOrdERInfo.order_id, sizeof(stOrdERInfo.order_id), sErrMsg);  
            GetJsonCharStringField(jsOrdERInfo, "clordid", stOrdERInfo.clordid, sizeof(stOrdERInfo.clordid), sErrMsg);  
            GetJsonCharStringField(jsOrdERInfo, "security_id", stOrdERInfo.security_id, sizeof(stOrdERInfo.security_id), sErrMsg);  
            GetJsonUnsignedShortField(jsOrdERInfo, "market_id", stOrdERInfo.market_id, sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "exec_type", stOrdERInfo.exec_type, sErrMsg);  
            GetJsonUnsignedCharField(jsOrdERInfo, "ord_status", stOrdERInfo.ord_status, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "price", stOrdERInfo.price, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "order_qty", stOrdERInfo.order_qty, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "leaves_qty", stOrdERInfo.leaves_qty, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "cum_qty", stOrdERInfo.cum_qty, sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "side", stOrdERInfo.side, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "transact_time", stOrdERInfo.transact_time, sErrMsg);  
            GetJsonCharStringField(jsOrdERInfo, "user_info", stOrdERInfo.user_info, sizeof(stOrdERInfo.user_info), sErrMsg);  
            GetJsonCharStringField(jsOrdERInfo, "exec_id", stOrdERInfo.exec_id, sizeof(stOrdERInfo.exec_id), sErrMsg);  
            GetJsonCharStringField(jsOrdERInfo, "orig_clordid", stOrdERInfo.orig_clordid, sizeof(stOrdERInfo.orig_clordid), sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "ord_type", stOrdERInfo.ord_type, sErrMsg);  
            GetJsonUnsignedShortField(jsOrdERInfo, "ord_rej_reason", stOrdERInfo.ord_rej_reason, sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "time_in_force", stOrdERInfo.time_in_force, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "last_px", stOrdERInfo.last_px, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "last_qty", stOrdERInfo.last_qty, sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "cash_margin", stOrdERInfo.cash_margin, sErrMsg);  
            GetJsonCharField(jsOrdERInfo, "cancel_flag", stOrdERInfo.cancel_flag, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "clordno", stOrdERInfo.clordno, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "orig_clordno", stOrdERInfo.orig_clordno, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "index", stOrdERInfo.index, sErrMsg);  
            GetJsonUnsignedShortField(jsOrdERInfo, "code", stOrdERInfo.code, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "frozen_trade_value", stOrdERInfo.frozen_trade_value, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "frozen_fee", stOrdERInfo.frozen_fee, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "fee", stOrdERInfo.fee, sErrMsg);  
            GetJsonLongLongField(jsOrdERInfo, "total_value_traded", stOrdERInfo.total_value_traded, sErrMsg);  
        } else {
            // 赋值默认参数;    
        }
        
        return true;
    }

    //     //客户登录请求
    // struct LogOnReq
    // {
    // 	TradeOrderUser trade_order_user; //客户信息
    // 	unsigned int heart_bt_int; ///< 心跳检测时间
    // 	char password[101]; ///< 密码
    // 	char client_feature_code[1025]; //客户端特征码
    // };
    bool ParseLogOnReq(LogOnReq& stLogOnReq) {
        if (ResetData() || !m_jsonData.contains("LogOnReq")) {
            std::string sErrMsg;
            njson jsLogOnReq = m_jsonData["LogOnReq"];
            ParseTraderOrderUser(jsLogOnReq, stLogOnReq.trade_order_user);

            GetJsonCharStringField(jsLogOnReq, "password", stLogOnReq.password, sizeof(stLogOnReq.password), sErrMsg);  
            GetJsonCharStringField(jsLogOnReq, "client_feature_code", stLogOnReq.client_feature_code, sizeof(stLogOnReq.client_feature_code), sErrMsg);  

            GetJsonUnsignedIntField(jsLogOnReq, "heart_bt_int", stLogOnReq.heart_bt_int, sErrMsg); 
        } else {
            stLogOnReq.heart_bt_int = 3;
            stLogOnReq.trade_order_user.agw_seq_id = 10001;
            strcpy(stLogOnReq.password, "XXXXXX");
            strcpy(stLogOnReq.trade_order_user.fund_account_id, "XXXXXX");
            strcpy(stLogOnReq.trade_order_user.branch_id, "XXXXXX");
            strcpy(stLogOnReq.trade_order_user.account_id, "XXXXXX");
            strcpy(stLogOnReq.trade_order_user.cust_id, "TestLogin");
            stLogOnReq.trade_order_user.client_seq_id = 1;
        }
        return true;
    }


    // //客户登录应答
    // struct LogOnAns
    // {
    //     TradeOrderUser trade_order_user; //客户信息
    //     int session_status; ///< 会话状态
    //     unsigned int error_code; //错误码
    // };
    bool ParseLogOnAns(LogOnAns& stLogOnAns) {
        if (ResetData() || !m_jsonData.contains("LogOnAns")) {
            std::string sErrMsg;
            njson jsLogOnAns = m_jsonData["LogOnAns"];
            ParseTraderOrderUser(jsLogOnAns, stLogOnAns.trade_order_user);
            GetJsonIntField(jsLogOnAns, "session_status", stLogOnAns.session_status, sErrMsg);  
            GetJsonUnsignedIntField(jsLogOnAns, "error_code", stLogOnAns.error_code, sErrMsg); 
        } else {
            // 赋值默认参数;
            strcpy(stLogOnAns.trade_order_user.fund_account_id, "XXXXXX");
            strcpy(stLogOnAns.trade_order_user.branch_id, "XXXXXX");
            strcpy(stLogOnAns.trade_order_user.account_id, "XXXXXX");
            strcpy(stLogOnAns.trade_order_user.cust_id, "RspLogInSucess");

        }
        return true;
    }

    //     //客户登出请求
    // struct LogOutReq
    // {
    // 	TradeOrderUser trade_order_user; //客户信息
    //     char password[101]; ///< 密码
    // };
    bool ParseLogOutReq(LogOutReq& stLogOutReq) {
        if (ResetData() || !m_jsonData.contains("LogOutReq")) {
            std::string sErrMsg;
            njson jsLogOutReq = m_jsonData["LogOutReq"];
            ParseTraderOrderUser(jsLogOutReq, stLogOutReq.trade_order_user);

            GetJsonCharStringField(jsLogOutReq, "password", stLogOutReq.password, sizeof(stLogOutReq.password), sErrMsg);  

        } else {
            // 赋值默认参数;
        }
        return true;
    }

    //     //客户登出应答
    // struct LogOutAns
    // {
    //     TradeOrderUser trade_order_user; //客户信息
    //     int session_status; ///< 会话状态
    //     unsigned int error_code; ///< 错误码
    // };
    bool ParseLogOutAns(LogOutAns& stLogOutAns) {
        if (ResetData() || !m_jsonData.contains("LogOutAns")) {
            std::string sErrMsg;
            njson jsLogOutAns = m_jsonData["LogOutAns"];
            ParseTraderOrderUser(jsLogOutAns, stLogOutAns.trade_order_user);
            GetJsonIntField(jsLogOutAns, "session_status", stLogOutAns.session_status, sErrMsg);  
            GetJsonUnsignedIntField(jsLogOutAns, "error_code", stLogOutAns.error_code, sErrMsg); 

        } else {
            // 赋值默认参数;
        }
        return true;
    }

    // //委托请求
    // struct TradeOrderReq
    // {
    //     TradeOrderUser trade_order_user; //客户信息
    //     TradeOrderInfo trade_order_info; //委托信息
    // };    
    bool ParseTradeOrderReq(TradeOrderReq& stTradeOrderReq) {
        if (ResetData() || !m_jsonData.contains("TradeOrderReq")) {
            std::string sErrMsg;
            njson jsTradeOrderReq = m_jsonData["TradeOrderReq"];

            ParseTraderOrderUser(jsTradeOrderReq, stTradeOrderReq.trade_order_user);
            ParseTradeOrderInfo(jsTradeOrderReq, stTradeOrderReq.trade_order_info);

        } else {
            strcpy(stTradeOrderReq.trade_order_user.fund_account_id, "Client");
            strcpy(stTradeOrderReq.trade_order_user.branch_id, "XXXXXX");
            strcpy(stTradeOrderReq.trade_order_user.account_id, "XXXXXX");
            strcpy(stTradeOrderReq.trade_order_user.cust_id, "TestOrder");
            stTradeOrderReq.trade_order_user.client_seq_id = 10001;
            stTradeOrderReq.trade_order_info.order_qty = 800;
            stTradeOrderReq.trade_order_info.side = kBuy;
            stTradeOrderReq.trade_order_info.order_type = kLimited;
            stTradeOrderReq.trade_order_info.market_id = kShangHai;
            strcpy(stTradeOrderReq.trade_order_info.security_id, "XXXXXX");
        }
        return true;
    }

    //     //撤单请求
    // struct CancelOrderReq
    // {
    //     TradeOrderUser   trade_order_user; //客户信息
    //     CancelOrderInfo  cancel_order_info; //撤单信息
    // };
    bool ParseCancelOrderReq(CancelOrderReq& stCancelOrderReq) {
        if (ResetData() || !m_jsonData.contains("CancelOrderReq")) {
            std::string sErrMsg;
            njson jsCancelOrderReq = m_jsonData["CancelOrderReq"];

            ParseTraderOrderUser(jsCancelOrderReq, stCancelOrderReq.trade_order_user);
            ParseCancelOrderInfo(jsCancelOrderReq, stCancelOrderReq.cancel_order_info);

        } else {
            strcpy(stCancelOrderReq.trade_order_user.fund_account_id, "Client");
            strcpy(stCancelOrderReq.trade_order_user.branch_id, "XXXXXX");
            strcpy(stCancelOrderReq.trade_order_user.account_id, "XXXXXX");
            strcpy(stCancelOrderReq.trade_order_user.cust_id, "TestCancel");
            stCancelOrderReq.trade_order_user.client_seq_id = 10001;
        }
        return true;
    }

    //     //业务回报
    // struct TradeOrderER
    // {
    //     TradeOrderUser trade_order_user; //客户信息
    //     OrdERInfo order_er_info; //订单信息
    // };
    bool ParseTradeOrderER(TradeOrderER& stTradeOrderER) {
        if (ResetData() || !m_jsonData.contains("TradeOrderER")) {
            std::string sErrMsg;
            njson jsTradeOrderER = m_jsonData["TradeOrderER"];

            ParseTraderOrderUser(jsTradeOrderER, stTradeOrderER.trade_order_user);
            ParseOrdERInfo(jsTradeOrderER, stTradeOrderER.order_er_info);

        } else {
            // 赋值默认参数;
            strcpy(stTradeOrderER.trade_order_user.fund_account_id, "XXXXXX");
            strcpy(stTradeOrderER.trade_order_user.branch_id, "XXXXXX");
            strcpy(stTradeOrderER.trade_order_user.account_id, "XXXXXX");
            strcpy(stTradeOrderER.trade_order_user.cust_id, "OrderRsp");              
        }
        return true;
    }

    //     //拒绝返回
    // struct RejectMsg
    // {
    //     TradeOrderUser trade_order_user; //客户信息
    //     unsigned short reject_reason_code; /// 错误编码
    //     char cancel_flag; //撤单标识
    // };
    bool ParseRejectMsg(RejectMsg& stRejectMsg) {
        if (ResetData() || !m_jsonData.contains("RejectMsg")) {
            std::string sErrMsg;
            njson jsRejectMsg = m_jsonData["RejectMsg"];

            ParseTraderOrderUser(jsRejectMsg, stRejectMsg.trade_order_user);
            GetJsonUnsignedShortField(jsRejectMsg, "reject_reason_code", stRejectMsg.reject_reason_code, sErrMsg);  
            GetJsonCharField(jsRejectMsg, "cancel_flag", stRejectMsg.cancel_flag, sErrMsg);  

        } else {
            // 赋值默认参数;
        }
        return true;
    }    

    std::string m_strSrcJsonFileName;
    njson m_jsonData;

    LogOnReq m_stLogOnReq;
    LogOnAns m_stLogOnAns;
    LogOutReq m_stLogOutReq;
    LogOutAns m_stLogOutAns;
    TradeOrderReq m_stTradeOrderReq;
    CancelOrderReq m_stCancelOrderReq;
    TradeOrderER m_stTradeOrderER;
    RejectMsg m_stRejectMsg;
};
