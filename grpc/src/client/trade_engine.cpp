#include "trade_engin.h"

#include "data_struct.h"

#include "time_util.h"

#include "config.h"

TradeEngine::~TradeEngine()
{
    try
    {
        if (test_thread_ && test_thread_->joinable())
        {
            test_thread_->join();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void TradeEngine::start()
{
    try
    {
        init_client();

        test_thread_ = boost::make_shared<std::thread>(&TradeEngine::test_thread_fun, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void TradeEngine::init_client()
{
    try
    {
        async_client_ = boost::make_shared<AsyncClient>(grpc::CreateChannel("localhost:50051", 
                                                                            grpc::InsecureChannelCredentials()),
                                                        CONFIG->get_session_id());

        async_client_->start();

        /* code */
    }
    catch(const std::exception& e)
    {       
        std::cerr << e.what() << '\n';
    }
    
}

void TradeEngine::test_thread_fun()
{
    try
    {
    //    cout << "TradeEngine::test_thread_fun " << endl;

       
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        std::vector<Apple*> request_data;
    
       for (int i = 0; i < CONFIG->get_test_count(); ++i)
       {
            PackagePtr pkg = CreatePackage<Apple>("trade_engine", std::to_string(NanoTime()));

            pkg->SetRequestID(i+1);
            pkg->SetSessionID(CONFIG->get_session_id());
            pkg->SetRpcID("ServerStreamApple");

            async_client_->add_data(pkg);     

            pkg->SetRpcID("DoubleStreamApple");

            async_client_->add_data(pkg);
       }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}