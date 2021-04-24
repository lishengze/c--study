#include "client_rpc.h"

void ClientBaseRPC::process()
{
    try
    {
        if (status_ == CREATE)
        {
            init_request();

            spawn();

            status_ = PROCESS;
        }
        else if (status_ == PROCESS)
        {
            procceed();

            status_ = FINISH;
        }
        else if (status_ == FINISH)
        {
            cout << "Current Request IS Over" << endl;
        }
        else
        {
            cout << "Unknown Request Status" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientBaseRPC::process " << e.what() << '\n';
    }
    
}

void ClientBaseRPC::procceed()
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientBaseRPC::procceed " << e.what() << '\n';
    }
    
}

void ClientBaseRPC::release()
{
    try
    {
       delete this;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientBaseRPC::release " << e.what() << '\n';
    }
    
}


void ClientApplePRC::spawn()
{
    try
    {
        ClientApplePRC* client_apple = new ClientApplePRC(channel_, cq_);

        client_apple->set_async_client(async_client_);

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::spawn " <<  e.what() << '\n';
    }
}

void ClientApplePRC::init_request()
{
    try
    {
        grpc::Status status;

        responder_ = stub_->PrepareAsyncServerStreamApple(&context_, cq_);

        responder_->StartCall(this);

        // responder_->Finish(&status, this);

        // if (!status.ok())
        // {
        //     cout << "  " << endl;
        // }


    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::init_request " <<  e.what() << '\n';
    }
    
}


// Read Data;
void ClientApplePRC::procceed()
{
    try
    {
        /* request new data */
        if (is_request_data_updated_)
        {
            is_request_data_updated_ = false;

            string name = "ClientApplePRC";
            string time = utrade::pandora::NanoTimeStr();

            request_.set_session_id(session_id_);
            request_.set_name(name);
            request_.set_time(time);

            responder_->Write(request_, this);

            cout << "Request: session_id= " << request_.session_id() 
                    << " , name=" << request_.name() 
                    << " , time=" << request_.time()
                    << endl;

        }
        // New Response Data Coming!
        else
        {
            responder_->Read(&reply_, this);

            cout << "From Server: session_id= " << reply_.session_id() 
                    << " , name=" << reply_.name() 
                    << " , time=" << reply_.time()
                    << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::procceed " <<  e.what() << '\n';
    }
    
}

