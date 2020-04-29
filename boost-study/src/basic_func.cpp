#include "basic_func.h"
#include <string.h>
/** 
 * @brief 通过zmq 获取数据
 * @param void*   socket
 * 声明，初始化，获取数据， 获取数据大小， 拷贝， 加0， 关闭;
 * @return 获取的字符串
 */
string s_recv_u(void*socket, int flags)
{
    zmq_msg_t ori_msg;
    zmq_msg_init(&ori_msg);
    zmq_msg_recv(&ori_msg, socket, flags);
    int data_size = zmq_msg_size(&ori_msg);
    char* c_data = new char[data_size + 1];
    memcpy(c_data,  zmq_msg_data(&ori_msg), data_size);
    c_data[data_size] = 0;

    zmq_msg_close(&ori_msg);
    string result = c_data;
    return result;
}

string trans_zmq_msg(zmq_msg_t& zmq_data)
{
    int data_size = zmq_msg_size(&zmq_data);
    char* c_data = new char[data_size + 1];
    memcpy(c_data,  zmq_msg_data(&zmq_data), data_size);
    c_data[data_size] = 0;
    string result = c_data;
    return result;    
}

/** 
 * @brief 通过zmq 发送数据
 * @param void*   zmq socket
 * @param char*   数据地址
 * 声明，初始化，获取数据， 获取数据大小， 拷贝， 加0， 关闭;
 * @return 获取的字符串
 */
int s_send_u(void* socket, const char* data)
{
    int result;
    zmq_msg_t send_msg;
    zmq_msg_init_size(&send_msg, strlen(data));
    memcpy(zmq_msg_data(&send_msg), data, strlen(data));

    result = zmq_msg_send(&send_msg, socket,  0);
    zmq_msg_close(&send_msg);
    
    return result;
}

/** 
 * @brief 通过zmq 获取数据
 * @param void*   socket
 * 
 * @return 获取的字符串
 */
string s_recvmore(void* socket)
{
    string result;
    int is_end;
    while(1)
    {
        zmq_msg_t ori_msg;
        zmq_msg_init(&ori_msg);
        zmq_msg_recv(&ori_msg, socket, 0);
        int data_size = zmq_msg_size(&ori_msg);
        char* c_data = new char[data_size + 1];
        memcpy(c_data,  zmq_msg_data(&ori_msg), data_size);
        c_data[data_size] = 0;    
        string cur_strdata = c_data;
        result += cur_strdata;
        delete[] c_data;    
        size_t is_end_size = sizeof(is_end);
        zmq_getsockopt(socket, ZMQ_RCVMORE, &is_end, &is_end_size);

        if (!is_end)    break;
    }
    return result;
}