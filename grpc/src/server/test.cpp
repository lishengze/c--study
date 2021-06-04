#include "test.h"
#include "server.h"
#include "test/test_template_add.h"
#include "config.h"

#include "trade_engine.h"

void test_simple()
{
    CONFIG;

    BaseServer simple_rpc("0.0.0.0:50051");

    simple_rpc.start();
}

void test_syanc_server()
{
    SyncServer sync_server("0.0.0.0:50051");

    sync_server.start();
}

void test_trade_engine()
{
    TradeEngine trade;
    trade.start();
}

void TestMain()
{
    // TestTemplateAdd();

    // test_simple();

    // test_syanc_server();
    
    test_trade_engine();
}