#include "socket_head.h"

void initEnv()
{
    #ifdef WINVER
        WORD sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(sockVersion, &wsaData) != 0)
        {
            return 0;
        }    
    #endif
}

void CleanEnv()
{
    #ifdef WINVER
        WSACleanup();
    #endif
}