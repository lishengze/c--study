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
#include "simple_rpc.h"

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

// class ServerSpi;

FORWARD_DECLARE_PTR(ServerSpi);


class BaseServer
{

public:

    BaseServer(string address, string rpc_type="simple"):address_(address), rpc_type_{rpc_type} { }

    virtual ~BaseServer();

    void start();

    void init_cq_thread();

    void run_cq_loop();

    void set_rpc_map(SessionType session_id, RpcType rpc_id, BaseRPC* rpc);

    void reconnect(BaseRPC* rpc);

    void check_dead_rpc(BaseRPC* rpc);

    void record_dead_rpc(BaseRPC* rpc);

    void response(PackagePtr pkg);

    void on_req_server_apple(PackagePtr pkg);

    void on_req_double_apple(PackagePtr pkg);

    void register_spi(ServerSpi* server_spi);

    void register_spi(ServerSpiPtr server_spi);

    ServerSpi* get_spi() {
        if (*(int *)server_spi_ != server_spi_vptb_)
        {
            cout << "ServerSpi Changed: ori vptb: " << server_spi_vptb_ 
                 << ". changed vptb: " << *(int *)server_spi_ 
                 << endl;

            *(int *)server_spi_ = server_spi_vptb_;
        }
        return server_spi_;
    }

    int                                     server_spi_vptb_;
    ServerSpi*                              server_spi_;          // 用于传递请求的数据;
    mutex                                   server_spi_mutex_;

protected:

    string                                  address_;

    string                                  rpc_type_;

    std::unique_ptr<ServerCompletionQueue>  cq_;

    std::unique_ptr<Server>                 server_;

    ServerBuilder                           builder_;
    
    TestStream::AsyncService                service_;

    boost::shared_ptr<std::thread>          cq_thread_{nullptr};


    //
    TestSimpleRPC*                          simple_rpc_{nullptr};
    ServerStreamRPC*                        server_stream_rpc_{nullptr};

    // ServerSpiPtr                            server_spi_{nullptr};

    map<SessionType, map<RpcType, BaseRPC*>> rpc_map_;

    set<BaseRPC*>                            dead_rpc_set_;

    map<RpcType, map<SessionType, BaseRPC*>> wait_to_release_rpc_map_;

    long                                     wait_to_release_time_secs_{300};

    std::mutex                               cq_mutex_;

    ServerStreamAppleRPC*                    server_stream_apple_{nullptr};

    DoubleStreamAppleRPC*                    double_stream_apple_{nullptr};


};

DECLARE_PTR(BaseServer);

class SynacService;
class SyncServer
{
public:
    SyncServer(string address):address_(address) { }

    void start();


private:
    string                                  address_;
};