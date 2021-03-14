#include "server.h"

#include "boost/make_shared.hpp"

#include "pandora/util/time_util.h"

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

        simple_rpc = boost::make_shared<TestSimpleRPC>(&service_, cq_.get());

        // simple_rpc->register_all();

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

void BaseServer::run_cq_loop()
{
    try
    {
        void* tag;
        bool status;
        while(true)
        {
            // GPR_ASSERT(cq_->Next(&tag, &status));

            bool result = cq_->Next(&tag, &status);

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
