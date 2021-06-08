#include "client_api.h"

#include "data_struct.h"

#include "time_util.h"

#include "config.h"

ClientApi::~ClientApi()
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

void ClientApi::start()
{
    try
    {
        if (is_async_)
        {
            init_async_client();
        }
        else
        {
            init_sync_client();
        }

        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void ClientApi::init_async_client()
{
    try
    {
        async_client_ = boost::make_shared<AsyncClient>(grpc::CreateChannel("localhost:50051", 
                                                                            grpc::InsecureChannelCredentials()),
                                                        CONFIG->get_session_id());

        async_client_->start();

        // test_thread_ = boost::make_shared<std::thread>(&ClientApi::test_thread_fun, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void ClientApi::init_sync_client()
{
    try
    {
        sync_client_ = boost::make_shared<SyncClient>(grpc::CreateChannel("localhost:50051", 
                                                                            grpc::InsecureChannelCredentials()),
                                                        CONFIG->get_session_id());

        sync_client_->start();      
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}