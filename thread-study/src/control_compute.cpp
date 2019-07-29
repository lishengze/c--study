#include "control_compute.h"
#include <mutex>
#include <stdio.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

using std::unique_lock;
using std::lock_guard;

ControlClass::ControlClass()
{
    m_pComputeObj = new ComputeClass(this);
    m_pComputeThread = new thread(&ComputeClass::main, m_pComputeObj);
}

ControlClass::~ControlClass()
{
    if (NULL != m_pComputeObj)
    {
        delete m_pComputeObj;
        m_pComputeObj = NULL;
    }

    if (NULL != m_pComputeThread)
    {
        delete m_pComputeThread;
        m_pComputeThread = NULL;
    }    
}


void ControlClass::control()
{
    while (true)
    {
        chooseData();

        processData();
    }
}

void ControlClass::chooseData()
{
    cout << "\nInput D to Get DB Data or F to Get FILE Data, E to End" << endl;
    char result;
    cin >> result;

    if (result == 'D')
    {        
        startGetDataFromDB_signal();
    }

    if (result == 'F')
    {
        startGetDataFromFile_signal();
    }

    if (result == 'E')
    {
        cout << "End!";
    }
}

void ControlClass::processData()
{
    unique_lock<mutex> ul(m_mutex);
    m_processDataCV.wait(ul);

    for (auto tmpStr:m_stringList)
    {
        cout << tmpStr << endl;
    }
}

void ControlClass::appendString(string appendString)
{
    lock_guard<mutex> lk(m_mutex);
    m_stringList.push_back(appendString);
}

void ControlClass::appendString(list<string> appendStringList)
{
    lock_guard<mutex> lk(m_mutex);
    m_stringList.merge(appendStringList);
}

void ControlClass::startGetDataFromDB_signal()
{
    m_pComputeObj->startGetDataFromDB_slot();
}

void ControlClass::startGetDataFromFile_signal()
{
    m_pComputeObj->startGetDataFromFile_slot();
}

void ControlClass::getDataFromDB_slot()
{
    m_processDataCV.notify_one();
}

void ControlClass::getDataFromFile_slot()
{
    m_processDataCV.notify_one();
} 


ComputeClass::ComputeClass(ControlClass* pValue)
{
    m_pControlObj = pValue;
    m_bGetDataFromFileStart = false;
    m_bGetDataFromDBStart = false;
}

void ComputeClass::main()
{
    cout << "ComputeClass::main " << endl;
}

void ComputeClass::startGetDataFromDB_slot()
{
    getDataFromDB();
    m_getDataFromDBCV.notify_one();
}

void ComputeClass::startGetDataFromFile_slot()
{
    getDataFromFile();
    m_getDataFromFileCV.notify_one();
}

void ComputeClass::getDataFromDB_signal()
{
    m_pControlObj->getDataFromDB_slot();
}

void ComputeClass::getDataFromFile_signal()
{
    m_pControlObj->getDataFromFile_slot();
}

void ComputeClass::getDataFromFile()
{
    if (!m_bGetDataFromFileStart)
    {
        m_bGetDataFromFileStart = true;
        while(true)
        {
            unique_lock<mutex> ul(m_mutex);
            m_getDataFromFileCV.wait(ul);

            string appendMsg;
            cout << "Input Append String: " << endl;
            cin >> appendMsg;
            m_pControlObj->appendString(appendMsg + " from File!");

            getDataFromFile_signal();
        }
    }

}

void ComputeClass::getDataFromDB()
{
    if (!m_bGetDataFromDBStart)
    {
        m_bGetDataFromDBStart = true;
        while(true)
        {
            unique_lock<mutex> ul(m_mutex);
            m_getDataFromDBCV.wait(ul);

            string appendMsg;
            cout << "Input Append String: " << endl;
            cin >> appendMsg;
            m_pControlObj->appendString(appendMsg + " from DB!");

            getDataFromDB_signal();
        }        
    }

}

void TestControlCompute()
{
    ControlClass controlObj;
}