#include "server.h"
#include "boost/make_shared.hpp"

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
        builder_.AddListeningPort(address_, grpc::InsecureServerCredentials());
        builder_.RegisterService(&service_);

        cq_ = builder_.AddCompletionQueue();
        server_ = builder_.BuildAndStart();
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

void BaseServer::run_cq_loop()
{
    void* tag;
    bool status;
    while(true)
    {
        cq_->Next(&tag, &status);

        if (status)
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

void BaseRPC::process()
{

}

void BaseRPC::release()
{

}


void TestSimpleRPC::register_all()
{
    service_->RequestTestSimple(&context_, &request_, &responder_, cq_, cq_, this);
}

void TestSimpleRPC::process()
{
    
}

void TestSimpleRPC::release()
{
    reply_.set_name("TestSimpleRPC");
    reply_.set_time()
}

