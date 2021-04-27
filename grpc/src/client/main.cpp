#include "client.h"
#include "trade_engin.h"
#include "../include/global_declare.h"
#include <grpcpp/grpcpp.h>

void test_simple()
{
    // TestSimpleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    ServerStreamClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    client.start();
}

void test_client_double_stream_async()
{
    AsyncClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    client.start();
}

void test_trade_engine()
{
    TradeEngine trade;
    trade.start();
}

int main()
{
    cout << "****** Client ******\n" << endl;

    // test_simple();

    // test_client_double_stream_async();

    test_trade_engine();

    return 1;
}