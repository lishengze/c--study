#include "client_rpc.h"
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


ClientBaseRPC* ClientApplePRC::spawn()
{
    try
    {
        std::cout << "\n ******* Spawn A New ServerStreamAppleRPC Server For Next Client ********" << std::endl;

        std::shared_ptr<Channel>   channel = channel_;
        CompletionQueue*           cq = cq_;
        AsyncClient*               async_client = async_client_;

        // ClientApplePRC* client_apple = new (this) ClientApplePRC(channel, cq);

        ClientApplePRC* client_apple = new ClientApplePRC(channel, cq);

        client_apple->set_async_client(async_client);

        return client_apple;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::spawn " <<  e.what() << '\n';
    }
}

void ClientApplePRC::connect()
{
    try
    {
        cout << "ClientApplePRC::connect " << endl;

        responder_ = stub_->PrepareAsyncServerStreamApple(&context_, cq_);

        responder_->StartCall(this);

        last_cq_msg = "StartCall";

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::connect " <<  e.what() << '\n';
    }
    
}

void ClientApplePRC::process_write_cq()
{
    try
    {
        // cout << "last_cq_msg: " << last_cq_msg << endl;
        is_write_cq_ = false;
        // cout << "This is Write_CQ" << endl;
        last_cq_msg = "This is Write_CQ";

        if (req_id_ == 0) return;

        if (++test_cmp_write_count == CONFIG->get_test_count())
        {
            long sum_write_cq_time_ = (NanoTime() - test_start_time_)/1000;

            cout << "\n[CQ]Complete " << CONFIG->get_test_count() << " write request cost " 
                << sum_write_cq_time_  << " micros" 
                << " ave: " << sum_write_cq_time_ / CONFIG->get_test_count() << " micros"
                << endl;
        }

        if (!cached_request_data_.empty())
        {
            PackagePtr pkg = cached_request_data_.front();
            cached_request_data_.pop_front();
            // cout << "Restart Add Cached Data: " << first_data->request_id << endl;
            add_data(pkg);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::process_write_cq " << e.what() << '\n';
    }
}

void ClientApplePRC::process_read_cq()
{
    try
    {
        // cout << "ClientApplePRC::process_read_cq " << endl;

        std::lock_guard<std::mutex> lk(mutex_);                
        
        responder_->Read(&reply, this);

        if (reply.time().length() == 0)
        {
            last_cq_msg = "Get Empty Response Data";
            cout << "[W] Empty Response" << endl;
            return;
        }
        else
        {
            last_cq_msg = "Get Full Response Data";
            string rsp_message = reply.message();
            if (!is_connected_ && rsp_message == "connected")
            {
                cout << "[SERVER]:"
                        << "session_id=" << reply.session_id() 
                        << ", rsp_id="<< reply.response_id()
                        << ", time=" << reply.time()                        
                        << ", msg=" << reply.message()
                        << "\n"
                        << endl;

                on_connected();

            }
            else if (is_connected_ && rsp_message == "login_successfully")
            {
                cout << "[SERVER]:"
                        << "session_id= " << reply.session_id() 
                        << ", rsp_id="<< reply.response_id()
                        << ", time=" << reply.time()                        
                        << ", msg=" << reply.message()
                        << "\n"
                        << endl;

                on_rsp_login();
            }
            else
            {
                ++test_rsp_count_;

                if (test_rsp_count_ % 100 == 0)
                {
                    cout << "[SERVER]:"
                        << "session_id=" << reply.session_id() 
                        << ", rpc=" << rpc_id_
                        << ", rsp_id="<< reply.response_id()
                        << ", rsp_count=" << test_rsp_count_
                        << ", time=" << reply.time() 
                        << "\n"
                        << endl;
                }

                if (test_rsp_count_ == CONFIG->get_test_count())
                {
                    test_rsp_end_time_ = NanoTime();

                    long cost_time = (test_rsp_end_time_ - test_start_time_)/1000;

                    cout << "[R] Complete " << CONFIG->get_test_count() 
                        << " request cost: " << cost_time << " micros"  
                        << " ave: " << cost_time / CONFIG->get_test_count() << " micros " << endl;
                }                            
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientApplePRC::process_read_cq " << e.what() << '\n';
    }

}

void ClientApplePRC::on_connected()
{
    try
    {
        cout << "ClientApplePRC::on_connected " << endl;
        is_connected_ = true;

        req_login();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::on_connected() "  << e.what() << '\n';
    }
}

void ClientApplePRC::req_login()
{
    try
    {
        cout << "ClientApplePRC::login " << endl;

        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        TestRequest  request;

        request.set_session_id(session_id_);
        request.set_message("login");

        cout << "login: obj_id=" << obj_id_ << ", session_id= " << request.session_id() 
             << endl;            

        is_write_cq_ = true;

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::login() "  << e.what() << '\n';
    }

}

void ClientApplePRC::on_rsp_login()
{
    try
    {
        is_login_ = true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void ClientApplePRC::add_data(PackagePtr pkg)
{
    try
    {
        // cout << "ClientApplePRC add_data " << endl;

        // cout << "\n+++++++ ClientApplePRC::add_data ++++++++++" << endl;

        std::lock_guard<std::mutex> lk(mutex_);

        if (!is_connected_)
        {
            cout << "[E] rpc_id: " << rpc_id_ << ", obj_id: " << obj_id_  << " Server was not Connected " << endl;
            return;
        }

        if (!is_login_)
        {
            cout << "[E] rpc_id: " << rpc_id_ << ", obj_id: " << obj_id_  << " Server was not Login " << endl;
            return;
        }

        if (is_released_)
        {
            cout << "[E] rpc_id: " << rpc_id_ << ", obj_id: " << obj_id_  <<" RPC was released " << endl;
            return;
        }

        if (is_write_cq_)
        {
            cached_request_data_.push_back(pkg);
            // cout << "Last Write Was not Finished! Cached Data " << data->request_id << endl;
            return;
        }
        
        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        ApplePtr real_data = GetField<Apple>(pkg);

        TestRequest request;

        request.set_session_id(session_id_);
        request.set_rpc_id(rpc_id_);

        request.set_name(real_data->name);
        request.set_time(real_data->time);
        request.set_obj_id(std::to_string(obj_id_));

        request.set_request_id(std::to_string(pkg->RequestID()));

        if (pkg->RequestID() % 100 == 0)
        {
            cout << "[CLIENT]:"
                    << "session_id= " << request.session_id() 
                    << ", rpc=" << rpc_id_
                    << ", req_id=" << pkg->RequestID()
                    << ", time=" << request.time()                
                    << endl;  
        }
          

        // int sleep_secs = 3;
        // cout << "sleep for " << sleep_secs << " secs " << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;

        if (pkg->RequestID() == 1)
        {
            test_start_time_ = NanoTime();
        }

        // TestTime cur_test_write_cq_;
        // cur_test_write_cq_.start_time_ = NanoTime();

        req_id_ = pkg->RequestID();

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}