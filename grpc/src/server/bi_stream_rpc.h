#pragma once

#include "rpc.h"
#include "global_declare.h"

class ServerStreamAppleRPC:public BaseRPC
{
public:

    ServerStreamAppleRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
        BaseRPC{cq, service}, responder_(&context_)
    {
        rpc_id_ = "apple";
    }

    virtual ~ServerStreamAppleRPC() { cout << "Release ServerStreamAppleRPC! " << endl; }

    // virtual void release();    

    virtual void register_request();

    virtual BaseRPC* spawn();

    virtual void process_read_cq();

    // virtual void process_write_cq();    

    virtual void add_data(PackagePtr pkg);

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

// class DoubleStreamMangoRPC:public BaseRPC
// {
// public:

//     DoubleStreamMangoRPC(TestStream::AsyncService* service, ServerCompletionQueue* cq):
//                          BaseRPC{cq, service}, responder_(&context_)
//     {
//         rpc_id_ = "apple";
//     }

//     virtual ~DoubleStreamMangoRPC() { cout << "Release DoubleStreamMangoRPC! " << endl; }

//     virtual void release();    

//     virtual void register_request();

//     virtual void on_connect();

//     virtual void on_req_login();

//     virtual void rsp_login();

//     virtual BaseRPC* spawn();

//     virtual void process_read_cq();

//     virtual void process_write_cq();    

//     virtual void add_data(PackagePtr pkg);

//     void write_msg(string message="", string rsp_id="");
    
// private:
//     ServerContext                                        context_;    

//     TestRequest                                          request_;

//     TestResponse                                         reply_;

//     ServerAsyncReaderWriter<TestResponse, TestRequest>   responder_;

//     int  rsp_id_{0};

//     struct TimeStruct
//     {   
//         long             test_start_time_;
//         long             test_end_time_;
//     };

//     map<string, TimeStruct> test_time;

//     long             request_count_{0};        
// };