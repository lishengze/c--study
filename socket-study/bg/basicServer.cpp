#include <sys/socket.h>
#include "basicServer.h"
#include "connectProcess.h"

BasicServer::BasicServer(string serverAddress, int serverPort):
m_serverAddress(serverAddress), m_serverPort(serverPort)
{

}

BasicServer::BasicServer()
{

}

BasicServer::~BasicServer()
{

}

void BasicServer::setAddress(string serverAddress) 
{
    m_serverAddress = serverAddress;
}

void BasicServer::setPort(int serverPort)
{
    m_serverPort = serverPort;
}

void BasicServer::start()
{
    m_listenThread = thread(&BasicServer::processSocketListen, this);
    m_heartbeatThread = thread(&BasicServer::processHeartbeat, this);
}

void BasicServer::processSocketListen()
{
    while (true)
    {
        DoListen();
        while(true)
        {
            sockaddr clientAddr;
            int clientAddrLength = sizeof(clientAddr);
            int clientSocket = accept(m_serverSocket, &clientAddr, &clientSocket);
            if (clientSocket < 0)
            {
                printf("链接错误, 重新监听端口");
                break;
            }
            else
            {
                DoAccept(clientSocket);
            }
        }
    }
}

void BasicServer::processHeartbeat()
{

}

void BasicServer::DoListen()
{

}

void BasicServer::DoAccept(int clientSocket)
{
    ConnectProcess* connProcObj = new ConnectProcess(clientSocket);
    connProcObj->start();
    
    m_ConnProcVec.push_back(connProcObj);
}

void BasicServer::sendData(char* dataAddress, uint64 dataSize)
{

}

void BasicServer::sendDataArray(char* dataAddress, uint64 dataCount, uint64 dataSize)
{

}

void BasicServer::sendData(int32 clientID, char* dataAddress, uint64 dataSize)
{

}

void BasicServer::sendDataArray(int32 clientID, char* dataAddress, 
                                uint64 dataCount, uint64 dataSize)
{

}
