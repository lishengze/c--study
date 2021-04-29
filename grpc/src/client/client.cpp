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
        string session_id_ = "apple";

        ClientContext   context;

        request_.set_name(name);

        request_.set_time(time);

        request_.set_session_id(session_id_);

        cout << "From Request: " << session_id_ << " " << name << " " << time << endl;

        if (!is_ansyc_)
        {
            // sync_rpc_ = stub_->TestServerStream(&context, request_);

            // sync_rpc_ = stub_->ServerStreamApple(&context, request_);

            // stub_->ServerStreamApple()

            // while(sync_rpc_->Read(&reply_))
            // {
            //     cout <<"From Server: " << reply_.session_id_() << ", "  << reply_.name() << ", " << reply_.time() << endl;
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

        // cout << "------- AsyncClient::start() Over -----" << endl;
        
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

    apple_rpc_->process();

    // set_client_map(apple_rpc_->rpc_id_, apple_rpc_);

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
            std::cout << "\n++++++++ Loop Start " << " ++++++++"<< std::endl;

            bool result = cq_.Next(&tag, &status);

            std::lock_guard<std::mutex> lk(cq_mutex_);

            ClientBaseRPC* rpc = static_cast<ClientBaseRPC*>(tag);

            if (dead_rpc_id_set_.find(rpc->obj_id_) != dead_rpc_id_set_.end())
            {
                std::cout << "[E][CQ] Next: result: "<<  result << " status: " << status
                          << ", session_id_=" << rpc->session_id_ 
                          << ", rpc_id_=" << rpc->rpc_id_ 
                          << ", obj_id: " << rpc->obj_id_ 
                          << std::endl;

                cout << "[E] RPC id=" << rpc->obj_id_ << " has been released!" << endl;
                continue;
            }

            std::cout << "[E]result: "<<  result << " status: " << status  
                        << ", session_id_=" << rpc->session_id_ 
                        << ", rpc_id_=" << rpc->rpc_id_ 
                        << ", obj_id: " << rpc->obj_id_ << std::endl;

            check_dead_rpc(rpc);

            if (result && status)
            {            
                rpc->process();
            }
            else
            {   
                record_dead_rpc(rpc);
                
                reconnect(rpc);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::init_cq_thread() " <<e.what() << '\n';
    }
}

void AsyncClient::add_data(Fruit* data) 
{
    string rpc_id_ = data->rpc_id;

    if (client_rpc_map_.find(rpc_id_) != client_rpc_map_.end())
    {
        client_rpc_map_[rpc_id_]->add_data(data);
    }
    else
    {
        cout << "rpc: " << rpc_id_ << " was not found!" << endl;
    }
}   

void AsyncClient::set_client_map(RpcType rpc_id_, ClientBaseRPC* client_rpc)
{
    try
    {
        cout << "AsyncClient::set_client_map " << rpc_id_ << ", obj_id: " << client_rpc->obj_id_ << endl;
        client_rpc_map_[rpc_id_] = client_rpc;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  " << e.what() << '\n';
    }
    
}

void AsyncClient::reconnect(ClientBaseRPC* rpc)
{
    try
    {
        

        ClientBaseRPC* new_rpc =  rpc->spawn();

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        new_rpc->process();        
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::reconnect " << e.what() << '\n';
    }

}

void AsyncClient::record_dead_rpc(ClientBaseRPC* rpc)
{
    try
    {
        rpc->set_connected(false);       

        rpc->set_is_login(false);

        dead_rpc_id_set_.emplace(rpc->obj_id_);

        wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] = rpc;

        cout << "Recorde Dead Rpc, rpc_id=" << rpc->rpc_id_ << " ,obj_id=" << rpc->obj_id_ << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] AsyncClient::record_dead_rpc " << e.what() << '\n';
    }

}

void AsyncClient::check_dead_rpc(ClientBaseRPC* rpc)
{
    try
    {
        // std::cout << " BaseServer::check_dead_rpc  " << std::endl;

        if (wait_to_release_rpc_map_.find(rpc->rpc_id_) != wait_to_release_rpc_map_.end()
        && wait_to_release_rpc_map_[rpc->rpc_id_].find(rpc->session_id_) != wait_to_release_rpc_map_[rpc->rpc_id_].end()
        && wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_])
        {
            if (wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] != rpc)
            {

                ClientBaseRPC* release_rpc = wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_];

                cout << "WaitToRelease session_id_=" << rpc->session_id_ << ", rpc_id_=" << rpc->rpc_id_ << ", dead obj_id=" << release_rpc->obj_id_
                     << ", new obj_id=" << rpc->obj_id_ << endl;

                release_rpc->release();

                wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] = nullptr;
            }

        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}