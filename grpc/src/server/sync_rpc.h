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
#include "data_struct.h"
#include "../include/global_declare.h"

using grpc::Alarm;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerWriter;
using grpc::ServerAsyncReaderWriter;

using TestPackage::TestStream;
using TestPackage::TestRequest;
using TestPackage::TestResponse;

using std::string;


class SynacService: public TestStream::Service
{
    public:

    virtual ::grpc::Status TestSimple(::grpc::ServerContext* context, const ::TestPackage::TestRequest* request, ::TestPackage::TestResponse* response) { return grpc::Status();}
    virtual ::grpc::Status TestClientStream(::grpc::ServerContext* context, ::grpc::ServerReader< ::TestPackage::TestRequest>* reader, ::TestPackage::TestResponse* response) { return grpc::Status();}
    virtual ::grpc::Status TestServerStream(::grpc::ServerContext* context, const ::TestPackage::TestRequest* request, ::grpc::ServerWriter< ::TestPackage::TestResponse>* writer) { return grpc::Status();}
    virtual ::grpc::Status TestDoubleStream(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) { return grpc::Status();}
    virtual ::grpc::Status ServerStreamApple(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) ;
    virtual ::grpc::Status ServerStreamPear(::grpc::ServerContext* context, const ::TestPackage::TestRequest* request, ::grpc::ServerWriter< ::TestPackage::TestResponse>* writer) { return grpc::Status();}
    virtual ::grpc::Status ServerStreamMango(::grpc::ServerContext* context, const ::TestPackage::TestRequest* request, ::grpc::ServerWriter< ::TestPackage::TestResponse>* writer) { return grpc::Status();}
    virtual ::grpc::Status DoubleStreamApple(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) { return grpc::Status();}
    virtual ::grpc::Status DoubleStreamPear(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) { return grpc::Status();}
    virtual ::grpc::Status DoubleStreamMango(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) { return grpc::Status();}

    void on_connect(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream);

    void on_req_login(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream);

    void rsp_login(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream);

    void write_msg(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream, string message="");    
};