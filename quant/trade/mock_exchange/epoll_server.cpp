#include "epoll_server.h"

EpollServer::EpollServer(uint16_t port, int max_events)
    : m_port(port), m_max_events(max_events), m_listen_fd(-1), m_epoll_fd(-1), m_running(false) {
    m_events.resize(m_max_events);
}

EpollServer::~EpollServer() {
    stop();
}

bool EpollServer::set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "fcntl F_GETFL failed: " << strerror(errno) << std::endl;
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool EpollServer::add_fd_to_epoll(int fd, uint32_t events) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        std::cerr << "epoll_ctl ADD failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool EpollServer::mod_fd_in_epoll(int fd, uint32_t events) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = events;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        std::cerr << "epoll_ctl MOD failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool EpollServer::del_fd_from_epoll(int fd) {
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        std::cerr << "epoll_ctl DEL failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void EpollServer::handle_new_connection() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // 接受新连接
    int client_fd = accept(m_listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        std::cerr << "accept failed: " << strerror(errno) << std::endl;
        return;
    }
    // 设置非阻塞
    if (!set_non_blocking(client_fd)) {
        close(client_fd);
        return;
    }
    // 添加到epoll（监听读事件）
    if (!add_fd_to_epoll(client_fd, EPOLLIN | EPOLLET)) { // EPOLLET：边缘触发
        close(client_fd);
        return;
    }
    // 初始化客户端缓冲区
    m_client_buffers[client_fd] = "";
    std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr) 
              << ":" << ntohs(client_addr.sin_port) << " (fd: " << client_fd << ")" << std::endl;
}

void EpollServer::handle_client_read(int client_fd) {
    char buffer[1024] = {0};
    ssize_t recv_len = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (recv_len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 数据已读完，无需处理
            return;
        }
        // 其他错误，关闭连接
        std::cerr << "recv failed (fd: " << client_fd << "): " << strerror(errno) << std::endl;
        del_fd_from_epoll(client_fd);
        close(client_fd);
        m_client_buffers.erase(client_fd);
        return;
    } else if (recv_len == 0) {
        // 客户端关闭连接
        std::cout << "Client disconnected (fd: " << client_fd << ")" << std::endl;
        del_fd_from_epoll(client_fd);
        close(client_fd);
        m_client_buffers.erase(client_fd);
        return;
    }
    // 数据追加到缓冲区
    m_client_buffers[client_fd] += std::string(buffer, recv_len);
    std::cout << "Recv from client (fd: " << client_fd << "): " << m_client_buffers[client_fd] << std::endl;

    // 双向通讯：收到数据后，原样返回（回声服务，可自定义业务逻辑）
    std::string& recv_data = m_client_buffers[client_fd];
    handle_client_write(client_fd, recv_data.c_str(), recv_data.size());
    // 清空缓冲区
    recv_data.clear();
}

void EpollServer::handle_client_write(int client_fd, const char* data, int len) {
    if (len <= 0 || !data) {
        return;
    }
    ssize_t send_len = send(client_fd, data, len, 0);
    if (send_len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 发送缓冲区满，后续可监听EPOLLOUT事件重试
            mod_fd_in_epoll(client_fd, EPOLLOUT | EPOLLET);
            return;
        }
        std::cerr << "send failed (fd: " << client_fd << "): " << strerror(errno) << std::endl;
        del_fd_from_epoll(client_fd);
        close(client_fd);
        m_client_buffers.erase(client_fd);
        return;
    }
    std::cout << "Send to client (fd: " << client_fd << "): " << std::string(data, len) << std::endl;
    // 发送完成后，重新监听读事件
    mod_fd_in_epoll(client_fd, EPOLLIN | EPOLLET);
}

bool EpollServer::start() {
    // 1. 创建监听套接字
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd == -1) {
        std::cerr << "socket create failed: " << strerror(errno) << std::endl;
        return false;
    }
    // 设置端口复用
    int opt = 1;
    if (setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt failed: " << strerror(errno) << std::endl;
        close(m_listen_fd);
        return false;
    }
    // 设置非阻塞
    if (!set_non_blocking(m_listen_fd)) {
        close(m_listen_fd);
        return false;
    }
    // 2. 绑定地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    server_addr.sin_port = htons(m_port);
    if (bind(m_listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "bind failed: " << strerror(errno) << std::endl;
        close(m_listen_fd);
        return false;
    }
    // 3. 开始监听
    if (listen(m_listen_fd, SOMAXCONN) == -1) {
        std::cerr << "listen failed: " << strerror(errno) << std::endl;
        close(m_listen_fd);
        return false;
    }
    // 4. 创建epoll实例
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd == -1) {
        std::cerr << "epoll_create1 failed: " << strerror(errno) << std::endl;
        close(m_listen_fd);
        return false;
    }
    // 5. 添加监听套接字到epoll
    if (!add_fd_to_epoll(m_listen_fd, EPOLLIN | EPOLLET)) {
        close(m_listen_fd);
        close(m_epoll_fd);
        return false;
    }
    // 6. 启动事件循环
    m_running = true;
    std::cout << "Epoll server started on port " << m_port << std::endl;
    while (m_running) {
        // 等待事件触发（-1：无限阻塞，可设置超时时间）
        int event_count = epoll_wait(m_epoll_fd, m_events.data(), m_max_events, -1);
        if (event_count == -1) {
            if (errno == EINTR) {
                // 信号中断，继续循环
                continue;
            }
            std::cerr << "epoll_wait failed: " << strerror(errno) << std::endl;
            break;
        }
        // 处理所有触发的事件
        for (int i = 0; i < event_count; ++i) {
            int fd = m_events[i].data.fd;
            uint32_t events = m_events[i].events;
            // 监听套接字事件：新连接
            if (fd == m_listen_fd) {
                handle_new_connection();
            }
            // 客户端读事件
            else if (events & EPOLLIN) {
                handle_client_read(fd);
            }
            // 客户端写事件（发送缓冲区可用）
            else if (events & EPOLLOUT) {
                // 此处可重新尝试发送数据，本示例简化处理
                mod_fd_in_epoll(fd, EPOLLIN | EPOLLET);
            }
            // 错误事件
            else if (events & (EPOLLERR | EPOLLHUP)) {
                std::cerr << "Client error (fd: " << fd << ")" << std::endl;
                del_fd_from_epoll(fd);
                close(fd);
                m_client_buffers.erase(fd);
            }
        }
    }
    return true;
}

void EpollServer::stop() {
    m_running = false;
    // 关闭所有客户端连接
    for (auto& pair : m_client_buffers) {
        close(pair.first);
    }
    m_client_buffers.clear();
    // 关闭监听套接字和epoll
    if (m_listen_fd != -1) {
        close(m_listen_fd);
        m_listen_fd = -1;
    }
    if (m_epoll_fd != -1) {
        close(m_epoll_fd);
        m_epoll_fd = -1;
    }
    std::cout << "Epoll server stopped" << std::endl;
}