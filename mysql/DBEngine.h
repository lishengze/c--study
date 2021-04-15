#pragma once

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include "sql_scripts.h"

#include "quark/cxx/ut/UtData.h"

#include <string>
#include <iostream>
#include <map>
using std::string;
using std::cout;
using std::endl;
using std::map;

class DBEngine
{
    public:

        DBEngine(string address, string usr, string pwd):
        usr_{usr}, pwd_{pwd}, host_{address}
        {

        }

        DBEngine(string usr, string pwd, string schema, int port, string host):
         usr_{usr}, pwd_{pwd}, schema_{schema}, port_{port}, host_{host} 
        {
            connect_mysql_schema();
        }        

        void connect_mysql();

        void connect_mysql_schema();

        void create_db(string db_name);

        bool check_db(string db_name);

        void prepare_statement(string account_name);

        void create_table(string account_name);

        void insert_req_create_order(const CUTReqCreateOrderField& reqCreateOrder);

        void insert_rsp_create_order(const CUTRspCreateOrderField& rspCreateOrder, const CUTRspInfoField& rspInfoField);

        void insert_rtn_order(const CUTRtnOrderField& rtnOrder, const CUTRspInfoField& rspInfoField);

        void insert_rtn_trade(const CUTRtnTradeField& rtnTrade, const CUTRspInfoField& rspInfoField);

        void insert_req_cancel_order(const CUTReqCancelOrderField& reqCancelOrder);

        void insert_rsp_cancel_order(const CUTRspCancelOrderField& rspCancelOrder, const CUTRspInfoField& rspInfoField);

        struct PrepareSMT
        {
            public:

            void prepare(sql::Connection* conn, string& account_name)
            {
                try
                {
                    if (conn)
                    {
                        stmtInsertReqCreateOrder_ = conn->prepareStatement(insert_req_create_order_sql(account_name));
                        stmtInsertRspCreateOrder_ = conn->prepareStatement(insert_rsp_create_order_sql(account_name));

                        stmtInsertReqCancelOrder_ = conn->prepareStatement(insert_req_cancel_order_sql(account_name));
                        stmtInsertRspCancelOrder_ = conn->prepareStatement(insert_rsp_cancel_order_sql(account_name));

                        stmtInsertRtnOrder = conn->prepareStatement(insert_rtn_order_sql(account_name));
                        stmtInsertRtnTrade = conn->prepareStatement(insert_rtn_trade_sql(account_name));

                        is_prepared_ = true;
                    }
                    else
                    {
                        std::cout << "DB was Not Connected!" << std::endl;
                    }
                }
                catch(const std::exception& e)
                {
                    std::cerr << "PrepareSMT::prepare " << e.what() << '\n';
                }
            }
            
            bool is_prepared() { return is_prepared_;}
            //下单请求回报
            sql::PreparedStatement* stmtInsertReqCreateOrder_;
            sql::PreparedStatement* stmtInsertRspCreateOrder_;


            //撤单请求回报
            sql::PreparedStatement* stmtInsertReqCancelOrder_;
            sql::PreparedStatement* stmtInsertRspCancelOrder_; 


            //RtnOrder RtnTrade
            sql::PreparedStatement* stmtInsertRtnOrder;

            //RtnTrade
            sql::PreparedStatement* stmtInsertRtnTrade;

            bool is_prepared_{false};
        };

    private:

    sql::Connection*        conn_{NULL};
    sql::Statement*         state_;

    string                  usr_;
    string                  pwd_;
    string                  host_;
    string                  schema_;
    int                     port_;

    map<string, PrepareSMT> accout_preparestmt_map_;

    

};