#include "trade_engine.h"
#include "config.h"

void TradeEngine::start()
{
    CONFIG;

    if (server_)
    {
        server_->start();
    }

    thread_ = boost::make_shared<std::thread>(&TradeEngine::thread_main, this);
}

void TradeEngine::thread_main()
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

void TradeEngine::on_req_server_apple(PackagePtr pkg)
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void TradeEngine::on_req_double_apple(PackagePtr pkg)
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}