#pragma once
#include "../include/global_declare.h"

#include "client.h"

class ClientApi
{

public:

    void start();

    void init_async_client();

    void init_sync_client();

    virtual ~ClientApi();

private:
    AsyncClientPtr                       async_client_;
    SyncClientPtr                        sync_client_;

    bool                                 is_async_{true};
};