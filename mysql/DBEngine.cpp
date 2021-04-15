#include "DBEngine.h"

#include <boost/smart_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "sql_scripts.h"
#include "quark/cxx/ut/UtPrintUtils.h"

void DBEngine::connect_mysql()
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        conn_ = driver->connect(host_, usr_, pwd_);
        
        if (conn_ == NULL)
        {
            cout << "conn is null" << endl;
        }
        cout << "connect suceess" << endl;    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void DBEngine::connect_mysql_schema()
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();

        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = host_;
        connection_properties["userName"] = usr_;
        connection_properties["password"] = pwd_;
        connection_properties["schema"] = schema_;
        connection_properties["port"] = port_;
        connection_properties["OPT_RECONNECT"] = true;

        conn_ = driver->connect(connection_properties);
        
        if (conn_ == NULL)
        {
            cout << "conn is null" << endl;
        }
        cout << "connect suceess" << endl;  
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void DBEngine::create_table(string account_name)
{
    try
    {
        cout << "DBEngine::create_table " << account_name << endl;

        if (conn_)
        {
            boost::scoped_ptr<sql::Statement> stmt1(conn_->createStatement());
            if (!stmt1->execute(get_req_create_order_sql_str(account_name)))
            {
                cout << "create req_create_table " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt2(conn_->createStatement());
            if (!stmt2->execute(get_rsp_create_order_sql_str(account_name)))
            {
                cout << "create rsp_create_table " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt3(conn_->createStatement());
            if (!stmt3->execute(get_rtn_order_sql_str(account_name)))
            {
                cout << "create rtn_order " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt4(conn_->createStatement());
            if ( !stmt4->execute(get_rtn_trade_sql_str(account_name)) )
            {
                cout << "create rtn_trade " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt5(conn_->createStatement());
            if (!stmt5->execute(get_req_cancel_order_sql_str(account_name)))
            {
                cout << "create req_cancel_order " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt6(conn_->createStatement());
            if ( !stmt6->execute(get_rsp_cancel_order_sql_str(account_name)) )
            {
                cout << "create rsp_cancel_order " << account_name << " failed!" << endl;
            }
        }
        else
        {
            cout << "db was not connected!" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void DBEngine::prepare_statement(string account_name)
{
    try
    {
        cout << "prepare_statement: " << account_name << endl;
        
        if (conn_)
        {
            if (accout_preparestmt_map_.find(account_name) == accout_preparestmt_map_.end())
            {
                accout_preparestmt_map_[account_name] = PrepareSMT();
            }

            if (!accout_preparestmt_map_[account_name].is_prepared())
            {
                accout_preparestmt_map_[account_name].prepare(conn_, account_name);
            }            
        }
        else
        {
            std::cout << "DB was Not Connected!" << std::endl;
        }
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << "DBEngine::prepare_statement: " << e.what() << '\n';
    }
}

// bool DBEngine::check_db(string db_name)
// {
//     try
//     {
//         bool result = false;

//         string check_db_sql = string("select * from information_schema.SCHEMATA where SCHEMA_NAME = '") + db_name + "'; ";

//         sql::Statement* state;
//         sql::ResultSet* sql_result;

//         cout << "check_db_sql: " << check_db_sql << endl;

//         if (!conn_)
//         {
//             std::cout << "Database was not connected!" << std::endl;
//             return result;
//         }

//         state = conn_->createStatement();

//         sql_result = state->executeQuery(check_db_sql);

//         if (!sql_result)
//         {
//             cout << "db: " << " does not exits!" << endl;
//             return false;
//         }
//         else
//         {
//             result = !(sql_result->wasNull());
//         }

//         cout << db_name << " " << result << endl;

//         return result;
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
    
// }

bool DBEngine::check_db(string db_name)
{
    try
    {
        bool result = false;

        string get_all_db_sql = "show databases;";

        sql::Statement* state;
        sql::ResultSet* sql_result;

        cout << "get_all_db_sql: " << get_all_db_sql << endl;

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
            return result;
        }

        state = conn_->createStatement();

        sql_result = state->executeQuery(get_all_db_sql);

        if (!sql_result)
        {
            cout << "db: " << " does not exits!" << endl;
            return false;
        }
        else
        {
            std::cout << "All DB: " << std::endl;
            while(sql_result->next())
            {
                // std::cout << result->getString(0).c_str() << " " << result->getString(1).c_str() << " " << result->getString(2).c_str() << std::endl;

                std::cout << sql_result->getString(1).c_str() << std::endl;

                if (db_name == sql_result->getString(1).asStdString())
                {
                    result = true;
                    break;
                }

                // std::cout << ++i << std::endl;
            }
            std::cout << std::endl;
        }

        cout << db_name << " " << result << endl;

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void DBEngine::create_db(string db_name)
{
    try
    {
        // string sql_str = "create databss";

        // string check_db_sql = string("select * from information_schema.SCHEMATA where SCHEMA_NAME = '") + db_name + "'; ";
        
        // string get_all_db_sql = "select * from information_schema.SCHEMATA;";

        // // string get_all_db_sql = "show databases;";


        // sql::Statement* state;
        // sql::ResultSet* result;

        // cout << "get_all_db_sql: " << get_all_db_sql << endl;

        // if (!conn_)
        // {
        //     std::cout << "Database was not connected!" << std::endl;
        //     return;
        // }

        // state = conn_->createStatement();
        // result = state->executeQuery(get_all_db_sql);

        // int i = 0;
        
        // while(result->next())
        // {
        //     // std::cout << result->getString(0).c_str() << " " << result->getString(1).c_str() << " " << result->getString(2).c_str() << std::endl;

        //     std::cout << result->getString(1).c_str() << std::endl;

        //     std::cout << ++i << std::endl;
        // }

        if (!check_db(db_name))
        {
            string create_db_sql = "create database " + db_name + ";";
            sql::ResultSet* result;

            cout << "create_db_sql: " << create_db_sql << endl;

            sql::Statement* state = conn_->createStatement();
            bool exc_result = state->execute(create_db_sql);
            if (!exc_result)
            {
                cout << " create db " << db_name << " failed!" << endl;
            }
            else
            {
                cout << "Set Schema: " << db_name << endl;
                conn_->setSchema(db_name);                
            }

            // cout << "create over" << endl;

            // if (check_db(db_name))
            // {
            //     cout << "Set Schema: " << db_name << endl;
            //     conn_->setSchema(db_name);
            // }

            // cout << "check over" << endl;

            // while(result->next())
            // {
            //     std::cout << result->getString(1).c_str() << std::endl;
            // }

            // conn_->setSchema(db_name);
        }
        else
        {
            cout << "Set Schema: " << db_name << endl;
            conn_->setSchema(db_name);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}

void DBEngine::insert_req_create_order(const CUTReqCreateOrderField& reqCreateOrder)
{
    try
    {
       prepare_statement(reqCreateOrder.AccountName);

       printUTData(&reqCreateOrder, UT_FID_ReqCreateOrder);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[reqCreateOrder.AccountName].stmtInsertReqCreateOrder_;
       stmt->setString(1, reqCreateOrder.UserName);
       stmt->setString(2, reqCreateOrder.AccountName);
       stmt->setString(3, char_to_string(reqCreateOrder.AccountType));
       stmt->setString(4, reqCreateOrder.ExchangeID);
       stmt->setString(5, reqCreateOrder.InstrumentID);
       stmt->setString(6, reqCreateOrder.InstrumentType);
       stmt->setString(7, reqCreateOrder.Symbol);
       stmt->setDouble(8, reqCreateOrder.Price);
       stmt->setDouble(9, reqCreateOrder.LeverRate);
       stmt->setDouble(10, reqCreateOrder.Volume);
       stmt->setString(11, char_to_string(reqCreateOrder.Direction));
       stmt->setString(12, char_to_string(reqCreateOrder.OffsetFlag));
       stmt->setString(13, reqCreateOrder.OrderLocalID);
       stmt->setString(14, char_to_string(reqCreateOrder.OrderMaker));
       stmt->setString(15, char_to_string(reqCreateOrder.OrderType));
       stmt->setBigInt(16, std::to_string(reqCreateOrder.LandTime));
       stmt->setBigInt(17, std::to_string(reqCreateOrder.SendTime));
       stmt->setString(18, reqCreateOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(reqCreateOrder.OrderMode));
       stmt->setString(20, char_to_string(reqCreateOrder.AssetType));
       stmt->setString(21, reqCreateOrder.TradeChannel);
       stmt->setString(22, char_to_string(reqCreateOrder.OrderXO));
       stmt->setBigInt(23, reqCreateOrder.PlatformTime);
       stmt->setString(24, reqCreateOrder.OrderSysID);
       stmt->setString(25, reqCreateOrder.OrderForeID);
       stmt->setBigInt(26, reqCreateOrder.CreateTime);
       stmt->setBigInt(27, reqCreateOrder.ModifyTime);
       stmt->setBigInt(28, reqCreateOrder.RspLocalTime);
       stmt->setDouble(39, reqCreateOrder.Cost);
       stmt->setDouble(30, reqCreateOrder.TradePrice);
       stmt->setDouble(31, reqCreateOrder.TradeVolume);
       stmt->setDouble(32, reqCreateOrder.RemainVolume);
       stmt->setDouble(33, reqCreateOrder.TradeValue);
       stmt->setString(34, char_to_string(reqCreateOrder.OrderStatus));
       stmt->setString(35, reqCreateOrder.SessionID);
       stmt->setString(36, std::to_string(reqCreateOrder.RequestID));        
       stmt->setString(37, std::to_string(reqCreateOrder.RequestForeID));        
       stmt->setDouble(38, reqCreateOrder.Fee);      
       stmt->setString(39, reqCreateOrder.FeeCurrency);              

       if (!stmt->execute())
       {
           cout << "Insert ReqCreateOrder: " << reqCreateOrder.UserName << " " << reqCreateOrder.AccountName << " " << reqCreateOrder.OrderLocalID << " Failed" << endl;
       }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_req_create_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rsp_create_order(const CUTRspCreateOrderField& rspCreateOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rspCreateOrder.AccountName);

       printUTData(&rspCreateOrder, UT_FID_RspCreateOrder);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[rspCreateOrder.AccountName].stmtInsertRspCreateOrder_;

       stmt->setString(1, rspCreateOrder.UserName);
       stmt->setString(2, rspCreateOrder.AccountName);
       stmt->setString(3, char_to_string(rspCreateOrder.AccountType));
       stmt->setString(4, rspCreateOrder.ExchangeID);
       stmt->setString(5, rspCreateOrder.InstrumentID);
       stmt->setString(6, rspCreateOrder.InstrumentType);
       stmt->setString(7, rspCreateOrder.Symbol);
       stmt->setDouble(8, rspCreateOrder.Price);
       stmt->setDouble(9, rspCreateOrder.LeverRate);
       stmt->setDouble(10, rspCreateOrder.Volume);
       stmt->setString(11, char_to_string(rspCreateOrder.Direction));
       stmt->setString(12, char_to_string(rspCreateOrder.OffsetFlag));
       stmt->setString(13, rspCreateOrder.OrderLocalID);
       stmt->setString(14, char_to_string(rspCreateOrder.OrderMaker));
       stmt->setString(15, char_to_string(rspCreateOrder.OrderType));
       stmt->setBigInt(16, std::to_string(rspCreateOrder.LandTime));
       stmt->setBigInt(17, std::to_string(rspCreateOrder.SendTime));
       stmt->setString(18, rspCreateOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rspCreateOrder.OrderMode));
       stmt->setString(20, char_to_string(rspCreateOrder.AssetType));
       stmt->setString(21, rspCreateOrder.TradeChannel);
       stmt->setString(22, char_to_string(rspCreateOrder.OrderXO));
       stmt->setBigInt(23, rspCreateOrder.PlatformTime);
       stmt->setString(24, rspCreateOrder.OrderSysID);
       stmt->setString(25, rspCreateOrder.OrderForeID);
       stmt->setBigInt(26, rspCreateOrder.CreateTime);
       stmt->setBigInt(27, rspCreateOrder.ModifyTime);
       stmt->setBigInt(28, rspCreateOrder.RspLocalTime);
       stmt->setDouble(39, rspCreateOrder.Cost);
       stmt->setDouble(30, rspCreateOrder.TradePrice);
       stmt->setDouble(31, rspCreateOrder.TradeVolume);
       stmt->setDouble(32, rspCreateOrder.RemainVolume);
       stmt->setDouble(33, rspCreateOrder.TradeValue);
       stmt->setString(34, char_to_string(rspCreateOrder.OrderStatus));
       stmt->setString(35, rspCreateOrder.SessionID);
       stmt->setString(36, std::to_string(rspCreateOrder.RequestID));        
       stmt->setString(37, std::to_string(rspCreateOrder.RequestForeID));        
       stmt->setDouble(38, rspCreateOrder.Fee);      
       stmt->setString(39, rspCreateOrder.FeeCurrency);  
       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);              

       if (!stmt->execute())
       {
           cout << "Insert RspCreateOrder: " << rspCreateOrder.UserName << " " << rspCreateOrder.AccountName << " " << rspCreateOrder.OrderLocalID << " Failed" << endl;
       }       
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_rsp_create_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rtn_order(const CUTRtnOrderField& rtnOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rtnOrder.AccountName);

       printUTData(&rtnOrder, UT_FID_RtnOrder);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[rtnOrder.AccountName].stmtInsertRtnOrder;

       stmt->setString(1, rtnOrder.UserName);
       stmt->setString(2, rtnOrder.AccountName);
       stmt->setString(3, char_to_string(rtnOrder.AccountType));
       stmt->setString(4, rtnOrder.ExchangeID);
       stmt->setString(5, rtnOrder.InstrumentID);
       stmt->setString(6, rtnOrder.InstrumentType);
       stmt->setString(7, rtnOrder.Symbol);
       stmt->setDouble(8, rtnOrder.Price);
       stmt->setDouble(9, rtnOrder.LeverRate);
       stmt->setDouble(10, rtnOrder.Volume);
       stmt->setString(11, char_to_string(rtnOrder.Direction));
       stmt->setString(12, char_to_string(rtnOrder.OffsetFlag));
       stmt->setString(13, rtnOrder.OrderLocalID);
       stmt->setString(14, char_to_string(rtnOrder.OrderMaker));
       stmt->setString(15, char_to_string(rtnOrder.OrderType));
       stmt->setBigInt(16, std::to_string(rtnOrder.LandTime));
       stmt->setBigInt(17, std::to_string(rtnOrder.SendTime));
       stmt->setString(18, rtnOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rtnOrder.OrderMode));
       stmt->setString(20, char_to_string(rtnOrder.AssetType));
       stmt->setString(21, rtnOrder.TradeChannel);
       stmt->setString(22, char_to_string(rtnOrder.OrderXO));
       stmt->setBigInt(23, rtnOrder.PlatformTime);
       stmt->setString(24, rtnOrder.OrderSysID);
       stmt->setString(25, rtnOrder.OrderForeID);
       stmt->setBigInt(26, rtnOrder.CreateTime);
       stmt->setBigInt(27, rtnOrder.ModifyTime);
       stmt->setBigInt(28, rtnOrder.RspLocalTime);
       stmt->setDouble(39, rtnOrder.Cost);
       stmt->setDouble(30, rtnOrder.TradePrice);
       stmt->setDouble(31, rtnOrder.TradeVolume);
       stmt->setDouble(32, rtnOrder.RemainVolume);
       stmt->setDouble(33, rtnOrder.TradeValue);
       stmt->setString(34, char_to_string(rtnOrder.OrderStatus));
       stmt->setString(35, rtnOrder.SessionID);
       stmt->setString(36, std::to_string(rtnOrder.RequestID));        
       stmt->setString(37, std::to_string(rtnOrder.RequestForeID));        
       stmt->setDouble(38, rtnOrder.Fee);      
       stmt->setString(39, rtnOrder.FeeCurrency);  
       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);    

       if (!stmt->execute())
       {
           cout << "Insert RtnOrder: " << rtnOrder.UserName << " " << rtnOrder.AccountName << " " << rtnOrder.OrderLocalID << " Failed" << endl;
       }    
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_rtn_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rtn_trade(const CUTRtnTradeField& rtnTrade, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rtnTrade.AccountName);

       printUTData(&rtnTrade, UT_FID_RtnTrade);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[rtnTrade.AccountName].stmtInsertRtnTrade;

       stmt->setString(1, rtnTrade.UserName);
       stmt->setString(2, rtnTrade.AccountName);
       stmt->setString(3, char_to_string(rtnTrade.AccountType));
       stmt->setString(4, rtnTrade.InternalAccountName);
       stmt->setString(5, rtnTrade.TradeID);
       stmt->setString(6, rtnTrade.ExchangeID);
       stmt->setString(7, rtnTrade.InstrumentID);
       
       stmt->setDouble(8, rtnTrade.MatchPrice);
       stmt->setDouble(9, rtnTrade.MatchVolume);
       stmt->setDouble(10, rtnTrade.MatchValue);
       stmt->setString(11, char_to_string(rtnTrade.Direction));
       stmt->setString(12, rtnTrade.OrderLocalID);
       stmt->setDouble(13, rtnTrade.Fee);
       stmt->setString(14, rtnTrade.FeeCurrency);
       stmt->setString(15, rtnTrade.PlatformTime);
       stmt->setString(16, rtnTrade.TradeTime);
       stmt->setBigInt(17, rtnTrade.RspLocalTime);
       stmt->setDouble(18, rtnTrade.Price);
       stmt->setString(19, rtnTrade.TradeChannel);
       stmt->setString(20, rtnTrade.StrategyOrderID);
       stmt->setString(21, char_to_string(rtnTrade.OrderMaker));
       stmt->setString(22, char_to_string(rtnTrade.AssetType));
       stmt->setString(23, rtnTrade.TradeChannel);
       stmt->setString(24, rtnTrade.SessionID);

       stmt->setInt(23, rspInfoField.ErrorID);
       stmt->setString(24, rspInfoField.ErrorMsg);       

       if (!stmt->execute())
       {
           cout << "Insert RtnTrade: " << rtnTrade.UserName << " " << rtnTrade.AccountName << " " << rtnTrade.OrderLocalID << " Failed" << endl;
       }   
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_rtn_trade " << e.what() << '\n';
    }
}

void DBEngine::insert_req_cancel_order(const CUTReqCancelOrderField& reqCancelOrder)
{
    try
    {
       
       prepare_statement(reqCancelOrder.AccountName);

       printUTData(&reqCancelOrder, UT_FID_ReqCancelOrder);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[reqCancelOrder.AccountName].stmtInsertReqCancelOrder_;

       stmt->setString(1, reqCancelOrder.UserName);
       stmt->setString(2, reqCancelOrder.AccountName);
       stmt->setString(3, char_to_string(reqCancelOrder.AccountType));
       stmt->setString(4, reqCancelOrder.Symbol);
       stmt->setString(5, char_to_string(reqCancelOrder.AssetType));
       stmt->setString(6, reqCancelOrder.OrderLocalID);
       stmt->setString(7, reqCancelOrder.OrderSysID);
       stmt->setString(8, reqCancelOrder.OrderForeID);
       stmt->setString(9, reqCancelOrder.ExchangeID);
       stmt->setString(10, reqCancelOrder.StrategyOrderID);
       stmt->setString(11, reqCancelOrder.TradeChannel);
       stmt->setBigInt(12, std::to_string(reqCancelOrder.SendTime));

       if (!stmt->execute())
       {
           cout << "Insert ReqCancelOrder: " << reqCancelOrder.UserName << " " << reqCancelOrder.AccountName << " " << reqCancelOrder.OrderLocalID << " Failed" << endl;
       }   
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_req_cancel_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rsp_cancel_order(const CUTRspCancelOrderField& rspCancelOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rspCancelOrder.AccountName);

       printUTData(&rspCancelOrder, UT_FID_RspCancelOrder);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[rspCancelOrder.AccountName].stmtInsertRspCancelOrder_;

       stmt->setString(1, rspCancelOrder.UserName);
       stmt->setString(2, rspCancelOrder.AccountName);
       stmt->setString(3, char_to_string(rspCancelOrder.AccountType));
       stmt->setString(4, rspCancelOrder.ExchangeID);
       stmt->setString(5, rspCancelOrder.InstrumentID);
       stmt->setString(6, rspCancelOrder.InstrumentType);
       stmt->setString(7, rspCancelOrder.Symbol);
       stmt->setDouble(8, rspCancelOrder.Price);
       stmt->setDouble(9, rspCancelOrder.LeverRate);
       stmt->setDouble(10, rspCancelOrder.Volume);
       stmt->setString(11, char_to_string(rspCancelOrder.Direction));
       stmt->setString(12, char_to_string(rspCancelOrder.OffsetFlag));
       stmt->setString(13, rspCancelOrder.OrderLocalID);
       stmt->setString(14, char_to_string(rspCancelOrder.OrderMaker));
       stmt->setString(15, char_to_string(rspCancelOrder.OrderType));
       stmt->setBigInt(16, std::to_string(rspCancelOrder.LandTime));
       stmt->setBigInt(17, std::to_string(rspCancelOrder.SendTime));
       stmt->setString(18, rspCancelOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rspCancelOrder.OrderMode));
       stmt->setString(20, char_to_string(rspCancelOrder.AssetType));
       stmt->setString(21, rspCancelOrder.TradeChannel);
       stmt->setString(22, char_to_string(rspCancelOrder.OrderXO));
       stmt->setBigInt(23, rspCancelOrder.PlatformTime);
       stmt->setString(24, rspCancelOrder.OrderSysID);
       stmt->setString(25, rspCancelOrder.OrderForeID);
       stmt->setBigInt(26, rspCancelOrder.CreateTime);
       stmt->setBigInt(27, rspCancelOrder.ModifyTime);
       stmt->setBigInt(28, rspCancelOrder.RspLocalTime);
       stmt->setDouble(39, rspCancelOrder.Cost);
       stmt->setDouble(30, rspCancelOrder.TradePrice);
       stmt->setDouble(31, rspCancelOrder.TradeVolume);
       stmt->setDouble(32, rspCancelOrder.RemainVolume);
       stmt->setDouble(33, rspCancelOrder.TradeValue);
       stmt->setString(34, char_to_string(rspCancelOrder.OrderStatus));
       stmt->setString(35, rspCancelOrder.SessionID);
       stmt->setString(36, std::to_string(rspCancelOrder.RequestID));        
       stmt->setString(37, std::to_string(rspCancelOrder.RequestForeID));        
       stmt->setDouble(38, rspCancelOrder.Fee);      
       stmt->setString(39, rspCancelOrder.FeeCurrency);  

       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);    

       if (!stmt->execute())
       {
           cout << "Insert rspCancelOrder: " << rspCancelOrder.UserName << " " << rspCancelOrder.AccountName << " " << rspCancelOrder.OrderLocalID << " Failed" << endl;
       }  
    }
    catch(const std::exception& e)
    {
        std::cerr <<"DBEngine::insert_rsp_cancel_order " << e.what() << '\n';
    }
}
