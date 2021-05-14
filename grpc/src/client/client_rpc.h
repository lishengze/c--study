#pragma once

#include "cpp/test.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/alarm.h>

#include "global_declare.h"

#include "package_simple.h"

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

#include "config.h"

#include "package_simple.h"

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
            session_id_ = CONFIG->get_session_id();
            obj_id_ = ++obj_count_;
        }

        virtual ~ClientBaseRPC() { }

        virtual void connect(){ }        

        virtual void process();

        virtual void procceed();

        virtual void release();

        virtual ClientBaseRPC* spawn() { return this; }

        virtual void make_active();

        virtual void add_data(PackagePtr package){ }

        virtual void set_client_map();

        virtual void on_connected() { }

        virtual void req_login() { }

        virtual void on_rsp_login() { }

        virtual void set_connected(bool is_connected) {
            is_connected_ = is_connected;
        }

        virtual bool is_connected() { return is_connected_;}

        virtual bool is_login() { return is_login_;}

        virtual void set_is_login(bool is_login) {
            is_login_ = is_login;
        }

        virtual void process_read_cq(){ }

        virtual void process_write_cq(){ }        
        
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
        static int                              obj_count_;        

        bool                                    is_write_cq_{false};

        bool                                    is_released_{false};

        bool                                    is_connected_{false};
        bool                                    is_login_{false};

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

        virtual void connect();

        virtual void on_connected();

        virtual void req_login();

        virtual void on_rsp_login();

        virtual void add_data(PackagePtr package);

        virtual void process_read_cq();

        virtual void process_write_cq();

    private:

    TestResponse reply;

    list<PackagePtr>                                         cached_request_data_;
    std::mutex                                               cached_data_mutex_;

    string                                                   last_cq_msg;

    grpc::ClientContext                                      context_;
    std::unique_ptr<grpc::ClientAsyncReaderWriter<TestRequest, TestResponse>> responder_;

    int  req_id_{0};

    long test_start_time_;
    long test_rsp_end_time_;
    long test_write_cq_end_time_;

    long test_cmp_write_count{0};
    long test_rsp_count_{0};

};

