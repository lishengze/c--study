#pragma once

#include "../include/global_declare.h"

#include "server.h"

#include "server_spi.h"

class TradeEngine:public ServerSpi
{
public:
    TradeEngine() {        
    }

    void start();

    void thread_main();

    virtual void on_req_server_apple(PackagePtr pkg);

    virtual void on_req_double_apple(PackagePtr pkg);    

private:
    boost::shared_ptr<std::thread>      thread_{nullptr};    
};
