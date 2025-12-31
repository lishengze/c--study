#include "epoll_client.h"

EpollClient::EpollClient(const char* server_ip, uint16_t server_port)
    : m_server_ip(server_ip), m_server_port(server_port), m_client_fd(-1), m_connected(false) {
    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(m_server_port);
    // 转换IP地址
    if (inet_pton(AF_INET, m_server_ip, &m_server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP: " << m_server_ip << std::endl;
    }
}

EpollClient::~EpollClient() {
    close_connection();
}

bool EpollClient::set_non_blocking(int fd) {
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

bool EpollClient::connect_to_server() {
    // 1. 创建客户端套接字
    m_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_client_fd == -1) {
        std::cerr << "socket create failed: " << strerror(errno) << std::endl;
        return false;
    }
    // 设置非阻塞（可选，本示例用非阻塞连接）
    if (!set_non_blocking(m_client_fd)) {
        close(m_client_fd);
        m_client_fd = -1;
        return false;
    }
    // 2. 连接服务端
    int ret = connect(m_client_fd, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));
    if (ret == 0) {
        // 立即连接成功
        m_connected = true;
    } else if (errno == EINPROGRESS) {
        // 连接正在进行中（非阻塞特性），此处简化处理，直接标记为连接成功
        // 生产环境可通过epoll监听EPOLLOUT事件确认连接完成
        m_connected = true;
    } else {
        std::cerr << "connect failed: " << strerror(errno) << std::endl;
        close(m_client_fd);
        m_client_fd = -1;
        return false;
    }
    std::cout << "Connected to server " << m_server_ip << ":" << m_server_port << std::endl;
    return true;
}

bool EpollClient::send_data(const char* data, int len) {
    if (!m_connected || m_client_fd == -1 || len <= 0 || !data) {
        std::cerr << "Send failed: invalid state or data" << std::endl;
        return false;
    }
    ssize_t send_len = send(m_client_fd, data, len, 0);
    if (send_len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::cerr << "Send failed: buffer full, try again later" << std::endl;
            return false;
        }
        std::cerr << "send failed: " << strerror(errno) << std::endl;
        close_connection();
        return false;
    }
    std::cout << "Send to server: " << std::string(data, len) << " (sent len: " << send_len << ")" << std::endl;
    return true;
}

int EpollClient::recv_data(char* buffer, int buffer_len) {
    if (!m_connected || m_client_fd == -1 || !buffer || buffer_len <= 0) {
        std::cerr << "Recv failed: invalid state or buffer" << std::endl;
        return -1;
    }
    memset(buffer, 0, buffer_len);
    ssize_t recv_len = recv(m_client_fd, buffer, buffer_len - 1, 0);
    if (recv_len == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 暂无数据可读
            return 0;
        }
        std::cerr << "recv failed: " << strerror(errno) << std::endl;
        close_connection();
        return -1;
    } else if (recv_len == 0) {
        // 服务端关闭连接
        std::cout << "Server disconnected" << std::endl;
        close_connection();
        return 0;
    }
    std::cout << "Recv from server: " << std::string(buffer, recv_len) << std::endl;
    return (int)recv_len;
}

void EpollClient::close_connection() {
    if (m_connected && m_client_fd != -1) {
        close(m_client_fd);
        m_client_fd = -1;
        m_connected = false;
        std::cout << "Client connection closed" << std::endl;
    }
}