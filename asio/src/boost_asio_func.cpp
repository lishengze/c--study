#include "boost_asio_func.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
using namespace boost::asio;
using std::cout;
using std::endl;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

boost::asio::io_service g_ios;

void boost_simple_client()
{
    cout << "Test Simple Client" << endl;
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket socket(service);
    socket.connect(ep);
}

void boost_simple_server()
{
    cout << "Test Simple Server" << endl;
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001);
    ip::tcp::acceptor acc(service, ep);

    while (true)
    {
        socket_ptr cur_socket(new ip::tcp::socket(service));
        acc.accept(*cur_socket);
    }
}

void boost_client_session(socket_ptr sock)
{
    while(true)
    {
        char data[512];
        size_t len = sock->read_some(boost::asio::buffer(data));
        if (len > 0)
        {
            boost::asio::write(*sock, buffer("ok", 2));
        }
    }
}


typedef std::function<void()> handler_t;
void boost_foo(handler_t handler)
{
    std::cout << "Hello asio! Thread_id: " << std::this_thread::get_id() <<endl;
    
    handler();
}
 
void boost_handler_fun(int i)
{
    std::cout << "i = " << i  <<", Thread_id: " << std::this_thread::get_id() <<endl;
}

void boost_work_thread()
{
    g_ios.run();
}

void boost_test_basic_post()
{
    cout << "Test basic post start, Thread_id " << std::this_thread::get_id() <<endl;
    int work_numb = 10;
    for (int i = 0; i < work_numb; ++i)
    {
        g_ios.post(std::bind(boost_handler_fun, i));
    }
    
    boost::thread_group threads;

    int thread_numb = 5;
    for (int i = 0; i < thread_numb; ++i)
    {
        threads.create_thread(boost_work_thread);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000*2));

    threads.join_all();

    cout << "after post" << endl;  
}

void test_boost_asio_main()
{
    // boost_simple_client();

    boost_test_basic_post();
}