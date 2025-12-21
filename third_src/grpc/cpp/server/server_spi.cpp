#include "server_spi.h"

void ServerSpi::start_server(ServerSpi* server_spi)
{
    try
    {
        cout << "start_server: " << server_spi->get_class_name() 
             << " server_spi: " << server_spi
             << " vp: " << *(int *)server_spi 
             << endl;

        if (server_)
        {
            server_->register_spi(server_spi);

            server_->start();
        }
        else{
            cout << "server was not inited! " << endl;
        }

        // server_thread_ = boost::make_shared<std::thread>(&ServerSpi::server_main, this, server_spi);

        // std::this_thread::sleep_for(std::chrono::seconds(100));

        // cout << "start_server end: " << server_spi->get_class_name() 
        //              << " server_spi: " << server_spi
        //      << " vp: " << *(int *)server_spi 
        // << endl;        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void ServerSpi::server_main(ServerSpi* server_spi)
{
    try
    {
        cout << "server_main: " << server_spi->get_class_name() 
                     << " server_spi: " << server_spi
             << " vp: " << *(int *)server_spi 
        << endl;


        if (server_)
        {
            server_->register_spi(server_spi);

            server_->start();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void ServerSpi::rsp_server_apple(PackagePtr pkg)
{
    try
    {
        if (server_)
        {
            server_->response(pkg);
        }
        else
        {
            cout << "[W] Server is Null " << endl;
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
        else
        {
            cout << "[W] Server is Null " << endl;
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}


