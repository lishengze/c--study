#include "socket_head.h"

#include <string>
#include <sys/socket.h>  
#include <sys/epoll.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <errno.h>  
#include <iostream>  
#include <memory.h>

#include "basicServer.h"
#include "connectProcess.h"

BasicServer::BasicServer(string serverAddress, int serverPort):
m_serverAddress(serverAddress), m_serverPort(serverPort)
{
    m_serverBacklog = 20;
}

BasicServer::BasicServer()
{
    m_serverBacklog = 20;

    initEnv();
}

BasicServer::~BasicServer()
{
    for (vector<ConnectProcess*>::const_iterator it = m_ConnProcVec.begin();
        it != m_ConnProcVec.end(); ++it)
    {
        if (*it != NULL)
        {
            delete *it;
        }
    }

    cleanEnv();
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
            int clientSocket = accept(m_serverSocket, &clientAddr, (socklen_t*)(&clientAddrLength));
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
    while(true)
    {
        sleep(5);
        for (vector<ConnectProcess*>::const_iterator it = m_ConnProcVec.begin();
            it != m_ConnProcVec.end(); ++it)
        {
            ConnectProcess* currConnectProcess = *it;
            if (currConnectProcess->isAvailable())
            {
                currConnectProcess->checkAndSendHeartbeat();
            }
        }        
    }
}

void BasicServer::DoListen()
{
    // 设置地址, 创建, 绑定, 监听;
    close(m_serverSocket);
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(m_serverPort);
    if (m_serverAddress.empty())
    {
        socketAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        socketAddress.sin_addr.s_addr = inet_addr(m_serverAddress.c_str());
    }

    while(true)
    {
        if (bind(m_serverSocket, (sockaddr *)&socketAddress, sizeof(socketAddress) != 0))
        {
            printf("bind error, id = %d, msg = %s", errno, strerror(errno));
            sleep(5);
            continue;
        }

        if (listen(m_serverSocket, m_serverBacklog) != 0)
        {
            printf("bind error, id = %d, msg = %s", errno, strerror(errno));
            sleep(5);
            continue;
        }
        break;
    }
}

void BasicServer::DoAccept(int clientSocket)
{
    ConnectProcess* connProcObj = new ConnectProcess(clientSocket);
    connProcObj->start();
    
    m_ConnProcVec.push_back(connProcObj);
}

void BasicServer::sendData(char* dataAddress, uint64 dataSize)
{
    for (vector<ConnectProcess*>::const_iterator it = m_ConnProcVec.begin();
        it != m_ConnProcVec.end(); ++it)
    {
        if ((*it)->isAvailable())
        {
            (*it)->sendData(dataAddress, dataSize);
        }
    }
}

void BasicServer::sendDataArray(char* dataAddress, uint64 dataCount, uint64 dataSize)
{
    for (vector<ConnectProcess*>::const_iterator it = m_ConnProcVec.begin();
        it != m_ConnProcVec.end(); ++it)
    {
        if ((*it)->isAvailable())
        {
            (*it)->sendData(dataAddress, dataSize*dataCount);
        }
    }
}


void BasicServer::sendData(int32 clientID, char* dataAddress, uint64 dataSize)
{
    if (clientID < int(m_ConnProcVec.size()) && m_ConnProcVec[clientID]->isAvailable())
    {
        m_ConnProcVec[clientID]->sendData(dataAddress, dataSize);
    }
}

void BasicServer::sendDataArray(int32 clientID, char* dataAddress, 
                                uint64 dataCount, uint64 dataSize)
{
    if (clientID < int(m_ConnProcVec.size()) && m_ConnProcVec[clientID]->isAvailable())
    {
        m_ConnProcVec[clientID]->sendData(dataAddress, dataSize*dataCount);
    }
}


class TestBasicServer
{

};

void testBasicServer()
{

}