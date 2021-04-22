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

#include "rpc.h"

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

class BaseServer
{

public:

    BaseServer(string address):address_(address) { }

    virtual ~BaseServer();

    void start();

    void init_cq_thread();

    void run_cq_loop();

protected:

    string                                  address_;

    std::unique_ptr<ServerCompletionQueue>  cq_;
    std::unique_ptr<Server>                 server_;

    ServerBuilder                           builder_;
    TestStream::AsyncService                service_;

    boost::shared_ptr<std::thread>          cq_thread_{nullptr};

    TestSimpleRPC*                          simple_rpc;      

    ServerStreamRPC*                        server_stream_rpc;                     
};


// class SyncServer
// {

// };