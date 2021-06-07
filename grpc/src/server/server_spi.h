#pragma once

#include "server.h"
#include "../include/global_declare.h"

FORWARD_DECLARE_PTR(ServerSpi);
class ServerSpi:public boost::enable_shared_from_this<ServerSpi>
{

public:

    ServerSpi()
    {
        server_ = boost::make_shared<BaseServer>("0.0.0.0:50051");
    }

    ServerSpiPtr get_shared_ptr()
    {
        return shared_from_this();
    }

    virtual string get_class_name() { return "ServerSpi";}

    virtual void rsp_server_apple(PackagePtr pkg);

    virtual void rsp_double_apple(PackagePtr pkg);

    virtual void on_req_server_apple(PackagePtr pkg) 
    
    {
        cout << "on_req_server_apple " << endl;
    }

    virtual void on_req_double_apple(PackagePtr pkg)
    
    {
        cout << "on_req_double_apple " << endl;
    }

protected:

    BaseServerPtr                       server_{nullptr};
};