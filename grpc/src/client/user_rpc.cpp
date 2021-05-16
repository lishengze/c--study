#include "user_rpc.h"


ClientBaseRPC* ClientApplePRC::spawn()
{
    try
    {
        std::cout << "\n ******* Spawn A New ServerStreamAppleRPC Server For Next Client ********" << std::endl;

        std::shared_ptr<Channel>   channel = channel_;
        CompletionQueue*           cq = cq_;
        AsyncClient*               async_client = async_client_;

        // ClientApplePRC* client_apple = new (this) ClientApplePRC(channel, cq);

        ClientApplePRC* client_apple = new ClientApplePRC(channel, cq, session_id_);

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

void ClientApplePRC::req_login()
{
    try
    {
        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        TestRequest  request;

        request.set_session_id(session_id_);
        request.set_message("login");

        cout << "login: obj_id=" << obj_id_ << ", rpc_id: " << rpc_id_ << ", session_id= " << request.session_id() 
             << endl;            

        is_write_cq_ = true;

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::login() "  << e.what() << '\n';
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

            cout << "\n[CQ] " << rpc_id_ << "Complete " << CONFIG->get_test_count() << " write request cost " 
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

ClientBaseRPC* DoubleApplePRC::spawn()
{
    try
    {
        std::cout << "\n ******* Spawn A New DoubleApplePRC Server For Next Client ********" << std::endl;

        std::shared_ptr<Channel>   channel = channel_;
        CompletionQueue*           cq = cq_;
        AsyncClient*               async_client = async_client_;

        // ClientApplePRC* client_apple = new (this) ClientApplePRC(channel, cq);

        DoubleApplePRC* client_apple = new DoubleApplePRC(channel, cq, session_id_);

        client_apple->set_async_client(async_client);

        return client_apple;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DoubleApplePRC::spawn " <<  e.what() << '\n';
    }
}

void DoubleApplePRC::connect()
{
    try
    {
        cout << "DoubleApplePRC::connect " << endl;

        responder_ = stub_->PrepareAsyncDoubleStreamApple(&context_, cq_);

        responder_->StartCall(this);

        last_cq_msg = "StartCall";

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DoubleApplePRC::connect " <<  e.what() << '\n';
    }
    
}

void DoubleApplePRC::req_login()
{
    try
    {
        string name = "DoubleApplePRC";
        string time = NanoTimeStr();

        TestRequest  request;

        request.set_session_id(session_id_);
        request.set_message("login");

        cout << "login: obj_id=" << obj_id_ << ", rpc_id: " << rpc_id_ << ", session_id= " << request.session_id() 
             << endl;            

        is_write_cq_ = true;

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] DoubleApplePRC::login() "  << e.what() << '\n';
    }

}

void DoubleApplePRC::process_write_cq()
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

            cout << "\n[CQ] " << rpc_id_ << " Complete " << CONFIG->get_test_count() << " write request cost " 
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
        std::cerr << "\n[E] DoubleApplePRC::process_write_cq " << e.what() << '\n';
    }
}

void DoubleApplePRC::process_read_cq()
{
    try
    {
        // cout << "ClientApplePRC::process_read_cq " << endl;

        std::lock_guard<std::mutex> lk(mutex_);                
        
        responder_->Read(&reply, this);

        if (reply.time().length() == 0)
        {
            last_cq_msg = "Get Empty Response Data";
            cout << "[W] " << rpc_id_ <<  " Empty Response" << endl;
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

void DoubleApplePRC::add_data(PackagePtr pkg)
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