#pragma once

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



using TestPackage::TestStream;
using TestPackage::TestRequest;
using TestPackage::TestResponse;


#include "../include/global_declare.h"

class AsyncClient;

class ClientBaseRPC
{
    public:
        ClientBaseRPC(std::shared_ptr<Channel> channel, CompletionQueue* cq): 
        channel_{channel},
        stub_{TestStream::NewStub(channel)},
        cq_{cq} 
        { 

            process();
        }

        virtual ~ClientBaseRPC() { }

        virtual void init_request(){ }        

        virtual void process();

        virtual void procceed();

        virtual void release();

        virtual void spawn() {}

        virtual void make_actinve() { }
       

        void set_async_client(AsyncClient* async_client)
        {
            async_client_ = async_client;
        }

    enum CallStatus     { CREATE, PROCESS, FINISH };
    CallStatus          status_{CREATE};                // The current client state.            


    protected:
        bool     is_request_data_updated_{true};

        string                                  session_id_;
        AsyncClient*                            async_client_{nullptr};
        CompletionQueue*                        cq_{nullptr};
        std::unique_ptr<TestStream::Stub>       stub_;
        std::shared_ptr<Channel>                channel_;               
};


class ClientApplePRC:public ClientBaseRPC
{
    public:
        ClientApplePRC(std::shared_ptr<Channel> channel, CompletionQueue* cq):
        ClientBaseRPC(channel, cq),
        responder_{&context_, false, this}
        { 
            session_id_ = "apple";
        }

        virtual ~ClientApplePRC() { }

        virtual void spawn();

        virtual void init_request();

        virtual void procceed();
    private:

    TestRequest                                              request_;
    TestResponse                                             reply_;

    grpc::ClientContext                                      context_;
    std::unique_ptr<grpc::ClientAsyncReaderWriter<TestRequest, TestResponse>> responder_;
};

