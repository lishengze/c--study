#include "test.h"

#include "quark/cxx/assign.h"
#include "quark/cxx/ut/UtPrintUtils.h"

#include "pandora/util/time_util.h"

#include <boost/make_shared.hpp>

#include "trade_console.h"

#include <thread>
#include <chrono>

TestDBEngine::TestDBEngine()
{
    // test_connect_schema();

    test_create_table();    

    // test_trade_console();
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
    assign(pUTReqCreateOrderField.UserName, "TestUser9");
    assign(pUTReqCreateOrderField.AccountName, account_name); 
    assign(pUTReqCreateOrderField.AccountType, AT_Physical);
    assign(pUTReqCreateOrderField.ExchangeID, "HUOBI");
    assign(pUTReqCreateOrderField.InstrumentID, "XRP_USDT"); 
    // assign(pUTReqCreateOrderField.InstrumentType, "this_week"); //
    
    assign(pUTReqCreateOrderField.Symbol, "XRP_USDT");
    assign(pUTReqCreateOrderField.Price, 0.26); 
    assign(pUTReqCreateOrderField.LeverRate, 10);
    assign(pUTReqCreateOrderField.Volume, 20);      

    assign(pUTReqCreateOrderField.Direction, BS_Long);
    assign(pUTReqCreateOrderField.OffsetFlag, OF_Open); 
    assign(pUTReqCreateOrderField.OrderLocalID, std::to_string(OrderLocalID));
    assign(pUTReqCreateOrderField.OrderMaker, OM_Maker);      

    assign(pUTReqCreateOrderField.OrderType, ORDT_Limit);

    // assign(pUTReqCreateOrderField.LandTime, utrade::pandora::NanoTime()); //

    assign(pUTReqCreateOrderField.SendTime, utrade::pandora::NanoTime()); //

    assign(pUTReqCreateOrderField.StrategyOrderID, std::to_string(OrderLocalID)); 

    assign(pUTReqCreateOrderField.OrderMode, ODM_GTC); 
    assign(pUTReqCreateOrderField.AssetType, AST_DigitalSpot);
    assign(pUTReqCreateOrderField.TradeChannel, account_name);      

    assign(pUTReqCreateOrderField.OrderXO, OX_New);

    // assign(pUTReqCreateOrderField.PlatformTime, utrade::pandora::NanoTimeStr()); //
    // assign(pUTReqCreateOrderField.OrderSysID, std::to_string(OrderLocalID)); //
    // assign(pUTReqCreateOrderField.OrderForeID, std::to_string(OrderLocalID)); //
    // assign(pUTReqCreateOrderField.CreateTime, utrade::pandora::NanoTimeStr()); //
    // assign(pUTReqCreateOrderField.ModifyTime, utrade::pandora::NanoTimeStr()); //
    // assign(pUTReqCreateOrderField.RspLocalTime, std::to_string(utrade::pandora::NanoTime())); // 
    // assign(pUTReqCreateOrderField.Cost, 0); //
    // assign(pUTReqCreateOrderField.TradePrice, 0); //
    // assign(pUTReqCreateOrderField.TradeVolume, 0); //
    // assign(pUTReqCreateOrderField.RemainVolume, 0); //
    // assign(pUTReqCreateOrderField.TradeValue, 0); //


    // assign(pUTReqCreateOrderField.OrderStatus, OST_Unknown);//
    assign(pUTReqCreateOrderField.SessionID, "TestSession");
    // assign(pUTReqCreateOrderField.RequestID, OrderLocalID);     
    // assign(pUTReqCreateOrderField.RequestForeID, OrderLocalID);     
    // assign(pUTReqCreateOrderField.Fee, 0);     //
    // assign(pUTReqCreateOrderField.FeeCurrency, "USDT");     //
}

void SetCUTRspCreateOrderField(CUTRspCreateOrderField& rspCreateOrderField, string account_name, long& OrderLocalID)
{
    try
    {
        assign(rspCreateOrderField.UserName, "TestUser9");
        assign(rspCreateOrderField.AccountName, account_name); 
        assign(rspCreateOrderField.AccountType, AT_Physical);
        assign(rspCreateOrderField.ExchangeID, "HUOBI");
        assign(rspCreateOrderField.InstrumentID, "XRP_USDT"); 
        // assign(rspCreateOrderField.InstrumentType, "this_week"); //
        
        assign(rspCreateOrderField.Symbol, "XRP_USDT");
        assign(rspCreateOrderField.Price, 0.26); 
        assign(rspCreateOrderField.LeverRate, 10);
        assign(rspCreateOrderField.Volume, 20);      

        assign(rspCreateOrderField.Direction, BS_Long);
        assign(rspCreateOrderField.OffsetFlag, OF_Open); 
        assign(rspCreateOrderField.OrderLocalID, std::to_string(OrderLocalID));
        assign(rspCreateOrderField.OrderMaker, OM_Maker);      

        assign(rspCreateOrderField.OrderType, ORDT_Limit);

        // assign(rspCreateOrderField.LandTime, utrade::pandora::NanoTime()); //
        // assign(rspCreateOrderField.SendTime, utrade::pandora::NanoTime()); //

        assign(rspCreateOrderField.StrategyOrderID, std::to_string(OrderLocalID)); 

        assign(rspCreateOrderField.OrderMode, ODM_GTC); 
        assign(rspCreateOrderField.AssetType, AST_DigitalSpot);
        assign(rspCreateOrderField.TradeChannel, account_name);      

        assign(rspCreateOrderField.OrderXO, OX_New);

        // assign(rspCreateOrderField.PlatformTime, utrade::pandora::NanoTimeStr()); //
        // assign(rspCreateOrderField.OrderSysID, std::to_string(OrderLocalID)); //
        // assign(rspCreateOrderField.OrderForeID, std::to_string(OrderLocalID)); //
        // assign(rspCreateOrderField.CreateTime, utrade::pandora::NanoTimeStr()); //
        // assign(rspCreateOrderField.ModifyTime, utrade::pandora::NanoTimeStr()); //
        assign(rspCreateOrderField.RspLocalTime, utrade::pandora::NanoTime()); // 
        // assign(rspCreateOrderField.Cost, 0); //
        // assign(rspCreateOrderField.TradePrice, 0); //
        // assign(rspCreateOrderField.TradeVolume, 0); //
        // assign(rspCreateOrderField.RemainVolume, 0); //
        // assign(rspCreateOrderField.TradeValue, 0); //


        assign(rspCreateOrderField.OrderStatus, OST_Sent);//
        assign(rspCreateOrderField.SessionID, "TestSession");

        // cout << "\nSetCUTRspCreateOrderField " << endl;
        // printUTData(&rspCreateOrderField, UT_FID_RspCreateOrder);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[E] SetCUTRspCreateOrderField"<< e.what() << '\n';
    }
    
}

void SetCUTRtnOrderField(CUTRtnOrderField& rtnOrder, string account_name, long& OrderLocalID)
{
    try
    {
        assign(rtnOrder.UserName, "TestUser9");
        assign(rtnOrder.AccountName, account_name); 
        assign(rtnOrder.AccountType, AT_Physical);
        assign(rtnOrder.ExchangeID, "HUOBI");
        assign(rtnOrder.InstrumentID, "XRP_USDT"); 
        // assign(rtnOrder.InstrumentType, "this_week"); //
        
        assign(rtnOrder.Symbol, "XRP_USDT");
        assign(rtnOrder.Price, 0.26); 
        assign(rtnOrder.LeverRate, 10);
        assign(rtnOrder.Volume, 20);      

        assign(rtnOrder.Direction, BS_Long);
        assign(rtnOrder.OffsetFlag, OF_Open); 
        assign(rtnOrder.OrderLocalID, std::to_string(OrderLocalID));
        assign(rtnOrder.OrderMaker, OM_Maker);      

        assign(rtnOrder.OrderType, ORDT_Limit);

        // assign(rtnOrder.LandTime, utrade::pandora::NanoTime()); //
        // assign(rtnOrder.SendTime, utrade::pandora::NanoTime()); //

        assign(rtnOrder.StrategyOrderID, std::to_string(OrderLocalID)); 

        assign(rtnOrder.OrderMode, ODM_GTC); 
        assign(rtnOrder.AssetType, AST_DigitalSpot);
        assign(rtnOrder.TradeChannel, account_name);      

        assign(rtnOrder.OrderXO, OX_New);

        // assign(rtnOrder.PlatformTime, utrade::pandora::NanoTimeStr()); //
        // assign(rtnOrder.OrderSysID, std::to_string(OrderLocalID)); //
        // assign(rtnOrder.OrderForeID, std::to_string(OrderLocalID)); //
        // assign(rtnOrder.CreateTime, utrade::pandora::NanoTimeStr()); //
        // assign(rtnOrder.ModifyTime, utrade::pandora::NanoTimeStr()); //
        assign(rtnOrder.RspLocalTime, utrade::pandora::NanoTime()); // 
        // assign(rtnOrder.Cost, 0); //
        // assign(rtnOrder.TradePrice, 0); //
        // assign(rtnOrder.TradeVolume, 0); //
        // assign(rtnOrder.RemainVolume, 0); //
        // assign(rtnOrder.TradeValue, 0); //


        assign(rtnOrder.OrderStatus, OST_Filled);//
        assign(rtnOrder.SessionID, "TestSession");
    }
    catch(const std::exception& e)
    {
        std::cerr << "[E] SetCUTrtnOrder"<< e.what() << '\n';
    }    
}

void SetCUTRtnTradeField(CUTRtnTradeField& rtnTrade, string account_name, long& OrderLocalID)
{
    try
    {
        assign(rtnTrade.UserName, "TestUser9");
        assign(rtnTrade.AccountName, account_name); 
        assign(rtnTrade.AccountType, AT_Physical);

        assign(rtnTrade.ExchangeID, "HUOBI");
        assign(rtnTrade.InstrumentID, "XRP_USDT"); 
        
        
        assign(rtnTrade.MatchPrice, 0.26);         
        assign(rtnTrade.MatchVolume, 20);     
        assign(rtnTrade.MatchValue, 20 * 0.26); 

        assign(rtnTrade.Direction, BS_Long);
        assign(rtnTrade.OrderLocalID, std::to_string(OrderLocalID));

        assign(rtnTrade.Fee, 0);     //
        assign(rtnTrade.FeeCurrency, "USDT");     //

        assign(rtnTrade.PlatformTime, utrade::pandora::NanoTimeStr()); //
        assign(rtnTrade.TradeTime, utrade::pandora::NanoTimeStr()); //
        assign(rtnTrade.RspLocalTime, utrade::pandora::NanoTime()); // 
        
        assign(rtnTrade.StrategyOrderID, std::to_string(OrderLocalID)); 
        assign(rtnTrade.OrderMaker, OM_Maker);      

        assign(rtnTrade.AssetType, AST_DigitalSpot);
        assign(rtnTrade.TradeChannel, account_name);      
        assign(rtnTrade.SessionID, "TestSession");        
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] SetCUTRtnTradeField" << e.what() << '\n';
    }    
}

void SetCUTReqCancelOrderField(CUTReqCancelOrderField& reqCancelOrderField, string account_name, long& OrderLocalID)
{
    assign(reqCancelOrderField.UserName, "TestUser9");
    assign(reqCancelOrderField.AccountName, account_name);
    assign(reqCancelOrderField.AccountType, AT_Physical);
    assign(reqCancelOrderField.Symbol, "XRP_USDT");
    assign(reqCancelOrderField.AssetType, AST_DigitalSpot);
    assign(reqCancelOrderField.OrderLocalID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.OrderForeID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.OrderSysID, std::to_string(OrderLocalID));
    assign(reqCancelOrderField.ExchangeID, "");
    assign(reqCancelOrderField.TradeChannel, "TestSession");
    assign(reqCancelOrderField.SendTime, utrade::pandora::NanoTime()); 
    assign(reqCancelOrderField.SessionID, "TestSession");
}

void SetCUTRspCancelOrderField(CUTRspCancelOrderField& rspCancelOrder, string account_name, long& OrderLocalID)
{
    try
    {
        assign(rspCancelOrder.UserName, "TestUser9");
        assign(rspCancelOrder.AccountName, account_name); 
        assign(rspCancelOrder.AccountType, AT_Physical);
        assign(rspCancelOrder.ExchangeID, "HUOBI");
        assign(rspCancelOrder.InstrumentID, "XRP_USDT"); 
        // assign(rspCancelOrder.InstrumentType, "this_week"); //
        
        assign(rspCancelOrder.Symbol, "XRP_USDT");
        assign(rspCancelOrder.Price, 0.26); 
        assign(rspCancelOrder.LeverRate, 10);
        assign(rspCancelOrder.Volume, 20);      

        assign(rspCancelOrder.Direction, BS_Long);
        assign(rspCancelOrder.OffsetFlag, OF_Open); 
        assign(rspCancelOrder.OrderLocalID, std::to_string(OrderLocalID));
        assign(rspCancelOrder.OrderMaker, OM_Maker);      

        assign(rspCancelOrder.OrderType, ORDT_Limit);

        // assign(rspCancelOrder.LandTime, utrade::pandora::NanoTime()); //
        assign(rspCancelOrder.SendTime, utrade::pandora::NanoTime()); //

        assign(rspCancelOrder.StrategyOrderID, std::to_string(OrderLocalID)); 

        assign(rspCancelOrder.OrderMode, ODM_GTC); 
        assign(rspCancelOrder.AssetType, AST_DigitalSpot);
        assign(rspCancelOrder.TradeChannel, account_name);      

        assign(rspCancelOrder.OrderXO, OX_New);

        // assign(rspCancelOrder.PlatformTime, utrade::pandora::NanoTimeStr()); //
        // assign(rspCancelOrder.OrderSysID, std::to_string(OrderLocalID)); //
        // assign(rspCancelOrder.OrderForeID, std::to_string(OrderLocalID)); //
        // assign(rspCancelOrder.CreateTime, utrade::pandora::NanoTimeStr()); //
        // assign(rspCancelOrder.ModifyTime, utrade::pandora::NanoTimeStr()); //
        assign(rspCancelOrder.RspLocalTime, utrade::pandora::NanoTime()); // 
        // assign(rspCancelOrder.Cost, 0); //
        // assign(rspCancelOrder.TradePrice, 0); //
        // assign(rspCancelOrder.TradeVolume, 0); //
        // assign(rspCancelOrder.RemainVolume, 0); //
        // assign(rspCancelOrder.TradeValue, 0); //


        assign(rspCancelOrder.OrderStatus, OST_Canceled);//
        assign(rspCancelOrder.SessionID, "TestSession");
    }
    catch(const std::exception& e)
    {
        std::cerr << "[E] SetCUTrspCancelOrder"<< e.what() << '\n';
    }    
}

void TestDBEngine::test_create_table()
{
    string host = "127.0.0.1:3306";
    string usr = "wormhole";
    string pwd = "worm";
    string schema = "TestTrade";
    int    port = 3306;

    DBEngine db(usr, pwd, schema, port, host);

    string account_name = "HUOBI_QA_MAIN";

    // db.create_table(account_name);

    test_insert_data(db, account_name);

    // test_get_db_data(db, account_name);

    // db.connect_mysql();            
}

void TestDBEngine::test_insert_data(DBEngine& db,string& account_name)
{
    try
    {        
        int test_numb = 10;

        CUTRspInfoField rspInfoField;

        for (int i = 0; i < test_numb; ++i)
        {

            long OrderLocalID = i;

            CUTReqCreateOrderField pUTReqCreateOrderField;
            SetReqCreateOrderField(pUTReqCreateOrderField, account_name, OrderLocalID);
            db.insert_req_create_order(pUTReqCreateOrderField);

            CUTRspCreateOrderField rspCreateOrderField;
            SetCUTRspCreateOrderField(rspCreateOrderField, account_name, OrderLocalID);
            db.insert_rsp_create_order(rspCreateOrderField, rspInfoField);

            CUTRtnOrderField rtnOrderField;
            SetCUTRtnOrderField(rtnOrderField, account_name, OrderLocalID);
            db.insert_rtn_order(rtnOrderField, rspInfoField);

            CUTRtnTradeField rtnTradeField;
            SetCUTRtnTradeField(rtnTradeField, account_name, OrderLocalID);
            db.insert_rtn_trade(rtnTradeField, rspInfoField);

            CUTReqCancelOrderField reqCancelOrderField;
            SetCUTReqCancelOrderField(reqCancelOrderField, account_name, OrderLocalID);
            db.insert_req_cancel_order(reqCancelOrderField);

            CUTRspCancelOrderField rspCancelOrderField;
            SetCUTRspCancelOrderField(rspCancelOrderField, account_name, OrderLocalID);
            db.insert_rsp_cancel_order(rspCancelOrderField, rspInfoField);

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }


    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] TestDBEngine::test_insert_data " << e.what() << '\n';
    }
    
}

void TestDBEngine::test_get_db_data(DBEngine& db,string& account_name)
{
    try
    {
        unsigned long start_time = 1618886105423156762;
        unsigned long end_time = 1618886114667327594;

        string order_local_id = "3";

        std::vector<PackagePtr> package_list;
        std::vector<string> order_local_id_list;

        // vector<PackagePtr> req_create_order_package_list = db.get_req_create_order_by_time(account_name, start_time, end_time);

        // for(PackagePtr package:req_create_order_package_list)
        // {
        //     auto* p = GET_FIELD(package, CUTRspReqCreateOrderField);
        //     cout << endl;
        //     printUTData(p, UT_FID_RspReqCreateOrder);
        // }

        // PackagePtr package = db.get_rsp_create_order_by_orderlocalid(account_name, order_local_id);
        // auto* p = GET_FIELD(package, CUTRspCreateOrderField);
        // printUTData(p, UT_FID_RspCreateOrder);

        // PackagePtr package = db.get_rtn_order_by_orderlocalid(account_name, order_local_id);
        // auto* p = GET_FIELD(package, CUTRtnOrderField);
        // printUTData(p, UT_FID_RtnOrder);        

        // PackagePtr package = db.get_rtn_trade_by_orderlocalid(account_name, order_local_id);
        // auto* p = GET_FIELD(package, CUTRtnTradeField);
        // printUTData(p, UT_FID_RtnTrade);        

        vector<PackagePtr> req_cancel_order_package_list = db.get_req_cancel_order_by_time(account_name, start_time, end_time);

        for(PackagePtr package:req_cancel_order_package_list)
        {
            auto* p = GET_FIELD(package, CUTRspReqCancelOrderField);
            cout << endl;
            printUTData(p, UT_FID_RspReqCancelOrder);
        }        

        // PackagePtr package = db.get_rsp_cancel_order_by_orderlocalid(account_name, order_local_id);
        // auto* p = GET_FIELD(package, CUTRspCancelOrderField);
        // printUTData(p, UT_FID_RspCancelOrder);           
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] TestDBEngine::test_get_db_data " << e.what() << '\n';
    }
    
}

PackagePtr get_req_rollback_info_package()
{
    try
    {
        PackagePtr package = boost::make_shared<Package>();

        CUTReqRollbackInfoField* pReqRollbackInfo = CREATE_FIELD(package, CUTReqRollbackInfoField);

        assign(pReqRollbackInfo->AccountName, "HUOBI_QA_MAIN");
        assign(pReqRollbackInfo->TimeStart, 1618886105423156762);
        assign(pReqRollbackInfo->TimeEnd, 1618886114667327594);

        package->prepare_request(UT_TID_ReqRollbackInfo, 1);

        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] get_req_rollback_info_package " << e.what() << '\n';
    }
    
}

PackagePtr get_insert_package()
{
    try
    {
        PackagePtr package = boost::make_shared<Package>();

        string account_name = "HUOBI_QA_MAIN";
        long OrderLocalID = 666;

        CUTRspInfoField* pRspInfo = CREATE_FIELD(package, CUTRspInfoField);

        CUTReqCreateOrderField* pReqCreateOrder= CREATE_FIELD(package, CUTReqCreateOrderField);
        SetReqCreateOrderField(*pReqCreateOrder, account_name, OrderLocalID);

        CUTRspCreateOrderField* pRspCreateOrder = CREATE_FIELD(package, CUTRspCreateOrderField);
        SetCUTRspCreateOrderField(*pRspCreateOrder, account_name, OrderLocalID);

        CUTRtnOrderField* pRtnOrder = CREATE_FIELD(package, CUTRtnOrderField);
        SetCUTRtnOrderField(*pRtnOrder, account_name, OrderLocalID);

        CUTRtnTradeField* pRtnTrade = CREATE_FIELD(package, CUTRtnTradeField);
        SetCUTRtnTradeField(*pRtnTrade, account_name, OrderLocalID);

        CUTReqCancelOrderField* pReqCancelOrder= CREATE_FIELD(package, CUTReqCancelOrderField);
        SetCUTReqCancelOrderField(*pReqCancelOrder, account_name, OrderLocalID);
        
        CUTRspCancelOrderField* pRspCancelOrder = CREATE_FIELD(package, CUTRspCancelOrderField);
        SetCUTRspCancelOrderField(*pRspCancelOrder, account_name, OrderLocalID);        
                                        
        // package->prepare_request(UT_TID_ReqCreateOrder, 0);

        // package->prepare_response(UT_TID_RspCreateOrder, 0);

        // package->prepare_response(UT_TID_RtnOrder, 0);

        // package->prepare_response(UT_TID_RtnTrade, 0);


        // package->prepare_request(UT_TID_ReqCancelOrder, 0);

        package->prepare_response(UT_TID_RspCancelOrder, 0);


        return package;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] get_insert_package " << e.what() << '\n';
    }
}

void TestDBEngine::test_trade_console()
{
    try
    {
        utrade::pandora::io_service_pool engine_pool(3);

        TradeConsole trade_console(engine_pool, nullptr);

        trade_console.launch();

        engine_pool.start();
        
        // PackagePtr reqRollbackInfoPackage = get_req_rollback_info_package();
        // const CUTReqRollbackInfoField* p = GET_NON_CONST_FIELD(reqRollbackInfoPackage, CUTReqRollbackInfoField);
        // trade_console.request_message(reqRollbackInfoPackage);
        // // printUTData(p, UT_FID_ReqRollbackInfo);

        PackagePtr package = get_insert_package();
        if (package->is_request_package())
        {
            trade_console.request_message(package);
        }
        else
        {
            trade_console.response_message(package);
        }


        

        engine_pool.block();    
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] test_trade_console " << e.what() << '\n';
    }
}