#include "trade_engin.h"

#include "data_struct.h"

#include "time_util.h"

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
        async_client_ = boost::make_shared<AsyncClient>(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

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
       Apple apple;

       apple.session_id = "lsz";
       apple.name = "trade_engine";
       apple.rpc_id = "apple";
       
    
       while(true)
       {
           apple.time = NanoTimeStr();
           apple.request_id = NanoTimeStr();

           std::this_thread::sleep_for(std::chrono::milliseconds(1500));

           async_client_->add_data(&apple);           
       }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}