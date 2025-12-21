
#include "rpc.h"
#include "sync_rpc.h"
#include "time_util.h"


grpc::Status SynacService::ServerStreamApple(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream) 
{ 
    try
    {
        TestRequest request;

        while(stream->Read(&request))
        {
            cout << "[CLIENT]: session_id_=" << request.session_id() 
                << ", name=" << request.name() 
                << ", time=" << request.time() 
                << ", obj_id=" << request.obj_id() << endl;            
        }
        
        return grpc::Status();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] SynacService::ServerStreamApple " << e.what() << '\n';
    }
}

void SynacService::on_connect(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream)
{
    try
    {
        write_msg(stream, "connected");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void SynacService::on_req_login(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream)
{
    try
    {
        cout << "SynacService::on_req_login " << endl;

        rsp_login(stream);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] SynacService::on_login  " << e.what() << '\n';
    }
}

void SynacService::rsp_login(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream)
{
    try
    {
        cout << "ServerStreamAppleRPC::rsp_login " << endl;
        write_msg(stream,"login_successfully");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void SynacService::write_msg(grpc::ServerReaderWriter< ::TestPackage::TestResponse, ::TestPackage::TestRequest>* stream, string message)
{
    try
    {
        cout << "\nSynacService::write_msg " << endl;
        int sleep_secs = 100;

        grpc::Status status;

        TestPackage::TestResponse reply;
        
        string name = "ServerStreamAppleRPC";
        string time = NanoTimeStr();
        reply.set_name(name);
        reply.set_time(time);
        reply.set_message(message);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_secs)); 
        
        

        cout << "[SERVER] "
             << ", name=" << name 
             << ", time=" << time 
             << ", message=" << message
             << endl;

        stream->Write(reply);             
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] SynacService::write_msg " << e.what() << '\n';
    }

}