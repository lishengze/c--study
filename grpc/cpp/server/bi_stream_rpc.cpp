#include "bi_stream_rpc.h"

#include "../include/time_util.h"
#include <thread>
#include <chrono>

#include "server.h"
#include "config.h"

#include "log.h"


void ServerStreamAppleRPC::register_request()
{
    cout << "ServerStreamAppleRPC::register_request!" << endl;

    service_->RequestServerStreamApple(&context_, &responder_, cq_, cq_, this);
}

void ServerStreamAppleRPC::send_msg(string message, string rsp_id)
{
    try
    {
        // cout << "\nServerStreamAppleRPC::write_msg " << endl;
        int sleep_secs = 100;

        grpc::Status status;
        
        string name = "ServerStreamAppleRPC";
        string time = NanoTimeStr();
        reply_.set_name(name);
        reply_.set_time(time);
        reply_.set_session_id(session_id_);
        reply_.set_obj_id(std::to_string(obj_id_));
        reply_.set_response_id(rsp_id);
        reply_.set_message(message);

        // std::this_thread::sleep_for(std::chrono::milliseconds(sleep_secs)); 
        
        responder_.Write(reply_, this);

        stringstream s_obj;

        s_obj << "[SERVER]: "
             << "session_id_=" << session_id_ 
             << ", rpc=" << rpc_id_
             << ", rsp_id=" << rsp_id 
             << ", msg=" << message
             << ", time=" << time 
             << "\n";

        LOG_INFO(s_obj.str());
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

void ServerStreamAppleRPC::read_data()
{
    try
    {
        responder_.Read(&request_, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

bool ServerStreamAppleRPC::is_connect_init() 
{ 
    return request_.session_id().length() == 0 ;
}

bool ServerStreamAppleRPC::is_login_request() 
{ 
    return request_.message() == "login";
}

void ServerStreamAppleRPC::init_session_id()
{
    try
    {
        session_id_ = request_.session_id();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void ServerStreamAppleRPC::process_business_request()
{
    try
    {
        if (session_id_ == "" && request_.session_id() != "")
        {
            session_id_ = request_.session_id();
            set_rpc_map();
        }

        ++request_count_;
        if (request_count_ % 1 == 0)
        {
            stringstream s_obj;

            s_obj << "[CLIENT]: session_id_=" << request_.session_id() 
                << ", rpc=" << request_.rpc_id()
                << ", req_id=" << request_.request_id()
                << ", req_count=" << request_count_
                << ", time=" << request_.time() 
                << "\n";
            LOG_INFO(s_obj.str());
        }

        string session_id = request_.session_id();
        if (request_count_ == 1) 
        {
            TimeStruct time_struct;
            time_struct.test_start_time_ = std::stol(request_.time());
            
            test_time[session_id] = time_struct;
        }

        if (request_count_ == CONFIG->get_test_count())
        {
            test_time[session_id].test_end_time_ = NanoTime();

            long cost_micros = (test_time[session_id].test_end_time_ - test_time[session_id].test_start_time_) /1000;

            stringstream s_obj;

            s_obj << "[R]Get " << rpc_id_ << " " << session_id << " " << request_count_ << " request cost " 
                << cost_micros << " micros" 
                << " ave: " << cost_micros / request_count_ << " micros"
                << "\n";
            LOG_INFO(s_obj.str());
        }

        string name = request_.name();
        string time = request_.time();

        PackagePtr pkg = CreatePackage<Apple>(name, time);

        if (server_)
        {
            server_->on_req_server_apple(pkg);
        }
        else
        {
            cout << "ServerStreamAppleRPC::process_read_cq server is null" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ServerStreamAppleRPC::response(PackagePtr pkg)
{
    try
    {
        grpc::Status status;

        ApplePtr apple = GetField<Apple>(pkg);
        
        reply_.set_name(apple->name);
        reply_.set_time(apple->time);
        reply_.set_session_id(session_id_);
        reply_.set_obj_id(std::to_string(obj_id_));
        reply_.set_response_id(std::to_string(++rsp_id_));

        responder_.Write(reply_, this);

        cout << "[SERVER]: "
             << "session_id_=" << session_id_ 
             << ", rpc=" << rpc_id_
             << ", rsp_id=" << rsp_id_
             << ", msg=" << apple->message
             << ", time=" << apple->time 
             << "\n"
             << endl;

        is_write_cq_ = true;

        if (!status.ok())
        {
            cout << "ServerStreamAppleRPC Write Error: " << status.error_details() << " " << status.error_message() << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ServerStreamAppleRPC::add_data " << e.what() << '\n';
    }
}

BaseRPC* ServerStreamAppleRPC::spawn()
{
    try
    {
        std::cout << "******* Spawn A New ServerStreamAppleRPC Server For Next Client ********" << std::endl;
        std::lock_guard<std::mutex> lk(mutex_);

        ServerStreamAppleRPC* new_rpc = new ServerStreamAppleRPC(service_, cq_);
        new_rpc->register_server(server_);

        return new_rpc;
    } 
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  ServerStreamAppleRPC::spawn" << e.what() << '\n';
    }    
}


void DoubleStreamAppleRPC::register_request()
{
    cout << "DoubleStreamAppleRPC::register_request!\n" << endl;

    service_->RequestDoubleStreamApple(&context_, &responder_, cq_, cq_, this);
}

void DoubleStreamAppleRPC::send_msg(string message, string rsp_id)
{
    try
    {
        // cout << "\nServerStreamAppleRPC::write_msg " << endl;
        int sleep_secs = 100;

        grpc::Status status;
        
        string name = "DoubleStreamAppleRPC";
        string time = NanoTimeStr();
        reply_.set_name(name);
        reply_.set_time(time);
        reply_.set_session_id(session_id_);
        reply_.set_obj_id(std::to_string(obj_id_));
        reply_.set_response_id(rsp_id);
        reply_.set_message(message);

        // std::this_thread::sleep_for(std::chrono::milliseconds(sleep_secs)); 
        
        responder_.Write(reply_, this);

        stringstream s_obj;

        s_obj << "[SERVER]: "
             << "session_id_=" << session_id_ 
             << ", rpc=" << rpc_id_
             << ", rsp_id=" << rsp_id 
             << ", msg=" << message
             << ", time=" << time 
             << "\n";
        
        LOG_INFO(s_obj.str());

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

void DoubleStreamAppleRPC::read_data()
{
    try
    {
        responder_.Read(&request_, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

bool DoubleStreamAppleRPC::is_connect_init() 
{ 
    return request_.session_id().length() == 0 ;
}

bool DoubleStreamAppleRPC::is_login_request() 
{ 
    return request_.message() == "login";
}

void DoubleStreamAppleRPC::init_session_id()
{
    try
    {
        session_id_ = request_.session_id();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void DoubleStreamAppleRPC::process_business_request()
{
    try
    {
        if (session_id_ == "" && request_.session_id() != "")
        {
            session_id_ = request_.session_id();
            set_rpc_map();
        }

        if (++request_count_ % 1 == 0)
        {
            stringstream s_obj;

            s_obj << "[CLIENT]: session_id_=" << request_.session_id()
                << ", rpc=" << request_.rpc_id()
                << ", req_id=" << request_.request_id()
                << ", req_count=" << request_count_
                << ", time=" << request_.time() 
                << "\n";

            LOG_INFO(s_obj.str());
        }

        string session_id = request_.session_id();
        if (request_count_ == 1) 
        {
            TimeStruct time_struct;
            time_struct.test_start_time_ = std::stol(request_.time());
            
            test_time[session_id] = time_struct;
        }

        if (request_count_ == CONFIG->get_test_count())
        {
            test_time[session_id].test_end_time_ = NanoTime();

            long cost_micros = (test_time[session_id].test_end_time_ - test_time[session_id].test_start_time_) /1000;

            stringstream s_obj;

            s_obj << "[R]Get " << rpc_id_ << " " << session_id << " " << request_count_ << " request cost " 
                << cost_micros << " micros" 
                << " ave: " << cost_micros / request_count_ << " micros"
                << "\n";

            LOG_INFO(s_obj.str());
        }
    
        string name = request_.name();
        string time = request_.time();

        PackagePtr pkg = CreatePackage<Apple>(name, time);

        if (server_)
        {
            server_->on_req_double_apple(pkg);
        }
        else
        {
            cout << "DoubleStreamAppleRPC::process_read_cq server is null" << endl;
        }     
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void DoubleStreamAppleRPC::response(PackagePtr pkg)
{
    try
    {
        if (is_write_cq_)
        {
            cache_pkg_list_.push_back(pkg);
            cout << "cache DoubleStreamAppleRPC " << pkg->RequestID() << endl;
            return;
        }
        
        grpc::Status status;

        ApplePtr apple = GetField<Apple>(pkg);
        
        reply_.set_name(apple->name);
        reply_.set_time(apple->time);
        reply_.set_session_id(session_id_);
        reply_.set_obj_id(std::to_string(obj_id_));
        reply_.set_response_id(std::to_string(++rsp_id_));

        cout << "[SERVER]: "
             << "session_id_=" << session_id_ 
             << ", rpc=" << rpc_id_
             << ", rsp_id=" << rsp_id_
             << ", msg=" << apple->message
             << ", time=" << apple->time 
             << "\n"
             << endl;

        responder_.Write(reply_, this);

        is_write_cq_ = true;

        if (!status.ok())
        {
            cout << "DoubleStreamAppleRPC Write Error: " << status.error_details() << " " << status.error_message() << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] DoubleStreamAppleRPC::add_data " << e.what() << '\n';
    }
}

BaseRPC* DoubleStreamAppleRPC::spawn()
{
    try
    {
        std::cout << "******* Spawn A New DoubleStreamAppleRPC Server For Next Client ********" << std::endl;
        std::lock_guard<std::mutex> lk(mutex_);

        DoubleStreamAppleRPC* new_rpc = new DoubleStreamAppleRPC(service_, cq_);
        new_rpc->register_server(server_);

        return new_rpc;
    } 
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  DoubleStreamAppleRPC::spawn" << e.what() << '\n';
    }    
}