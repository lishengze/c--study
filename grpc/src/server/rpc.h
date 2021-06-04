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
#include "package_simple.h"

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
        // std::cout << "obj_count: " << ++obj_count << std::endl;

        obj_id_ = obj_count;
    }

    virtual ~BaseRPC() { cout << "Release BaseRPC " << endl; }

    virtual void process();

    virtual void proceed();

    virtual void release();

    virtual void register_request() { }

    virtual BaseRPC* spawn() { return this; }

    virtual void response(PackagePtr pkg) {}

    virtual void on_connect();

    virtual void rsp_connect();

    virtual void on_req_login();

    virtual void rsp_login();

    virtual void send_msg(string message, string rsp_id) { }

    void make_active();

   void set_server(BaseServer* server) { server_ = server;}

   void set_rpc_map();

    virtual void process_read_cq(){ }

    virtual void process_write_cq();

    void set_disconnect_time(long time) {
        disconnect_time_ = time;
    }

    
    enum CallStatus     { CREATE, PROCESS, FINISH };
    CallStatus          status_{CREATE};                // The current serving state.        

    ServerCompletionQueue*                      cq_{nullptr};

    TestStream::AsyncService*                   service_;

    grpc::Alarm                                 alarm_;
    


    static int                                  obj_count;    

    SessionType                                 session_id_{""};

    RpcType                                     rpc_id_{""};

    BaseServer*                                 server_{nullptr};

    bool                                        is_first_{true};

    bool                                        is_released_{false};

    bool                                        is_write_cq_{false};

    int                                         obj_id_{0};

    std::mutex                                  mutex_;

    unsigned long                               disconnect_time_;

    long                                        rsp_id_{0};

    list<PackagePtr>                            cache_pkg_list_;
};
