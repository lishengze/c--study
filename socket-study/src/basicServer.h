#ifndef  BASIC_SERVER_H
#define  BASIC_SERVER_H

#include <string>
#include <vector>
#include <thread>
#include "connectProcess.h"
#include "typedefine.h"

using std::string;
using std::vector;
using std::thread;

class BasicServer
{
    public:
        BasicServer(string serverAddress, int serverPort);
        BasicServer();
        ~BasicServer();

        void setAddress(string);
        void setPort(int);

        void start();
        void processSocketListen();
        void processHeartbeat();

        void DoListen();
        void DoAccept(int clientSocket);

        void sendData(char* dataAddress, uint64 dataSize);
        void sendDataArray(char* dataAddress, uint64 dataCount, uint64 dataSize);

        void sendData(int32 clientID, char* dataAddress, uint64 dataSize);
        void sendDataArray(int32 clientID, char* dataAddress, 
                            uint64 dataCount, uint64 dataSize);

    private:
        string                  m_serverAddress;
        int                     m_serverPort;
        int                     m_serverSocket;
        int                     m_serverBacklog;
        vector<ConnectProcess*> m_ConnProcVec;

        thread                  m_listenThread;
        thread                  m_heartbeatThread;        
};

void testBasicServer();

#endif // ! BASIC_SERVER_H;
