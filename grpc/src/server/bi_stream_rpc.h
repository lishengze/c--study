#pragma once

#include "rpc.h"
#include "global_declare.h"

class ServerStreamAppleRPC:public BaseRPC
{
public:

    ServerStreamAppleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service}, responder_(&context_)
    {
        rpc_id_ = "ServerStreamApple";
    }

    virtual ~ServerStreamAppleRPC() { cout << "Release ServerStreamAppleRPC! " << endl; }

    virtual void register_request();

    virtual BaseRPC* spawn();

    virtual void process_read_cq();

    virtual void response(PackagePtr pkg);

    virtual void send_msg(string message, string rsp_id);
    
private:
    ServerContext                                        context_;    

    TestRequest                                          request_;

    TestResponse                                         reply_;

    ServerAsyncReaderWriter<TestResponse, TestRequest>   responder_;

    struct TimeStruct
    {   
        long             test_start_time_;
        long             test_end_time_;
    };

    map<string, TimeStruct> test_time;

    long             request_count_{0};                  
};

using ServerStreamAppleRPCPtr = boost::shared_ptr<ServerStreamAppleRPC>;

class DoubleStreamAppleRPC:public BaseRPC
{
public:

    DoubleStreamAppleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service}, responder_(&context_)
    {
        rpc_id_ = "DoubleStreamApple";
    }

    virtual ~DoubleStreamAppleRPC() { cout << "Release DoubleStreamAppleRPC! " << endl; }

    virtual void register_request();

    virtual BaseRPC* spawn();

    virtual void process_read_cq();

    virtual void response(PackagePtr pkg);

    virtual void send_msg(string message, string rsp_id);
    
private:
    ServerContext                                        context_;    

    TestRequest                                          request_;

    TestResponse                                         reply_;

    ServerAsyncReaderWriter<TestResponse, TestRequest>   responder_;

    struct TimeStruct
    {   
        long             test_start_time_;
        long             test_end_time_;
    };

    map<string, TimeStruct> test_time;

    long             request_count_{0};                  
};