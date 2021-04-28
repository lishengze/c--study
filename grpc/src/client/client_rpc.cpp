#include "client_rpc.h"
#include "client.h"

int ClientBaseRPC::obj_count_ = 0;

void ClientBaseRPC::make_active() 
{
    try
    {
        std::cout << " ClientBaseRPC make active " << std::endl;
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

            init_request();

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

void ClientApplePRC::init_request()
{
    try
    {
        is_rsp_init_ = false;

        cout << "ClientApplePRC::init_request " << endl;

        responder_ = stub_->PrepareAsyncServerStreamApple(&context_, cq_);

        responder_->StartCall(this);

        last_cq_msg = "StartCall";

    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientBaseRPC::init_request " <<  e.what() << '\n';
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
        std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

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
        std::lock_guard<std::mutex> lk(mutex_);

        cout << "ClientApplePRC::procceed " << endl;

        if (is_start_call_)
        {
            is_start_call_ = false;
            cout << "Is Start Call " << endl;
            last_cq_msg = "Is Start Call";
            is_connected_ = true;
            if (!is_rsp_init_)
            {
                make_active();

                is_rsp_init_ = true;
            }   
        }
        else if (is_write_cq_)
        {
            cout << "last_cq_msg: " << last_cq_msg << endl;
            is_write_cq_ = false;
            cout << "This is Write_CQ" << endl;
            last_cq_msg = "This is Write_CQ";

            // std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!is_rsp_init_)
            {
                make_active();

                is_rsp_init_ = true;
            }            
        }
        // New Response Data Coming!
        else
        {
            cout << "last_cq_msg: " << last_cq_msg << endl;

            // TestResponse    reply;
            
            responder_->Read(&reply, this);

            if (reply.response_id().length() == 0)
            {
                last_cq_msg = "Get Empty Response Data";
                cout << "[W] Empty Response" << endl;
                return;
            }

            cout << "\n[SERVER]: obj_id = " << reply.obj_id() 
                    << ", session_id= " << reply.session_id() 
                    << " , name=" << reply.name() 
                    << " , time=" << reply.time()
                    << " , rsp_id="<< reply.response_id()
                    << endl;
            last_cq_msg = "Get Full Response Data";
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] ClientApplePRC::procceed " <<  e.what() << '\n';
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



        cout << "\n+++++++ ClientApplePRC::add_data ++++++++++" << endl;
        std::lock_guard<std::mutex> lk(mutex_);

        if (!is_connected_)
        {
            cout << "[E] rpc_id: " << rpc_id_ << ", obj_id: " << obj_id_  << " Server was not Connected " << endl;
            return;
        }

        if (is_released_)
        {
            cout << "[E] rpc_id: " << rpc_id_ << ", obj_id: " << obj_id_  <<" RPC was released " << endl;
            return;
        }


        
        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        Apple* real_data = (Apple*)(data);

        request.set_session_id(session_id_);
        request.set_name(real_data->name);
        request.set_time(real_data->time);
        request.set_obj_id(std::to_string(obj_id_));
        request.set_rpc_id(rpc_id_);
        request.set_request_id(std::to_string(++req_id_));

        cout << "[CLIENT]: obj_id=" << obj_id_ 
                << ", session_id= " << request.session_id() 
                << ", name=" << request.name() 
                << ", time=" << request.time()
                << ", req_id_=" << req_id_
                << endl;            

        int sleep_secs = 3;

        // cout << "sleep for " << sleep_secs << " secs " << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;
        is_request_data_updated_ = true;

        responder_->Write(request, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}