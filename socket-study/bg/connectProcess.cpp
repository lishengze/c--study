#include "connectProcess.h"

ConnectProcess::ConnectProcess(int clientSocket):
m_clientSocket(clientSocket)
{

}

ConnectProcess::~ConnectProcess()
{

}

void ConnectProcess::start()
{
    m_sendDataThread = thread(&ConnectProcess::processSendData, this);
}

void ConnectProcess::sendData(char* dataAddress, int dataSize)
{

}

void ConnectProcess::processSendData()
{

}
