#pragma once

#include "../include/global_declare.h"

#include "cpp/test.grpc.pb.h"

#include <grpcpp/grpcpp.h>
using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using TestPackage::TestStream;

using TestPackage::TestRequest;
using TestPackage::TestResponse;

class BaseClient
{
public:

    BaseClient(std::shared_ptr<Channel> channel):
        stub_{TestStream::NewStub(channel)}
    {

    }

    virtual ~BaseClient();

    // virtual void start() { }

    // virtual void thread_run() { }

    // virtual void request() { }

      

    std::unique_ptr<TestPackage::TestStream::Stub>  stub_;
    ClientContext                                   context_;
    CompletionQueue                                 cq_;

};

class TestSimpleClient:public BaseClient
{

public:
    TestSimpleClient(std::shared_ptr<Channel> channel):BaseClient(channel)
    {

    }

    virtual ~TestSimpleClient(){ }

    virtual void start();

    virtual void thread_run();

    virtual void request();


private:
    TestRequest                 request_;
    TestResponse                reply_;    
    std::shared_ptr<std::thread>                    thread_{nullptr};  
    std::unique_ptr<ClientAsyncResponseReader<TestResponse>> rpc_;
};