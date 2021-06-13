#include "DBEngine.h"

#include <boost/smart_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>

#include "sql_scripts.h"
#include "quark/cxx/ut/UtPrintUtils.h"
#include "quark/cxx/assign.h"
#include "pandora/util/time_util.h"

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
        std::cerr << "\n[E] " <<  e.what() << '\n';
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
        std::cerr << "\n[E] " <<  e.what() << '\n';
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
                // cout << "create req_create_table " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt2(conn_->createStatement());
            if (!stmt2->execute(get_rsp_create_order_sql_str(account_name)))
            {
                // cout << "create rsp_create_table " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt3(conn_->createStatement());
            if (!stmt3->execute(get_rtn_order_sql_str(account_name)))
            {
                // cout << "create rtn_order " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt4(conn_->createStatement());
            if ( !stmt4->execute(get_rtn_trade_sql_str(account_name)) )
            {
                // cout << "create rtn_trade " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt5(conn_->createStatement());
            if (!stmt5->execute(get_req_cancel_order_sql_str(account_name)))
            {
                // cout << "create req_cancel_order " << account_name << " failed!" << endl;
            }

            boost::scoped_ptr<sql::Statement> stmt6(conn_->createStatement());
            if ( !stmt6->execute(get_rsp_cancel_order_sql_str(account_name)) )
            {
                // cout << "create rsp_cancel_order " << account_name << " failed!" << endl;
            }
        }
        else
        {
            cout << "db was not connected!" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " <<  e.what() << '\n';
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
                create_table(account_name);

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
        std::cerr << "\n[E] " <<  "DBEngine::prepare_statement: " << e.what() << '\n';
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
//         std::cerr << "\n[E] " <<  e.what() << '\n';
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
        std::cerr << "\n[E] " <<  e.what() << '\n';
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
        std::cerr << "\n[E] " <<  e.what() << '\n';
    }
    

}

void DBEngine::clean_db()
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DBEngine::clean_db " << e.what() << '\n';
    }    
}

void DBEngine::insert_req_create_order(const CUTReqCreateOrderField& reqCreateOrder)
{
    try
    {
       prepare_statement(reqCreateOrder.AccountName);

       cout << "\nDBEngine::insert_req_create_order " << endl;
       cout << "insert_rsp_create_order " << reqCreateOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(reqCreateOrder.SendTime)  << endl;

    //    printUTData(&reqCreateOrder, UT_FID_ReqCreateOrder);

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
       stmt->setInt64(16, reqCreateOrder.LandTime);
       stmt->setInt64(17, (reqCreateOrder.SendTime));
       stmt->setString(18, reqCreateOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(reqCreateOrder.OrderMode));
       stmt->setString(20, char_to_string(reqCreateOrder.AssetType));
       stmt->setString(21, reqCreateOrder.TradeChannel);
       stmt->setString(22, char_to_string(reqCreateOrder.OrderXO));
       stmt->setInt64(23, (reqCreateOrder.PlatformTime));
       stmt->setString(24, reqCreateOrder.OrderSysID);
       stmt->setString(25, reqCreateOrder.OrderForeID);
       stmt->setInt64(26, (reqCreateOrder.CreateTime));
       stmt->setInt64(27, (reqCreateOrder.ModifyTime));
       stmt->setInt64(28, (reqCreateOrder.RspLocalTime));
       stmt->setDouble(29, reqCreateOrder.Cost);
       stmt->setDouble(30, reqCreateOrder.TradePrice);
       stmt->setDouble(31, reqCreateOrder.TradeVolume);
       stmt->setDouble(32, reqCreateOrder.RemainVolume);
       stmt->setDouble(33, reqCreateOrder.TradeValue);
       stmt->setString(34, char_to_string(reqCreateOrder.OrderStatus));
       stmt->setString(35, reqCreateOrder.SessionID);
       stmt->setInt64(36, (reqCreateOrder.RequestID));        
       stmt->setInt64(37, (reqCreateOrder.RequestForeID));        
       stmt->setDouble(38, reqCreateOrder.Fee);      
       stmt->setString(39, reqCreateOrder.FeeCurrency);             

       if (!stmt->execute())
       {
        //    cout << "Insert ReqCreateOrder: " << reqCreateOrder.UserName << " " << reqCreateOrder.AccountName << " " << reqCreateOrder.OrderLocalID << " Failed" << endl;
       }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_req_create_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rsp_create_order(const CUTRspCreateOrderField& rspCreateOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rspCreateOrder.AccountName);

       cout << "insert_rsp_create_order " << rspCreateOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(rspCreateOrder.RspLocalTime)  << endl;

    //    printUTData(&rspCreateOrder, UT_FID_RspCreateOrder);

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
       stmt->setInt64(16, (rspCreateOrder.LandTime));
       stmt->setInt64(17, (rspCreateOrder.SendTime));
       stmt->setString(18, rspCreateOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rspCreateOrder.OrderMode));
       stmt->setString(20, char_to_string(rspCreateOrder.AssetType));
       stmt->setString(21, rspCreateOrder.TradeChannel);
       stmt->setString(22, char_to_string(rspCreateOrder.OrderXO));
       stmt->setInt64(23, (rspCreateOrder.PlatformTime));
       stmt->setString(24, rspCreateOrder.OrderSysID);
       stmt->setString(25, rspCreateOrder.OrderForeID);
       stmt->setInt64(26, (rspCreateOrder.CreateTime));
       stmt->setInt64(27, (rspCreateOrder.ModifyTime));
       stmt->setInt64(28, (rspCreateOrder.RspLocalTime));
       stmt->setDouble(29, rspCreateOrder.Cost);
       stmt->setDouble(30, rspCreateOrder.TradePrice);
       stmt->setDouble(31, rspCreateOrder.TradeVolume);
       stmt->setDouble(32, rspCreateOrder.RemainVolume);
       stmt->setDouble(33, rspCreateOrder.TradeValue);
       stmt->setString(34, char_to_string(rspCreateOrder.OrderStatus));
       stmt->setString(35, rspCreateOrder.SessionID);
       stmt->setInt64(36, (rspCreateOrder.RequestID));        
       stmt->setInt64(37, (rspCreateOrder.RequestForeID));        
       stmt->setDouble(38, rspCreateOrder.Fee);      
       stmt->setString(39, rspCreateOrder.FeeCurrency);  
       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);              

       if (!stmt->execute())
       {
        //    cout << "Insert RspCreateOrder: " << rspCreateOrder.UserName << " " << rspCreateOrder.AccountName << " " << rspCreateOrder.OrderLocalID << " Failed" << endl;
       }       
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_rsp_create_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rtn_order(const CUTRtnOrderField& rtnOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rtnOrder.AccountName);

       std::cout << "insert_rtn_order " << rtnOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(rtnOrder.RspLocalTime) << std::endl;
    //    printUTData(&rtnOrder, UT_FID_RtnOrder);

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
       stmt->setInt64(16, (rtnOrder.LandTime));
       stmt->setInt64(17, (rtnOrder.SendTime));
       stmt->setString(18, rtnOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rtnOrder.OrderMode));
       stmt->setString(20, char_to_string(rtnOrder.AssetType));
       stmt->setString(21, rtnOrder.TradeChannel);
       stmt->setString(22, char_to_string(rtnOrder.OrderXO));
       stmt->setInt64(23, (rtnOrder.PlatformTime));
       stmt->setString(24, rtnOrder.OrderSysID);
       stmt->setString(25, rtnOrder.OrderForeID);
       stmt->setInt64(26, (rtnOrder.CreateTime));
       stmt->setInt64(27, (rtnOrder.ModifyTime));
       stmt->setInt64(28, (rtnOrder.RspLocalTime));
       stmt->setDouble(29, rtnOrder.Cost);
       stmt->setDouble(30, rtnOrder.TradePrice);
       stmt->setDouble(31, rtnOrder.TradeVolume);
       stmt->setDouble(32, rtnOrder.RemainVolume);
       stmt->setDouble(33, rtnOrder.TradeValue);
       stmt->setString(34, char_to_string(rtnOrder.OrderStatus));
       stmt->setString(35, rtnOrder.SessionID);
       stmt->setInt64(36, (rtnOrder.RequestID));        
       stmt->setInt64(37, (rtnOrder.RequestForeID));        
       stmt->setDouble(38, rtnOrder.Fee);      
       stmt->setString(39, rtnOrder.FeeCurrency);  
       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);    

       if (!stmt->execute())
       {
        //    cout << "Insert RtnOrder: " << rtnOrder.UserName << " " << rtnOrder.AccountName << " " << rtnOrder.OrderLocalID << " Failed" << endl;
       }    
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_rtn_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rtn_trade(const CUTRtnTradeField& rtnTrade, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rtnTrade.AccountName);
       std::cout << "insert_rtn_trade " << rtnTrade.OrderLocalID << " " << utrade::pandora::ToSecondStr(rtnTrade.RspLocalTime) << std::endl;

    //    printUTData(&rtnTrade, UT_FID_RtnTrade);

       sql::PreparedStatement* stmt = accout_preparestmt_map_[rtnTrade.AccountName].stmtInsertRtnTrade;

       stmt->setString(1, rtnTrade.UserName);
       stmt->setString(2, rtnTrade.AccountName);
       stmt->setString(3, char_to_string(rtnTrade.AccountType));
       stmt->setString(4, rtnTrade.InternalAccountName);
       stmt->setString(5, rtnTrade.TradeID);
       stmt->setString(6, rtnTrade.OrderSysID);
       stmt->setString(7, rtnTrade.ExchangeID);
       stmt->setString(8, rtnTrade.InstrumentID);
       
       stmt->setDouble(9, rtnTrade.MatchPrice);
       stmt->setDouble(10, rtnTrade.MatchVolume);
       stmt->setDouble(11, rtnTrade.MatchValue);
       stmt->setString(12, char_to_string(rtnTrade.Direction));
       stmt->setString(13, rtnTrade.OrderLocalID);
       stmt->setDouble(14, rtnTrade.Fee);
       stmt->setString(15, rtnTrade.FeeCurrency);
       stmt->setString(16, rtnTrade.PlatformTime);
       stmt->setString(17, rtnTrade.TradeTime);
       stmt->setInt64(18, (rtnTrade.RspLocalTime));
       stmt->setDouble(19, rtnTrade.Price);
       stmt->setString(20, rtnTrade.StrategyOrderID);
       stmt->setString(21, char_to_string(rtnTrade.OrderMaker));
       stmt->setString(22, char_to_string(rtnTrade.AssetType));

       stmt->setString(23, rtnTrade.TradeChannel);
       stmt->setString(24, rtnTrade.SessionID);

       stmt->setInt(25, rspInfoField.ErrorID);
       stmt->setString(26, rspInfoField.ErrorMsg);       

       if (!stmt->execute())
       {
        //    cout << "Insert RtnTrade: " << rtnTrade.UserName << " " << rtnTrade.AccountName << " " << rtnTrade.OrderLocalID << " Failed" << endl;
       }   
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_rtn_trade " << e.what() << '\n';
    }
}

void DBEngine::insert_req_cancel_order(const CUTReqCancelOrderField& reqCancelOrder)
{
    try
    {
       
       prepare_statement(reqCancelOrder.AccountName);

       cout << "insert_req_cancel_order " << reqCancelOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(reqCancelOrder.SendTime) << endl;
        
    //    printUTData(&reqCancelOrder, UT_FID_ReqCancelOrder);

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
       stmt->setInt64(12, (reqCancelOrder.SendTime));
       stmt->setString(13, (reqCancelOrder.SessionID));

       if (!stmt->execute())
       {
        //    cout << "Insert ReqCancelOrder: " << reqCancelOrder.UserName << " " << reqCancelOrder.AccountName << " " << reqCancelOrder.OrderLocalID << " Failed" << endl;
       }   
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_req_cancel_order " << e.what() << '\n';
    }
}

void DBEngine::insert_rsp_cancel_order(const CUTRspCancelOrderField& rspCancelOrder, const CUTRspInfoField& rspInfoField)
{
    try
    {
       prepare_statement(rspCancelOrder.AccountName);

       std::cout << "insert_rsp_cancel_order " << rspCancelOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(rspCancelOrder.RspLocalTime) << std::endl;

    //    printUTData(&rspCancelOrder, UT_FID_RspCancelOrder);

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
       stmt->setInt64(16, (rspCancelOrder.LandTime));
       stmt->setInt64(17, (rspCancelOrder.SendTime));
       stmt->setString(18, rspCancelOrder.StrategyOrderID);
       stmt->setString(19, char_to_string(rspCancelOrder.OrderMode));
       stmt->setString(20, char_to_string(rspCancelOrder.AssetType));
       stmt->setString(21, rspCancelOrder.TradeChannel);
       stmt->setString(22, char_to_string(rspCancelOrder.OrderXO));
       stmt->setInt64(23, (rspCancelOrder.PlatformTime));
       stmt->setString(24, rspCancelOrder.OrderSysID);
       stmt->setString(25, rspCancelOrder.OrderForeID);
       stmt->setInt64(26, (rspCancelOrder.CreateTime));
       stmt->setInt64(27, (rspCancelOrder.ModifyTime));
       stmt->setInt64(28, (rspCancelOrder.RspLocalTime));
       stmt->setDouble(29, rspCancelOrder.Cost);
       stmt->setDouble(30, rspCancelOrder.TradePrice);
       stmt->setDouble(31, rspCancelOrder.TradeVolume);
       stmt->setDouble(32, rspCancelOrder.RemainVolume);
       stmt->setDouble(33, rspCancelOrder.TradeValue);
       stmt->setString(34, char_to_string(rspCancelOrder.OrderStatus));
       stmt->setString(35, rspCancelOrder.SessionID);
       stmt->setInt64(36, (rspCancelOrder.RequestID));        
       stmt->setInt64(37, (rspCancelOrder.RequestForeID));        
       stmt->setDouble(38, rspCancelOrder.Fee);      
       stmt->setString(39, rspCancelOrder.FeeCurrency);  

       stmt->setInt(40, rspInfoField.ErrorID);      
       stmt->setString(41, rspInfoField.ErrorMsg);    

       if (!stmt->execute())
       {
        //    cout << "Insert rspCancelOrder: " << rspCancelOrder.UserName << " " << rspCancelOrder.AccountName << " " << rspCancelOrder.OrderLocalID << " Failed" << endl;
       }  
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] " << "DBEngine::insert_rsp_cancel_order " << e.what() << '\n';
    }
}

void DBEngine::get_req_create_order(sql::ResultSet* result, CUTRspReqCreateOrderField& reqCreateOrder)
{
    try
    {
        if (true || !result->wasNull())
        {
            assign(reqCreateOrder.UserName, result->getString(1).c_str());
            assign(reqCreateOrder.AccountName, result->getString(2).c_str());
            assign(reqCreateOrder.AccountType, result->getString(3).c_str());
            assign(reqCreateOrder.ExchangeID, result->getString(4).c_str());
            assign(reqCreateOrder.InstrumentID, result->getString(5).c_str());
            assign(reqCreateOrder.InstrumentType, result->getString(6).c_str());  
            assign(reqCreateOrder.Symbol, result->getString(7).c_str());
            assign(reqCreateOrder.Price, result->getDouble(8));
            assign(reqCreateOrder.LeverRate, result->getDouble(9));
            assign(reqCreateOrder.Volume, result->getDouble(10));
            assign(reqCreateOrder.Direction, result->getString(11).c_str());
            assign(reqCreateOrder.OffsetFlag, result->getString(12).c_str());  
            assign(reqCreateOrder.OrderLocalID, result->getString(13).c_str());
            assign(reqCreateOrder.OrderMaker, result->getString(14).c_str());
            assign(reqCreateOrder.OrderType, result->getString(15).c_str());
            assign(reqCreateOrder.LandTime, result->getInt64(16));
            assign(reqCreateOrder.SendTime, result->getInt64(17));
            assign(reqCreateOrder.StrategyOrderID, result->getString(18).c_str());    
            assign(reqCreateOrder.OrderMode, result->getString(19).c_str());
            assign(reqCreateOrder.AssetType, result->getString(20).c_str());
            assign(reqCreateOrder.TradeChannel, result->getString(21).c_str());
            assign(reqCreateOrder.OrderXO, result->getString(22).c_str());
            assign(reqCreateOrder.PlatformTime, result->getInt64(23));
            assign(reqCreateOrder.OrderSysID, result->getString(24).c_str());
            assign(reqCreateOrder.OrderForeID, result->getString(25).c_str());  
            assign(reqCreateOrder.CreateTime, result->getInt64(26));
            assign(reqCreateOrder.ModifyTime, result->getInt64(27));
            assign(reqCreateOrder.RspLocalTime, result->getInt64(28));
            assign(reqCreateOrder.Cost, result->getDouble(29));
            assign(reqCreateOrder.TradePrice, result->getDouble(30));
            assign(reqCreateOrder.TradeVolume, result->getDouble(31));
            assign(reqCreateOrder.RemainVolume, result->getDouble(32));
            assign(reqCreateOrder.TradeValue, result->getDouble(33));
            assign(reqCreateOrder.OrderStatus, result->getString(34).c_str());
            assign(reqCreateOrder.SessionID, result->getString(35).c_str());
            assign(reqCreateOrder.RequestID, result->getInt64(36));   
            assign(reqCreateOrder.RequestForeID, result->getInt64(37));
            assign(reqCreateOrder.UserName, result->getString(38).c_str());
            assign(reqCreateOrder.UserName, result->getString(39).c_str());    


                                                                    
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_req_create_order " << e.what() << '\n';
    }

}

void DBEngine::get_rsp_create_order(sql::ResultSet* result, CUTRspCreateOrderField& rspCreateOrder, CUTRspInfoField& rspInfoField)
{
    try
    {
        assign(rspCreateOrder.UserName, result->getString(1).c_str());
        assign(rspCreateOrder.AccountName, result->getString(2).c_str());
        assign(rspCreateOrder.AccountType, result->getString(3).c_str());
        assign(rspCreateOrder.ExchangeID, result->getString(4).c_str());
        assign(rspCreateOrder.InstrumentID, result->getString(5).c_str());
        assign(rspCreateOrder.InstrumentType, result->getString(6).c_str());  
        assign(rspCreateOrder.Symbol, result->getString(7).c_str());
        assign(rspCreateOrder.Price, result->getDouble(8));
        assign(rspCreateOrder.LeverRate, result->getDouble(9));
        assign(rspCreateOrder.Volume, result->getDouble(10));
        assign(rspCreateOrder.Direction, result->getString(11).c_str());
        assign(rspCreateOrder.OffsetFlag, result->getString(12).c_str());  
        assign(rspCreateOrder.OrderLocalID, result->getString(13).c_str());
        assign(rspCreateOrder.OrderMaker, result->getString(14).c_str());
        assign(rspCreateOrder.OrderType, result->getString(15).c_str());
        assign(rspCreateOrder.LandTime, result->getInt64(16));
        assign(rspCreateOrder.SendTime, result->getInt64(17));
        assign(rspCreateOrder.StrategyOrderID, result->getString(18).c_str());    
        assign(rspCreateOrder.OrderMode, result->getString(19).c_str());
        assign(rspCreateOrder.AssetType, result->getString(20).c_str());
        assign(rspCreateOrder.TradeChannel, result->getString(21).c_str());
        assign(rspCreateOrder.OrderXO, result->getString(22).c_str());
        assign(rspCreateOrder.PlatformTime, result->getInt64(23));
        assign(rspCreateOrder.OrderSysID, result->getString(24).c_str());
        assign(rspCreateOrder.OrderForeID, result->getString(25).c_str());  
        assign(rspCreateOrder.CreateTime, result->getInt64(26));
        assign(rspCreateOrder.ModifyTime, result->getInt64(27));
        assign(rspCreateOrder.RspLocalTime, result->getInt64(28));
        assign(rspCreateOrder.Cost, result->getDouble(29));
        assign(rspCreateOrder.TradePrice, result->getDouble(30));
        assign(rspCreateOrder.TradeVolume, result->getDouble(31));
        assign(rspCreateOrder.RemainVolume, result->getDouble(32));
        assign(rspCreateOrder.TradeValue, result->getDouble(33));
        assign(rspCreateOrder.OrderStatus, result->getString(34).c_str());
        assign(rspCreateOrder.SessionID, result->getString(35).c_str());
        assign(rspCreateOrder.RequestID, result->getInt64(36));   
        assign(rspCreateOrder.RequestForeID, result->getInt64(37));
        assign(rspCreateOrder.UserName, result->getString(38).c_str());
        assign(rspCreateOrder.UserName, result->getString(39).c_str());    

        assign(rspInfoField.ErrorID, result->getInt(40));
        assign(rspInfoField.ErrorMsg, result->getString(41).c_str());

        // cout << "DBEngine::get_rsp_create_order " << endl;
        // printUTData(&rspCreateOrder, UT_FID_RspCreateOrder);
        // printUTData(&rspInfoField, UT_FID_RspInfo);
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_rsp_create_order" << e.what() << '\n';
    }
}

void DBEngine::get_rtn_order(sql::ResultSet* result, CUTRtnOrderField& rtnOrder, CUTRspInfoField& rspInfoField)
{
    try
    {
            assign(rtnOrder.UserName, result->getString(1).c_str());
            assign(rtnOrder.AccountName, result->getString(2).c_str());
            assign(rtnOrder.AccountType, result->getString(3).c_str());
            assign(rtnOrder.ExchangeID, result->getString(4).c_str());
            assign(rtnOrder.InstrumentID, result->getString(5).c_str());
            assign(rtnOrder.InstrumentType, result->getString(6).c_str());  
            assign(rtnOrder.Symbol, result->getString(7).c_str());
            assign(rtnOrder.Price, result->getDouble(8));
            assign(rtnOrder.LeverRate, result->getDouble(9));
            assign(rtnOrder.Volume, result->getDouble(10));
            assign(rtnOrder.Direction, result->getString(11).c_str());
            assign(rtnOrder.OffsetFlag, result->getString(12).c_str());  
            assign(rtnOrder.OrderLocalID, result->getString(13).c_str());
            assign(rtnOrder.OrderMaker, result->getString(14).c_str());
            assign(rtnOrder.OrderType, result->getString(15).c_str());
            assign(rtnOrder.LandTime, result->getInt64(16));
            assign(rtnOrder.SendTime, result->getInt64(17));
            assign(rtnOrder.StrategyOrderID, result->getString(18).c_str());    
            assign(rtnOrder.OrderMode, result->getString(19).c_str());
            assign(rtnOrder.AssetType, result->getString(20).c_str());
            assign(rtnOrder.TradeChannel, result->getString(21).c_str());
            assign(rtnOrder.OrderXO, result->getString(22).c_str());
            assign(rtnOrder.PlatformTime, result->getInt64(23));
            assign(rtnOrder.OrderSysID, result->getString(24).c_str());
            assign(rtnOrder.OrderForeID, result->getString(25).c_str());  
            assign(rtnOrder.CreateTime, result->getInt64(26));
            assign(rtnOrder.ModifyTime, result->getInt64(27));
            assign(rtnOrder.RspLocalTime, result->getInt64(28));
            assign(rtnOrder.Cost, result->getDouble(29));
            assign(rtnOrder.TradePrice, result->getDouble(30));
            assign(rtnOrder.TradeVolume, result->getDouble(31));
            assign(rtnOrder.RemainVolume, result->getDouble(32));
            assign(rtnOrder.TradeValue, result->getDouble(33));
            assign(rtnOrder.OrderStatus, result->getString(34).c_str());
            assign(rtnOrder.SessionID, result->getString(35).c_str());
            assign(rtnOrder.RequestID, result->getInt64(36));   
            assign(rtnOrder.RequestForeID, result->getInt64(37));
            assign(rtnOrder.UserName, result->getString(38).c_str());
            assign(rtnOrder.UserName, result->getString(39).c_str());    

            assign(rspInfoField.ErrorID, result->getInt(40));
            assign(rspInfoField.ErrorMsg, result->getString(41).c_str());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_rtn_order" << e.what() << '\n';
    }

}

void DBEngine::get_rtn_trade(sql::ResultSet* result, CUTRtnTradeField& rtnTrade, CUTRspInfoField& rspInfoField)
{
    try
    {
        assign(rtnTrade.UserName, result->getString(1).c_str());
        assign(rtnTrade.AccountName, result->getString(2).c_str());
        assign(rtnTrade.AccountType, result->getString(3).c_str());
        assign(rtnTrade.InternalAccountName, result->getString(4).c_str());
        assign(rtnTrade.TradeID, result->getString(5).c_str());
        assign(rtnTrade.OrderSysID, result->getString(6).c_str());
        assign(rtnTrade.ExchangeID, result->getString(7).c_str());
        assign(rtnTrade.InstrumentID, result->getString(8).c_str());

        assign(rtnTrade.MatchPrice, result->getDouble(9));
        assign(rtnTrade.MatchVolume, result->getDouble(10));
        assign(rtnTrade.MatchValue, result->getDouble(11));

        assign(rtnTrade.Direction, result->getString(12).c_str());
        assign(rtnTrade.OrderLocalID, result->getString(13).c_str());
        assign(rtnTrade.Fee, result->getDouble(14));
        assign(rtnTrade.FeeCurrency, result->getString(15).c_str());
        assign(rtnTrade.PlatformTime, result->getString(16).c_str());
        assign(rtnTrade.TradeTime, result->getString(17).c_str());
        assign(rtnTrade.RspLocalTime, result->getInt64(18));
        assign(rtnTrade.Price, result->getDouble(19));

        assign(rtnTrade.StrategyOrderID, result->getString(20).c_str());
        assign(rtnTrade.OrderMaker, result->getString(21).c_str());
        assign(rtnTrade.AssetType, result->getString(22).c_str());
        assign(rtnTrade.TradeChannel, result->getString(23).c_str());      
        assign(rtnTrade.SessionID, result->getString(24).c_str());        

        assign(rspInfoField.ErrorID, result->getInt(25));
        assign(rspInfoField.ErrorMsg, result->getString(26).c_str());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_rtn_trade" << e.what() << '\n';
    }

}

void DBEngine::get_req_cancel_order(sql::ResultSet* result, CUTRspReqCancelOrderField& reqCancelOrder)
{
    try
    {
        assign(reqCancelOrder.UserName, result->getString(1).c_str());
        assign(reqCancelOrder.AccountName, result->getString(2).c_str());
        assign(reqCancelOrder.AccountType, result->getString(3).c_str());
        assign(reqCancelOrder.Symbol, result->getString(4).c_str());
        assign(reqCancelOrder.AssetType, result->getString(5).c_str());
        assign(reqCancelOrder.OrderLocalID, result->getString(6).c_str());
        assign(reqCancelOrder.OrderSysID, result->getString(7).c_str());
        assign(reqCancelOrder.OrderForeID, result->getString(8).c_str());
        assign(reqCancelOrder.ExchangeID, result->getString(9).c_str());
        assign(reqCancelOrder.StrategyOrderID, result->getString(10).c_str());
        assign(reqCancelOrder.TradeChannel, result->getString(11).c_str());
        assign(reqCancelOrder.SendTime, result->getInt64(12));
        assign(reqCancelOrder.SessionID, result->getString(13).c_str()); 
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_req_cancel_order" << e.what() << '\n';
    }

}

void DBEngine::get_rsp_cancel_order(sql::ResultSet* result, CUTRspCancelOrderField& rspCancelOrder, CUTRspInfoField& rspInfoField)
{
    try
    {
            assign(rspCancelOrder.UserName, result->getString(1).c_str());
            assign(rspCancelOrder.AccountName, result->getString(2).c_str());
            assign(rspCancelOrder.AccountType, result->getString(3).c_str());
            assign(rspCancelOrder.ExchangeID, result->getString(4).c_str());
            assign(rspCancelOrder.InstrumentID, result->getString(5).c_str());
            assign(rspCancelOrder.InstrumentType, result->getString(6).c_str());  
            assign(rspCancelOrder.Symbol, result->getString(7).c_str());
            assign(rspCancelOrder.Price, result->getDouble(8));
            assign(rspCancelOrder.LeverRate, result->getDouble(9));
            assign(rspCancelOrder.Volume, result->getDouble(10));
            assign(rspCancelOrder.Direction, result->getString(11).c_str());
            assign(rspCancelOrder.OffsetFlag, result->getString(12).c_str());  
            assign(rspCancelOrder.OrderLocalID, result->getString(13).c_str());
            assign(rspCancelOrder.OrderMaker, result->getString(14).c_str());
            assign(rspCancelOrder.OrderType, result->getString(15).c_str());
            assign(rspCancelOrder.LandTime, result->getInt64(16));
            assign(rspCancelOrder.SendTime, result->getInt64(17));
            assign(rspCancelOrder.StrategyOrderID, result->getString(18).c_str());    
            assign(rspCancelOrder.OrderMode, result->getString(19).c_str());
            assign(rspCancelOrder.AssetType, result->getString(20).c_str());
            assign(rspCancelOrder.TradeChannel, result->getString(21).c_str());
            assign(rspCancelOrder.OrderXO, result->getString(22).c_str());
            assign(rspCancelOrder.PlatformTime, result->getInt64(23));
            assign(rspCancelOrder.OrderSysID, result->getString(24).c_str());
            assign(rspCancelOrder.OrderForeID, result->getString(25).c_str());  
            assign(rspCancelOrder.CreateTime, result->getInt64(26));
            assign(rspCancelOrder.ModifyTime, result->getInt64(27));
            assign(rspCancelOrder.RspLocalTime, result->getInt64(28));
            assign(rspCancelOrder.Cost, result->getDouble(29));
            assign(rspCancelOrder.TradePrice, result->getDouble(30));
            assign(rspCancelOrder.TradeVolume, result->getDouble(31));
            assign(rspCancelOrder.RemainVolume, result->getDouble(32));
            assign(rspCancelOrder.TradeValue, result->getDouble(33));
            assign(rspCancelOrder.OrderStatus, result->getString(34).c_str());
            assign(rspCancelOrder.SessionID, result->getString(35).c_str());
            assign(rspCancelOrder.RequestID, result->getInt64(36));   
            assign(rspCancelOrder.RequestForeID, result->getInt64(37));
            assign(rspCancelOrder.UserName, result->getString(38).c_str());
            assign(rspCancelOrder.UserName, result->getString(39).c_str());    

            assign(rspInfoField.ErrorID, result->getInt(40));
            assign(rspInfoField.ErrorMsg, result->getString(41).c_str());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_rsp_cancel_order" << e.what() << '\n';
    }
}

vector<PackagePtr> DBEngine::get_req_create_order_by_time(string account_name, unsigned long start_time, unsigned long end_time)
{
    try
    {
        cout << "get_req_create_order_by_time " << account_name << " "
             << "start: " << utrade::pandora::ToSecondStr(start_time) << " "
             << "end: " << utrade::pandora::ToSecondStr(end_time) << " "
             << endl;
        string sql_str = select_req_create_order_by_time(account_name, start_time, end_time);

        sql::Statement* state;
        sql::ResultSet* sql_result;
        vector<PackagePtr> package_list;

        // cout << "sql_result: " << sql_str << endl;

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);

            int i = 0;
            while(sql_result->next())
            {   
                PackagePtr package = boost::make_shared<Package>();
                CUTRspReqCreateOrderField* reqCreateOrder = CREATE_FIELD(package, CUTRspReqCreateOrderField);

                get_req_create_order(sql_result, *reqCreateOrder);

                // cout << "\nDBEngine::get_req_create_order " << ++i << endl;
                // printUTData(&reqCreateOrder, UT_FID_RspReqCreateOrder);  
                package_list.push_back(package);
            }
        }        
        return package_list;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_req_create_order_by_time" << e.what() << '\n';
    }
    
}


PackagePtr DBEngine::get_rsp_create_order_by_orderlocalid(string account_name, string order_local_id)
 {
    try
    {
        cout << "get_rsp_create_order_by_orderlocalid " << account_name << " "
             << "order_local_id: " << order_local_id << " "
             << endl;
        string sql_str = select_rsp_create_order_by_orderlocalid(account_name, order_local_id);

        sql::Statement* state;
        sql::ResultSet* sql_result;

        // cout << "sql_result: " << sql_str << endl;

        PackagePtr package = boost::make_shared<Package>();

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl; 
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);

            while(sql_result->next())
            {   
                CUTRspCreateOrderField* pRspCreateOrder = CREATE_FIELD(package, CUTRspCreateOrderField);
                CUTRspInfoField* rspInfo = CREATE_FIELD(package, CUTRspInfoField);
                get_rsp_create_order(sql_result, *pRspCreateOrder, *rspInfo);
            }
        }        
        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DBEngine::get_rsp_create_order_by_orderlocalid" << e.what() << '\n';
    }     
 }

PackagePtr DBEngine::get_rtn_order_by_orderlocalid(string account_name, string order_local_id)
{
    try
    {
        cout << "get_rtn_order_by_orderlocalid " << account_name << " "
             << "order_local_id: " << order_local_id << " "
             << endl;
        string sql_str = select_rtn_order_by_orderlocalid(account_name, order_local_id);

        sql::Statement* state;
        sql::ResultSet* sql_result;

        // cout << "sql_result: " << sql_str << endl;

        PackagePtr package = boost::make_shared<Package>();

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);

            while(sql_result->next())
            {   
                CUTRtnOrderField* rtnOrder = CREATE_FIELD(package, CUTRtnOrderField);
                CUTRspInfoField* rspInfo = CREATE_FIELD(package, CUTRspInfoField);

                get_rtn_order(sql_result, *rtnOrder, *rspInfo);

                // printUTData(rtnOrder, UT_FID_RtnOrder);
                // printUTData(rspInfo, UT_FID_RspInfo);                
            }
        }    

        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DBEngine::get_rtn_order_by_orderlocalid " << e.what() << '\n';
    }
    
}

PackagePtr DBEngine::get_rtn_trade_by_orderlocalid(string account_name, string order_local_id)
{
    try
    {
        cout << "get_rtn_trade_by_orderlocalid " << account_name << " "
             << "order_local_id: " << order_local_id << " "
             << endl;
        string sql_str = select_rtn_trade_by_orderlocalid(account_name, order_local_id);

        sql::Statement* state;
        sql::ResultSet* sql_result;

        // cout << "sql_result: " << sql_str << endl;

        PackagePtr package = boost::make_shared<Package>();

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);
      

            while(sql_result->next())
            {   
                CUTRtnTradeField* rtnTrade = CREATE_FIELD(package, CUTRtnTradeField);
                CUTRspInfoField* rspInfo = CREATE_FIELD(package, CUTRspInfoField);      

                get_rtn_trade(sql_result, *rtnTrade, *rspInfo);

                // printUTData(rtnTrade, UT_FID_RtnTrade);
                // printUTData(rspInfo, UT_FID_RspInfo);
            }
        }   

        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DBEngine::get_rtn_trade_by_orderlocalid " << e.what() << '\n';
    }
        
}

vector<PackagePtr> DBEngine::get_req_cancel_order_by_time(string account_name, unsigned long start_time, unsigned long end_time)
{
    try
    {
        cout << "get_req_cancel_order_by_time " << account_name << " "
             << "start: " << utrade::pandora::ToSecondStr(start_time) << " "
             << "end: " << utrade::pandora::ToSecondStr(end_time) << " "
             << endl;
        string sql_str = select_req_cancel_order_by_time(account_name, start_time, end_time);

        sql::Statement* state;
        sql::ResultSet* sql_result;

        vector<PackagePtr> package_list;

        // cout << "sql_result: " << sql_str << endl;

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);

            int i = 0;
            while(sql_result->next())
            {   
                PackagePtr package = boost::make_shared<Package>();
                CUTRspReqCancelOrderField* reqCancelOrder = CREATE_FIELD(package, CUTRspReqCancelOrderField);

                get_req_cancel_order(sql_result, *reqCancelOrder);

                // cout << "DBEngine::get_req_cancel_order_by_time: " << ++i << endl;
                // cout << "get_req_cancel_order " << reqCancelOrder.OrderLocalID << " " << utrade::pandora::ToSecondStr(reqCancelOrder.SendTime) << endl;
                // printUTData(reqCancelOrder, UT_FID_RspReqCancelOrder);

                package_list.push_back(package);           
            }
        }    

        return package_list;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DBEngine::get_req_cancel_order_by_time " << e.what() << '\n';
    }
        
}

PackagePtr DBEngine::get_rsp_cancel_order_by_orderlocalid(string account_name, string order_local_id)
{
    try
    {
        cout << "get_rsp_cancel_order_by_orderlocalid " << account_name << " "
             << "order_local_id: " << order_local_id << " "
             << endl;

        string sql_str = select_rsp_cancel_order_by_orderlocalid(account_name, order_local_id);

        sql::Statement* state;
        sql::ResultSet* sql_result;

        // cout << "sql_result: " << sql_str << endl;

        PackagePtr package = boost::make_shared<Package>();

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
        }
        else
        {
            state = conn_->createStatement();
            sql_result = state->executeQuery(sql_str);

            while(sql_result->next())
            {   

                CUTRspCancelOrderField* rspCancelOrderField = CREATE_FIELD(package, CUTRspCancelOrderField);
                CUTRspInfoField* rspInfo = CREATE_FIELD(package, CUTRspInfoField);    
                                
                get_rsp_cancel_order(sql_result, *rspCancelOrderField, *rspInfo);

                // printUTData(rspCancelOrderField, UT_FID_RspCancelOrder);
                // printUTData(rspInfo, UT_FID_RspInfo);
            }
        }   

        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DBEngine::get_rsp_cancel_order_by_orderlocalid " << e.what() << '\n';
    }
        
}
