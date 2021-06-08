#include "test_spi.h"
#include "../server/config.h"

void TestServerSpi::start()
{
    CONFIG;

    start_server(this);
    
    // thread_ = boost::make_shared<std::thread>(&TestServerSpi::thread_main, this);
}

void TestServerSpi::thread_main()
{
    int test_count = 10;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    while (test_count--)
    {
        PackagePtr pkg = CreatePackage<Apple>("trade_engine", std::to_string(NanoTime()));

        pkg->SetRequestID(test_count);
        pkg->SetSessionID(CONFIG->get_session_id());

        // pkg->SetRpcID("ServerStreamApple");
        // async_client_->add_data(pkg);

        pkg->SetRpcID("DoubleStreamApple");

        // server_->response(pkg);

        rsp_double_apple(pkg);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void TestServerSpi::on_req_server_apple(PackagePtr pkg)
{
    try
    {
        if (pkg)
        {
            ApplePtr apple_data = GetField<Apple>(pkg);

            if (apple_data)
            {
                cout << "TestServerSpi::on_req_server_apple "<< "name: " << apple_data->name << " "
                     << "time: " << apple_data->time 
                     << endl;                                   
            }
            else
            {
                cout << "[W] TestServerSpi::on_req_server_apple apple_data is null" << endl;
            }
        }
        else
        {
            cout << "[W] TestServerSpi::on_req_server_apple  pkg is null" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void TestServerSpi::on_req_double_apple(PackagePtr pkg)
{
    try
    {
        if (pkg)
        {
            ApplePtr apple_data = GetField<Apple>(pkg);

            if (apple_data)
            {
                cout << "TestServerSpi::on_req_double_apple " << "name: " << apple_data->name << " "
                        << "time: " << apple_data->time 
                        << endl;                                   
            }
            else
            {
                cout << "[W] TestServerSpi::on_req_double_apple apple_data is null" << endl;
            }                
        }
        else
        {
            cout << "[W] TestServerSpi::on_req_double_apple  pkg is null" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}