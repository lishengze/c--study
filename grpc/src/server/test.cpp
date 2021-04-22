#include "test.h"
#include "server.h"

void test_simple()
{
    BaseServer simple_rpc("0.0.0.0:50051");

    simple_rpc.start();
}

void TestMain()
{
    test_simple();
}