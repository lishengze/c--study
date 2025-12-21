#include "../bridge_declare.h"
#include "../Log/log.h"

#define ORDER_TABLENAME     "orders"
#define TRADE_TABLENAME     "trades"
#define CANCEL_REQUEST_TABLENAME   "cancel_requests"

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

inline string insert_order_str()
{
    try
    {
        string result = string("INSERT INTO ") + string(ORDER_TABLENAME) + string(" VALUES (") + get_qm_string(39) +");";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }  
}

inline string insert_trade_str()
{
    try
    {
        string result = string("INSERT INTO ") + string(TRADE_TABLENAME) + string(" VALUES (") + get_qm_string(25) +");";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }  
}

inline string insert_cancel_req_str()
{
    try
    {
        string result = string("INSERT INTO ") + string(CANCEL_REQUEST_TABLENAME) + string(" VALUES (") + get_qm_string(13) +");";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }  
}

inline string update_order_by_localID_sql_str()
{
    try
    {
        string result = string("UPDATE ") + string(ORDER_TABLENAME) 
                      +  " SET UserName=?,AccountName=?,AccountType=?,\
                            ExchangeID=?,InstrumentID=?,InstrumentType=?,\
                            Symbol=?,Price=?,LeverRate=?,\
                            Volume=?,Direction=?,OffsetFlag=?,\
                            OrderLocalID=?,OrderMaker=?,OrderType=?,\
                            LandTime=?,SendTime=?,StrategyOrderID=?,\
                            OrderMode=?,AssetType=?,TradeChannel=?,\
                            OrderXO=?,PlatformTime=?,OrderSysID=?,\
                            OrderForeID=?,CreateTime=?,ModifyTime=?,\
                            RspLocalTime=?,Cost=?,TradePrice=?,\
                            TradeVolume=?,RemainVolume=?,TradeValue=?,\
                            OrderStatus=?,SessionID=?,RequestID=?,\
                            RequestForeID=?,Fee=?,FeeCurrency=?,\
                            WHERE OrderLocalID=?;";
        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }  
}

inline string update_order_by_SYSID_sql_str()
{
    try
    {
        string result = string("UPDATE ") + string(ORDER_TABLENAME) 
                      +  " SET UserName=?,AccountName=?,AccountType=?,\
                            ExchangeID=?,InstrumentID=?,InstrumentType=?,\
                            Symbol=?,Price=?,LeverRate=?,\
                            Volume=?,Direction=?,OffsetFlag=?,\
                            OrderLocalID=?,OrderMaker=?,OrderType=?,\
                            LandTime=?,SendTime=?,StrategyOrderID=?,\
                            OrderMode=?,AssetType=?,TradeChannel=?,\
                            OrderXO=?,PlatformTime=?,OrderSysID=?,\
                            OrderForeID=?,CreateTime=?,ModifyTime=?,\
                            RspLocalTime=?,Cost=?,TradePrice=?,\
                            TradeVolume=?,RemainVolume=?,TradeValue=?,\
                            OrderStatus=?,SessionID=?,RequestID=?,\
                            RequestForeID=?,Fee=?,FeeCurrency=?,\
                            WHERE OrderSysID=?;";
        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }  
}


inline string create_order_simple_sql_str()
{
    try
    {
        string result = "CREATE TABLE " ORDER_TABLENAME "(\
                        OrderLocalID TEXT, \
                        UserName TEXT, \
                        SessionID TEXT, \
                        RequestID INTEGER, \
                        ExchangeID TEXT,\
                        InstrumentID TEXT, \
                        Price REAL, \
                        Volume REAL, \
                        Direction TEXT, \
                        OrderMaker TEXT, \
                        OrderType TEXT, \
                        OrderSysID TEXT, \
                        OrderStatus TEXT, \
                        TradePrice REAL, \
                        TradeVolume REAL, \
                        ExpectVolume REAL, \
                        Fee REAL, \
                        FeeCurrency TEXT, \
                        CreateTime TEXT, \
                        ModifyTime TEXT, \
                        LastCheckpoint INTEGER ,\
                        AccountName TEXT ,\
                        OrderForeID \
                    );";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

inline string create_order_sql_str()
{
    string result = "CREATE TABLE " ORDER_TABLENAME "(\
                    UserName TEXT, \                
                    AccountName TEXT,\
                    AccountType TEXT,\
                    ExchangeID TEXT, \
                    InstrumentID TEXT, \
                    InstrumentType TEXT, \
                    Symbol TEXT,\
                    Price REAL,\
                    LeverRate REAL,\
                    Volume REAL, \
                    Direction TEXT,\
                    OffsetFlag TEST,\
                    OrderLocalID TEXT, \
                    OrderMaker TEXT, \
                    OrderType TEXT, \
                    LandTime INTEGER, \
                    SendTime INTEGER,\
                    StrategyOrderID TEXT,\
                    OrderMode TEXT, \
                    AssetType TEXT, \
                    TradeChannel TEXT, \
                    OrderXO TEXT, \
                    PlatformTime INTEGER, \
                    OrderSysID TEXT, \
                    OrderForeID TEXT, \
                    CreateTime INTEGER, \
                    ModifyTime INTEGER, \
                    RspLocalTime INTEGER, \
                    Cost REAL, \
                    TradePrice REAL, \
                    TradeVolume REAL, \
                    RemainVolume REAL, \
                    TradeValue REAL, \
                    OrderStatus TEXT,\
                    SessionID TEXT, \
                    RequestID INTEGER, \
                    RequestForeID INTEGER, \
                    Fee REAL, \
                    FeeCurrency TEXT";

    return result;
}

inline string create_trade_simple_sql_str()
{
    try
    {
        string result = "CREATE TABLE " TRADE_TABLENAME "(\
                            TradeID TEXT, \
                            ExecuteTimestamp INTEGER \
                        );";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

inline string create_trade_sql_str()
{
    try
    {
        string result = "CREATE TABLE " TRADE_TABLENAME "(\
                            UserName TEXT, \
                            AccountName TEXT, \
                            InternalAccountName TEXT,\
                            AccountType TEXT,\
                            TradeSubID TEXT, \
                            TradeID TEXT, \
                            OrderSysID TEXT,\
                            ExchangeID TEXT,\
                            InstrumentID TEXT,\
                            MatchPrice REAL,\
                            MatchVolume REAL,\
                            MatchValue REAL,\
                            Direction TEXT,\
                            OrderLocalID TEXT, \
                            Fee REAL,\
                            FeeCurrency REAL,\
                            PlatformTime TEXT,\
                            TradeTime TEXT,\
                            RspLocalTime TEXT,\
                            Price REAL, \
                            StrategyOrderID TEXT,\
                            OrderMaker TEXT,\
                            AssetType TEXT,\
                            TradeChannel TEXT,\
                            SessionID TEXT\
                        );";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

inline string create_cancel_simple_sql_str()
{
    try
    {
        string result = "CREATE TABLE " CANCEL_REQUEST_TABLENAME "(\
                RequestID INTEGER, \
                OrderLocalID TEXT, \
                OrderSysID TEXT, \
                UserName TEXT, \
                SessionID TEXT, \
                AccountName \
            );";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

inline string create_cancel_sql_str()
{
    try
    {
        string result = "CREATE TABLE " CANCEL_REQUEST_TABLENAME "(\
                            UserName TEXT, \
                            AccountName TEXT, \
                            AccountType TEXT, \
                            Symbol TEXT,\
                            AssetType TEXT,\
                            OrderLocalID TEXT,\
                            OrderForeID TEXT, \
                            OrderSysID TEXT,\
                            ExchangeID TEXT,\
                            StrategyOrderID TEXT,\
                            TradeChannel TEXT,\
                            SendTime INTEGER,\
                            SessionID TEXT \
                            );";

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

