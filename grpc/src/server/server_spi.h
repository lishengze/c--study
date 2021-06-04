#pragma once

#include "server.h"

class ServerSpi
{

public:

    ServerSpi()
    {
        server_ = boost::make_shared<BaseServer>("0.0.0.0:50051");
    }

    virtual void rsp_server_apple(PackagePtr pkg);

    virtual void rsp_double_apple(PackagePtr pkg);

    virtual void on_req_server_apple(PackagePtr pkg) = 0;

    virtual void on_req_double_apple(PackagePtr pkg) = 0;

protected:

    BaseServerPtr                       server_{nullptr};
};