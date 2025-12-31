#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <errno.h>

class EpollClient {
public:
    // 构造函数：初始化服务端IP和端口
    EpollClient(const char* server_ip, uint16_t server_port);
    // 析构函数：释放资源
    ~EpollClient();
    // 连接服务端
    bool connect_to_server();
    // 发送数据到服务端
    bool send_data(const char* data, int len);
    // 接收服务端数据（阻塞/非阻塞可选，此处非阻塞）
    int recv_data(char* buffer, int buffer_len);
    // 关闭连接
    void close_connection();

private:
    // 设置文件描述符为非阻塞
    bool set_non_blocking(int fd);

private:
    const char* m_server_ip;        // 服务端IP
    uint16_t m_server_port;         // 服务端端口
    int m_client_fd;                // 客户端套接字
    struct sockaddr_in m_server_addr; // 服务端地址结构
    bool m_connected;               // 连接状态标志
};