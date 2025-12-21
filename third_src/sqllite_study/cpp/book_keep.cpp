///@brief class for contain sorting center internal state
///@history
///20200409  赵宇时   创建该文件

#include "book_keeper.h"

#include "quark/cxx/Utils.h"
#include "pandora/util/float_util.h"
#include "quark/cxx/ut/UtPrintUtils.h"
#include "Log/log.h"
#include "sql_script.h"

#include <sstream>
using std::stringstream;

#include "pandora/util/timestamp.h"

BookKeeper::Table::Table(SQLite::Database& db) {
    B_LOG_INFO("Create Table");
    create_table_order(db);
    create_table_trade(db);
    create_table_cancel_req(db);
}

// stmtSelectRequestByRequestID(db_, "SELECT * FROM " CANCEL_REQUEST_TABLENAME " WHERE RequestID = ?;"),
// stmtDeleteRequestByRequestID(db_, "DELETE FROM " CANCEL_REQUEST_TABLENAME " WHERE RequestID = ?;"),

BookKeeper::BookKeeper(const std::string& db_name)
    :db_(db_name, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE),
    table_(db_),
    stmtInsertOrder(db_, insert_order_str()),
    stmtInsertTrade(db_, insert_trade_str()),
    stmtInsertCancelReq(db_, insert_cancel_req_str()),

    stmtSelectOrderByOrderLocalID(db_, "SELECT * FROM " ORDER_TABLENAME " WHERE OrderLocalID = ?;"),
    stmtSelectOrderByOrderSysID(db_, "SELECT * FROM " ORDER_TABLENAME " WHERE OrderSysID = ?;"),
    stmtSelectTradeByTradeID(db_, "SELECT * FROM " TRADE_TABLENAME " WHERE TradeID = ?;"),
    stmtSelectCancelReqByLocalID(db_, "SELECT * FROM " CANCEL_REQUEST_TABLENAME " WHERE OrderLocalID = ?;"),
    stmtSelectCancelReqBySysID(db_, "SELECT * FROM " CANCEL_REQUEST_TABLENAME " WHERE OrderSysID = ?;"),

    stmtUpdateOrderByLocalID(db_, update_order_by_localID_sql_str()),
    stmtUpdateOrderBySysID(db_, update_order_by_SYSID_sql_str()),
    
    stmtDeleteOrderBySysID(db_, "DELETE FROM " ORDER_TABLENAME " WHERE OrderSysID = ?;"),
    stmtDeleteOrderByLocalID(db_, "DELETE FROM " ORDER_TABLENAME " WHERE OrderLocalID = ?;"),
    stmtDeleteTradeByLocalID(db_, "DELETE FROM " TRADE_TABLENAME " WHERE OrderLocalID = ?;"),
    stmtDeleteCancelReqByLocalID(db_, "DELETE FROM " CANCEL_REQUEST_TABLENAME " WHERE OrderLocalID = ?;"),

    stmtSelectDead(db_, "SELECT * FROM " ORDER_TABLENAME " WHERE OrderStatus NOT IN ('Rejected','Killed','Filled') AND LastCheckpoint IS NOT ?;"),
    stmtCleanTrade(db_, "DELETE FROM " TRADE_TABLENAME " WHERE ExecuteTimestamp < ?;"),
    stmtCleanOrder(db_, "DELETE FROM " ORDER_TABLENAME " WHERE OrderStatus IN ('Rejected','Killed','Filled');")
{
    init();
}

void BookKeeper::init() {
    try 
    {
        db_.exec("PRAGMA synchronous = OFF;");
        db_.exec("PRAGMA mmap_size=4294967296;");

        loadCheckpoint(); 
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}


void BookKeeper::Table::create_table_order(SQLite::Database& db) {
    try
    {

        if (db.tableExists(ORDER_TABLENAME)) 
        {
            return; 
        }

        db.exec(create_order_simple_sql_str());
        db.exec("CREATE INDEX " ORDER_TABLENAME "_OrderLocalID_index ON " ORDER_TABLENAME "(OrderLocalID);");
        db.exec("CREATE INDEX " ORDER_TABLENAME "_OrderSysID_index ON " ORDER_TABLENAME "(OrderSysID);");
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

void BookKeeper::Table::create_table_trade(SQLite::Database& db) {
    try
    {
        if (db.tableExists(TRADE_TABLENAME)) {
            return; 
        }

        db.exec(create_trade_simple_sql_str());

        db.exec("CREATE INDEX " TRADE_TABLENAME "_TradeID_index ON " TRADE_TABLENAME "(TradeID);");
    }
    catch(const std::exception& e)
    {
       B_LOG_ERROR(e.what());
    }
}

void BookKeeper::Table::create_table_cancel_req(SQLite::Database& db) {
    try
    {
        if (db.tableExists(CANCEL_REQUEST_TABLENAME)) {
            return; 
        }

        db.exec( create_cancel_simple_sql_str());
        db.exec("CREATE INDEX " CANCEL_REQUEST_TABLENAME "_RequestID_index ON " CANCEL_REQUEST_TABLENAME "(RequestID);");
        db.exec("CREATE INDEX " CANCEL_REQUEST_TABLENAME "_OrderLocalID_index ON " CANCEL_REQUEST_TABLENAME "(OrderLocalID);");
        db.exec("CREATE INDEX " CANCEL_REQUEST_TABLENAME "_OrderSysID_index ON " CANCEL_REQUEST_TABLENAME "(OrderSysID);");
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

void BookKeeper::loadCheckpoint() 
{
    checkpoint_ = db_.execAndGet("SELECT MIN(LastCheckpoint) FROM " ORDER_TABLENAME).getInt();

    if (checkpoint_ == 0) 
    {
        checkpoint_ = utrade::pandora::NanoTime()/NANOSECONDS_PER_SECOND - 3600 * 12;
    }
}

void BookKeeper::InsertOrderOnReqCreateOrder(const PackagePtr package) {
    try
    {
        CUTReqCreateOrderField* pCreateOrder = GET_NON_CONST_FIELD(package, CUTReqCreateOrderField);
        if (!pCreateOrder) 
        {
            B_LOG_ERROR("pCreateOrder is Empty");
            return;
        }

        pCreateOrder->RemainVolume = pCreateOrder->Volume;

        if (!insert_order(*pCreateOrder))
        {
            B_LOG_ERROR("Insert Order Failed");
        }
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [InsertOrderOnReqCreateOrder] SQLite exception: " << e.what());
    }
}

void BookKeeper::InsertTradeOnRtnTrade(PackagePtr package) {
    try
    {
        const CUTRtnTradeField* pRtnTrade = GET_FIELD(package, CUTRtnTradeField);
        if (!pRtnTrade) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [InsertTradeOnRtnTrade] empty fields!");
            return;
        }

        stmtInsertTrade.reset();
        SQLite::bind(stmtInsertTrade,
            pRtnTrade->TradeID,
            pandora::ToSecond(pRtnTrade->TradeTime, DATETIME_FORMAT)
        );
        stmtInsertTrade.exec();
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [InsertTradeOnRtnTrade] SQLite exception: " << e.what());
    }
}

void BookKeeper::InsertRequestOnReqCancelOrder(PackagePtr package) {
    try
    {
        const CUTReqCancelOrderField* pReqCancelOrder = GET_FIELD(package, CUTReqCancelOrderField);
        if (!pReqCancelOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [InsertRequestOnReqCancelOrder] empty fields!");
            return;
        }

        stmtInsertCancelReq.reset();
        SQLite::bind(stmtInsertCancelReq,
            package->RequestID(),
            pReqCancelOrder->OrderLocalID,
            pReqCancelOrder->OrderSysID,
            pReqCancelOrder->UserName,
            package->SessionID(),
            pReqCancelOrder->AccountName
        );
        stmtInsertCancelReq.exec();
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [InsertRequestOnReqCancelOrder] SQLite exception: " << e.what());
    }
}

/*
 * 判断是否过期, 根据成交时间来判断
 * 判断这笔成交是否已经收到过
 */
bool BookKeeper::IsNewTradeOnRtnTrade(PackagePtr package) 
{
    cout<<"BookKeeper::IsNewTradeOnRtnTrade"<<endl;
    try
    {
        const CUTRtnTradeField* pRtnTrade = GET_FIELD(package, CUTRtnTradeField);
        if (!pRtnTrade) 
        {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [IsNewTradeOnRtnTrade] empty fields!");
            return false;
        }

        stmtSelectTradeByTradeID.reset();
        SQLite::bind(stmtSelectTradeByTradeID,
                     pRtnTrade->TradeID );

        if (stmtSelectTradeByTradeID.executeStep()) 
        {
            UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [IsNewTradeOnRtnTrade] SQLite Store TradeInfo Failed!");
            return false;
        }

        return true;
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [IsNewTradeOnRtnTrade] SQLite exception: " << e.what());
        return false;
    }
}

bool BookKeeper::FetchOrderOnRspCreateOrder(PackagePtr package) {
    cout<< "BookKeeper::FetchOrderOnRspCreateOrder: " << package->RequestID() << endl;
    try
    {
        CUTRspCreateOrderField* pRspCreateOrder = GET_NON_CONST_FIELD(package, CUTRspCreateOrderField);
        if (!pRspCreateOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [FetchOrderOnRspCreateOrder] empty fields!");
            return false;
        }

        cout<<"BookKeeper::FetchOrderOnRspCreateOrder OrderLocalID "<< pRspCreateOrder->OrderLocalID <<endl;
        stmtSelectOrderByOrderLocalID.reset();
        SQLite::bind(stmtSelectOrderByOrderLocalID,
            pRspCreateOrder->OrderLocalID
        );

        cout << "BookKeeper::FetchOrderOnRspCreateOrder Bind success!" << endl;
        if (stmtSelectOrderByOrderLocalID.executeStep()) {

            // cout << "\nDatabase Stored Info: " << stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderStatus).getString() << endl;

            // Why?
            if (stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderStatus).getString() != "Pending") {
                // return false;
            }

            package->SetSessionID(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());
            package->SetRequestID(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt());

            // cout << "Stored session id: " << stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText()
            //      << "\nStored request id: " << stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt()
            //      << endl;

            assign(pRspCreateOrder->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());
            assign(pRspCreateOrder->ExchangeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ExchangeID).getText());
            assign(pRspCreateOrder->InstrumentID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_InstrumentID).getText());
            assign(pRspCreateOrder->Price, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Price).getDouble());
            assign(pRspCreateOrder->Volume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Volume).getDouble());
            assign(pRspCreateOrder->Direction, getDirection(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Direction).getText()));
            assign(pRspCreateOrder->OrderMaker, getOrderMaker(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderMaker).getText()));
            assign(pRspCreateOrder->OrderType, getOrderType(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderType).getText()));

            //wang.hy 添加字段
            assign(pRspCreateOrder->OrderForeID, pRspCreateOrder->OrderLocalID);
            assign(pRspCreateOrder->RequestID, package->RequestID());
            assign(pRspCreateOrder->RequestForeID, package->RequestID());
            return true;
        }
        cout<<"BookKeeper::FetchOrderOnRspCreateOrder ExecuteStep False"<<endl;
        return false;
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [FetchOrderOnRspCreateOrder] SQLite exception: " << e.what());
        return false;
    }
}

bool BookKeeper::FetchRequestOnRspCancelOrder(PackagePtr package) {
    try
    {
        cout<< "BookKeeper::FetchRequestOnRspCancelOrder"<<endl;

        CUTRspCancelOrderField* pRspCancelOrder = GET_NON_CONST_FIELD(package, CUTRspCancelOrderField);
        // cout<< convertUTData(pRspCancelOrder, UT_FID_RspCancelOrder) <<endl;
        if (!pRspCancelOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [FetchRequestOnRspCancelOrder] empty fields!");
            return false;
        }

        if (strcmp(pRspCancelOrder->OrderLocalID,"") != 0) {
            stmtSelectCancelReqByLocalID.reset();
            SQLite::bind(stmtSelectCancelReqByLocalID,
                pRspCancelOrder->OrderLocalID
            );

            if (stmtSelectCancelReqByLocalID.executeStep()) {
                package->SetRequestID(stmtSelectCancelReqByLocalID.getColumn(REQUEST_INDEX_RequestID).getInt());
                package->SetSessionID(stmtSelectCancelReqByLocalID.getColumn(REQUEST_INDEX_SessionID).getText());
                assign(pRspCancelOrder->UserName, stmtSelectCancelReqByLocalID.getColumn(REQUEST_INDEX_UserName).getText());
                return true;        
            }
        }
        cout<< "BookKeeper::FetchRequestOnRspCancelOrder  OrderLocalID: " << pRspCancelOrder->OrderLocalID <<endl;

        //wang.hy 如果自己生成RspCancelOrder的 RequestID都是-1
        // if (package->RequestID() != -1) {
        //     stmtSelectRequestByRequestID.reset();
        //     SQLite::bind(stmtSelectRequestByRequestID,
        //         package->RequestID()
        //     );
        //     if (stmtSelectRequestByRequestID.executeStep()) {
        //         package->SetSessionID(stmtSelectRequestByRequestID.getColumn(REQUEST_INDEX_SessionID).getText());
        //         assign(pRspCancelOrder->UserName, stmtSelectRequestByRequestID.getColumn(REQUEST_INDEX_UserName).getText());
        //         return true;        
        //     }
        // }

        cout<< "BookKeeper::FetchRequestOnRspCancelOrder  RequestID: " << package->RequestID() <<endl;

        if (strcmp(pRspCancelOrder->OrderSysID, "") != 0) {
            stmtSelectCancelReqBySysID.reset();
            SQLite::bind(stmtSelectCancelReqBySysID,
                pRspCancelOrder->OrderSysID
            );

            if (stmtSelectCancelReqBySysID.executeStep()) {
                package->SetRequestID(stmtSelectCancelReqBySysID.getColumn(REQUEST_INDEX_RequestID).getInt());
                package->SetSessionID(stmtSelectCancelReqBySysID.getColumn(REQUEST_INDEX_SessionID).getText());
                assign(pRspCancelOrder->UserName, stmtSelectCancelReqBySysID.getColumn(REQUEST_INDEX_UserName).getText());
                return true;        
            }
        }
        cout<< "BookKeeper::FetchRequestOnRspCancelOrder  OrderSysID: " << pRspCancelOrder->OrderSysID <<endl;

        return false;
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [FetchRequestOnRspCancelOrder] SQLite exception: " << e.what());
        return false;
    }
}

void BookKeeper::UpdateOrderOnRspCreateOrder(PackagePtr package) {
    cout<<__func__<<endl;
    try
    {
        const CUTRspCreateOrderField* pCreateOrder = GET_FIELD(package, CUTRspCreateOrderField);
        if (!pCreateOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [UpdateOrderOnRspCreateOrder] empty fields!");
            return;
        }

        stmtUpdateOrderByLocalID.reset();
        double ExpectVolume = pCreateOrder->Volume;
        SQLite::bind(stmtUpdateOrderByLocalID,
            pCreateOrder->OrderSysID,
            getOrderStatusString(pCreateOrder->OrderStatus),
            pCreateOrder->TradePrice,
            pCreateOrder->TradeVolume,
            ExpectVolume,
            pCreateOrder->Fee,
            pCreateOrder->FeeCurrency,
            pCreateOrder->CreateTime,
            pCreateOrder->ModifyTime,
            checkpoint_,
            pCreateOrder->OrderLocalID
        );
        stmtUpdateOrderByLocalID.exec();
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [UpdateOrderOnRspCreateOrder] SQLite exception: " << e.what());
    }
}

void BookKeeper::UpdateOrderOnRtnOrder(PackagePtr package) {
    try
    {
        const CUTRtnOrderField* pRtnOrder = GET_FIELD(package, CUTRtnOrderField);
        if (!pRtnOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [UpdateOrderOnRtnOrder] empty fields!");
            return;
        }

        stmtUpdateOrderByLocalID.reset();
        double ExpectVolume;
        if (pRtnOrder->OrderStatus == OST_Killed || pRtnOrder->OrderStatus == OST_Canceled) {
            ExpectVolume = pRtnOrder->TradeVolume;
        } else {
            ExpectVolume = pRtnOrder->Volume;
        }

        if (is_order_total_over(pRtnOrder->OrderStatus))
        {
            delete_cancel_req(pRtnOrder->OrderLocalID);
        }
        else
        {
            SQLite::bind(stmtUpdateOrderByLocalID,
                pRtnOrder->OrderSysID,
                getOrderStatusString(pRtnOrder->OrderStatus),
                pRtnOrder->TradePrice,
                pRtnOrder->TradeVolume,
                ExpectVolume,
                pRtnOrder->Fee,
                pRtnOrder->FeeCurrency,
                pRtnOrder->CreateTime,
                pRtnOrder->ModifyTime,
                checkpoint_,
                pRtnOrder->OrderLocalID
            );
            stmtUpdateOrderByLocalID.exec();
        }


    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [UpdateOrderOnRtnOrder] SQLite exception: " << e.what());
    }
}

bool BookKeeper::CompleteRtnOrderPackage(PackagePtr package)
{
    CUTRtnOrderField* pRtnOrderField = GET_NON_CONST_FIELD(package, CUTRtnOrderField);

    CUTRtnOrderField db_rtn_order;

    if (get_order_field(pRtnOrderField->OrderLocalID, pRtnOrderField->OrderSysID, db_rtn_order))
    {
        package->prepare_response(
            UT_TID_RtnOrder,
            db_rtn_order.RequestID,
            CHAIN_LAST,
            db_rtn_order.SessionID
        );

        assign(pRtnOrderField->UserName, db_rtn_order.UserName);
        assign(pRtnOrderField->SessionID, db_rtn_order.SessionID);
        assign(pRtnOrderField->RequestID, db_rtn_order.RequestID);
        assign(pRtnOrderField->OrderForeID, db_rtn_order.OrderForeID);

        return true;
    }
    else
    {
        B_LOG_ERROR("Get Data From DB Failed");
        return false;
    }

    // if (!check_order_by_local_id(pRtnOrderField->OrderLocalID))
    // {
    //     cout << "CompleteRtnOrderPackage Failed , Order By Local ID: " << pRtnOrderField->OrderLocalID << " does not exits " << endl;
    //     return false;
    // }
    // else
    // {
    //     package->prepare_response(
    //         UT_TID_RtnOrder,
    //         stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt(),
    //         CHAIN_LAST,
    //         stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText()
    //     );

    //     assign(pRtnOrderField->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());
    //     assign(pRtnOrderField->SessionID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());
    //     assign(pRtnOrderField->RequestID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt());
    //     assign(pRtnOrderField->OrderForeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderForeID).getText());
    //     return true;
    // }    
}

bool BookKeeper::CompleteRtnTradePackage(PackagePtr package)
{

    CUTRtnTradeField* pRtnTrade = GET_NON_CONST_FIELD(package, CUTRtnTradeField);

    if (check_order_by_local_id(pRtnTrade->OrderLocalID))
    {
        UT_LOG_INFO(BRIDGE_LOGGER, "CompleteRtnTradePackage by order_local_id:  " << pRtnTrade->OrderLocalID);
        package->prepare_response(
            UT_TID_RtnTrade,
            stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt(),
            CHAIN_LAST,
            stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText()
        );

        assign(pRtnTrade->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());
        assign(pRtnTrade->SessionID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());
        return true;
    }
    else if (check_order_by_sys_id(pRtnTrade->OrderSysID))
    {
        UT_LOG_INFO(BRIDGE_LOGGER, "CompleteRtnTradePackage by OrderSysID:  " << pRtnTrade->OrderSysID);
        package->prepare_response(
            UT_TID_RtnTrade,
            stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_RequestID).getInt(),
            CHAIN_LAST,
            stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_SessionID).getText()
        );

        assign(pRtnTrade->UserName, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_UserName).getText());
        assign(pRtnTrade->SessionID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_SessionID).getText());
        return true;
    }  
    else
    {
        UT_LOG_INFO(BRIDGE_LOGGER, "CompleteRtnTradePackage Failed!  OrderSysID: " 
                                    << pRtnTrade->OrderSysID 
                                    << ", OrderLocalID: " << pRtnTrade->OrderLocalID
                                    << " is error!");
        return false;        
    }
}

// void BookKeeper::DeleteRequestOnRspCancelOrder(PackagePtr package) {
//     try
//     {
//         stmtDeleteRequestByRequestID.reset();
//         SQLite::bind(stmtDeleteRequestByRequestID,
//             package->RequestID()
//         );
//         stmtDeleteRequestByRequestID.exec();
//     }
//     catch(const std::exception& e)
//     {
//         UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [DeleteRequestOnRspCancelOrder] SQLite exception: " << e.what());
//     }
// }

bool BookKeeper::is_order_over(SQLite::Statement& stmt) 
{
    std::string orderStatus = stmt.getColumn(ORDER_INDEX_OrderStatus).getText();
    cout<<"BookKeeper is_order_over "<< orderStatus<<endl;
    if (orderStatus == "Rejected" || orderStatus == "Filled" || orderStatus == "Killed") 
    {
        return false;
    }

    return true;
}

PackagePtr BookKeeper::MakeRspCreateOrderPackageByOrderLocalID(const std::string& orderLocalID) 
{
    cout<<__func__<<" "<< orderLocalID<<endl;
    try
    {
        if (check_order_by_local_id(orderLocalID))
        {
            if (!is_order_over(stmtSelectOrderByOrderLocalID)) 
            {
                cout << "MakeRspCreateOrderPackageByOrderLocalID Failed! BookKeeper: "<< orderLocalID << " is over!" <<endl;
                return nullptr;
            }

            PackagePtr package_new{new Package()};
            
            package_new->prepare_response(
                UT_TID_RspCreateOrder,
                stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt(),
                CHAIN_LAST,
                stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText()
            );

            CREATE_FIELD(package_new, CUTRspInfoField); 
            CUTRspCreateOrderField* pRspCreateOrder = CREATE_FIELD(package_new, CUTRspCreateOrderField);
            assign(pRspCreateOrder->OrderLocalID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderLocalID).getText());
            assign(pRspCreateOrder->AccountName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_AccountName).getText());
            assign(pRspCreateOrder->OrderForeID, pRspCreateOrder->OrderLocalID);

            assign(pRspCreateOrder->RequestForeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt());
            assign(pRspCreateOrder->RequestID, pRspCreateOrder->RequestForeID);
            assign(pRspCreateOrder->SessionID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());

            assign(pRspCreateOrder->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());

            assign(pRspCreateOrder->ExchangeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ExchangeID).getText());
            assign(pRspCreateOrder->InstrumentID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_InstrumentID).getText());
            assign(pRspCreateOrder->Price, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Price).getDouble());
            assign(pRspCreateOrder->Volume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Volume).getDouble());
            assign(pRspCreateOrder->Direction, getDirection(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Direction).getText()));
            assign(pRspCreateOrder->OrderMaker, getOrderMaker(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderMaker).getText()));
            assign(pRspCreateOrder->OrderType, getOrderType(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderType).getText()));
            assign(pRspCreateOrder->OrderSysID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderSysID).getText());
            assign(pRspCreateOrder->OrderStatus, getOrderStatus(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderStatus).getText()));
            assign(pRspCreateOrder->TradePrice, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradePrice).getDouble());
            assign(pRspCreateOrder->TradeVolume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradeVolume).getDouble());

            assign(pRspCreateOrder->Fee, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Fee).getDouble());
            assign(pRspCreateOrder->FeeCurrency, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_FeeCurrency).getText());
            assign(pRspCreateOrder->CreateTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_CreateTime).getText());
            assign(pRspCreateOrder->ModifyTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ModifyTime).getText());

            return package_new;
        }
        else
        {
            cout << "CompleteRtnOrderPackage Failed , Order By Local ID: " << orderLocalID << " does not exits " << endl;
            return nullptr;
        }
        
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [MakeRspCreateOrderPackageByOrderLocalID] SQLite exception: " << e.what());
        return nullptr;
    }
}

PackagePtr BookKeeper::MakeRtnOrderPackageByOrderLocalID(const std::string& orderLocalID)
{
    cout<<__func__<<" "<< orderLocalID<<endl;
    try
    {
        if (check_order_by_local_id(orderLocalID))
        {
            if (!is_order_over(stmtSelectOrderByOrderLocalID)) 
            {
                stringstream sstream_data;
                sstream_data << "MakeRtnOrderPackageByOrderLocalID Failed , Order By LocalID: " << orderLocalID << " is over!" << endl; 
                UT_LOG_ERROR(BRIDGE_LOGGER, sstream_data.str());                   
                return nullptr;
            }

            PackagePtr package_new{new Package()};
            
            package_new->prepare_response(
                UT_TID_RtnOrder,
                stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt(),
                CHAIN_LAST,
                stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText()
            );

            CUTRtnOrderField* pRtnOrder = CREATE_FIELD(package_new, CUTRtnOrderField);

            assign(pRtnOrder->OrderLocalID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderLocalID).getText());
            assign(pRtnOrder->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());

            assign(pRtnOrder->OrderForeID, pRtnOrder->OrderLocalID);
            assign(pRtnOrder->RequestForeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt());
            assign(pRtnOrder->RequestID, pRtnOrder->RequestForeID);
            assign(pRtnOrder->SessionID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());


            assign(pRtnOrder->ExchangeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ExchangeID).getText());
            assign(pRtnOrder->InstrumentID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_InstrumentID).getText());
            assign(pRtnOrder->Price, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Price).getDouble());
            assign(pRtnOrder->Volume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Volume).getDouble());
            assign(pRtnOrder->Direction, getDirection(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Direction).getText()));
            assign(pRtnOrder->OrderMaker, getOrderMaker(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderMaker).getText()));
            assign(pRtnOrder->OrderType, getOrderType(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderType).getText()));
            assign(pRtnOrder->OrderSysID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderSysID).getText());
            assign(pRtnOrder->OrderStatus, getOrderStatus(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderStatus).getText()));
            assign(pRtnOrder->TradePrice, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradePrice).getDouble());
            assign(pRtnOrder->TradeVolume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradeVolume).getDouble());

            assign(pRtnOrder->Fee, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Fee).getDouble());
            assign(pRtnOrder->FeeCurrency, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_FeeCurrency).getText());
            assign(pRtnOrder->CreateTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_CreateTime).getText());
            assign(pRtnOrder->ModifyTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ModifyTime).getText());

            // CUTRspCreateOrderField* pRspCreateOrder = CREATE_FIELD(package_new, CUTRspCreateOrderField);
            // assign(pRspCreateOrder->OrderLocalID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderLocalID).getText());
            // assign(pRspCreateOrder->OrderForeID, pRspCreateOrder->OrderLocalID);

            // assign(pRspCreateOrder->RequestForeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_RequestID).getInt());
            // assign(pRspCreateOrder->RequestID, pRspCreateOrder->RequestForeID);
            // assign(pRspCreateOrder->SessionID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_SessionID).getText());

            // assign(pRspCreateOrder->UserName, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_UserName).getText());

            // assign(pRspCreateOrder->ExchangeID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ExchangeID).getText());
            // assign(pRspCreateOrder->InstrumentID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_InstrumentID).getText());
            // assign(pRspCreateOrder->Price, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Price).getDouble());
            // assign(pRspCreateOrder->Volume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Volume).getDouble());
            // assign(pRspCreateOrder->Direction, getDirection(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Direction).getText()));
            // assign(pRspCreateOrder->OrderMaker, getOrderMaker(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderMaker).getText()));
            // assign(pRspCreateOrder->OrderType, getOrderType(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderType).getText()));
            // assign(pRspCreateOrder->OrderSysID, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderSysID).getText());
            // assign(pRspCreateOrder->OrderStatus, getOrderStatus(stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_OrderStatus).getText()));
            // assign(pRspCreateOrder->TradePrice, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradePrice).getDouble());
            // assign(pRspCreateOrder->TradeVolume, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_TradeVolume).getDouble());

            // assign(pRspCreateOrder->Fee, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_Fee).getDouble());
            // assign(pRspCreateOrder->FeeCurrency, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_FeeCurrency).getText());
            // assign(pRspCreateOrder->CreateTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_CreateTime).getText());
            // assign(pRspCreateOrder->ModifyTime, stmtSelectOrderByOrderLocalID.getColumn(ORDER_INDEX_ModifyTime).getText());

            return package_new;
        }
        else
        {
            stringstream sstream_data;
            sstream_data << "MakeRtnOrderPackageByOrderLocalID Failed , Order By Local ID: " << orderLocalID << " does not exits " << endl; 
            UT_LOG_ERROR(BRIDGE_LOGGER, sstream_data.str());            
            return nullptr;
        }
        
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [MakeRspCreateOrderPackageByOrderLocalID] SQLite exception: " << e.what());
        return nullptr;
    }    
}

PackagePtr BookKeeper::MakeRtnOrderPackageByOrderSysID(const std::string& orderSysID) 
{
    try
    {
        if (check_order_by_sys_id(orderSysID))
        {
            if (!is_order_over(stmtSelectOrderByOrderSysID)) 
            {
                stringstream sstream_data;
                sstream_data << "MakeRtnOrderPackageByOrderSysID Failed , Order By SysID: " << orderSysID << " is over!" << endl; 
                UT_LOG_ERROR(BRIDGE_LOGGER, sstream_data.str());                   
                return nullptr;
            }

            PackagePtr package_new{new Package()};

            package_new->prepare_response(
                UT_TID_RtnOrder,
                stmtSelectOrderByOrderSysID.getColumn(3).getInt(),
                CHAIN_LAST,
                stmtSelectOrderByOrderSysID.getColumn(2).getText()
            );

            CUTRtnOrderField* pRtnOrder = CREATE_FIELD(package_new, CUTRtnOrderField);
            assign(pRtnOrder->OrderLocalID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_OrderLocalID).getText());
            assign(pRtnOrder->UserName, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_UserName).getText());

            assign(pRtnOrder->OrderForeID, pRtnOrder->OrderLocalID);
            assign(pRtnOrder->RequestForeID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_RequestID).getInt());
            assign(pRtnOrder->RequestID, pRtnOrder->RequestForeID);
            assign(pRtnOrder->SessionID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_SessionID).getText());


            assign(pRtnOrder->ExchangeID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_ExchangeID).getText());
            assign(pRtnOrder->InstrumentID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_InstrumentID).getText());
            assign(pRtnOrder->Price, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_Price).getDouble());
            assign(pRtnOrder->Volume, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_Volume).getDouble());
            assign(pRtnOrder->Direction, getDirection(stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_Direction).getText()));
            assign(pRtnOrder->OrderMaker, getOrderMaker(stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_OrderMaker).getText()));
            assign(pRtnOrder->OrderType, getOrderType(stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_OrderType).getText()));
            assign(pRtnOrder->OrderSysID, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_OrderSysID).getText());
            assign(pRtnOrder->OrderStatus, getOrderStatus(stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_OrderStatus).getText()));
            assign(pRtnOrder->TradePrice, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_TradePrice).getDouble());
            assign(pRtnOrder->TradeVolume, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_TradeVolume).getDouble());

            assign(pRtnOrder->Fee, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_Fee).getDouble());
            assign(pRtnOrder->FeeCurrency, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_FeeCurrency).getText());
            assign(pRtnOrder->CreateTime, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_CreateTime).getText());
            assign(pRtnOrder->ModifyTime, stmtSelectOrderByOrderSysID.getColumn(ORDER_INDEX_ModifyTime).getText());

            return package_new;
        }
        else
        {
            stringstream sstream_data;
            sstream_data << "MakeRtnOrderPackageByOrderSysID Failed , Order By SysID: " << orderSysID << " does not exits " << endl; 
            UT_LOG_ERROR(BRIDGE_LOGGER, sstream_data.str());            
            return nullptr;            
        }        
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [MakeRtnOrderPackageByOrderSysID] SQLite exception: " << e.what());
        return nullptr;
    }
}

PackagePtr BookKeeper::MakeRtnOrderPackageOnRtnTrade(PackagePtr package) 
{
    cout<<__func__<<endl;
    try
    {
        CUTRtnTradeField* pRtnTrade = GET_NON_CONST_FIELD(package, CUTRtnTradeField);
        if (!pRtnTrade) 
        {
            UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [MakeRtnOrderPackageOnRtnTrade] GET RtnTradeField Failed!");
            return nullptr;
        }

        PackagePtr package_new;

        if (check_order_by_local_id(pRtnTrade->OrderLocalID))
        {
            package_new = MakeRtnOrderPackageByOrderLocalID(pRtnTrade->OrderLocalID);
        }
        else if (check_order_by_sys_id(pRtnTrade->OrderSysID))
        {
            package_new = MakeRtnOrderPackageByOrderSysID(pRtnTrade->OrderSysID);
        }
        else
        {
            return nullptr;
        }

        //wang.hy 判空处理
        if(!package_new)
        {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [MakeRtnOrderPackageOnRtnTrade] Create RtnOrder Package Failed!");
            return nullptr;
        }
            
        CUTRtnOrderField* pRtnOrder = GET_NON_CONST_FIELD(package_new, CUTRtnOrderField);
        if (!pRtnOrder) {
            UT_LOG_INFO(BRIDGE_LOGGER, "[Book Keeper] [MakeRtnOrderPackageOnRtnTrade] Create RtnOrder Package Failed!");
            return nullptr;
        }
        //assign(pRtnTrade->SessionID)

        double newTradeVolume = pRtnTrade->MatchVolume + pRtnOrder->TradeVolume;
        double newTradeValue  = pRtnTrade->MatchValue + pRtnOrder->TradeValue;
        double newTradePrice  = newTradeValue / newTradeVolume;

        assign(pRtnOrder->TradeVolume, newTradeVolume);
        assign(pRtnOrder->TradeValue, newTradeValue);
        assign(pRtnOrder->TradePrice, newTradePrice);
        assign(pRtnOrder->Fee, pRtnTrade->Fee + pRtnOrder->Fee);
        assign(pRtnOrder->FeeCurrency, pRtnTrade->FeeCurrency);
        assign(pRtnOrder->ModifyTime, pRtnTrade->TradeTime);

        if (utrade::pandora::great_equal(pRtnOrder->TradeVolume, pRtnOrder->Volume)) 
        {
            assign(pRtnOrder->OrderStatus, OST_Filled); 
        }
        // else if (utrade::pandora::great_equal(pRtnOrder->TradeVolume, expectVolume)) 
        // {
        //     assign(pRtnOrder->OrderStatus, OST_Killed); 
        // }

        //设置RtnTrade的Session, UserName
        assign(pRtnTrade->UserName, pRtnOrder->UserName);
        package->SetSessionID(package_new->SessionID());
        
        return package_new;        
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [MakeRtnOrderPackageOnRtnTrade] SQLite exception: " << e.what());
        return nullptr;
    }
}

PackagePtr BookKeeper::makeRtnOrderPackageByOrderSysIDAtom(SQLite::Statement& stmt) {
    cout<<__func__<<endl;
    if (!is_order_over(stmt)) {
        return nullptr;
    }

    PackagePtr package_new{new Package()};

    package_new->prepare_response(
        UT_TID_RtnOrder,
        stmt.getColumn(3).getInt(),
        CHAIN_LAST,
        stmt.getColumn(2).getText()
    );

    CUTRtnOrderField* pRtnOrder = CREATE_FIELD(package_new, CUTRtnOrderField);
    assign(pRtnOrder->OrderLocalID, stmt.getColumn(ORDER_INDEX_OrderLocalID).getText());
    assign(pRtnOrder->UserName, stmt.getColumn(ORDER_INDEX_UserName).getText());

    assign(pRtnOrder->OrderForeID, pRtnOrder->OrderLocalID);
    assign(pRtnOrder->RequestForeID, stmt.getColumn(ORDER_INDEX_RequestID).getInt());
    assign(pRtnOrder->RequestID, pRtnOrder->RequestForeID);
    assign(pRtnOrder->SessionID, stmt.getColumn(ORDER_INDEX_SessionID).getText());


    assign(pRtnOrder->ExchangeID, stmt.getColumn(ORDER_INDEX_ExchangeID).getText());
    assign(pRtnOrder->InstrumentID, stmt.getColumn(ORDER_INDEX_InstrumentID).getText());
    assign(pRtnOrder->Price, stmt.getColumn(ORDER_INDEX_Price).getDouble());
    assign(pRtnOrder->Volume, stmt.getColumn(ORDER_INDEX_Volume).getDouble());
    assign(pRtnOrder->Direction, getDirection(stmt.getColumn(ORDER_INDEX_Direction).getText()));
    assign(pRtnOrder->OrderMaker, getOrderMaker(stmt.getColumn(ORDER_INDEX_OrderMaker).getText()));
    assign(pRtnOrder->OrderType, getOrderType(stmt.getColumn(ORDER_INDEX_OrderType).getText()));
    assign(pRtnOrder->OrderSysID, stmt.getColumn(ORDER_INDEX_OrderSysID).getText());
    assign(pRtnOrder->OrderStatus, getOrderStatus(stmt.getColumn(ORDER_INDEX_OrderStatus).getText()));
    assign(pRtnOrder->TradePrice, stmt.getColumn(ORDER_INDEX_TradePrice).getDouble());
    assign(pRtnOrder->TradeVolume, stmt.getColumn(ORDER_INDEX_TradeVolume).getDouble());

    assign(pRtnOrder->Fee, stmt.getColumn(ORDER_INDEX_Fee).getDouble());
    assign(pRtnOrder->FeeCurrency, stmt.getColumn(ORDER_INDEX_FeeCurrency).getText());
    assign(pRtnOrder->CreateTime, stmt.getColumn(ORDER_INDEX_CreateTime).getText());
    assign(pRtnOrder->ModifyTime, stmt.getColumn(ORDER_INDEX_ModifyTime).getText());

    return package_new;
}

std::vector<PackagePtr> BookKeeper::PresumeDeath() {
    cout<<__func__<<endl;
    try
    {
        stmtSelectDead.reset();
        SQLite::bind(stmtSelectDead,
            checkpoint_
        );

        std::vector<PackagePtr> res;
        while(stmtSelectDead.executeStep()) {
            if (stmtSelectDead.getColumn(11).getString() == "") {
                // res.push_back(MakeRspCreateOrderPackageByOrderLocalID(stmtSelectDead));
            } else {
                res.push_back(makeRtnOrderPackageByOrderSysIDAtom(stmtSelectDead));
            }
        }
        return res;
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [PresumeDeath] SQLite exception: " << e.what());
        return std::vector<PackagePtr>();
    }
}

void BookKeeper::CommitCheckpoint(const char* timeStr) {
    cout<<__func__<<" "<<timeStr<<endl;
    try
    {
        checkpoint_ = pandora::ToSecond(timeStr, DATETIME_FORMAT);

        stmtCleanTrade.reset();
        stmtCleanTrade.exec();

        stmtCleanOrder.reset();
        stmtCleanOrder.exec();
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [CommitCheckpoint] SQLite exception: " << e.what());
    }
}

bool BookKeeper::check_order_by_local_id(const std::string& order_local_id)
{
    try
    {
        if (order_local_id == "") 
        {
            B_LOG_WARN("OrderLocalID is Empty!");
            return false;
        }

        stmtSelectOrderByOrderLocalID.reset();
        SQLite::bind(stmtSelectOrderByOrderLocalID, order_local_id);

        if (stmtSelectOrderByOrderLocalID.executeStep()) 
        {
            return true;
        }
        else
        {
            B_LOG_ERROR("OrderLocalID: " + order_local_id + " does not exits");
            return false;
        }        
    }
    catch(const std::exception& e)
    {
        UT_LOG_ERROR(BRIDGE_LOGGER, "[Book Keeper] [check_order_by_local_id] SQLite exception: " << e.what());
        return false;
    }    
}

bool BookKeeper::check_order_by_sys_id(const std::string& order_sys_id)
{
    try
    {
        stmtSelectOrderByOrderSysID.reset();
        SQLite::bind(stmtSelectOrderByOrderSysID, order_sys_id);

        if (stmtSelectOrderByOrderSysID.executeStep()) 
        {
            return true;
        }
        else
        {
            B_LOG_ERROR("OrderSysID: " + order_sys_id + " does not exits");
            return false;
        }
        

        return false;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
        return false;
    }    
}

bool BookKeeper::get_order_status(string order_local_id, COrderStatusType& status)
{
    try
    {

        SQLite::Statement stmtSelectOrderStatusByLocalID(db_, "SELECT OrderStatus FROM " ORDER_TABLENAME " WHERE OrderLocalID = ?;");

        SQLite::bind(stmtSelectOrderStatusByLocalID, order_local_id );

        if (stmtSelectOrderStatusByLocalID.executeStep()) 
        {
            string status_str = stmtSelectOrderStatusByLocalID.getColumn(0).getText();
            status = getOrderStatus(status_str);

            return true;
        }   
        else
        {
            B_LOG_ERROR("OrderLocalID: " + order_local_id + " does not exits");

            return false;
        }

    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

void BookKeeper::get_order_filed_from_stmt(SQLite::Statement& select_stmt, CUTRtnOrderField& rtn_order)
{
    try
    {
        assign(rtn_order.UserName, select_stmt.getColumn(0).getText());
        assign(rtn_order.AccountName, select_stmt.getColumn(1).getText());
        assign(rtn_order.AccountType, getAccountType(select_stmt.getColumn(2).getText()));
        assign(rtn_order.ExchangeID, select_stmt.getColumn(3).getText());
        assign(rtn_order.InstrumentID, select_stmt.getColumn(4).getText()); //
        assign(rtn_order.InstrumentType, select_stmt.getColumn(5).getText());
        assign(rtn_order.Symbol, select_stmt.getColumn(6).getText());
        assign(rtn_order.Price, select_stmt.getColumn(7).getDouble());
        assign(rtn_order.LeverRate, select_stmt.getColumn(8).getDouble());
        assign(rtn_order.Volume, select_stmt.getColumn(9).getDouble());
        assign(rtn_order.Direction, getDirection(select_stmt.getColumn(10).getText()));
        assign(rtn_order.OffsetFlag, getOffsetFlag(select_stmt.getColumn(11).getText()));
        assign(rtn_order.OrderLocalID, select_stmt.getColumn(12).getText());
        assign(rtn_order.OrderMaker, getOrderMaker(select_stmt.getColumn(13).getText()));
        assign(rtn_order.OrderType, getOrderType(select_stmt.getColumn(14).getText()));
        assign(rtn_order.LandTime, select_stmt.getColumn(15).getInt64());
        assign(rtn_order.SendTime, select_stmt.getColumn(16).getInt64());
        assign(rtn_order.StrategyOrderID, select_stmt.getColumn(17).getText());

        assign(rtn_order.OrderMode, getOrderMode(select_stmt.getColumn(18).getText()));
        assign(rtn_order.AssetType, getAssetType(select_stmt.getColumn(19).getText()));
        assign(rtn_order.TradeChannel, select_stmt.getColumn(20).getText());
        assign(rtn_order.OrderXO, getOrderXO(select_stmt.getColumn(21).getText()));
        assign(rtn_order.PlatformTime, select_stmt.getColumn(22).getInt64());
        assign(rtn_order.OrderSysID, select_stmt.getColumn(23).getText());
        assign(rtn_order.OrderForeID, select_stmt.getColumn(24).getText());
        assign(rtn_order.CreateTime, select_stmt.getColumn(25).getInt64());
        assign(rtn_order.ModifyTime, select_stmt.getColumn(26).getInt64());     
        assign(rtn_order.RspLocalTime, select_stmt.getColumn(27).getInt64());                        


        assign(rtn_order.Cost, select_stmt.getColumn(28).getDouble());
        assign(rtn_order.TradePrice, select_stmt.getColumn(29).getDouble());
        assign(rtn_order.TradeVolume, select_stmt.getColumn(30).getDouble());
        assign(rtn_order.RemainVolume, select_stmt.getColumn(31).getDouble());
        assign(rtn_order.TradeValue, select_stmt.getColumn(32).getDouble());
        assign(rtn_order.OrderStatus, getOrderStatus(select_stmt.getColumn(33).getText()));
        assign(rtn_order.SessionID, select_stmt.getColumn(34).getText());
        assign(rtn_order.RequestID, select_stmt.getColumn(35).getInt64());
        assign(rtn_order.RequestForeID, select_stmt.getColumn(36).getInt64());
        assign(rtn_order.Fee, select_stmt.getColumn(37).getDouble());
        assign(rtn_order.FeeCurrency, select_stmt.getColumn(38).getText());

        B_LOG_INFO("----------- Book.RtnOrder \n" + convertUTData(&rtn_order, UT_FID_RtnOrder));          
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
    
}

bool BookKeeper::get_order_field(string order_local_id, string order_sys_id, CUTRtnOrderField& dst)
{
    try
    {
        bool result = true;
        SQLite::Statement& select_stmt = stmtSelectOrderByOrderLocalID;

        if (order_local_id != "")
        {
            stmtSelectOrderByOrderLocalID.reset();
            SQLite::bind(stmtSelectOrderByOrderLocalID, order_local_id);

            if (stmtSelectOrderByOrderLocalID.executeStep()) 
            {
                get_order_filed_from_stmt(stmtSelectOrderByOrderLocalID, dst);
            }
            else
            {
                result = false;
                B_LOG_ERROR("Get Order From DB By LocalID: " + order_local_id +  " Failed!");
            }
        }
        else if (order_sys_id != "")
        {
            
            stmtSelectOrderByOrderSysID.reset();

            SQLite::bind(stmtSelectOrderByOrderSysID,order_sys_id);

            if (stmtSelectOrderByOrderSysID.executeStep()) 
            {
                get_order_filed_from_stmt(stmtSelectOrderByOrderSysID, dst);
            }
            else
            {
                result = false;
                B_LOG_ERROR("Get Order From DB By SysID: " + order_local_id +  " Failed!");                
            }
        }
        else 
        {
            result = false;
            B_LOG_ERROR("Get Order From DB Failed! LocalID, SysID are all Empty!");     
        }
        
        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

int BookKeeper::delete_order(string order_local_id, string order_sys_id)
{
    try
    {
        int result;

        if (order_local_id != "")
        {
            stmtDeleteOrderByLocalID.reset();
            SQLite::bind(stmtDeleteOrderByLocalID, order_local_id);
            result = stmtDeleteOrderByLocalID.exec();
        }
        else if (order_sys_id != "")
        {
            stmtDeleteOrderBySysID.reset();
            SQLite::bind(stmtDeleteOrderBySysID, order_local_id);
            result = stmtDeleteOrderBySysID.exec();
        }
        else
        {
            B_LOG_ERROR("Delete Order DB Failed! LocalID, SysID are all Empty!");  
        }

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

int BookKeeper::insert_order(constCUTReqCreateOrderField& pCreateOrder)
{
    try
    {
        int result;

        stmtInsertOrder.reset();
        SQLite::bind(stmtInsertOrder,
            pCreateOrder.UserName,
            pCreateOrder.AccountName,
            pCreateOrder.AccountType,
            pCreateOrder.ExchangeID,
            pCreateOrder.InstrumentID,
            pCreateOrder.InstrumentType,
            pCreateOrder.Symbol,
            pCreateOrder.Price,
            pCreateOrder.LeverRate,
            pCreateOrder.Volume,
            getDirectionString(pCreateOrder.Direction),
            getOffsetFlagString(pCreateOrder.OffsetFlag),
            pCreateOrder.OrderLocalID,
            getOrderMakerString(pCreateOrder.OrderMaker),
            getOrderTypeString(pCreateOrder.OrderType),
            pCreateOrder.LandTime,
            pCreateOrder.SendTime,
            pCreateOrder.StrategyOrderID,
            getOrderModeString(pCreateOrder.OrderMode),
            getAssetTypeString(pCreateOrder.AssetType),
            pCreateOrder.TradeChannel,
            getOrderXOString(pCreateOrder.OrderXO),
            pCreateOrder.PlatformTime,
            pCreateOrder.OrderSysID,
            pCreateOrder.OrderForeID,
            pCreateOrder.CreateTime,
            pCreateOrder.ModifyTime,
            pCreateOrder.RspLocalTime,
            pCreateOrder.Cost,
            pCreateOrder.TradePrice,
            pCreateOrder.TradeVolume,
            pCreateOrder.RemainVolume,
            pCreateOrder.TradeValue,
            getOrderStatusString(pCreateOrder.OrderStatus),
            pCreateOrder.SessionID,
            pCreateOrder.RequestID,
            pCreateOrder.RequestForeID,
            pCreateOrder.Fee,
            pCreateOrder.FeeCurrency
        );
        result = stmtInsertOrder.exec();

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

int BookKeeper::update_order(CUTRtnOrderField& rtn_order)
{
    try
    {
        int result;

        if (strcmp(rtn_order.OrderLocalID, "") != 0)
        {
            stmtUpdateOrderByLocalID.reset();
            SQLite::bind(stmtUpdateOrderByLocalID,
                rtn_order.UserName,
                rtn_order.AccountName,
                rtn_order.AccountType,
                rtn_order.ExchangeID,
                rtn_order.InstrumentID,
                rtn_order.InstrumentType,
                rtn_order.Symbol,
                rtn_order.Price,
                rtn_order.LeverRate,
                rtn_order.Volume,
                getDirectionString(rtn_order.Direction),
                getOffsetFlagString(rtn_order.OffsetFlag),
                rtn_order.OrderLocalID,
                getOrderMakerString(rtn_order.OrderMaker),
                getOrderTypeString(rtn_order.OrderType),
                rtn_order.LandTime,
                rtn_order.SendTime,
                rtn_order.StrategyOrderID,
                getOrderModeString(rtn_order.OrderMode),
                getAssetTypeString(rtn_order.AssetType),
                rtn_order.TradeChannel,
                getOrderXOString(rtn_order.OrderXO),
                rtn_order.PlatformTime,
                rtn_order.OrderSysID,
                rtn_order.OrderForeID,
                rtn_order.CreateTime,
                rtn_order.ModifyTime,
                rtn_order.RspLocalTime,
                rtn_order.Cost,
                rtn_order.TradePrice,
                rtn_order.TradeVolume,
                rtn_order.RemainVolume,
                rtn_order.TradeValue,
                getOrderStatusString(rtn_order.OrderStatus),
                rtn_order.SessionID,
                rtn_order.RequestID,
                rtn_order.RequestForeID,
                rtn_order.Fee,
                rtn_order.FeeCurrency,

                rtn_order.OrderLocalID
            );
            result = stmtUpdateOrderByLocalID.exec();            
        }
        else if (strcmp(rtn_order.OrderSysID, "") != 0)
        {
            stmtUpdateOrderBySysID.reset();
            SQLite::bind(stmtUpdateOrderBySysID,
                rtn_order.UserName,
                rtn_order.AccountName,
                rtn_order.AccountType,
                rtn_order.ExchangeID,
                rtn_order.InstrumentID,
                rtn_order.InstrumentType,
                rtn_order.Symbol,
                rtn_order.Price,
                rtn_order.LeverRate,
                rtn_order.Volume,
                getDirectionString(rtn_order.Direction),
                getOffsetFlagString(rtn_order.OffsetFlag),
                rtn_order.OrderLocalID,
                getOrderMakerString(rtn_order.OrderMaker),
                getOrderTypeString(rtn_order.OrderType),
                rtn_order.LandTime,
                rtn_order.SendTime,
                rtn_order.StrategyOrderID,
                getOrderModeString(rtn_order.OrderMode),
                getAssetTypeString(rtn_order.AssetType),
                rtn_order.TradeChannel,
                getOrderXOString(rtn_order.OrderXO),
                rtn_order.PlatformTime,
                rtn_order.OrderSysID,
                rtn_order.OrderForeID,
                rtn_order.CreateTime,
                rtn_order.ModifyTime,
                rtn_order.RspLocalTime,
                rtn_order.Cost,
                rtn_order.TradePrice,
                rtn_order.TradeVolume,
                rtn_order.RemainVolume,
                rtn_order.TradeValue,
                getOrderStatusString(rtn_order.OrderStatus),
                rtn_order.SessionID,
                rtn_order.RequestID,
                rtn_order.RequestForeID,
                rtn_order.Fee,
                rtn_order.FeeCurrency,

                rtn_order.OrderSysID
            );
            result = stmtUpdateOrderBySysID.exec();
        }
        else
        {
            B_LOG_ERROR("Update Order DB Failed! LocalID, SysID are all Empty!");  
        }

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

int BookKeeper::delete_cancel_req(string order_local_id)
{
    try
    {
        int result;

        if (order_local_id != "")
        {
            stmtDeleteCancelReqByLocalID.reset();
            SQLite::bind(stmtDeleteCancelReqByLocalID, order_local_id);
            result = stmtDeleteCancelReqByLocalID.exec();
        }
        else
        {
            B_LOG_ERROR("Delete Order DB Failed! LocalID is Empty!");  
        }

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }    
}

int BookKeeper::insert_cancel_req(CUTReqCancelOrderField& req_cancel_order)
{
    try
    {
        int result;
        stmtInsertCancelReq.reset();
        SQLite::bind(stmtInsertCancelReq,
            req_cancel_order.UserName,
            req_cancel_order.AccountName,
            getAccountTypeString(req_cancel_order.AccountType),
            req_cancel_order.Symbol,
            getAssetTypeString(req_cancel_order.AssetType),
            req_cancel_order.OrderLocalID,
            req_cancel_order.OrderForeID,
            req_cancel_order.OrderSysID,
            req_cancel_order.ExchangeID,
            req_cancel_order.StrategyOrderID,
            req_cancel_order.TradeChannel,
            req_cancel_order.SendTime,
            req_cancel_order.SessionID
        );
        result = stmtInsertCancelReq.exec();
        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}

int BookKeeper::delete_trade(string order_local_id)
{
    try
    {
        int result;

        if (order_local_id != "")
        {
            stmtDeleteTradeByLocalID.reset();
            SQLite::bind(stmtDeleteTradeByLocalID, order_local_id);
            result = stmtDeleteTradeByLocalID.exec();
        }
        else
        {
            B_LOG_ERROR("Delete Order DB Failed! LocalID is Empty!");  
        }

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

int BookKeeper::insert_trade(CUTRtnTradeField& rtn_trade)
{
    try
    {
        int result;

        stmtInsertTrade.reset();

        SQLite::bind(stmtInsertTrade,
            rtn_trade.UserName,
            rtn_trade.AccountName,
            rtn_trade.InternalAccountName,
            getAccountTypeString(rtn_trade.AccountType),
            rtn_trade.TradeSubID,
            rtn_trade.TradeID,
            rtn_trade.OrderSysID,
            rtn_trade.ExchangeID,
            rtn_trade.InstrumentID,
            rtn_trade.MatchPrice,
            rtn_trade.MatchVolume,
            rtn_trade.MatchValue,
            getDirectionString(rtn_trade.Direction),
            rtn_trade.OrderLocalID,
            rtn_trade.Fee,
            rtn_trade.FeeCurrency,
            rtn_trade.PlatformTime,
            rtn_trade.TradeTime,
            rtn_trade.RspLocalTime,
            rtn_trade.Price,
            rtn_trade.StrategyOrderID,
            getOrderMakerString(rtn_trade.OrderMaker),
            getAssetTypeString(rtn_trade.AssetType),
            rtn_trade.TradeChannel,
            rtn_trade.SessionID
        );

        stmtInsertTrade.exec();

        return result;
    }
    catch(const std::exception& e)
    {
        B_LOG_ERROR(e.what());
    }
}