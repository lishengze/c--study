#include "client.h"
#include "global_declare.h"

#include "../include/global_declare.h"
#include "../include/time_util.h"

#include "client_rpc.h"

BaseClient::~BaseClient()
{
    if (thread_ && thread_->joinable())
    {
        thread_->join();
    }
}

void BaseClient::start()
{
    cout << "TestSimpleClient::start " << endl;

    // thread_ = std::make_shared<std::thread>(&TestSimpleClient::thread_run, this);

    thread_run();
}

void BaseClient::thread_run()
{
    cout << "TestSimpleClient::thread_run " << endl;

    int test_numb = 3;

    while(test_numb--)
    {
        request();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    
    // while (true)
    // {
    //     request();

    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    // }
}

void TestSimpleClient::request()
{
    try
    {
        cout << "\nTestSimpleClient::request " << endl;

        grpc::Status status;

        string name = "TestSimpleClient";
        string time = NanoTimeStr();

        ClientContext   context;

        request_.set_name(name);

        request_.set_time(time);

        cout << "From Request: " << name << " " << time << endl;

        if (!is_ansyc_)
        {
            status = stub_->TestSimple(&context, request_, &reply_);
        }
        else
        {
            // rpc_ = stub_->PrepareAsyncTestSimple(&context, request_, &cq_);
            // cout <<"rpc_ = stub_->PrepareAsyncTestSimple(&context, request_, &cq_);" << endl;
            
            // rpc_->StartCall();
            // cout <<"rpc_->StartCall();" << endl;
            
            // rpc_->Finish(&reply_, &status, (void*)1);
            // cout <<"rpc_->Finish(&reply_, &status, (void*)(&data));" << endl;

            

            rpc_ = stub_->AsyncTestSimple(&context, request_, cq_);
            cout <<"rpc_ = stub_->AsyncTestSimple(&context, request_, &cq_);" << endl;

            // rpc_->StartCall();
            // cout <<"rpc_->StartCall();" << endl;

            int data;
            rpc_->Finish(&reply_, &status, (void*)(&data));
            cout <<"rpc_->Finish(&reply_, &status, (void*)(&data));" << endl;

            void* got_tag;
            bool ok = false;

            if (!cq_->Next(&got_tag, &ok))
            {
                cout << "cq_.Next(&got_tag, &ok) Failed!" << endl;
                return;
            }
            cout << "cq_.Next(&got_tag, &ok) " << endl;

            if (got_tag != (void*)1)
            {

            }

            if (!ok)
            {
                cout << "!ok " << endl;
            }
        }

        if (!status.ok())
        {
            cout << " Status Not OK!" << endl;
        }
        else
        {
            cout <<"Front Server: " << reply_.name() << ", " << reply_.time() << endl;
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr <<"TestSimpleClient::request " << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr <<"TestSimpleClient::request unkonw exceptions! " << '\n';
    }
}

void ServerStreamClient::request()
{
    try
    {
        cout << "\nServerStreamClient::request " << endl;

        grpc::Status status;

        string name = "ServerStreamClient";
        string time = NanoTimeStr();
        string session_id = "apple";

        ClientContext   context;

        request_.set_name(name);

        request_.set_time(time);

        request_.set_session_id(session_id);

        cout << "From Request: " << session_id << " " << name << " " << time << endl;

        if (!is_ansyc_)
        {
            // sync_rpc_ = stub_->TestServerStream(&context, request_);

            // sync_rpc_ = stub_->ServerStreamApple(&context, request_);

            // stub_->ServerStreamApple()

            // while(sync_rpc_->Read(&reply_))
            // {
            //     cout <<"From Server: " << reply_.session_id() << ", "  << reply_.name() << ", " << reply_.time() << endl;
            // }
            // status = sync_rpc_->Finish();

            // if (!status.ok())
            // {
            //     cout << "Sync Read Status Not OK!" << endl;
            // }            
        }
        else
        {
             stub_->AsyncServerStreamApple(&context, cq_, this);
        }

        if (!status.ok())
        {
            cout << " Status Not OK!" << endl;
        }
        else
        {
            cout <<"Front Server: " << reply_.name() << ", " << reply_.time() << endl;
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr <<"ServerStreamClient::request " << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr <<"ServerStreamClient::request unkonw exceptions! " << '\n';
    }
}




void AsyncClient::start()
{
    try
    {
        init_cq_thread();
        
        init_rpc_client();
        
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::start " << e.what() << '\n';
    }
}

void AsyncClient::init_rpc_client()
{
    apple_rpc_ = new ClientApplePRC(channel_, &cq_);

    apple_rpc_->set_async_client(this);
}

void AsyncClient::init_cq_thread()
{
    try
    {
        cq_thread_ = boost::make_shared<std::thread>(&AsyncClient::run_cq_loop, this);

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::init_cq_thread() " << e.what() << '\n';
    }
}

void AsyncClient::run_cq_loop()
{
    try
    {
        void* tag;
        bool status;
        while(true)
        {
            std::cout << "Before Next; " << std::endl;

            bool result = cq_.Next(&tag, &status);

            std::cout << "After Next: result: "<<  result << " status: " << status << std::endl;

            if (result && status)
            {
                ClientBaseRPC* rpc = static_cast<ClientBaseRPC*>(tag);
                rpc->process();
            }
            else
            {
                ClientBaseRPC* rpc = static_cast<ClientBaseRPC*>(tag);

                rpc->reconnect();

                rpc->release();
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::init_cq_thread() " <<e.what() << '\n';
    }
}

void AsyncClient::set_client_map(RpcType rpc_id, ClientBaseRPC* client_rpc)
{
    try
    {
        client_rpc_map_[rpc_id] = client_rpc;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  " << e.what() << '\n';
    }
    
}