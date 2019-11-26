#ifdef WINVER
    #include<WINSOCK2.H>
    #include<cstring>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
#endif

void initEnv();

void cleanEnv();