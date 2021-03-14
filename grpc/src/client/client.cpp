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
        cout << "\nTestSimpleClient::request " << endl;

        grpc::Status status;

        string name = "TestSimpleClient";
        string time = utrade::pandora::NanoTimeStr();

        request_.set_name(name);

        request_.set_time(time);

        cout << "From Request: " << name << " " << time << endl;

        if (!is_ansyc_)
        {
            // status = stub_->TestSimple(&context_, request_, &reply_);
        }
        else
        {
            // rpc_ = stub_->PrepareAsyncTestSimple(&context_, request_, &cq_);
            // cout <<"rpc_ = stub_->PrepareAsyncTestSimple(&context_, request_, &cq_);" << endl;
            
            // rpc_->StartCall();
            // cout <<"rpc_->StartCall();" << endl;
            
            // rpc_->Finish(&reply_, &status, (void*)1);
            // cout <<"rpc_->Finish(&reply_, &status, (void*)(&data));" << endl;

            

            rpc_ = stub_->AsyncTestSimple(&context_, request_, &cq_);
            cout <<"rpc_ = stub_->AsyncTestSimple(&context_, request_, &cq_);" << endl;

            // rpc_->StartCall();
            // cout <<"rpc_->StartCall();" << endl;

            int data;
            rpc_->Finish(&reply_, &status, (void*)(&data));
            cout <<"rpc_->Finish(&reply_, &status, (void*)(&data));" << endl;

            void* got_tag;
            bool ok = false;

            if (!cq_.Next(&got_tag, &ok))
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

            if (!status.ok())
            {
                cout << " rpc_->Finish(&reply_, &status, (void*)1); Failed!" << endl;
            }
            else
            {
                cout <<"Front Server: " << reply_.name() << ", " << reply_.time() << endl;
            }
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