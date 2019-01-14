#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "connectProcess.h"
#include "func_time.h"

using std::lock_guard;
using std::unique_lock;


ConnectProcess::ConnectProcess(int clientSocket):
m_clientSocket(clientSocket)
{
    m_hearbeatSec = 5;
    m_status = 0;
}

ConnectProcess::~ConnectProcess()
{

}

bool ConnectProcess::isAvailable()
{
    bool result = true;
    return result;
}

void ConnectProcess::start()
{
    m_sendDataThread      = thread(&ConnectProcess::processSendData, this);
    m_sendHeartBeatThread = thread(&ConnectProcess::checkAndSendHeartbeat, this);
}

void ConnectProcess::sendDataAtom(char* dataAddress, uint64 dataSize)
{
    int startPos = 0;
    int leftSize = dataSize;
    while (dataSize > 0)
    {
        int sendedSize = send(m_clientSocket, dataAddress+startPos, dataSize, MSG_NOSIGNAL);
        if (sendedSize < 0)
        {
            printf("%s: connection error, id = %d, msg = %s", __FUNCTION__, errno, strerror(errno));
        }
        else if (sendedSize < dataSize)
        {
            startPos += sendedSize;
            dataSize -= sendedSize;
        }
        else
        {
            break;
        }
    }
}


void ConnectProcess::sendData(char* dataAddress, uint64 dataSize)
{
    lock_guard<mutex> gurad(m_mutex);

    DataItem currDataItem;
    currDataItem.dataAddress = dataAddress;
    currDataItem.dataSize = dataSize;
    m_dataItemList.push_back(currDataItem);

    m_cv.notify_all();
}

void ConnectProcess::processSendData()
{
    while(true)
    {
        unique_lock<mutex> lk(m_mutex);
        m_cv.wait(lk);

        list<DataItem> tmpDataItemList;
        tmpDataItemList.swap(m_dataItemList);
        for(list<DataItem>::const_iterator it = tmpDataItemList.begin();
            it != tmpDataItemList.end(); ++it)
        {
            sendDataAtom(it->dataAddress, it->dataSize);
        }

        m_dataItemList.clear();
    }
}

void ConnectProcess::checkAndSendHeartbeat()
{
    while(true)
    {
        sleep(m_hearbeatSec);
        int currTimeSecs = getCurrTimeSecs();
        if (currTimeSecs < m_lastSendSec || currTimeSecs - m_lastSendSec >= m_hearbeatSec)
        {
            // Send Heartbeat;
            string dataStr = getCurrDateTimeStr() + " send heartbeat";
            send(m_clientSocket, dataStr.c_str(), dataStr.size(), MSG_NOSIGNAL);
        }
    }
}