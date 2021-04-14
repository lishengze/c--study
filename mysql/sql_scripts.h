#pragma once

#include <string>
#include <iostream>
using std::string;
using std::cout;
using std::endl;


inline string get_req_create_order_sql_str(string account_name)
{
    try
    {
        string result = "";
        string table_name = account_name + "_ReqCreateOrder";
        result += "CREATE TABLE IF NOT EXISTS " +  table_name +  string("( \
                    UserName VARCHAR(32), \
                    AccountName VARCHAR(64), \
                    AccountType VARCHAR(32), \
                    ExchangeID VARCHAR(64), \
                    InstrumentID VARCHAR(64),\
                    InstrumentType VARCHAR(64), \
                    Symbol VARCHAR(64), \
                    Price DECIMAL(32, 8), \
                    LeverRate DECIMAL(32, 8), \
                    Volume DECIMAL(32, 8), \
                    Direction VARCHAR(32), \
                    OffsetFlag VARCHAR(32), \
                    OrderLocalID VARCHAR(64), \
                    OrderMaker VARCHAR(32), \
                    OrderType VARCHAR(32), \
                    LandTime BIGINT, \
                    SendTime BIGINT, \
                    StrategyOrderID VARCHAR(64), \
                    OrderMode VARCHAR(32), \
                    AssetType VARCHAR(32), \
                    TradeChannel VARCHAR(64), \
                    OrderXO VARCHAR(32), \
                    PlatformTime BIGINT, \
                    OrderSysID VARCHAR(64), \
                    OrderForeID VARCHAR(64), \
                    CreateTime BIGINT, \
                    ModifyTime BIGINT, \
                    RspLocalTime BIGINT, \
                    Cost DECIMAL(32, 8), \
                    TradePrice DECIMAL(32, 8), \
                    TradeVolume DECIMAL(32, 8), \
                    RemainVolume DECIMAL(32, 8), \
                    TradeValue DECIMAL(32, 8), \
                    OrderStatus VARCHAR(32), \
                    SessionID VARCHAR(64), \
                    RequestID BIGINT, \
                    RequestForeID BIGINT, \
                    Fee DECIMAL(32, 8), \
                    FeeCurrency VARCHAR(32), \
                    ErrorID INTEGER, \
                    ErrorMsg VARCHAR(256), \
                    PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

inline string get_rsp_create_order_sql_str(string account_name)
{
    try
    {        
        string table_name = account_name + " RspCreateOrder";
        string result = string("CREATE TABLE IF NOT EXISTS ") +  table_name +  string("( \
                    UserName VARCHAR(32), \
                    AccountName VARCHAR(64), \
                    AccountType VARCHAR(32), \
                    ExchangeID VARCHAR(64), \
                    InstrumentID VARCHAR(64),\
                    InstrumentType VARCHAR(64), \
                    Symbol VARCHAR(64), \
                    Price DECIMAL(32, 8), \
                    LeverRate DECIMAL(32, 8), \
                    Volume DECIMAL(32, 8), \
                    Direction VARCHAR(32), \
                    OffsetFlag VARCHAR(32), \
                    OrderLocalID VARCHAR(64), \
                    OrderMaker VARCHAR(32), \
                    OrderType VARCHAR(32), \
                    LandTime BIGINT, \
                    SendTime BIGINT, \
                    StrategyOrderID VARCHAR(64), \
                    OrderMode VARCHAR(32), \
                    AssetType VARCHAR(32), \
                    TradeChannel VARCHAR(64), \
                    OrderXO VARCHAR(32), \
                    PlatformTime BIGINT, \
                    OrderSysID VARCHAR(64), \
                    OrderForeID VARCHAR(64), \
                    CreateTime BIGINT, \
                    ModifyTime BIGINT, \
                    RspLocalTime BIGINT, \
                    Cost DECIMAL(32, 8), \
                    TradePrice DECIMAL(32, 8), \
                    TradeVolume DECIMAL(32, 8), \
                    RemainVolume DECIMAL(32, 8), \
                    TradeValue DECIMAL(32, 8), \
                    OrderStatus VARCHAR(32), \
                    SessionID VARCHAR(64), \
                    RequestID BIGINT, \
                    RequestForeID BIGINT, \
                    Fee DECIMAL(32, 8), \
                    FeeCurrency VARCHAR(32), \
                    ErrorID INTEGER, \
                    ErrorMsg VARCHAR(256), \
                    PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");
        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

inline string get_rtn_order_sql_str(string account_name)
{
    try
    {
        
        string table_name = account_name + " RtnOrder";
        string result = string("CREATE TABLE IF NOT EXISTS ") +  table_name +  string("( \
                        UserName VARCHAR(32), \
                        AccountName VARCHAR(64), \
                        AccountType VARCHAR(32), \
                        ExchangeID VARCHAR(64), \
                        InstrumentID VARCHAR(64),\
                        InstrumentType VARCHAR(64), \
                        Symbol VARCHAR(64), \
                        Price DECIMAL(32, 8), \
                        LeverRate DECIMAL(32, 8), \
                        Volume DECIMAL(32, 8), \
                        Direction VARCHAR(32), \
                        OffsetFlag VARCHAR(32), \
                        OrderLocalID VARCHAR(64), \
                        OrderMaker VARCHAR(32), \
                        OrderType VARCHAR(32), \
                        LandTime BIGINT, \
                        SendTime BIGINT, \
                        StrategyOrderID VARCHAR(64), \
                        OrderMode VARCHAR(32), \
                        AssetType VARCHAR(32), \
                        TradeChannel VARCHAR(64), \
                        OrderXO VARCHAR(32), \
                        PlatformTime BIGINT, \
                        OrderSysID VARCHAR(64), \
                        OrderForeID VARCHAR(64), \
                        CreateTime BIGINT, \
                        ModifyTime BIGINT, \
                        RspLocalTime BIGINT, \
                        Cost DECIMAL(32, 8), \
                        TradePrice DECIMAL(32, 8), \
                        TradeVolume DECIMAL(32, 8), \
                        RemainVolume DECIMAL(32, 8), \
                        TradeValue DECIMAL(32, 8), \
                        OrderStatus VARCHAR(32), \
                        SessionID VARCHAR(64), \
                        RequestID BIGINT, \
                        RequestForeID BIGINT, \
                        Fee DECIMAL(32, 8), \
                        FeeCurrency VARCHAR(32), \
                        ErrorID INTEGER, \
                        ErrorMsg VARCHAR(256), \
                        PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

inline string get_rtn_trade_sql_str(string account_name)
{
    try
    {        
        string table_name = account_name + " RtnTrade";
        string result = "";

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

inline string get_req_cancel_order_sql_str(string account_name)
{
    try
    {
        string result = "";
        string table_name = account_name + " ReqCancelOrder";

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

inline string get_rsp_cancel_order_sql_str(string account_name)
{
    try
    {
        string result = "";
        string table_name = account_name + " RspCancelOrder";

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}