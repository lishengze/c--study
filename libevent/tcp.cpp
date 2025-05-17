#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 新连接回调函数
/**
 * @brief 处理连接读取事件的回调函数。
 * 
 * 当从客户端接收到数据时，此函数会被调用。它会将接收到的数据
 * 从输入缓冲区复制到输出缓冲区，实现数据的回显功能。
 * 
 * @param bev 指向 bufferevent 结构体的指针，代表客户端连接。
 * @param ctx 回调函数的上下文指针，当前未使用。
 */
void conn_readcb(struct bufferevent *bev, void *ctx) {
    // 获取与 bufferevent 关联的输入缓冲区，用于存储从客户端接收到的数据
    struct evbuffer *input = bufferevent_get_input(bev);
    // 获取与 bufferevent 关联的输出缓冲区，用于存储要发送给客户端的数据
    struct evbuffer *output = bufferevent_get_output(bev);
    
    // 将输入缓冲区的数据复制到输出缓冲区，实现数据的回显功能
    // 复制完成后，输入缓冲区的数据会被清空
    evbuffer_add_buffer(output, input);
}

// 连接错误回调函数
void conn_eventcb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_EOF) {
        printf("连接关闭\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("发生错误\n");
    }
    
    // 关闭连接
    bufferevent_free(bev);
}

// 新连接监听回调函数
/**
 * @brief 新连接监听回调函数，当有新的客户端连接时被调用。
 * 
 * 此函数会为新连接创建一个 bufferevent 对象，并设置相应的回调函数，
 * 以便处理数据读写和连接错误事件。
 * 
 * @param listener 指向 evconnlistener 结构体的指针，代表监听对象。
 * @param fd 新连接的套接字描述符。
 * @param addr 指向 sockaddr 结构体的指针，包含新连接的地址信息。
 * @param socklen 地址结构体的长度。
 * @param ctx 回调函数的上下文指针，当前未使用。
 */
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                 struct sockaddr *addr, int socklen, void *ctx) {
    // 获取与监听器关联的事件基
    struct event_base *base = evconnlistener_get_base(listener);
    // 定义一个指向 bufferevent 结构体的指针，用于处理新连接的数据读写
    struct bufferevent *bev;
    
    // 为新连接的套接字创建一个 bufferevent 对象，BEV_OPT_CLOSE_ON_FREE 表示释放 bufferevent 时关闭套接字
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        // 如果创建 bufferevent 失败，输出错误信息并终止事件循环
        fprintf(stderr, "创建bufferevent失败\n");
        event_base_loopbreak(base);
        return;
    }
    
    // 设置 bufferevent 的回调函数
    // conn_readcb 用于处理数据读取事件
    // NULL 表示不设置数据写入回调函数
    // conn_eventcb 用于处理连接错误和关闭事件
    // NULL 表示不传递上下文信息
    bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, NULL);
    // 启用 bufferevent 的读取和写入功能
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    
    // 输出新连接的客户端地址和端口信息
    printf("新连接来自: %s:%d\n", 
           inet_ntoa(((struct sockaddr_in*)addr)->sin_addr),
           ntohs(((struct sockaddr_in*)addr)->sin_port));
}

// 监听错误回调函数
void accept_errorcb(struct evconnlistener *listener, void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "接受连接错误 %d (%s)\n", err, evutil_socket_error_to_string(err));
    event_base_loopbreak(base);
}

int main(int argc, char **argv) {
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;
    
    int port = 9999;
    
    // 创建事件基
    base = event_base_new();
    if (!base) {
        fprintf(stderr, "无法创建事件基\n");
        return 1;
    }
    
    // 设置监听地址和端口
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);  // 监听所有地址
    sin.sin_port = htons(port);
    
    // 创建监听器
    listener = evconnlistener_new_bind(base, listener_cb, NULL,
                                      LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                      -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        fprintf(stderr, "无法创建监听器\n");
        return 1;
    }
    
    // 设置错误回调
    evconnlistener_set_error_cb(listener, accept_errorcb);
    
    printf("服务器启动，监听端口 %d\n", port);
    
    // 进入事件循环
    event_base_dispatch(base);
    
    // 释放资源
    evconnlistener_free(listener);
    event_base_free(base);
    
    return 0;
}