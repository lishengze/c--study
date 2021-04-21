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
#include "../include/global_declare.h"

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

    BaseRPC(ServerCompletionQueue* cq): cq_{cq}
    {

    }

    virtual void process();

    virtual void proceed() { }

    virtual void release() { }

    virtual void register_request() { }

    void make_active();
    
    enum CallStatus     { CREATE, PROCESS, FINISH };
    CallStatus          status_{CREATE};                // The current serving state.        

    ServerCompletionQueue*                      cq_;
    grpc::Alarm                                 alarm_;
};



class TestSimpleRPC:public BaseRPC
{

public:
    TestSimpleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq}, service_(service), responder_(&context_)
    {
        process();
    }

    virtual void proceed();

    virtual void release();    

    virtual void register_request();

    
    
private:

    TestStream::AsyncService*                   service_;

    ServerContext                               context_;

    TestRequest                                 request_;

    TestResponse                                reply_;
    
    ServerAsyncResponseWriter<TestResponse>     responder_;
};
using TestSimpleRPCPtr = boost::shared_ptr<TestSimpleRPC>;