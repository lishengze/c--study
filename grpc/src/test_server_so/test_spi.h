#pragma once

#include "../server/server_spi.h"
#include "../include/global_declare.h"

class TestServerSpi:public ServerSpi
{
    public:

    void start();

    void thread_main();

    virtual string get_class_name() { return "TestServerSpi";}

    virtual void on_req_server_apple(PackagePtr pkg) override;

    virtual void on_req_double_apple(PackagePtr pkg) override;

private:
    boost::shared_ptr<std::thread>      thread_{nullptr};


};