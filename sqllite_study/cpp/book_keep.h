///@brief class for contain sorting center internal state
///@history
///20200409  赵宇时   创建该文件

#pragma once

#include "global_declare.h"
#include "quark/cxx/ut/UtData.h"
#include "quark/cxx/ut/UtPackageDesc.h"
#include "quark/cxx/assign.h"
#include "pandora/util/time_util.h"
#include "pandora/package/package.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include "pandora/util/thread_safe_singleton.hpp"
#define BOOK_KEEPER utrade::pandora::ThreadSafeSingleton<BookKeeper>::DoubleCheckInstance()


#define DATETIME_FORMAT "%Y%m%d-%H:%M:%S"

//wang.hy 20200601 Order表index索引值
const int ORDER_INDEX_OrderLocalID = 0;
const int ORDER_INDEX_UserName = 1;
const int ORDER_INDEX_SessionID = 2;
const int ORDER_INDEX_RequestID = 3;
const int ORDER_INDEX_ExchangeID = 4;
const int ORDER_INDEX_InstrumentID = 5;
const int ORDER_INDEX_Price = 6;
const int ORDER_INDEX_Volume = 7;
const int ORDER_INDEX_Direction = 8;
const int ORDER_INDEX_OrderMaker = 9;
const int ORDER_INDEX_OrderType = 10;
const int ORDER_INDEX_OrderSysID = 11;
const int ORDER_INDEX_OrderStatus = 12;
const int ORDER_INDEX_TradePrice = 13;
const int ORDER_INDEX_TradeVolume = 14;
const int ORDER_INDEX_ExpectVolume = 15;
const int ORDER_INDEX_Fee = 16;
const int ORDER_INDEX_FeeCurrency = 17;
const int ORDER_INDEX_CreateTime = 18;
const int ORDER_INDEX_ModifyTime = 19;
const int ORDER_INDEX_LastCheckpoint = 20;
const int ORDER_INDEX_AccountName = 21;
const int ORDER_INDEX_OrderForeID = 22;


//wang.hy Request Table Index
const int REQUEST_INDEX_RequestID = 0;
const int REQUEST_INDEX_OrderLocalID = 1;
const int REQUEST_INDEX_OrderSysID = 2;
const int REQUEST_INDEX_UserName = 3;
const int REQUEST_INDEX_SessionID = 4;


class BookKeeper 
{
public:
    BookKeeper(const std::string& db_name);

    ~BookKeeper()
    {

    }

    void InsertOrderOnReqCreateOrder(PackagePtr package);
    void InsertTradeOnRtnTrade(PackagePtr package);
    void InsertRequestOnReqCancelOrder(PackagePtr package);

    bool FetchOrderOnRspCreateOrder(PackagePtr package);
    bool IsNewTradeOnRtnTrade(PackagePtr package);
    bool FetchRequestOnRspCancelOrder(PackagePtr package);

    void UpdateOrderOnRspCreateOrder(PackagePtr package);
    void UpdateOrderOnRtnOrder(PackagePtr package);

    bool CompleteRtnOrderPackage(PackagePtr package);

    bool CompleteRtnTradePackage(PackagePtr package);
    
    // void DeleteRequestOnRspCancelOrder(PackagePtr package);

    void GetCheckpoint(CTimeType& res) {
        assign(res, pandora::ToSecondStr(checkpoint_ * NANOSECONDS_PER_SECOND, DATETIME_FORMAT));
    }

    long GetCheckpointLong() {
        return checkpoint_;
    }
    
    PackagePtr MakeRspCreateOrderPackageByOrderLocalID(const std::string& orderLocalID);
    PackagePtr MakeRtnOrderPackageByOrderLocalID(const std::string& orderLocalID);

    PackagePtr MakeRtnOrderPackageByOrderSysID(const std::string& orderSysID);

    PackagePtr MakeRtnOrderPackageOnRtnTrade(PackagePtr package);

    std::vector<PackagePtr> PresumeDeath();

    bool check_order_by_local_id(const std::string& orderLocalID);

    bool check_order_by_sys_id(const std::string& order_sys_id);

    void CommitCheckpoint(const char* timeStr);

    bool get_order_status(string order_local_id, COrderStatusType& status);

    // add db interface;
    bool get_order_field(string order_local_id, string order_sys_id, CUTRtnOrderField& dst);

    void get_order_filed_from_stmt(SQLite::Statement& select_stmt, CUTRtnOrderField& dst);

    int delete_order(string order_local_id="", string order_sys_id="");

    int insert_order(CUTReqCreateOrderField& order_info);

    int update_order(CUTRtnOrderField& rtn_order);    

    int delete_cancel_req(string order_local_id="");

    int insert_cancel_req(CUTReqCancelOrderField& info);

    int delete_trade(string order_local_id="");

    int insert_trade(CUTRtnTradeField& info);
    
private:
    void init();

    void loadCheckpoint();

    PackagePtr makeRtnOrderPackageByOrderSysIDAtom(SQLite::Statement& stmt);

    bool is_order_over(SQLite::Statement& stmt);
    bool isOutdated(const char* timeStr) 
    {
        return (pandora::ToNano(timeStr, DATETIME_FORMAT) / NANOSECONDS_PER_SECOND) < checkpoint_;
    }
    bool isOutdated(long time) {
        return time < checkpoint_;
    }

private:
    class Table {
    public:
        Table(SQLite::Database& db);
        ~Table()
        {

        }
    private:
        void create_table_order(SQLite::Database& db);
        void create_table_trade(SQLite::Database& db);
        void create_table_cancel_req(SQLite::Database& db);
    };

private:
    SQLite::Database        db_;
    Table                   table_;
    int64_t                 checkpoint_{0};

    SQLite::Statement stmtInsertOrder;
    SQLite::Statement stmtInsertTrade;
    SQLite::Statement stmtInsertCancelReq;

    SQLite::Statement stmtSelectOrderByOrderLocalID;
    SQLite::Statement stmtSelectOrderByOrderSysID;
    SQLite::Statement stmtSelectTradeByTradeID;

    SQLite::Statement stmtSelectCancelReqByLocalID;
    SQLite::Statement stmtSelectCancelReqBySysID;

    SQLite::Statement stmtUpdateOrderByLocalID;
    SQLite::Statement stmtUpdateOrderBySysID;

    SQLite::Statement stmtDeleteOrderBySysID;
    SQLite::Statement stmtDeleteOrderByLocalID;
    SQLite::Statement stmtDeleteTradeByLocalID;
    SQLite::Statement stmtDeleteCancelReqByLocalID;

    SQLite::Statement stmtSelectDead;

    SQLite::Statement stmtCleanTrade;
    SQLite::Statement stmtCleanOrder;
};
