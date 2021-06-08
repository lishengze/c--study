#pragma once

#include "../include/global_declare.h"

#include "client_api.h"


class TradeEngine:public ClientApi
{
public:
    void test_thread_fun();

    virtual ~TradeEngine();

private:
    boost::shared_ptr<std::thread>       test_thread_{nullptr};
};