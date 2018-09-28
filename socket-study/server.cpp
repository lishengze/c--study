#ifdef WINVER
    #include<WINSOCK2.H>
    #include<cstring>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
#endif

#include<stdio.h>
#include<iostream>
#include<string>
using namespace std;

int main(int argc, char* argv[])
{
	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口
	sockaddr simpSin;
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(serverSocket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监控
	if (listen(serverSocket, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据  
	SOCKET clientSocket;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		printf("等待连接...\n");
		clientSocket = accept(serverSocket, (SOCKADDR *)&remoteAddr, &nAddrlen); // 阻塞
		if (clientSocket == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

		//接收数据  
		int ret = recv(clientSocket, revData, 255, 0); // 阻塞
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//发送数据
		const char * sendData = "你好，TCP客户端！\n";
		send(clientSocket, sendData, strlen(sendData), 0);
		closesocket(clientSocket);
	}

	closesocket(serverSocket);
	WSACleanup();
	return 0;
}
