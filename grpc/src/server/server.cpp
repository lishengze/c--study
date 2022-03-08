#include "server.h"

#include "boost/make_shared.hpp"

#include "../include/time_util.h"

#include "../include/global_declare.h"

#include "rpc.h"

#include "server_spi.h"

#include "trade_engine.h"


// ServerSpi*  BaseServer::server_spi_ = nullptr;

BaseServer::~BaseServer()
{
    cout << "\n~BaseServer" << endl;

    if (cq_thread_ && cq_thread_->joinable())
    {
        cq_thread_->join();
    }
}

void BaseServer::start()
{
    try
    {
        cout << "BaseServer listen: " << address_ << endl;

        builder_.AddListeningPort(address_, grpc::InsecureServerCredentials());
        builder_.RegisterService(&service_);

        cq_ = builder_.AddCompletionQueue();
        server_ = builder_.BuildAndStart();

        simple_rpc_ = new TestSimpleRPC(&service_, cq_.get());
        simple_rpc_->register_server(this);
        simple_rpc_->process();

        // server_stream_rpc = new ServerStreamRPC(&service_, cq_.get());

        // server_stream_apple_ = new ServerStreamAppleRPC(&service_, cq_.get());
        // server_stream_apple_->register_server(this);
        // server_stream_apple_->process();

        // double_stream_apple_ = new DoubleStreamAppleRPC(&service_, cq_.get());
        // double_stream_apple_->register_server(this);
        // double_stream_apple_->process();

        // server_stream_pear_ = new ServerStreamPearRPC(&service_, cq_.get());
        // server_stream_pear_->register_server(this);

        // server_stream_mango_ = new ServerStreamMangoRPC(&service_, cq_.get());
        // server_stream_mango_->register_server(this);

        init_cq_thread();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::register_spi(ServerSpi* server_spi)
{
    try
    {
        server_spi_ = server_spi;

        server_spi_vptb_ = *(int *)server_spi;

        // cout << "server_spi class: " << server_spi_->get_class_name()         
        //      << " ori_vp: " << *(int *)server_spi_ 
        //     //  << ", " << sizeof(server_spi_) << ", " <<  static_cast<void *>(server_spi_) 
        //      << endl;       
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::register_spi(ServerSpiPtr server_spi)
{
    try
    {
        // server_spi_ = server_spi;

        cout << "server_spi class: " << server_spi_->get_class_name() 
            //  << ", " << sizeof(server_spi_) << ", " <<  static_cast<void *>(server_spi_) 
             << endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::init_cq_thread()
{
    // cout << "[init_cq_thread] cq loop server_spi class: " << server_spi_->get_class_name() 
    //      << " thread_id: " << std::this_thread::get_id() 
    //      << " server_spi: " << server_spi_
    //      << " vp: " << *(int *)server_spi_ 
    //      << endl;

    cq_thread_ = boost::make_shared<std::thread>(&BaseServer::run_cq_loop, this);

    cq_thread_->join();

    // int test_sleep_secs = 10;
    // std::cout << "\n\n****** BaseServer::init_cq_thread sleep " << test_sleep_secs << " secs " << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(test_sleep_secs));
    // std::cout << "\n***** SLEEP END!" << endl;

    // run_cq_loop();
}

void BaseServer::set_rpc_map(SessionType session_id_, RpcType rpc_id_, BaseRPC* rpc)
{
    try
    {
        rpc_map_[session_id_][rpc_id_] = rpc;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::run_cq_loop()
{
    try
    {
        // cout << "[CQ Start]: " << get_spi()->get_class_name() 
        //      << " thread_id: " << std::this_thread::get_id() 
        //      << " server_spi: " << server_spi_
        //      << " vp: " << *(int *)server_spi_ 
        //      << endl;

        get_spi();

        void* tag;
        bool status;
        while(true)
        {
            // std::cout << "\n++++++++ Loop Start " << " ++++++++"<< std::endl;

            bool result = cq_->Next(&tag, &status);

            std::lock_guard<std::mutex> lk(cq_mutex_);

            // cout << "cq loop server_spi class: " << server_spi_->get_class_name() 
            //      << " thread_id: " << std::this_thread::get_id() 
            //      << " vp: " << *(int *)server_spi_ 
            //      << endl;            

            BaseRPC* rpc = static_cast<BaseRPC*>(tag);

            if (dead_rpc_set_.find(rpc) != dead_rpc_set_.end())
            {
                cout << "[E] RPC id=" << rpc->obj_id_ << " has been released!" << endl;

                std::cout << "[E][CQ] result: "<<  result << " status: " << status  
                          << ", session_id_=" << rpc->session_id_ 
                          << ", rpc_id_=" << rpc->rpc_id_ 
                          << ", obj_id: " << rpc->obj_id_ 
                          << std::endl;

                continue;
            }

            if (rpc->is_stream_)
            {

                check_dead_rpc(rpc);

                if (result && status)
                {                
                    rpc->process();
                }
                else
                {
                    record_dead_rpc(rpc);

                    reconnect(rpc);
                }
            }
            else
            {
                if (result && status)
                {                
                    rpc->process();
                }
                else
                {
                    reconnect(rpc);
                }                
            }

            // std::cout << "[E] result: "<<  result << " status: " << status  << ", session_id_=" << rpc->session_id_ << ", rpc_id_=" << rpc->rpc_id_ << ", obj_id: " << rpc->obj_id_ << std::endl;

        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "BaseServer::run_cq_loop " << e.what() << '\n';
    }
    catch(...)
    {
        cout << "BaseServer::run_cq_loop unkonwn exceptions" << endl;
    }    

}

void BaseServer::record_dead_rpc(BaseRPC* rpc)
{
    try
    {
        dead_rpc_set_.emplace(rpc);

        rpc->set_disconnect_time(NanoTime());
        
        if (rpc->session_id_.length() != 0)
        {
            if (rpc_map_.find(rpc->session_id_) != rpc_map_.end() 
            && rpc_map_[rpc->session_id_].find(rpc->rpc_id_) != rpc_map_[rpc->session_id_].end())
            {
                rpc_map_[rpc->session_id_][rpc->rpc_id_] = nullptr;
            }

            wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] = rpc;
        }       
    }
    catch(const std::exception& e)
    {
        std::cerr <<"\n[E] BaseServer::record_dead_rpc " << e.what() << '\n';
    }

}

void BaseServer::reconnect(BaseRPC* rpc)
{
    try
    {

        if (!rpc->is_stream_)
        {
            delete rpc;
        }

        BaseRPC* new_rpc = rpc->spawn();

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        new_rpc->process();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] BaseServer::reconnect " << e.what() << '\n';
    }
    
}

void BaseServer::check_dead_rpc(BaseRPC* rpc)
{
    try
    {
        // std::cout << " BaseServer::check_dead_rpc  " << std::endl;

        if (rpc->session_id_.length() != 0)
        {
            if (!dead_rpc_set_.empty() 
            && wait_to_release_rpc_map_.find(rpc->rpc_id_) != wait_to_release_rpc_map_.end()
            && wait_to_release_rpc_map_[rpc->rpc_id_].find(rpc->session_id_) != wait_to_release_rpc_map_[rpc->rpc_id_].end()
            && wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_])
            {
                if (wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] != rpc)
                {

                    BaseRPC* release_rpc = wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_];
                    dead_rpc_set_.erase(release_rpc);

                    cout << "WaitToRelease session_id_=" << rpc->session_id_ << ", rpc_id_=" << rpc->rpc_id_ << ", dead obj_id=" << release_rpc->obj_id_
                        << ", new obj_id=" << rpc->obj_id_ << endl;
                    release_rpc->release();

                    wait_to_release_rpc_map_[rpc->rpc_id_][rpc->session_id_] = nullptr;
                }
            }
        }

        long cur_time = NanoTime() / NANOSECONDS_PER_SECOND;

        list<BaseRPC*> dead_rpc_list_;

        for (BaseRPC* dead_rpc:dead_rpc_set_)
        {
            if (dead_rpc->session_id_.length() == 0 
            && (cur_time - dead_rpc->disconnect_time_) > wait_to_release_time_secs_)
            {
                cout << "Rpc: rpc_id: " << dead_rpc->rpc_id_ 
                     << ", connect_time: " << ToSecondStr(dead_rpc->disconnect_time_)
                     << ", dead_time: " << ToSecondStr(cur_time) << endl;

                dead_rpc_list_.push_back(dead_rpc);
            }
        }

        for (BaseRPC* dead_rpc:dead_rpc_list_)
        {
            dead_rpc->release();
            dead_rpc_set_.erase(dead_rpc);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void BaseServer::response(PackagePtr pkg)
{
    try
    {
        if (rpc_map_.find(pkg->SessionID()) != rpc_map_.end())
        {
            if (rpc_map_[pkg->SessionID()].find(pkg->RpcID()) != rpc_map_[pkg->SessionID()].end())
            {
                rpc_map_[pkg->SessionID()][pkg->RpcID()]->response(pkg);
            }
            else
            {
                std::cout << "\n[Warning] rpc_map_ does not have rpc_id: " << pkg->RpcID() << endl;
            }            
        }
        else
        {
            std::cout << "\n[Warning] rpc_map_ does not have session_id: " << pkg->SessionID() << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] BaseServer::response" << e.what() << '\n';
    }

}

void BaseServer::on_req_server_apple(PackagePtr pkg)
{
    try
    {
        if (server_spi_)
        {
            server_spi_->on_req_server_apple(pkg);
        }
        else
        {
            cout << "BaseServer::on_req_server_apple server_spi_ is null" << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void BaseServer::on_req_double_apple(PackagePtr pkg)
{
    try
    {
        if (server_spi_)
        {
            server_spi_->on_req_double_apple(pkg);
        }
        else
        {
            cout << "BaseServer::on_req_double_apple server_spi_ is null" << endl;
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void SyncServer::start()
{
    try
    {
        // SynacService service;

        // ServerBuilder builder;
        // builder.AddListeningPort(address_, grpc::InsecureServerCredentials());
        // builder.RegisterService(&service);
        // std::unique_ptr<Server> server(builder.BuildAndStart());
        // std::cout << "Server listening on " << address_ << std::endl;
        // server->Wait();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] SyncServer::start() " << e.what() << '\n';
    }
    
}