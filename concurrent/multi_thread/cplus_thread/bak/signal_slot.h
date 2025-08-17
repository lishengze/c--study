#ifndef _SIGNAL_SLOT_H_
#define _SIGNAL_SLOT_H_

#include <thread>
#include <string>
#include <list>
#include <mutex>
#include <condition_variable>
using std::string;
using std::list;
using std::mutex;
using std::condition_variable;
using std::thread;

class ControlClass;
class ComputeClass;

class SignalSlot
{
    public:
        SignalSlot();

        void test();

        void start();

        void addData();

        void minusData();

        void control();

        void processData();

        void chooseOption();

        void setEnd(bool value);

        bool checkEnd();

    private:
        int                     m_value;

        mutex                   m_addMinusMutex;
        mutex                   m_controlMutex;
        mutex                   m_endMutex;

        condition_variable      m_addCV;
        condition_variable      m_minusCV;
        condition_variable      m_controlCV;

        thread*                 m_pAddThread;
        thread*                 m_pMinusThread;
        thread*                 m_pControlThread;

        bool                    m_bEnd;

        thread::id              m_mainThreadID;
};

void TestSignalSlot();


#endif // !_TIME_CLOCK_H_
