#include "signal_slot.h"
#include <mutex>
#include <stdio.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

using std::unique_lock;

SignalSlot::SignalSlot()
{
    m_value = 0;

    m_mainThreadID = std::this_thread::get_id();

    m_bEnd = false;

    test();
}


void SignalSlot::test()
{
    start();
}

void SignalSlot::start()
{   
    printf("\n---- Start Test Signal Slot ----\n");
    m_pAddThread        = new thread(addData, this);
    m_pMinusThread      = new thread(minusData, this);
    m_pControlThread    = new thread(control, this);

    if (m_pAddThread->joinable())
    {
        m_pAddThread->join();
    }

    if (m_pMinusThread->joinable())
    {
        m_pMinusThread->join();
    }

    if (m_pControlThread->joinable())
    {
        m_pControlThread->join();
    }    
}

void SignalSlot::addData()
{
    while(true)
    {
        unique_lock<mutex> ulk(m_addMinusMutex);
        m_addCV.wait(ulk);

        if (checkEnd()) break;

        cout << "Input Added Value: " <<endl;
        int addedValue;
        cin >> addedValue;

        m_value += addedValue;
        printf("write data m_value: %d \n", m_value);

        m_controlCV.notify_one();
    }
}

void SignalSlot::minusData()
{
    while(true)
    {
        unique_lock<mutex> ulk(m_addMinusMutex);
        m_minusCV.wait(ulk);

        if (checkEnd()) break;

        int minusValue;
        cout << "Input Minus Value: " <<endl; 
        cin >> minusValue;

        m_value -= minusValue;

        printf("read data m_value: %d \n", m_value);

        m_controlCV.notify_one();
    }
}

void SignalSlot::control()
{
    while (true)
    {
        chooseOption();

        if (checkEnd()) break;

        processData();
    }
}

void SignalSlot::processData()
{
    unique_lock<mutex> ulk(m_addMinusMutex);
    m_controlCV.wait(ulk);

    cout << "ProcessData Current Value: " << m_value << endl;
}

void SignalSlot::chooseOption()
{
    cout << "\nInput A to Start Add or M to Start Minus, E to End" << endl;
    char result;
    cin >> result;

    if (result == 'A')
    {        
        m_addCV.notify_one();
    }

    if (result == 'M')
    {
        m_minusCV.notify_one();
    }

    if (result == 'E')
    {
        cout << "End!";
        setEnd(true);
    }
}

void SignalSlot::setEnd(bool value)
{
    std::lock_guard<mutex> lk(m_endMutex);
    m_bEnd = value;

    if (value == true)
    {
        m_addCV.notify_all();
        m_minusCV.notify_all();
    }
}

bool SignalSlot::checkEnd()
{
    std::lock_guard<mutex> lk(m_endMutex);

    return m_bEnd;
}

void TestSignalSlot()
{
    SignalSlot signalSlotObj;
}
