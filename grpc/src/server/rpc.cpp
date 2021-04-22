#include "rpc.h"
#include "pandora/util/time_util.h"
#include <thread>
#include <chrono>

void BaseRPC::make_active()
{
    try
    {
        std::cout << "make active " << std::endl;
        gpr_timespec t = gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME);
        alarm_.Set(cq_, t, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  BaseRPC::make_active" << e.what() << '\n';
    }
    
}

void BaseRPC::process()
{
    try
    {
        if (CREATE == status_)
        {
            cout << "\nStatus is CREATE" << endl;
            status_ = PROCESS;
            register_request();
        }
        else if (PROCESS == status_)
        {
            if (is_first_)
            {
                is_first_ = false;
                spawn();
            }

            cout << "\nStatus is PROCESS" << endl;
            status_ = FINISH;
            proceed();
        }
        else if (FINISH == status_)
        {
            cout << "\nStatus is FINISH" << endl;
            // register_request();
            make_active();
            status_ = PROCESS;
        }
        else
        {
            // release();
            
            cout << "\nUnkonw Status: " << status_ << endl;
        }

    }
    catch(const std::exception& e)
    {
        std::cerr <<"BaseRPC::process " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "BaseRPC::process unkonwn exceptions" << endl;
    }    
}

// void make_active();

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
        string time = utrade::pandora::NanoTimeStr();
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


int ServerStreamRPC::obj_count = 0;

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

        cout << "From Request: name = " << request_.name() << ", time = " << request_.time() << endl;

        int sleep_secs = 2;

        grpc::Status status;
        
        int responder_numb = 1;
        while(responder_numb--)
        {
            string name = "ServerStreamRPC";
            string time = utrade::pandora::NanoTimeStr();
            reply_.set_name(name);
            reply_.set_time(time);

            
            responder_.Write(reply_, this);

            cout << "Server Response " << name <<" " << time << endl;

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

void ServerStreamRPC::spawn()
{
    try
    {
        std::cout << "\n ******* Spawn A New Server For Next Client ********" << std::endl;
        ServerStreamRPC* new_rpc = new ServerStreamRPC(service_, cq_);

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  ServerStreamRPC::spawn" << e.what() << '\n';
    }
    
}