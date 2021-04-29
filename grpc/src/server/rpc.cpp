#include "rpc.h"
#include "../include/time_util.h"
#include <thread>
#include <chrono>

#include "server.h"

int BaseRPC::obj_count = 0;

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

void BaseRPC::set_rpc_map()
{
    try
    {
        if (server_)
        {
            server_->set_rpc_map(session_id_, rpc_id_, this);
        }
        else
        {
            cout << session_id_ << " server was not inited!" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "BaseRPC::set_rpc_map "  << e.what() << '\n';
    }
    
}

void BaseRPC::process()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        if (CREATE == status_)
        {
            cout << "\nStatus is CREATE" << endl;
            status_ = PROCESS;
            register_request();
        }
        else if (PROCESS == status_)
        {
            // if (is_first_)
            // {
            //     is_first_ = false;
            //     spawn();
            // }

            // cout << "\nStatus is PROCESS" << endl;
            // status_ = FINISH;
            proceed();
        }
        else if (FINISH == status_)
        {
            cout << "\nStatus is FINISH" << endl;
            // register_request();
            // make_active();
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

void BaseRPC::release()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);
        cout << "BaseRPC::release Obj_Count:  " << obj_id_ << endl;

        if (!is_released_)
        {
            is_released_ = true;
            delete this;
        }
        else
        {
            cout << "[E] BaseRPC::release id=" << obj_id_ << " has been Released!!! " << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

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


void ServerStreamAppleRPC::register_request()
{
    cout << "ServerStreamAppleRPC::register_request!" << endl;

    // service_->RequestServerStreamApple(&context_, &request_, &responder_, cq_, cq_, this);

    service_->RequestServerStreamApple(&context_, &responder_, cq_, cq_, this);
}

void ServerStreamAppleRPC::write_msg(string message)
{
    try
    {
        cout << "\nServerStreamAppleRPC::write_msg " << endl;
        int sleep_secs = 100;

        grpc::Status status;
        
        string name = "ServerStreamAppleRPC";
        string time = NanoTimeStr();
        reply_.set_name(name);
        reply_.set_time(time);
        reply_.set_session_id(session_id_);
        reply_.set_obj_id(std::to_string(obj_id_));
        reply_.set_response_id(std::to_string(++rsp_id_));
        reply_.set_message(message);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_secs)); 
        
        responder_.Write(reply_, this);

        cout << "[SERVER] obj_id = " << obj_id_ 
             << ", session=" << session_id_ 
             << ", name=" << name 
             << ", time=" << time 
             << ", rsp_id=" << rsp_id_ 
             << ", message=" << message
             << endl;

        // responder_.Finish(status, this);

        is_write_cq_ = true;

        if (!status.ok())
        {
            cout << "ServerStreamAppleRPC Write Error: " << status.error_details() << " " << status.error_message() << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ServerStreamAppleRPC::write_msg " << e.what() << '\n';
    }

}

void ServerStreamAppleRPC::proceed()
{
    try
    {
        cout << "\nServerStreamAppleRPC::process obj_id = "  << obj_id_ << endl;

        if (is_write_cq_)
        {
            is_write_cq_ = false;
            cout << "This is Write_CQ" << endl;
        }
        else
        {
            responder_.Read(&request_, this);

            cout << "[CLIENT]: session_id_=" << request_.session_id() 
                << ", name=" << request_.name() 
                << ", time=" << request_.time() 
                << ", obj_id=" << request_.obj_id() << endl;

            if (request_.time().length() == 0)
            {
                cout << "Client Connect!" << endl;

                on_connect();
                
                return;
            }

            if (request_.message() == "login")
            {
                on_req_login();
            }
            else
            {
                write_msg();
            }            
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"ServerStreamAppleRPC::process " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "ServerStreamAppleRPC::process unkonwn exceptions" << endl;
    }
}

void ServerStreamAppleRPC::on_connect()
{
    try
    {
        connect_time_ = NanoTime();

        write_msg("connected");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void ServerStreamAppleRPC::on_req_login()
{
    try
    {
        cout << "ServerStreamAppleRPC::on_req_login " << endl;

        session_id_ = request_.session_id();

        set_rpc_map();

        rsp_login();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ServerStreamAppleRPC::on_login  " << e.what() << '\n';
    }
}

void ServerStreamAppleRPC::rsp_login()
{
    try
    {
        cout << "ServerStreamAppleRPC::rsp_login " << endl;
        write_msg("login_successfully");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void ServerStreamAppleRPC::release()
{
    try
    {
        cout << "\n------- ServerStreamAppleRPC::release obj_id:  " << obj_id_ << " ---------"<< endl;
        std::lock_guard<std::mutex> lk(mutex_);

        if (!is_released_)
        {
            is_released_ = true;
            delete this;
        }
        else
        {
            cout << "[E] ServerStreamAppleRPC::release id=" << obj_id_ << " has been Released!!! " << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ServerStreamAppleRPC::release() " << e.what() << '\n';
    }
    

}

BaseRPC* ServerStreamAppleRPC::spawn()
{
    try
    {
        std::cout << "\n ******* Spawn A New ServerStreamAppleRPC Server For Next Client ********" << std::endl;
        std::lock_guard<std::mutex> lk(mutex_);

        ServerStreamAppleRPC* new_rpc = new ServerStreamAppleRPC(service_, cq_);
        new_rpc->set_server(server_);

        return new_rpc;
    } 
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  ServerStreamAppleRPC::spawn" << e.what() << '\n';
    }    
}

