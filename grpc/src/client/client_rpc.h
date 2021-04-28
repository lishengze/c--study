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
#include "../include/data_struct.h"



class AsyncClient;

struct BaseRPCParam
{
    std::shared_ptr<Channel> channel;
    CompletionQueue* cq;
};

class ClientBaseRPC
{
    public:
        ClientBaseRPC(std::shared_ptr<Channel> channel, CompletionQueue* cq): 
        channel_{channel},
        stub_{TestStream::NewStub(channel)},
        cq_{cq} 
        { 
            session_id_ = "lsz";
            obj_id_ = ++obj_count_;
        }

        virtual ~ClientBaseRPC() { }

        virtual void init_request(){ }        

        virtual void process();

        virtual void procceed();

        virtual BaseRPCParam release();

        virtual ClientBaseRPC* spawn() { return this; }

        virtual void make_active();

        virtual void reconnect() { }

        virtual void add_data(Fruit* data){ }

        virtual void set_client_map();

        virtual void set_connected(bool is_connected) {
            is_connected_ = is_connected;
        }

        virtual bool is_connected() { return is_connected_;}

        void set_async_client(AsyncClient* async_client);

        enum CallStatus     { CREATE, PROCESS, FINISH };
        CallStatus          status_{CREATE};                // The current client state.            


    public:
        AsyncClient*                            async_client_{nullptr};
        CompletionQueue*                        cq_{nullptr};
        std::unique_ptr<TestStream::Stub>       stub_{nullptr};
        std::shared_ptr<Channel>                channel_{nullptr};       

        Alarm                                   alarm_;   



        string                                  session_id_{""};
        string                                  rpc_id_{""};        
        int                                     obj_id_{0};

        string                                  cur_response_id_{""};

        static int                              obj_count_;        

        bool                                    is_request_data_updated_{true};
        bool                                    is_write_cq_{false};
        bool                                    is_start_call_{true}; 
        bool                                    is_rsp_init_{false};
        bool                                    is_released_{false};
        bool                                    is_connected_{false};

        std::mutex                              mutex_;
};


class ClientApplePRC:public ClientBaseRPC
{
    public:
        ClientApplePRC(std::shared_ptr<Channel> channel, CompletionQueue* cq):
        ClientBaseRPC(channel, cq)
        { 
            cout << "\n-------- Create ClientApplePRC id = " << obj_id_ << " --------"<< endl;
            rpc_id_ = "apple";
        }

        virtual ~ClientApplePRC() { }

        virtual ClientBaseRPC* spawn();

        virtual void init_request();

        virtual void procceed();

        virtual void reconnect();

        virtual BaseRPCParam release();

        virtual void add_data(Fruit* data);

        void write_msg();

    private:

    TestRequest  request;
    TestResponse reply;

    string                                                   last_cq_msg;

    grpc::ClientContext                                      context_;
    std::unique_ptr<grpc::ClientAsyncReaderWriter<TestRequest, TestResponse>> responder_;

    int  req_id_{0};
};

