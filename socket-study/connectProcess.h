#ifndef CONNECT_PROCESS_H
#define CONNECT_PROCESS_H
#include <thread>
using std::thread;

class ConnectProcess
{
    public:
        ConnectProcess(int clientSocket);
        ~ConnectProcess();
        
        void start();
        void sendData(char* dataAddress, int dataSize);

        void processSendData();

    private:
        int         m_status;
        int         m_lastSendSec;
        int         m_clientSocket;
        thread      m_sendDataThread;
};

#endif // !CONNECT_PROCESS_H;

