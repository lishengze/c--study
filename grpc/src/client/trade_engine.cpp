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

       
       std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        std::vector<Apple*> request_data;
    
       for (int i = 0; i < CONFIG->get_test_count(); ++i)
       {
            Apple* apple = new Apple();

            apple->session_id = "lsz";
            apple->name = "trade_engine";
            apple->rpc_id = "apple";

            apple->time = NanoTimeStr();
            apple->request_id = i+1;

        //    std::this_thread::sleep_for(std::chrono::milliseconds(1));

        //    std::this_thread::sleep_for(std::chrono::microseconds(1));

            request_data.push_back(apple);
            async_client_->add_data(apple);           
       }

    //    for (auto apple:request_data)
    //    {

    //    }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}