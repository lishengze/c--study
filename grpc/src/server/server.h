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
#include "bi_stream_rpc.h"

#include "package_simple.h"

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

    void reconnect(BaseRPC* rpc);

    void check_dead_rpc(BaseRPC* rpc);

    void record_dead_rpc(BaseRPC* rpc);

    void add_data(PackagePtr pkg);
  
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

    TradeEngine*                            trade_engine_;

    map<SessionType, map<RpcType, BaseRPC*>> rpc_map_;

    set<BaseRPC*>                            dead_rpc_set_;

    map<RpcType, map<SessionType, BaseRPC*>> wait_to_release_rpc_map_;

    long                                     wait_to_release_time_secs_{300};

    std::mutex                               cq_mutex_;
};

class SynacService;
class SyncServer
{
public:
    SyncServer(string address):address_(address) { }

    void start();


private:
    string                                  address_;
};