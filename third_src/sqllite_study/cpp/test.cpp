#include "test.h"
#include "book_keep.h"
#include "quark/cxx/ut/UtData.h"
#include "Log/log.h"

void set_order(CUTReqCreateOrderField& dst)
{

}

void set_order(CUTRtnOrderField& dst)
{

}

void set_cancel(CUTReqCancelOrderField& dst)
{

}

void set_trade(CUTRtnTradeField& dst)
{
    

}



void init_db(BookKeeper& bk)
{
    CUTReqCreateOrderField order_filed;
    set_order(order_filed);
    bk.insert_order(order_filed);

    CUTReqCancelOrderField cancel_field;
    set_cancel(cancel_field);
    bk.insert_cancel_req(cancel_field);

    CUTRtnTradeField trade_field;
    set_trade(trade_field);
    bk.insert_trade(trade_field);
}

void test_order(BookKeeper& bk)
{
    CUTReqCreateOrderField order_filed;
    set_order(order_filed);
    bk.insert_order(order_filed);


    CUTRtnOrderField new_order_filed;
    set_order(new_order_filed)
    bk.update_order(new_order_filed);

    bk.delete_order(new_order_filed.OrderLocalID, new_order_filed.OrderSysID);
}


void TestBookKeep()
{
    string db_name = "test_book";
    BookKeeper bk(db_name);
    init_db(bk);

}

void TestMain()
{
    TestBookKeep();
}