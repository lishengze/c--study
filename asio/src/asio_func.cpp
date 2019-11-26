#include "asio_func.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <iostream>
using namespace boost::asio;
using std::cout;
using std::endl;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

void simple_client()
{
    cout << "Test Simple Client " << endl;
    // io_service service;
    // ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    // ip::tcp::socket socket(service);
    // socket.connect(ep);    
}

void simple_server()
{
    cout << "Test Simple Server " << endl;
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001);
    ip::tcp::acceptor acc(service, ep);

    while (true)
    {
        socket_ptr cur_socket(new ip::tcp::socket(service));
        acc.accept(*cur_socket);
    }
}

void client_session(socket_ptr sock)
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

void test_main()
{
    simple_client();
}