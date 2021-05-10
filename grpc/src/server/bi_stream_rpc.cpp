#include "bi_stream_rpc.h"

#include "../include/time_util.h"
#include <thread>
#include <chrono>

#include "server.h"


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

        // std::this_thread::sleep_for(std::chrono::milliseconds(sleep_secs)); 
        
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

            // 初次连接;
            if (request_.time().length() == 0)
            {
                cout << "Client Connect!" << endl;

                on_connect();
                
                return;
            }

            // 登陆请求；
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
