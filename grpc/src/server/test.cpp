#include "test.h"
#include "server.h"
#include "test/test_template_add.h"

void test_simple()
{
    BaseServer simple_rpc("0.0.0.0:50051");

    simple_rpc.start();
}

void TestMain()
{
    // TestTemplateAdd();

    test_simple();
}