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

int main()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	while (true) {
		SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return 0;
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		memset(serAddr.sin_zero, 0, sizeof(serAddr.sin_zero));

		if (connect(clientSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //连接失败 
			printf("connect error !");
			closesocket(clientSocket);
			return 0;
		}

		string data;
		getline(cin, data);
		const char * sendData;
		sendData = data.c_str();   //string转const char* 
								   //char * sendData = "你好，TCP服务端，我是客户端\n";
		int sentDataLen = send(clientSocket, sendData, strlen(sendData), 0);
		//send()用来将数据由指定的socket传给对方主机
		//int send(int s, const void * msg, int len, unsigned int flags)
		//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
		//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 
		cout << "sentDataLen: " << sentDataLen << endl;

		char recData[255];
		int ret = recv(clientSocket, recData, 255, 0);
		if (ret>0) {
			recData[ret] = 0x00;
			printf(recData);
		}
		closesocket(clientSocket);
	}


	WSACleanup();
	return 0;

}

