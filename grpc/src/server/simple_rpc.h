#pragma once

#include "rpc.h"

class TestSimpleRPC:public BaseRPC
{

public:
    TestSimpleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service_}, responder_(&context_)
    {
        rpc_id_ = "ServerStreamRPC";

        process();
    }

    virtual ~TestSimpleRPC() { }

    virtual void proceed();

    virtual void release();    

    virtual void register_request();

private:
    ServerContext                               context_;

    TestRequest                                 request_;

    TestResponse                                reply_;
    
    ServerAsyncResponseWriter<TestResponse>     responder_;

    
};

using TestSimpleRPCPtr = boost::shared_ptr<TestSimpleRPC>;

class ServerStreamRPC:public BaseRPC
{
public:

    ServerStreamRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service}, responder_(&context_)
    {
        rpc_id_ = "ServerStreamRPC";

        process();
    }

    virtual ~ServerStreamRPC() { }

    virtual void proceed();

    virtual void release();    

    virtual void register_request();
    
private:

    ServerContext                               context_;

    TestRequest                                 request_;

    TestResponse                                reply_;

    ServerAsyncWriter<TestResponse>             responder_;
};

using ServerStreamRPCPtr = boost::shared_ptr<ServerStreamRPC>;