#include "test_spi.h"

void start_server()
{
    TestServerSpi server;
    
    server.start();    
}

int main()
{
    start_server();

    return 1;
}