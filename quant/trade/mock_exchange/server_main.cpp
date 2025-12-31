#include "epoll_server.h"
#include "epoll_client.h"
#include <string>
#include <thread>
#include <chrono>


int server_main(int argc, char* argv[]) {
    // 默认端口 8888
    uint16_t port = 8888;
    if (argc >= 2) {
        port = (uint16_t)atoi(argv[1]);
    }
    // 创建并启动服务端
    EpollServer server(port);
    if (!server.start()) {
        std::cerr << "Epoll server start failed" << std::endl;
        return -1;
    }
    return 0;
}


int client_main(int argc, char* argv[]) {
    // 默认服务端 IP 127.0.0.1，端口 8888
    const char* server_ip = "127.0.0.1";
    uint16_t port = 8888;
    if (argc >= 3) {
        server_ip = argv[1];
        port = (uint16_t)atoi(argv[2]);
    }
    // 创建客户端并连接服务端
    EpollClient client(server_ip, port);
    if (!client.connect_to_server()) {
        std::cerr << "Client connect to server failed" << std::endl;
        return -1;
    }
    // 双向通讯：循环发送数据，并接收服务端响应
    char recv_buffer[1024] = {0};
    for (int i = 0; i < 5; ++i) {
        // 构造发送数据
        std::string send_data = "Hello Epoll Server! (" + std::to_string(i + 1) + ")";
        // 发送数据
        client.send_data(send_data.c_str(), send_data.size());
        // 接收响应（非阻塞，此处短暂休眠等待数据）
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        client.recv_data(recv_buffer, sizeof(recv_buffer));
    }
    // 关闭连接
    client.close_connection();
    return 0;
}

int main() {
    server_main(0, nullptr);
    client_main(0, nullptr);
    return 0;
}