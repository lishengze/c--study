#include "server.h"

#include "boost/make_shared.hpp"

#include "../include/time_util.h"

#include "../include/global_declare.h"

#include "rpc.h"


BaseServer::~BaseServer()
{
    if (cq_thread_ && cq_thread_->joinable())
    {
        cq_thread_->join();
    }
}

void BaseServer::start()
{
    try
    {
        cout << "BaseServer listen: " << address_ << endl;

        builder_.AddListeningPort(address_, grpc::InsecureServerCredentials());
        builder_.RegisterService(&service_);

        cq_ = builder_.AddCompletionQueue();
        server_ = builder_.BuildAndStart();

        // simple_rpc = new TestSimpleRPC(&service_, cq_.get());

        // server_stream_rpc = new ServerStreamRPC(&service_, cq_.get());

        server_stream_apple_ = new ServerStreamAppleRPC(&service_, cq_.get());
        server_stream_apple_->set_server(this);

        // server_stream_pear_ = new ServerStreamPearRPC(&service_, cq_.get());
        // server_stream_pear_->set_server(this);

        // server_stream_mango_ = new ServerStreamMangoRPC(&service_, cq_.get());
        // server_stream_mango_->set_server(this);

        init_cq_thread();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::init_cq_thread()
{
    cq_thread_ = boost::make_shared<std::thread>(&BaseServer::run_cq_loop, this);
}

void BaseServer::set_rpc_map(SessionType session_id, RpcType rpc_id, BaseRPC* rpc)
{
    try
    {
        rpc_map_[session_id][rpc_id] = rpc;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::run_cq_loop()
{
    try
    {
        void* tag;
        bool status;
        while(true)
        {
            std::cout << "Before Next; " << std::endl;

            bool result = cq_->Next(&tag, &status);

            std::cout << "After Next: result: "<<  result << " status: " << status << std::endl;

            if (result && status)
            {
                BaseRPC* rpc = static_cast<BaseRPC*>(tag);
                rpc->process();
            }
            else
            {
                BaseRPC* rpc = static_cast<BaseRPC*>(tag);
                rpc->release();
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "BaseServer::run_cq_loop " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "BaseServer::run_cq_loop unkonwn exceptions" << endl;
    }    

}
