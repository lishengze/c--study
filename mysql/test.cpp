#include "test.h"

#include "quark/cxx/assign.h"

#include "pandora/util/time_util.h"

TestDBEngine::TestDBEngine()
{
    // test_connect_schema();

    test_create_table();    
}

void TestDBEngine::test_connect()
{
    string address = "tcp://127.0.0.1:3306";
    string usr = "wormhole";
    string pwd = "worm";

    DBEngine db(address, usr, pwd);
    db.connect_mysql();
    db.create_db("TestTrade17");
}

void TestDBEngine::test_connect_schema()
{
    string host = "127.0.0.1:3306";
    string usr = "wormhole";
    string pwd = "worm";
    string schema = "TestTrade";
    int    port = 3306;

    DBEngine db(usr, pwd, schema, port, host);
    // db.connect_mysql();
    // db.create_db("TestTrade17");            
}

void SetReqCreateOrderField(CUTReqCreateOrderField& pUTReqCreateOrderField, string& account_name, long& OrderLocalID)
{
    assign(pUTReqCreateOrderField.UserName, "LszTest");
    assign(pUTReqCreateOrderField.AccountName, account_name); 
    assign(pUTReqCreateOrderField.AccountType, AT_Physical);
    assign(pUTReqCreateOrderField.ExchangeID, "HUOBI");
    assign(pUTReqCreateOrderField.InstrumentID, "XRP_USDT");      

    assign(pUTReqCreateOrderField.Symbol, "XRP_USDT");
    assign(pUTReqCreateOrderField.Price, 0.26); 
    assign(pUTReqCreateOrderField.LeverRate, 10);
    assign(pUTReqCreateOrderField.Volume, 20);      

    assign(pUTReqCreateOrderField.Direction, BS_Long);
    assign(pUTReqCreateOrderField.OffsetFlag, OF_Open); 
    assign(pUTReqCreateOrderField.OrderLocalID, std::to_string(OrderLocalID));
    assign(pUTReqCreateOrderField.OrderMaker, OM_Maker);      

    assign(pUTReqCreateOrderField.OrderType, ORDT_Limit);
    assign(pUTReqCreateOrderField.OrderMode, ODM_GTC); 
    assign(pUTReqCreateOrderField.AssetType, AST_DigitalSpot);
    assign(pUTReqCreateOrderField.TradeChannel, account_name);      

    assign(pUTReqCreateOrderField.OrderXO, OX_New);
    assign(pUTReqCreateOrderField.TestReqGalaxyEndTime, utrade::pandora::NanoTime()); 
    assign(pUTReqCreateOrderField.SessionID, "TestSession");
    assign(pUTReqCreateOrderField.RequestID, OrderLocalID);     
}

void SetCUTReqCancelOrderField(CUTReqCancelOrderField& reqCancelOrderField, string account_name, long& OrderLocalID)
{
    assign(reqCancelOrderField.UserName, "LSZ");
    assign(reqCancelOrderField.AccountName, account_name);
    assign(reqCancelOrderField.AccountType, AT_Physical);
    assign(reqCancelOrderField.Symbol, "XRP_USDT");
    assign(reqCancelOrderField.AssetType, AST_DigitalSpot);
    assign(reqCancelOrderField.OrderLocalID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.OrderForeID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.OrderSysID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.ExchangeID, "");
    assign(reqCancelOrderField.TradeChannel, "TestSession");
    assign(reqCancelOrderField.SendTime, 2021); 
}

// void Set


void TestDBEngine::test_create_table()
{
    string host = "127.0.0.1:3306";
    string usr = "wormhole";
    string pwd = "worm";
    string schema = "TestTrade";
    int    port = 3306;

    DBEngine db(usr, pwd, schema, port, host);
    string account_name = "HUOBI_QA_MAIN";
    long OrderLocalID = 2;
    db.create_table(account_name);

    // CUTReqCreateOrderField pUTReqCreateOrderField;
    // SetReqCreateOrderField(pUTReqCreateOrderField, account_name, OrderLocalID);
    // db.insert_req_create_order(pUTReqCreateOrderField);

    CUTReqCancelOrderField reqCancelOrderField;
    SetCUTReqCancelOrderField(reqCancelOrderField, account_name, OrderLocalID);
    db.insert_req_cancel_order(reqCancelOrderField);

    

    // db.connect_mysql();            
}