#ifndef CONNECT_PROCESS_H
#define CONNECT_PROCESS_H

#include <condition_variable>
#include <thread>
#include <mutex>
#include <list>
#include "typedefine.h"
using std::condition_variable;
using std::thread;
using std::mutex;
using std::list;

struct DataItem
{
    char*  dataAddress;
    uint64 dataSize;
};

class ConnectProcess
{
    public:
        ConnectProcess(int clientSocket);
        ~ConnectProcess();
        
        void start();
        void sendDataAtom(char* dataAddress, uint64 dataSize);
        void sendData(char* dataAddress, uint64 dataSize);
        bool isAvailable();

        void processSendData();
        void checkAndSendHeartbeat();

    private:
        int                 m_status;
        int                 m_lastSendSec;
        int                 m_hearbeatSec;
        int                 m_clientSocket;

        thread              m_sendDataThread;
        thread              m_sendHeartBeatThread;

        condition_variable  m_cv;
        mutable mutex       m_mutex;
        list<DataItem>      m_dataItemList;

};

#endif // !CONNECT_PROCESS_H;

