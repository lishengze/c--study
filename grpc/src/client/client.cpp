#include "client.h"
#include "global_declare.h"

#include "../include/global_declare.h"
#include "pandora/util/time_util.h"


BaseClient::~BaseClient()
{
    if (thread_ && thread_->joinable())
    {
        thread_->join();
    }
}

void TestSimpleClient::start()
{
    cout << "TestSimpleClient::start " << endl;

    // thread_ = std::make_shared<std::thread>(&TestSimpleClient::thread_run, this);

    thread_run();
}

void TestSimpleClient::thread_run()
{
    cout << "TestSimpleClient::thread_run " << endl;

    request();

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
        cout << "TestSimpleClient::request " << endl;

        grpc::Status status;

        string name = "TestSimpleClient";
        string time = utrade::pandora::NanoTimeStr();

        request_.set_name(name);

        request_.set_time(time);

        cout << "Request: " << name << " " << time << endl;

        status = stub_->TestSimple(&context_, request_, &reply_);

        // rpc_ = stub_->PrepareAsyncTestSimple(&context_, request_, &cq_);
        // cout << "[T] 2" << endl;
        
        // rpc_->StartCall();
        
        // cout << "[T] 3" << endl;
        // rpc_->Finish(&reply_, &status, (void*)1);

        // cout << 4 << endl;

        if (status.ok())
        {
            cout <<"Client OK " << reply_.name() << " " << reply_.time() << endl;
        }
        else
        {
            cout <<"Client Failed " << status.error_message() << "\n" << status.error_details() << endl;
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