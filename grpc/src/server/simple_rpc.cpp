#include "simple_rpc.h"

void TestSimpleRPC::register_request()
{
    cout << "TestSimpleRPC::register_request again!" << endl;

    service_->RequestTestSimple(&context_, &request_, &responder_, cq_, cq_, this);
}

void TestSimpleRPC::proceed()
{
    try
    {
        cout << "\nTestSimpleRPC::process " << endl;

        cout << "From Request: name = " << request_.name() << ", time = " << request_.time() << endl;



        int sleep_secs = 2;
        cout << "Sleep " << sleep_secs << " Secs" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        string name = "TestSimpleRPC";
        string time = NanoTimeStr();
        reply_.set_name(name);
        reply_.set_time(time);
        
        grpc::Status status;
        
        responder_.Finish(reply_, status, this);

        if (!status.ok())
        {
            cout << "TestSimpleRPC Write Error: " << status.error_details() << " " << status.error_message() << endl;
        }    
        else
        {
            cout << "Server Response " << name <<" " << time << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"TestSimpleRPC::process " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "TestSimpleRPC::process unkonwn exceptions" << endl;
    }
}

void TestSimpleRPC::release()
{
    cout << "TestSimpleRPC::release " << endl;
}


void ServerStreamRPC::register_request()
{
    cout << "ServerStreamRPC::register_request!" << endl;

    service_->RequestTestServerStream(&context_, &request_, &responder_, cq_, cq_, this);

    // service_->RequestTestServerStream()
}

void ServerStreamRPC::proceed()
{
    try
    {
        cout << "\nServerStreamRPC::process " << endl;

        cout << "From Request: id = " << request_.session_id() << ", name = " << request_.name() << ", time = " << request_.time() << endl;

        if (session_id_.length() == 0)
        {
            session_id_ = request_.session_id();

            set_rpc_map();
        }

        int sleep_secs = 2;

        grpc::Status status;
        
        int responder_numb = 1;
        while(responder_numb--)
        {
            string name = "ServerStreamRPC";
            string time = NanoTimeStr();
            reply_.set_name(name);
            reply_.set_time(time);
            reply_.set_session_id(session_id_);

            
            responder_.Write(reply_, this);

            cout << "Server Response, id = " << session_id_ << ", name = " << name << ", time = " << time << endl;

            std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));            
        }

        // responder_.Finish(status, this);

        if (!status.ok())
        {
            cout << "ServerStreamRPC Write Error: " << status.error_details() << " " << status.error_message() << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"ServerStreamRPC::process " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "ServerStreamRPC::process unkonwn exceptions" << endl;
    }
}

void ServerStreamRPC::release()
{
    cout << "ServerStreamRPC::release Obj_Count:  " << --obj_count << endl;
    delete this;
}
