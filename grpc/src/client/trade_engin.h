#pragma once

#include "../include/global_declare.h"

#include "client.h"

class TradeEngine
{
public:

    void start();

    void init_async_client();

    void init_sync_client();

    void test_thread_fun();

    virtual ~TradeEngine();

private:
    AsyncClientPtr                       async_client_;
    SyncClientPtr                        sync_client_;
    boost::shared_ptr<std::thread>       test_thread_{nullptr};

    bool                                 is_async_{false};
};