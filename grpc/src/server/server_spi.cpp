#include "server_spi.h"

void ServerSpi::rsp_server_apple(PackagePtr pkg)
{
    try
    {
        if (server_)
        {
            server_->response(pkg);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void ServerSpi::rsp_double_apple(PackagePtr pkg)
{
    try
    {
        if (server_)
        {
            server_->response(pkg);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}


