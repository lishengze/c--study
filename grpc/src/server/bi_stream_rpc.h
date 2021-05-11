#pragma once

#include "rpc.h"

class ServerStreamAppleRPC:public BaseRPC
{
public:

    ServerStreamAppleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service}, responder_(&context_)
    {
        rpc_id_ = "apple";
    }

    virtual ~ServerStreamAppleRPC() { cout << "Release ServerStreamAppleRPC! " << endl; }

    virtual void proceed();

    virtual void release();    

    virtual void register_request();

    virtual void on_connect();

    virtual void on_req_login();

    virtual void rsp_login();

    virtual BaseRPC* spawn();

    void write_msg(string message="", string rsp_id="");
    
private:
    ServerContext                                        context_;    

    TestRequest                                          request_;

    TestResponse                                         reply_;

    ServerAsyncReaderWriter<TestResponse, TestRequest>   responder_;

    int  rsp_id_{0};

    
};

using ServerStreamAppleRPCPtr = boost::shared_ptr<ServerStreamAppleRPC>;