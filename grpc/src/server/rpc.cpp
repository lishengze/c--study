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
        if (CREATE == status_)
        {
            cout << "Status is CREATE" << endl;
            status_ = PROCESS;
            register_request();
        }
        else if (PROCESS == status_)
        {
            // cout << "\nStatus is PROCESS" << endl;
            // status_ = FINISH;
            proceed();

            if (is_first_)
            {
                is_first_ = false;
                BaseRPC* next_rpc = spawn();
                next_rpc->process();
            }            
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

void BaseRPC::proceed()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        if (is_write_cq_)
        {
            process_write_cq();
        }
        else
        {
            process_read_cq();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void BaseRPC::release()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);
        cout << "Release " << rpc_id_ << " " << session_id_ << " Obj_Count:  " << obj_id_ << endl;

        if (!is_released_)
        {
            is_released_ = true;
            delete this;
        }
        else
        {
            cout << "[E] Release " << rpc_id_ << " " 
                 << session_id_ << " Obj_Count:  " 
                 << obj_id_ << " has been releaed " << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseRPC::on_connect() 
{
    try
    {
        rsp_connect();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseRPC::rsp_connect()
{
    try
    {
        send_msg("connected", std::to_string(rsp_id_));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void BaseRPC::on_req_login() 
{
    try
    {
        set_rpc_map();

        rsp_login();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void BaseRPC::rsp_login()
{
    try
    {
        send_msg("login_successfully", std::to_string(rsp_id_));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void BaseRPC::process_write_cq()
{
    try
    {
        // std::lock_guard<std::mutex> lk(mutex_);

        is_write_cq_ = false;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] BaseRPC::process_write_cq " << e.what() << '\n';
    }
}
