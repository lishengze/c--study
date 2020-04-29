#include "simple_test.h"
#include <thread>
#include <chrono>
#include <vector>
#include <zmq.h>
#include <iostream>
#include <memory>
#include <zhelpers.hpp>
#include <string>
#include "basic_func.h"

using std::cout;
using std::endl;
using std::thread;
using std::vector;
using std::shared_ptr;



void req_worker(void* context, const char* ipc_address, int work_numb)
{
    
    // void* req_socket = zmq_socket(context, ZMQ_REQ);

    // void* req_socket = zmq_socket(context, ZMQ_DEALER);

    void* req_socket = zmq_socket(context, ZMQ_PAIR);

    zmq_connect(req_socket, ipc_address);


    for(int i = 0;  i < 10; ++i)
    {
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }   

        s_send_u(req_socket, (std::to_string(work_numb) + "_" + std::to_string(i)).c_str());

        cout << "Client_" << work_numb << " send: " << std::to_string(work_numb) + "_" + std::to_string(i) << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        string result = s_recv_u(req_socket);

        cout << "Client_" << work_numb << " receive: " << result << endl;
    }

    zmq_close(req_socket);
    cout << "Client_" << work_numb << " End" << endl;    
}

void rep_worker(void* context, const char* ipc_address)
{

    // void* rep_socket = zmq_socket(context, ZMQ_ROUTER);

    // void* rep_socket = zmq_socket(context, ZMQ_REP);

    void* rep_socket = zmq_socket(context, ZMQ_PAIR);

    zmq_bind(rep_socket, ipc_address);

    for(int i = 0;  i < 10; ++i)
    {       
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }    

        string result = s_recv_u(rep_socket);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        cout << "Server Receive： " << result << endl;

        s_send_u(rep_socket, std::to_string(i).c_str());

        cout << "Server Send: " << std::to_string(i) << endl;
    }
   
    zmq_close(rep_socket);
    
    cout << "Server End"  << endl;    
}

// 测试最简单的 req, rep 模式， 一一对应的链接;
void test_req_rep()
{
    s_catch_signals();
    void* context = zmq_init(1);    
    const char* ipc_address = "ipc://test_req_rep.ipc";
    int req_worker_numb = 5;
    vector<shared_ptr<thread>> thread_vec;

    for (int i = 0; i < req_worker_numb; ++i)
    {
        thread_vec.push_back(std::make_shared<thread>(req_worker, context, ipc_address, i));
    }

    // thread req_thread = thread(req_worker, ipc_address);
    thread rep_thread = thread(rep_worker, context, ipc_address);

    if (rep_thread.joinable())
    {
        rep_thread.join();
    }    

    for (auto req_thread:thread_vec)
    {
        if (req_thread->joinable())
        {
            req_thread->join();
        }
    }

    zmq_term(context);
}


void sub_worker(void* context, const char* ipc_address, int work_numb)
{
    void* req_socket = zmq_socket(context, ZMQ_SUB);
    zmq_setsockopt(req_socket, ZMQ_SUBSCRIBE, "", 0);
    zmq_connect(req_socket, ipc_address);


    s_catch_signals();
    for(int i = 0;  i < 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }   

        string result = s_recv_u(req_socket);

        cout << "Subscriber_" << work_numb << " receive: " << result << endl;     
    }

    zmq_close(req_socket);  
    cout << "Subscriber_" << work_numb << " End" << endl;    
}

void pub_worker(void* context, const char* ipc_address)
{
    void* rep_socket = zmq_socket(context, ZMQ_PUB);
    zmq_bind(rep_socket, ipc_address);

    for(int i = 0;  i < 10; ++i)
    {       
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }    

        s_send_u(rep_socket, std::to_string(i).c_str());

        cout << "Publisher Send: " << std::to_string(i) << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));        
    }
   
    zmq_close(rep_socket);
    

    cout << "Publisher End"  << endl;    
}

void test_pub_sub()
{
    void* context = zmq_init(1);
    const char* ipc_address = "ipc://test.ipc";
    int sub_worker_numb = 5;
    vector<shared_ptr<thread>> thread_vec;

    for (int i = 0; i < sub_worker_numb; ++i)
    {
        thread_vec.push_back(std::make_shared<thread>(sub_worker, context, ipc_address, i));
    }

    thread pub_thread = thread(pub_worker, context, ipc_address);

    if (pub_thread.joinable())
    {
        pub_thread.join();
    }    

    for (auto req_thread:thread_vec)
    {
        if (req_thread->joinable())
        {
            req_thread->join();
        }
    }

    zmq_term(context);    
}

void task_dispacher(void* context, const char* ipc_address)
{
    void* dispacher_socket = zmq_socket(context, ZMQ_PUSH);

    zmq_bind(dispacher_socket, ipc_address);


    s_catch_signals();
    
    for(int i = 0;  i < 10; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }   

        s_send_u(dispacher_socket, std::to_string(i).c_str());

        cout << "Dispacher Send " << std::to_string(i) << endl;     
    }

    zmq_close(dispacher_socket);

    cout << "Dispacher End"  << endl;     
}

void task_worker(void* context, const char* task_dispacher_ipc_address, const char*  task_collector_ipc_address, int work_numb)
{
    void* task_worker_recv_socket = zmq_socket(context, ZMQ_PULL);
    zmq_connect(task_worker_recv_socket, task_dispacher_ipc_address);

    void* task_worker_send_socket = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(task_worker_send_socket, task_collector_ipc_address);

    for(int i = 0;  i < 10; ++i)
    {       
        
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }    

        string result = s_recv_u(task_worker_recv_socket);

        cout << "Worker_" << work_numb << " receive: " << result << endl;         

        string send_str = std::to_string(work_numb) + "_" + result;
        s_send_u(task_worker_send_socket, send_str.c_str());

        cout << "Worker_" << work_numb << " send: " << send_str << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));        
    }
   
    zmq_close(task_worker_recv_socket);
    zmq_close(task_worker_send_socket);
    cout << "Publisher End"  << endl;    
}

void task_collector(void* context, const char* ipc_address)
{
    void* task_collector_socket = zmq_socket(context, ZMQ_PULL);
    zmq_bind(task_collector_socket, ipc_address);

    for(int i = 0;  i < 10; ++i)
    {       
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }    

        string result = s_recv_u(task_collector_socket);

        cout << "Collector Receive: " << result << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));        
    }
   
    zmq_close(task_collector_socket);
    

    cout << "Publisher End"  << endl;    
}

void test_push_pull()
{
    void* context = zmq_init(1);
    const char* dispacher_ipc_address = "ipc://dispachter.ipc";
    const char* collector_ipc_address = "ipc://collector.ipc";


    int worker_numb = 5;
    vector<shared_ptr<thread>> thread_vec;

    for (int i = 0; i < worker_numb; ++i)
    {
        thread_vec.push_back(std::make_shared<thread>(task_worker, context, dispacher_ipc_address, collector_ipc_address, i));
    }

    thread dispacher_thread = thread(task_dispacher, context, dispacher_ipc_address);
    thread collector_thread = thread(task_collector, context, collector_ipc_address);

    if (dispacher_thread.joinable())
    {
        dispacher_thread.join();
    }    

    if (collector_thread.joinable())
    {
        collector_thread.join();
    }        

    for (auto work_thread:thread_vec)
    {
        if (work_thread->joinable())
        {
            work_thread->join();
        }
    }

    zmq_term(context);
}


void requester(void* context, const char* router_ipc_address, int work_number)
{
    // void* context = zmq_init(1);
    void* req_socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(req_socket, router_ipc_address);

    for (int i=0; i<10; ++i)
    {
        cout << "Requester_" << std::to_string(work_number) << " Send: " << std::to_string(i) << endl;

        s_send_u(req_socket, (std::to_string(work_number) + "_" +std::to_string(i)).c_str());

        std::this_thread::sleep_for(std::chrono::seconds(1));

        string result = s_recv_u(req_socket);

        cout << "Requester_" << std::to_string(work_number) << " Recv: " << result << endl;
    }

    zmq_close(req_socket);
    // zmq_term(context);
}

void responsor(void* context, const char* dealer_ipc_address, int work_numb)
{
    // void* context = zmq_init(1);
    void* rep_socket = zmq_socket(context, ZMQ_REP);
    zmq_connect(rep_socket, dealer_ipc_address);

    for(int i = 0;  i < 10; ++i)
    {       
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }    

        string result = s_recv_u(rep_socket);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        cout << "Server_" << std::to_string(work_numb) << " Recv: "<< result << endl;

        s_send_u(rep_socket, (std::to_string(work_numb) + "_" +std::to_string(i)).c_str());

        cout << "Server_" << std::to_string(work_numb) << " Send: " << std::to_string(i) << endl;
    }
   
    zmq_close(rep_socket);
    // zmq_term(context);
    cout << "Server End"  << endl;  
}

void broker_worker(void* context, const char* router_ipc_address, const char* dealer_ipc_address)
{
    // void* context = zmq_init(1);
    void* router_socket = zmq_socket(context, ZMQ_ROUTER);
    void* dealer_socket = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(router_socket, router_ipc_address);
    zmq_bind(dealer_socket, dealer_ipc_address);

    // zmq_device (ZMQ_QUEUE, router_socket, dealer_socket);

    // while (true)
    // {
    //     if (s_interrupted) {
    //         printf ("W: 收到中断消息，程序中止...\n");
    //         break;
    //     }   

    //     std::this_thread::sleep_for(std::chrono::seconds(1));

    //     string req_rst = s_recv_u(router_socket);


    //     cout << "Router From Requester: " << req_rst << endl;


    //     s_send_u(dealer_socket, req_rst.c_str());

    //     string rep_rst = s_recv_u(dealer_socket);

    //     cout << "Dealer From Responser: " << req_rst << endl;

    //     s_send_u(router_socket, rep_rst.c_str());
    // }

    //  初始化轮询对象
    zmq_pollitem_t items [] = {
        { router_socket, 0, ZMQ_POLLIN, 0 },
        { dealer_socket, 0, ZMQ_POLLIN, 0 }
    };

    while (true)
    {
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        }   

        zmq_msg_t message;
        int more;

        zmq_poll (items, 2, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            while (false)
            {
                // cout << "Router ZMQ_POLLIN" << endl;
                zmq_msg_init (&message);
                zmq_msg_recv (&message, router_socket, 0);

                string req_rst = trans_zmq_msg(message);
                cout << "Router From Requester: " << req_rst << endl;     

                size_t more_size = sizeof (more);
                zmq_getsockopt (router_socket, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send (&message, dealer_socket, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break;      //  最后一帧
            }

            // string req_rst = s_recvmore(router_socket);
            // cout << "Router From Requester: " << req_rst << endl;
            // s_send_u(dealer_socket, req_rst.c_str());
        }

        if (items[1].revents & ZMQ_POLLIN)
        {
            // while (true)
            // {
            //     cout << "Dealer ZMQ_POLLIN" << endl;
            //     zmq_msg_init (&message);
            //     zmq_msg_recv (&message, dealer_socket, 0);

            //     string rep_rst = trans_zmq_msg(message);
            //     cout << "Dealer From Responser: " << rep_rst << endl;     
                
            //     size_t more_size = sizeof (more);
            //     zmq_getsockopt (dealer_socket, ZMQ_RCVMORE, &more, &more_size);
            //     zmq_msg_send (&message, router_socket, more? ZMQ_SNDMORE: 0);
            //     zmq_msg_close (&message);
            //     if (!more)
            //         break;      //  最后一帧
            // }

            string rep_rst = s_recvmore(dealer_socket);
            cout << "Dealer From Responser: " << rep_rst << endl; 
            s_send_u(router_socket, rep_rst.c_str());
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }    

    zmq_close(router_socket);
    zmq_close(dealer_socket);
}

void test_router_dealer_broker()
{
    void* context = zmq_init(1);
    const char* router_ipc_address = "ipc://router.ipc";
    const char* dealer_ipc_address = "ipc://dealer.ipc";

    int requester_numb = 2;
    int responser_numb = 1;

    vector<std::shared_ptr<thread>> requester_thread_vec;
    vector<std::shared_ptr<thread>> responser_thread_vec;

    thread broker_thread = thread(broker_worker, context, router_ipc_address, dealer_ipc_address);

    for (int i=0; i < requester_numb; ++i)
    {
        requester_thread_vec.push_back(std::make_shared<thread>(requester, context, router_ipc_address, i));
    }

    for (int i=0; i < responser_numb; ++i)
    {
        responser_thread_vec.push_back(std::make_shared<thread>(responsor, context, dealer_ipc_address, i));
    }

    if (broker_thread.joinable())
    {
        broker_thread.join();
    }

    for (auto requester_thread:requester_thread_vec)
    {
        if (requester_thread->joinable())
        {
            requester_thread->join();
        }
    }

    for (auto responser_thread:responser_thread_vec)
    {
        if (responser_thread->joinable())
        {
            responser_thread->join();
        }        
    }

    zmq_term(context);
}

void dealer_work(void* context, const char* ipc_address, int work_numb)
{
    void* dealer_socket = zmq_socket(context, ZMQ_DEALER);

    zmq_connect(dealer_socket, ipc_address);

    zmq_pollitem_t items[] = {{dealer_socket, 0, ZMQ_POLLIN, 0}};

    for(int i = 0;  i < 10; ++i)
    {
        if (s_interrupted) {
            printf ("收到中断消息，程序中止...\n");
            break;
        } 

        string send_str = std::to_string(work_numb) + "_" + std::to_string(i);
        s_send_u(dealer_socket, send_str.c_str());
        cout << "Dealer_" << work_numb << " Send: " << send_str << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        zmq_msg_t message;
        int more;
        zmq_poll(items, 1, 0);
        if (items[0].revents & ZMQ_POLLIN)
        {
            while (true)
            {
                zmq_msg_init (&message);
                zmq_msg_recv (&message, dealer_socket, 0);

                string req_rst = trans_zmq_msg(message);
                cout <<  "Dealer_" << work_numb << " Recv: " << req_rst << endl;     

                size_t more_size = sizeof (more);
                zmq_getsockopt (dealer_socket, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send (&message, dealer_socket, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break;      //  最后一帧
            }
        }
    }

    // for(int i = 0;  i < 10; ++i)
    // {
    //     if (s_interrupted) {
    //         printf ("W: 收到中断消息，程序中止...\n");
    //         break;
    //     }   

    //     s_send_u(dealer_socket, (std::to_string(work_numb) + "_" + std::to_string(i)).c_str());

    //     cout << "Client_" << work_numb << " send: " << std::to_string(work_numb) + "_" + std::to_string(i) << endl;
    //     std::this_thread::sleep_for(std::chrono::seconds(1));

    //     string result = s_recv_u(dealer_socket);

    //     cout << "Client_" << work_numb << " receive: " << result << endl;
    // }

    zmq_close(dealer_socket);
    cout << "Dealer_" << work_numb << " End" << endl;    
}

void router_work(void* context, const char* ipc_address)
{

    void* router_socket = zmq_socket(context, ZMQ_ROUTER);

    zmq_bind(router_socket, ipc_address);

    zmq_pollitem_t items[] = {{router_socket, 0, ZMQ_POLLIN, 0}};

    while (true)
    {
        if (s_interrupted) {
            printf ("W: 收到中断消息，程序中止...\n");
            break;
        } 

        zmq_msg_t message;
        int more;
        zmq_poll(items, 1, -1);
        if (items[0].revents & ZMQ_POLLIN)
        {
            while (true)
            {
                zmq_msg_init (&message);
                zmq_msg_recv (&message, router_socket, 0);

                string req_rst = trans_zmq_msg(message);
                cout << "Router From Dealer: " << req_rst << endl;     

                size_t more_size = sizeof (more);
                zmq_getsockopt (router_socket, ZMQ_RCVMORE, &more, &more_size);
                zmq_msg_send (&message, router_socket, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break;      //  最后一帧
            }
        }
    }

    // for(int i = 0;  i < 10; ++i)
    // {       
    //     if (s_interrupted) {
    //         printf ("W: 收到中断消息，程序中止...\n");
    //         break;
    //     }    

    //     string result = s_recv_u(router_socket);
        
    //     std::this_thread::sleep_for(std::chrono::seconds(1));

    //     cout << "Server Receive： " << result << endl;

    //     s_send_u(router_socket, std::to_string(i).c_str());

    //     cout << "Server Send: " << std::to_string(i) << endl;
    // }
   
    zmq_close(router_socket);
    
    cout << "Server End"  << endl;    
}

void test_router_dealer_direct_connect()
{
    s_catch_signals();
    void* context = zmq_init(1);    
    const char* ipc_address = "ipc://test_router_dealer_direct_connect.ipc";
    int dealer_numb = 2;
    vector<shared_ptr<thread>> dealer_vec;

    for (int i = 0; i<dealer_numb; ++i)
    {
        dealer_vec.push_back(std::make_shared<thread>(dealer_work, context, ipc_address, i));
    }

    thread router_thread = thread(router_work, context, ipc_address);

    if (router_thread.joinable())
    {
        router_thread.join();
    }    

    for (auto dealer_thread:dealer_vec)
    {
        if (dealer_thread->joinable())
        {
            dealer_thread->join();
        }
    }

    zmq_term(context);
}

void simple_test()
{
    // test_req_rep();

    // test_pub_sub();

    // test_push_pull();

    // test_router_dealer_broker();    

    test_router_dealer_direct_connect();
}