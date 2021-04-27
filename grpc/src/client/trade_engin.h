#pragma once

#include "../include/global_declare.h"

#include "client.h"

class TradeEngine
{
public:

    void start();

    void init_client();

    void test_thread_fun();

    virtual ~TradeEngine();

private:
    AsyncClientPtr                       async_client_;
    boost::shared_ptr<std::thread>       test_thread_{nullptr};
};