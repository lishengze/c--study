#include "rpc.h"
#include "pandora/util/time_util.h"

void BaseRPC::process()
{

}

void BaseRPC::release()
{

}


void TestSimpleRPC::register_all()
{
    service_->RequestTestSimple(&context_, &request_, &responder_, cq_, cq_, this);
}

void TestSimpleRPC::process()
{
    try
    {
        cout << "TestSimpleRPC::process " << endl;

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
            cout << "Server " << name <<" " << time << endl;
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

