#include "client_rpc.h"
#include "client.h"

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
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] ClientBaseRPC::procceed " << e.what() << '\n';
    }
    
}

BaseRPCParam ClientBaseRPC::release()
{
    try
    {
        BaseRPCParam result;
        cout << " ClientBaseRPC::release " << endl;
        delete this;
        return result;
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

        // client_apple->process();

        // client_apple->set_client_map();

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
        is_rsp_init_ = false;

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

void ClientApplePRC::write_msg()
{
    try
    {
        cout << "ClientApplePRC::write_msg " << endl;

        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        TestRequest  request;

        request.set_session_id(session_id_);
        request.set_name(name);
        request.set_time(time);
        request.set_obj_id(std::to_string(obj_id_));

        cout << "Request: obj_id=" << obj_id_ << ", session_id= " << request.session_id() 
                << " , name=" << request.name() 
                << " , time=" << request.time()
                << endl;            

        int sleep_secs = 3;
        // cout << "sleep for " << sleep_secs << " secs " << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

// Read Data;
void ClientApplePRC::procceed()
{
    try
    {
        if (is_write_cq_)
        {
            process_write_cq();
        }
        // else if (is_start_call_)
        // {
        //     is_start_call_ = false;
        //     cout << "Is Start Call " << endl;
        //     last_cq_msg = "Is Start Call";  

        //     responder_->Read(&reply, this);
        //     if (reply.time().length() == 0)
        //     {
        //         last_cq_msg = "Get Empty Response Data";
        //         cout << "[W] Empty Response" << endl;
        //         return;
        //     }

        //     if (!is_rsp_init_)
        //     {
        //         // make_active();

        //         is_rsp_init_ = true;
        //     }   
        // }        
        // New Response Data Coming!
        else
        {
            process_read_cq();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::procceed " <<  e.what() << '\n';
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

        test_write_cq_[req_id_].end_time_ = NanoTime();

        long cur_cost_time = (test_write_cq_[req_id_].end_time_ - test_write_cq_[req_id_].start_time_) /1000;

        if (cur_cost_time < 0)
        {
            cout << "\n[Error] req_id:  " << req_id_ 
                << " start: " << ToCmpSecondStr(test_write_cq_[req_id_].start_time_) 
                << " end: " << ToCmpSecondStr(test_write_cq_[req_id_].end_time_)
                << endl;

            ++mix_numb_;
            return;
        }
        sum_write_cq_time_ += cur_cost_time;

        // cout << "Complete Write CQ: " << req_id_ << " cost: " << cur_cost_time << " micros " << " sum cost " << sum_write_cq_time_ 
        //      << "\n" << endl;

        if (++cmp_write_count == CONFIG->get_test_count()-mix_numb_)
        {
            cout << "\n[CQ]Complete " << CONFIG->get_test_count() << " write request cost " 
                << sum_write_cq_time_  << " micros" 
                << " ave: " << sum_write_cq_time_ / CONFIG->get_test_count() << " micros"
                << endl;
        }

        if (!cached_request_data_.empty())
        {
            Fruit* first_data = cached_request_data_.front();
            cached_request_data_.pop_front();

            // cout << "Restart Add Cached Data: " << first_data->request_id << endl;
            add_data(first_data);
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
        std::lock_guard<std::mutex> lk(mutex_);
        
        last_cq_msg = "Get Full Response Data";

        responder_->Read(&reply, this);
        if (reply.time().length() == 0)
        {
            last_cq_msg = "Get Empty Response Data";
            cout << "[W] Empty Response" << endl;
            return;
        }
        else
        {
            string rsp_message = reply.message();
            if (!is_connected_ && rsp_message == "connected")
            {
                cout << "[SERVER]:"
                        << "session_id= " << reply.session_id() 
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
                cout << "[SERVER]:"
                        << "session_id= " << reply.session_id() 
                        << ", rpc=" << rpc_id_
                        << ", rsp_id="<< reply.response_id()
                        << ", time=" << reply.time() 
                        << "\n"
                        << endl;

                long rsp_id = std::stol(reply.response_id());

                if (rsp_id == CONFIG->get_test_count())
                {
                    test_end_time_ = NanoTime();

                    cout << "[R] Complete " << CONFIG->get_test_count() << " request cost: " 
                        << (test_end_time_ - test_start_time_)/1000 << " microsecs" << endl;
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
        request.set_name(name);
        request.set_time(time);
        request.set_obj_id(std::to_string(obj_id_));
        request.set_message("login");

        cout << "login: obj_id=" << obj_id_ << ", session_id= " << request.session_id() 
                << " , name=" << request.name() 
                << " , time=" << request.time()
                << " , obj_id=" << obj_id_
                << " , msg=" << request.message()
                << endl;            

        int sleep_secs = 3;
        // cout << "sleep for " << sleep_secs << " secs " << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

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

void ClientApplePRC::reconnect()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        cout << "Client " << session_id_ << " Start Reconnect!" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));

        spawn();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

BaseRPCParam ClientApplePRC::release()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        BaseRPCParam result;

        result.channel = channel_;
        result.cq = cq_;

        cout << "\n********* ClientApplePRC::release id = " << obj_id_ << " ********\n"<< endl;
        if (!is_released_)
        {
            is_released_ = true;
            delete this;
        }
        else
        {
            cout << "[E] ClientApplePRC::release id=" << obj_id_ << " has been Released!!! " << endl;
        } 

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::release " << e.what() << '\n';
    }
}

void ClientApplePRC::add_data(Fruit* data)
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
            cached_request_data_.push_back(data);
            // cout << "Last Write Was not Finished!" << endl;
            // cout << "Last Write Was not Finished! Cached Data " << data->request_id << endl;
            return;
        }
        
        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        Apple* real_data = (Apple*)(data);

        TestRequest request;

        request.set_session_id(session_id_);
        request.set_name(real_data->name);
        request.set_time(real_data->time);
        request.set_obj_id(std::to_string(obj_id_));
        request.set_rpc_id(rpc_id_);
        // request.set_request_id(std::to_string(++req_id_));

        request.set_request_id(std::to_string(real_data->request_id));

        // cout << "[CLIENT]:"
        //         << "session_id= " << request.session_id() 
        //         << ", rpc=" << rpc_id_
        //         << ", req_id=" << real_data->request_id
        //         << ", time=" << request.time()                
        //         << endl;            

        int sleep_secs = 3;

        // cout << "sleep for " << sleep_secs << " secs " << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;
        is_request_data_updated_ = true;

        if (real_data->request_id == 1)
        {
            test_start_time_ = NanoTime();
        }

        TestTime cur_test_write_cq_;
        cur_test_write_cq_.start_time_ = NanoTime();
        req_id_ = real_data->request_id;

        test_write_cq_[req_id_] = cur_test_write_cq_;

        responder_->Write(request, this);

        delete data;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}