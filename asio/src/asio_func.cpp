#include "asio_func.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "asio.hpp"
// using namespace asio;
using std::cout;
using std::endl;

typedef std::shared_ptr<asio::ip::tcp::socket> g_socket_ptr;

asio::io_service g_asio_ios;

void simple_client()
{
    cout << "Test Simple Client" << endl;
    asio::io_service service;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 2001);
    asio::ip::tcp::socket socket(service);
    socket.connect(ep);
}

void simple_server()
{
    cout << "Test Simple Server" << endl;
    asio::io_service service;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 2001);
    asio::ip::tcp::acceptor acc(service, ep);

    while (true)
    {
        g_socket_ptr cur_socket(new asio::ip::tcp::socket(service));
        acc.accept(*cur_socket);
    }
}

void client_session(g_socket_ptr sock)
{
    while(true)
    {
        char data[512];
        size_t len = sock->read_some(asio::buffer(data));
        if (len > 0)
        {
            asio::write(*sock, asio::buffer("ok", 2));
        }
    }
}


typedef std::function<void()> handler_t;
void foo(handler_t handler)
{
    std::cout << "Hello asio! Thread_id: " << std::this_thread::get_id() <<endl;
    
    handler();
}
 
void handler_fun(int i)
{
    std::cout << "i = " << i  <<", Thread_id: " << std::this_thread::get_id() <<endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*2));
}

void work_thread()
{
    g_asio_ios.run();
}

void test_basic_post()
{
    cout << "Test basic post start, Thread_id " << std::this_thread::get_id() <<endl;
    int work_numb = 10;
    for (int i = 0; i < work_numb; ++i)
    {
        g_asio_ios.post(std::bind(handler_fun, i));
    }
    
    asio::detail::thread_group threads;

    int thread_numb = 2;
    for (int i = 0; i < thread_numb; ++i)
    {
        threads.create_thread(work_thread);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*2));

    threads.join();

    cout << "after post" << endl;  
}

void test_asio_main()
{
    std::cout << "test_asio_main, thread_id: " << std::this_thread::get_id() << std::endl;
    simple_client();

    // simple_server();

    // test_basic_post();
}