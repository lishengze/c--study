#pragma once

#include <string>
#include <iostream>
#include <sstream>>
using std::string;
using std::cout;
using std::endl;


inline string get_qm_string(int qm_count)
{
    try
    {
        string result = "";

        for (int i = 0; i < qm_count; ++i)
        {
            result += " ?,";
        }
        result.erase(result.length()-1);
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"get_qm_string " << e.what() << '\n';
    }
}

inline string char_to_string(char ori_char)
{
    try
    {
        std::stringstream s_obj;
        s_obj << ori_char;
        return s_obj.str();
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_req_create_order " << e.what() << '\n';
    }    
}

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
                    RequestID VARCHAR(64), \
                    RequestForeID VARCHAR(64), \
                    Fee DECIMAL(32, 8), \
                    FeeCurrency VARCHAR(32), \
                    PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        // cout << "create sql: " << result << endl;

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << "get_req_create_order_sql_str: " << e.what() << '\n';
    }
}

inline string insert_req_create_order_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_ReqCreateOrder";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(39) +");";
        cout << result << endl;
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "insert_req_create_order " << e.what() << '\n';
    }    
}

inline string select_req_create_order_by_time(string account_name, unsigned long start_time, unsigned long end_time)
{
    try 
    {
        string table_name = account_name + "_ReqCreateOrder";
        string result = string("select * from ") + table_name 
                      + " where SendTime>" + std::to_string(start_time) 
                      + " and SendTime<" + std::to_string(end_time)
                      + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_req_create_order_by_time " << e.what() << '\n';
    }        
}


inline string get_rsp_create_order_sql_str(string account_name)
{
    try
    {        
        string table_name = account_name + "_RspCreateOrder";
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
                    RequestID VARCHAR(64), \
                    RequestForeID VARCHAR(64), \
                    Fee DECIMAL(32, 8), \
                    FeeCurrency VARCHAR(32), \
                    ErrorID INTEGER, \
                    ErrorMsg VARCHAR(256), \
                    PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");
        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << "get_rsp_create_order_sql_str " << e.what() << '\n';
    }
}

inline string insert_rsp_create_order_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_RspCreateOrder";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(41) +");";
        cout << result << endl;
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "insert_rsp_create_order " << e.what() << '\n';
    }    
}

inline string select_rsp_create_order_by_orderlocalid(string account_name, string orderlocalid)
{
    try 
    {
        string table_name = account_name + "_RspCreateOrder";
        string result = string("select * from ") + table_name 
                      + " where OrderLocalID=" + orderlocalid + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_rsp_create_order_by_orderlocalid " << e.what() << '\n';
    }        
}


inline string get_rtn_order_sql_str(string account_name)
{
    try
    {
        
        string table_name = account_name + "_RtnOrder";
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
                        RequestID VARCHAR(64), \
                        RequestForeID VARCHAR(64), \
                        Fee DECIMAL(32, 8), \
                        FeeCurrency VARCHAR(32), \
                        ErrorID INTEGER, \
                        ErrorMsg VARCHAR(256), \
                        PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr <<"get_rtn_order_sql_str " << e.what() << '\n';
    }
}

inline string insert_rtn_order_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_RtnOrder";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(41) +");";
        cout << result << endl;
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "insert_rtn_order_order " << e.what() << '\n';
    }    
}

inline string select_rtn_order_by_orderlocalid(string account_name, string orderlocalid)
{
    try 
    {
        string table_name = account_name + "_RtnOrder";
        string result = string("select * from ") + table_name 
                      + " where OrderLocalID=" + orderlocalid + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_rtn_order_by_orderlocalid " << e.what() << '\n';
    }        
}


inline string get_rtn_trade_sql_str(string account_name)
{
    try
    {        
        string table_name = account_name + "_RtnTrade";
        string result = string("CREATE TABLE IF NOT EXISTS ") +  table_name +  string("( \
                        UserName VARCHAR(32), \
                        AccountName VARCHAR(64), \
                        AccountType VARCHAR(32), \
                        InternalAccountName VARCHAR(64), \
                        TradeID VARCHAR(128), \
                        OrderSysID VARCHAR(64), \
                        ExchangeID VARCHAR(64), \
                        InstrumentID VARCHAR(64),\
                        MatchPrice DECIMAL(32,8), \
                        MatchVolume DECIMAL(32,8), \
                        MatchValue DECIMAL(32, 8), \
                        Direction VARCHAR(32), \
                        OrderLocalID VARCHAR(64), \
                        Fee DECIMAL(32, 8), \
                        FeeCurrency VARCHAR(32), \
                        PlatformTime VARCHAR(64), \
                        TradeTime VARCHAR(64), \
                        RspLocalTime BIGINT, \
                        Price DECIMAL(32,8), \
                        StrategyOrderID VARCHAR(64), \
                        OrderMaker VARCHAR(32), \
                        AssetType VARCHAR(32), \
                        TradeChannel VARCHAR(64), \
                        SessionID VARCHAR(64), \
                        ErrorID INTEGER, \
                        ErrorMsg VARCHAR(256), \
                        PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << "get_rtn_trade_sql_str " << e.what() << '\n';
    }
}

inline string insert_rtn_trade_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_RtnTrade";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(26) +");";
        cout << result << endl;
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"insert_rtn_trade " << e.what() << '\n';
    }    
}

inline string select_rtn_trade_by_orderlocalid(string account_name, string orderlocalid)
{
    try 
    {
        string table_name = account_name + "_RtnTrade";
        string result = string("select * from ") + table_name 
                      + " where OrderLocalID=" + orderlocalid + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_req_create_order_by_time " << e.what() << '\n';
    }        
}


inline string get_req_cancel_order_sql_str(string account_name)
{
    try
    {
        string table_name = account_name + "_ReqCancelOrder";
        string result = "CREATE TABLE IF NOT EXISTS " +  table_name +  string("( \
                        UserName VARCHAR(32), \
                        AccountName VARCHAR(64), \
                        AccountType VARCHAR(32), \
                        Symbol VARCHAR(64), \
                        AssetType VARCHAR(32), \
                        OrderLocalID VARCHAR(64), \
                        OrderSysID VARCHAR(64), \
                        OrderForeID VARCHAR(64), \
                        ExchangeID VARCHAR(64), \
                        StrategyOrderID VARCHAR(64), \
                        TradeChannel VARCHAR(64), \
                        SendTime BIGINT, \
                        PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");
        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << "get_req_cancel_order_sql_str: " <<  e.what() << '\n';
    }
}

inline string insert_req_cancel_order_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_ReqCancelOrder";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(12) +");";
        cout << result << endl;
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "insert_req_cancel_order " <<  e.what() << '\n';
    }    
}

inline string select_req_cancel_order_by_time(string account_name, unsigned long start_time, unsigned long end_time)
{
    try 
    {
        string table_name = account_name + "_ReqCancelOrder";
        string result = string("select * from ") + table_name 
                      + " where SendTime>" + std::to_string(start_time) 
                      + " and SendTime<" + std::to_string(end_time)
                      + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_req_create_order_by_time " << e.what() << '\n';
    }        
}


inline string get_rsp_cancel_order_sql_str(string account_name)
{
    try
    {        
        string table_name = account_name + "_RspCancelOrder";
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
                    RequestID VARCHAR(64), \
                    RequestForeID VARCHAR(64), \
                    Fee DECIMAL(32, 8), \
                    FeeCurrency VARCHAR(32), \
                    ErrorID INTEGER, \
                    ErrorMsg VARCHAR(256), \
                    PRIMARY KEY (`AccountName`,`OrderLocalID`)) DEFAULT CHARSET utf8;");

        return result; 
    }
    catch(const std::exception& e)
    {
        std::cerr << "get_rsp_cancel_order_sql_str "  << e.what() << '\n';
    }
}

inline string insert_rsp_cancel_order_sql(string account_name)
{
    try
    {
        string table_name = account_name + "_RspCancelOrder";
        string result = string("INSERT INTO ") + table_name + " VALUES (" + get_qm_string(41) +");";
        cout << result << endl;

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "insert_rsp_cancel_order " << e.what() << '\n';
    }    
}

inline string select_rsp_cancel_order_by_orderlocalid(string account_name, string orderlocalid)
{
    try 
    {
        string table_name = account_name + "_RspCancelOrder";
        string result = string("select * from ") + table_name 
                      + " where OrderLocalID=" + orderlocalid + ";";
        cout << result << endl;
        return result;        
    }
    catch(const std::exception& e)
    {
        std::cerr << "select_rsp_cancel_order_by_orderlocalid " << e.what() << '\n';
    }        
}