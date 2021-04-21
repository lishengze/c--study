#include "rpc.h"
#include "pandora/util/time_util.h"
#include <thread>
#include <chrono>

void BaseRPC::make_active()
{
    try
    {
            gpr_timespec t = gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME);
            //tfm::printfln("make_active %u.%u", t.tv_sec, t.tv_nsec);
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

