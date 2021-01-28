#include "server.h"
#include "../include/global_declare.h"


void test_simple()
{
    BaseServer simple_rpc("0.0.0.0:50051");

    simple_rpc.start();
}

int main()
{
    cout << "Server " << endl;

    test_simple();

    return 1;
}