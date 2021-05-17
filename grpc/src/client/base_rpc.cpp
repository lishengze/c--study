#include "base_rpc.h"
#include "client.h"
#include "time_util.h"

#include "package_simple.h"

int ClientBaseRPC::obj_count_ = 0;

void ClientBaseRPC::make_active() 
{
    try
    {
        std::cout << "ClientBaseRPC make active " << std::endl;
        gpr_timespec t = gpr_now(gpr_clock_type::GPR_CLOCK_REALTIME);
        alarm_.Set(cq_, t, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E]  ClientBaseRPC::make_active" << e.what() << '\n';
    }
}

void ClientBaseRPC::process()
{
    try
    {
        // cout << "\nClientBaseRPC::process " << endl;        
        
        if (status_ == CREATE)
        {
            // cout << "Status is CREATE " << endl;

            connect();

            status_ = PROCESS;
        }
        else if (status_ == PROCESS)
        {
            // cout << "Status is PROCESS " << endl;

            procceed();
        }
        else if (status_ == FINISH)
        {
            
            cout << "Current Request IS Over" << endl;

            status_ = PROCESS;
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

void ClientBaseRPC::set_client_map()
{
    try
    {
        if (nullptr != async_client_)
        {
            async_client_->set_client_map(rpc_id_, this);
        }
        else
        {
            cout << "[E] async_client_ is NULL" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ClientBaseRPC::set_async_client(AsyncClient* async_client)
{
    try
    {
        if (!async_client)
        {
            cout << "[E] async_client is NULL " << endl;
        }
        else
        {
            async_client_ = async_client;

            async_client_->set_client_map(rpc_id_, this);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ClientBaseRPC::procceed()
{
    try
    {
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
        std::cerr <<"\n[E] ClientBaseRPC::procceed " << e.what() << '\n';
    }
    
}

void ClientBaseRPC::release()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        cout << "\n********* " << rpc_id_ <<  " release id = " << obj_id_ << " ********\n"<< endl;
        if (!is_released_)
        {
            is_released_ = true;
            delete this;
        }
        else
        {
            cout << "[E] " << rpc_id_ << " ::release id=" << obj_id_ << " has been Released!!! " << endl;
        } 
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientBaseRPC::release " << e.what() << '\n';
    }    
}

void ClientBaseRPC::on_connected()
{
    try
    {
        cout << session_id_<< " " << rpc_id_ << " " << obj_id_  << " on_connected " << endl;
        is_connected_ = true;

        req_login();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::on_connected() "  << e.what() << '\n';
    }
}

void ClientBaseRPC::on_rsp_login()
{
    try
    {
        // cout << session_id_<< " " << rpc_id_ << " " << obj_id_ << " logined!" << endl;
        is_login_ = true;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::on_rsp_login " << e.what() << '\n';
    }

}
