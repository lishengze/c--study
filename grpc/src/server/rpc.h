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
#include "data_struct.h"
#include "../include/global_declare.h"

using grpc::Alarm;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerWriter;
using grpc::ServerAsyncReaderWriter;

using TestPackage::TestStream;
using TestPackage::TestRequest;
using TestPackage::TestResponse;

using std::string;

class BaseServer;

class BaseRPC
{
    public:

    BaseRPC(ServerCompletionQueue* cq, TestStream::AsyncService* service): 
    cq_{cq},service_(service)
    {
        std::cout << "obj_count: " << ++obj_count << std::endl;

        obj_id_ = obj_count;
    }

    virtual ~BaseRPC() { cout << "Release BaseRPC " << endl; }

    virtual void process();

    virtual void proceed() { }

    virtual void release();

    virtual void register_request() { }

    virtual BaseRPC* spawn() { return this; }

    virtual void add_data(Fruit* fruit) {}

    void make_active();

   void set_server(BaseServer* server) { server_ = server;}

   void set_rpc_map();

    
    enum CallStatus     { CREATE, PROCESS, FINISH };
    CallStatus          status_{CREATE};                // The current serving state.        

    ServerCompletionQueue*                      cq_{nullptr};

    TestStream::AsyncService*                   service_;

    grpc::Alarm                                 alarm_;
    


    static int                                  obj_count;    

    SessionType                                 session_id_{""};

    RpcType                                     rpc_id_{""};

    BaseServer*                                 server_{NULL};

    bool                                        is_first_{true};

    bool                                        is_response_data_updated_{true};

    bool                                        is_write_cq_{false};

    bool                                        is_read_cq_{false};

    int                                         obj_id_{0};

    std::mutex                                  mutex_;

    bool                                        is_released_{false};

    string                                      cur_request_id_{""};                            
};



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

    virtual BaseRPC* spawn();

    void write_msg();
    
private:
    ServerContext                                        context_;    

    TestRequest                                          request_;

    TestResponse                                         reply_;

    ServerAsyncReaderWriter<TestResponse, TestRequest>   responder_;

    int  rsp_id_{0};

    
};

using ServerStreamAppleRPCPtr = boost::shared_ptr<ServerStreamAppleRPC>;

