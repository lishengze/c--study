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

    // printf("虚表地址:%p\n", *(int *)&trade);  
    // printf("第一个虚函数地址:%p\n", *(int *)*(int *)&trade);  
    // printf("第二个虚函数地址:%p\n", *((int *)*(int *)(&trade) + 1));  

    // int vaddr = *(int *)&trade;

    // int faddr = *(int *)*(int *)(&trade);//f()地址
 
	// int f1addr = *((int *)(*(int *)(&trade))+ 1); // 或者以4个字节作为整体 偏移1到f1()
 
	// int f2addr = *((int *)(*(int *)(&trade)) + 2);  //f2()地址


    // ((void(*)(void))faddr)();  

    trade.start();
}

void TestMain()
{
    // TestTemplateAdd();

    // test_simple();

    // test_syanc_server();
    
    test_trade_engine();
}