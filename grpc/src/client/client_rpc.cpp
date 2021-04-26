#include "client_rpc.h"


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
        std::lock_guard<std::mutex> lk(mutex_);

        cout << "\nClientBaseRPC::process " << endl;

        if (status_ == CREATE)
        {
            cout << "Status is CREATE " << endl;

            init_request();

            status_ = PROCESS;
        }
        else if (status_ == PROCESS)
        {
            cout << "Status is PROCESS " << endl;

            if (is_first_)
            {
                is_first_ = false;

                // spawn();
            }

            procceed();

            // status_ = FINISH;
        }
        else if (status_ == FINISH)
        {
            
            cout << "Current Request IS Over" << endl;

            status_ = PROCESS;

            // make_active();
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
       cout << " ClientBaseRPC::release " << endl;
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

        // std::cout << "Create A New Client For Request! " << std::endl;

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
        is_request_data_updated_ = true;

        is_rsp_init_ = false;

        cout << "ClientApplePRC::init_request " << endl;

        responder_ = stub_->PrepareAsyncServerStreamApple(&context_, cq_);

        // responder_ = stub_->AsyncServerStreamApple(&context_, cq_, this);

        // stub_->ServerStreamApple(&context_);

        // responder_->Write(request, this);

        // responder_->WritesDone(this);

        responder_->StartCall(this);

        last_cq_msg = "StartCall";

        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // responder_->Write(request, this);

        // std::this_thread::sleep_for(std::chrono::seconds(2));


        // make_active();

        // responder_->StartCall(this);

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

void ClientApplePRC::write_msg()
{
    try
    {
        cout << "ClientApplePRC::write_msg " << endl;

        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        request_.set_session_id(session_id_);
        request_.set_name(name);
        request_.set_time(time);
        request_.set_obj_id(std::to_string(obj_id_));

        cout << "Request: obj_id=" << obj_id_ << ", session_id= " << request_.session_id() 
                << " , name=" << request_.name() 
                << " , time=" << request_.time()
                << endl;            

        // responder_ = stub_->AsyncServerStreamApple(&context_, cq_, this);

        int sleep_secs = 3;
        cout << "sleep for " << sleep_secs << " secs " << endl;
        std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;

        responder_->Write(request_, this);
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
        // cout << "ClientApplePRC::procceed " << endl;

        if (is_start_call_)
        {
            is_start_call_ = false;
            cout << "Is Start Call " << endl;
        }
        /* request new data */
        else if (is_request_data_updated_)
        {
            cout << "First Request Data Come!" << endl;
            cout << "last_cq_msg: " << last_cq_msg << endl;

            is_request_data_updated_ = false;

            write_msg();

            last_cq_msg = "First Request Data ";
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
            
            responder_->Read(&reply_, this);

            if (reply_.session_id().length() == 0)
            {
                last_cq_msg = "Get Empty Response Data";
                cout << "[W] Empty Response" << endl;
                return;
            }

            cout << "From Server: obj_id = " << reply_.obj_id() 
                    << ", session_id= " << reply_.session_id() 
                    << " , name=" << reply_.name() 
                    << " , time=" << reply_.time()
                    << endl;
            last_cq_msg = "Get Full Response Data";

            write_msg();     

            // make_active();   
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
        cout << "Client " << session_id_ << " Start Reconnect!" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));

        spawn();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void ClientApplePRC::release()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

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
        cout << "\n+++++++ ClientApplePRC::add_data ++++++++++" << endl;
        string name = "ClientApplePRC";
        string time = NanoTimeStr();

        Apple* real_data = (Apple*)(data);

        request_.set_session_id(session_id_);
        request_.set_name(real_data->name);
        request_.set_time(real_data->time);
        request_.set_obj_id(std::to_string(obj_id_));
        request_.set_rpc_id(rpc_id_);

        cout << "Request: obj_id=" << obj_id_ << ", session_id= " << request_.session_id() 
                << " , name=" << request_.name() 
                << " , time=" << request_.time()
                << endl;            

        // responder_ = stub_->AsyncServerStreamApple(&context_, cq_, this);

        int sleep_secs = 3;
        cout << "sleep for " << sleep_secs << " secs " << endl;
        std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));

        is_write_cq_ = true;
        is_request_data_updated_ = true;

        responder_->Write(request_, this);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}