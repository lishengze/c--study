#pragma once

#include "base_rpc.h"

class ClientApplePRC:public ClientBaseRPC
{
    public:
        ClientApplePRC(std::shared_ptr<Channel> channel, CompletionQueue* cq, string session_id):
        ClientBaseRPC(channel, cq, session_id)
        { 
            cout << "\n-------- Create ClientApplePRC id = " << obj_id_ << " --------"<< endl;
            rpc_id_ = "ServerStreamApple";
        }

        virtual ~ClientApplePRC() 
        { 
            cout << session_id_<< " " << rpc_id_ << " " << obj_id_ << " released" << endl;
        }

        virtual ClientBaseRPC* spawn();

        virtual void connect();

        virtual void req_login();

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

class DoubleApplePRC:public ClientBaseRPC
{
    public:
        DoubleApplePRC(std::shared_ptr<Channel> channel, CompletionQueue* cq, string session_id):
        ClientBaseRPC(channel, cq, session_id)
        { 
            cout << "\n-------- Create DoubleApplePRC id = " << obj_id_ << " --------"<< endl;
            rpc_id_ = "DoubleStreamApple";
        }

        virtual ~DoubleApplePRC() 
        { 
            cout << session_id_<< " " << rpc_id_ << " " << obj_id_ << " released" << endl;
        }

        virtual ClientBaseRPC* spawn();

        virtual void connect();

        virtual void req_login();

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
