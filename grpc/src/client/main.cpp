#include "client.h"
#include "../include/global_declare.h"
#include <grpcpp/grpcpp.h>

void test_simple()
{
    TestSimpleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    client.start();
}

int main()
{
    cout << "Client " << endl;

    test_simple();

    return 1;
}