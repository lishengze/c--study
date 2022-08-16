#pragma once

#include "../include/global_declare.h"

#include "server.h"

#include "server_spi.h"

FORWARD_DECLARE_PTR(TradeEngine);

class TradeEngine:public ServerSpi
{
public:
    TradeEngine() { }

    virtual ~TradeEngine() {        
        cout << "\n~TradeEngine" << endl;
    }

    void start();

    void thread_main();

    virtual string get_class_name() { return "TradeEngine";}

    virtual void on_req_server_apple(PackagePtr pkg) override;

    virtual void on_req_double_apple(PackagePtr pkg) override;

    // TradeEnginePtr get_shared_ptr()
    // {
    //     Server
    //     return shared_from_this();
    // }


private:
    boost::shared_ptr<std::thread>      thread_{nullptr};
};
