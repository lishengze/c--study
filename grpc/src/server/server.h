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

class TradeEngine;

class BaseServer
{

public:

    BaseServer(string address):address_(address) { }

    virtual ~BaseServer();

    void start();

    void init_cq_thread();

    void run_cq_loop();

    void set_rpc_map(SessionType session_id, RpcType rpc_id, BaseRPC* rpc);

    template<class DataType>
    void add_data(DataType* data) 
    {
        string session_id = data->session_id;
        string rpc_id = data->rpc_id;

        if (rpc_map_.find(session_id) != rpc_map_.end() && rpc_map_[session_id].find(rpc_id) != rpc_map_[session_id].end())
        {
            // rpc_map_[session_id][rpc_id]->add_data(data);
        }
    }    

protected:

    string                                  address_;

    std::unique_ptr<ServerCompletionQueue>  cq_;
    std::unique_ptr<Server>                 server_;

    ServerBuilder                           builder_;
    TestStream::AsyncService                service_;

    boost::shared_ptr<std::thread>          cq_thread_{nullptr};

    TestSimpleRPC*                          simple_rpc;      

    ServerStreamRPC*                        server_stream_rpc;   

    ServerStreamAppleRPC*                   server_stream_apple_;
    ServerStreamPearRPC*                    server_stream_pear_;
    ServerStreamMangoRPC*                   server_stream_mango_;

    TradeEngine*                            trade_engine_;

    map<SessionType, map<RpcType, BaseRPC*>>  rpc_map_;

    map<BaseRPC*, int>                      released_rpc_map_;

    std::mutex                              cq_mutex_;
};


// class SyncServer
// {

// };