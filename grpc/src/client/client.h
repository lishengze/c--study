#pragma once

#include <iostream>

#include "../include/global_declare.h"
#include "data_struct.h"

#include "cpp/test.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/alarm.h>

using grpc::Alarm;
using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientAsyncReader;
using grpc::ClientReader;

using grpc::ClientReaderWriter;
using grpc::ClientAsyncReaderWriter;

using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using TestPackage::TestStream;

using TestPackage::TestRequest;
using TestPackage::TestResponse;

using std::cout;
using std::endl;

class BaseClient
{
public:

    BaseClient(std::shared_ptr<Channel> channel):
        stub_{TestStream::NewStub(channel)}
    {
        // cout << "Client connect: " << stub_-> 
    }

    virtual ~BaseClient();

    virtual void start();

    virtual void thread_run();

    virtual void request() { }

      

    std::unique_ptr<TestPackage::TestStream::Stub>  stub_;    
    CompletionQueue*                                cq_;
    std::shared_ptr<std::thread>                    thread_{nullptr};    
};

class TestSimpleClient:public BaseClient
{

public:
    TestSimpleClient(std::shared_ptr<Channel> channel):BaseClient(channel)
    {

    }

    virtual ~TestSimpleClient(){ }

    // virtual void start();

    // virtual void thread_run();

    virtual void request();


private:
    TestRequest                                     request_;
    TestResponse                                    reply_;    
    bool                                            is_ansyc_{false};
    
    std::unique_ptr<ClientAsyncResponseReader<TestResponse>> rpc_;
};

class ServerStreamClient:public BaseClient
{

public:
    ServerStreamClient(std::shared_ptr<Channel> channel):BaseClient(channel)
    {

    }

    virtual ~ServerStreamClient(){ }

    // virtual void start();

    // virtual void thread_run();

    virtual void request();


private:
    TestRequest                                     request_;
    TestResponse                                    reply_;    
    bool                                            is_ansyc_{true};

    std::unique_ptr<ClientReader<TestResponse>>     sync_rpc_;

    std::unique_ptr<ClientAsyncReaderWriter<TestRequest, TestResponse>> async_rpc_;
};

class ServerStreamAppleClient:public BaseClient
{

public:
    ServerStreamAppleClient(std::shared_ptr<Channel> channel):BaseClient(channel)
    {

    }

    virtual ~ServerStreamAppleClient(){ }

    // virtual void start();

    // virtual void thread_run();

    virtual void request();


private:
    TestRequest                                     request_;
    TestResponse                                    reply_;    
    bool                                            is_ansyc_{true};

    std::unique_ptr<ClientAsyncReaderWriter<TestRequest, TestResponse>> async_rpc_;
};

class ClientBaseRPC;
class ClientApplePRC;

class AsyncClient
{
public:

    AsyncClient(std::shared_ptr<Channel> channel):
                channel_{channel}
    {
        // cout << "Client connect: " << stub_-> 
    }

    virtual ~AsyncClient() 
    {
        if (cq_thread_ &&  cq_thread_->joinable())
        {
            cq_thread_->join();
        }
    }

    void start();

    void init_rpc_client();

    void init_cq_thread();

    void run_cq_loop();

    void set_client_map(RpcType rpc_id, ClientBaseRPC* client_rpc);

    // template<class DataType>
    // void add_data(DataType* data) 
    // {
    //     string rpc_id = data->rpc_id;

    //     if (client_rpc_map_.find(rpc_id) != client_rpc_map_.end())
    //     {
    //         client_rpc_map_[rpc_id]->add_data(data);
    //     }
    // }        

    void add_data(Fruit* data);   

    std::shared_ptr<Channel>                        channel_;

    CompletionQueue                                 cq_;
    
    boost::shared_ptr<std::thread>                  cq_thread_{nullptr};   

    ClientApplePRC*                                 apple_rpc_;

    map<RpcType, ClientBaseRPC*>                    client_rpc_map_;
};

using AsyncClientPtr = boost::shared_ptr<AsyncClient>;
