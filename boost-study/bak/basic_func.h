#ifndef _BASIC_FUNC_H_
#define _BASIC_FUNC_H_

#include <zmq.h>
#include <string>
using std::string;

string s_recv_u(void*socket, int flag=0);

string s_recvmore(void* socket);

string trans_zmq_msg(zmq_msg_t& zmq_data);

int s_send_u(void* socket, const char* data);

#endif
