#pragma once

#include <iostream>

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

    // virtual void start() { }

    // virtual void thread_run() { }

    // virtual void request() { }

      

    std::unique_ptr<TestPackage::TestStream::Stub>  stub_;
    ClientContext                                   context_;
    CompletionQueue                                 cq_;
    std::shared_ptr<std::thread>                    thread_{nullptr};  

    bool                                            is_ansyc_{true};

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
    TestRequest                                     request_;
    TestResponse                                    reply_;    
    
    std::unique_ptr<ClientAsyncResponseReader<TestResponse>> rpc_;
};