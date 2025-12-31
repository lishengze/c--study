#pragma once
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <errno.h>

class EpollServer {
public:
    // 构造函数：初始化端口和最大连接数
    EpollServer(uint16_t port, int max_events = 1024);
    // 析构函数：释放资源
    ~EpollServer();
    // 启动服务端
    bool start();
    // 停止服务端
    void stop();

private:
    // 设置文件描述符为非阻塞
    bool set_non_blocking(int fd);
    // 添加文件描述符到epoll
    bool add_fd_to_epoll(int fd, uint32_t events);
    // 修改epoll中文件描述符的事件
    bool mod_fd_in_epoll(int fd, uint32_t events);
    // 从epoll中删除文件描述符
    bool del_fd_from_epoll(int fd);
    // 处理新客户端连接
    void handle_new_connection();
    // 处理客户端数据接收
    void handle_client_read(int client_fd);
    // 处理数据发送（可选，此处同步发送）
    void handle_client_write(int client_fd, const char* data, int len);

private:
    uint16_t m_port;                // 服务端端口
    int m_listen_fd;                // 监听套接字
    int m_epoll_fd;                 // epoll文件描述符
    int m_max_events;               // 最大监听事件数
    std::vector<epoll_event> m_events; // 事件数组
    std::unordered_map<int, std::string> m_client_buffers; // 客户端接收缓冲区
    bool m_running;                 // 服务端运行标志
};