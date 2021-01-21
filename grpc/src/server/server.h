#pragma once

#include <iostream>
#include <thread>
#include <boost/shared_ptr.hpp>

#include <grpc/grpc.h>
#include <grpcpp/alarm.h>
#include <grpc/support/log.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "cpp/test.grpc.pb.h"

using grpc::Alarm;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncResponseWriter;

using TestPackage::TestStream;
using TestPackage::TestRequest;
using TestPackage::TestResponse;


using std::string;


class BaseRPC
{
    public:
        virtual void process();

        virtual void release();
};

class BaseServer
{

public:

    virtual ~BaseServer();

    void start();

    void init_cq_thread();

    void run_cq_loop();

private:

    string                                  address_;

    std::unique_ptr<ServerCompletionQueue>  cq_;
    std::unique_ptr<Server>                 server_;

    ServerBuilder                           builder_;
    TestStream::AsyncService                service_;

    boost::shared_ptr<std::thread>          cq_thread_{nullptr};
};

class TestSimpleRPC:public BaseRPC
{

public:
    TestSimpleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        service_(service), cq_(cq), responder_(&context_)
    {

    }

    virtual void process();

    virtual void release();    

    void register_all();
    


private:

    TestStream::AsyncService*                   service_;

    ServerCompletionQueue*                      cq_;

    ServerContext                               context_;

    TestRequest                                 request_;

    TestResponse                                reply_;
    
    ServerAsyncResponseWriter<TestResponse>     responder_;
};