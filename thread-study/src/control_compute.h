#ifndef _CONTROL_COMPUTE_H_
#define _CONTROL_COMPUTE_H_

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

class ControlClass
{
public:
    ControlClass();

    ~ControlClass();

    void control();

    void chooseData();

    void processData();

    void appendString(string appendString);

    void appendString(list<string> appendStringList);

    void startGetDataFromDB_signal();

    void startGetDataFromFile_signal();

    void getDataFromDB_slot();

    void getDataFromFile_slot();    

private:
    list<string>        m_stringList;
    mutex               m_mutex;
    condition_variable  m_processDataCV;

    ComputeClass*       m_pComputeObj;    
    thread*             m_pComputeThread;
};

class ComputeClass
{
public:
    ComputeClass(ControlClass* pValue);

    void main();

    void startGetDataFromDB_slot();

    void startGetDataFromFile_slot();

    void getDataFromDB_signal();

    void getDataFromFile_signal();    

    void getDataFromFile();

    void getDataFromDB();

private:
    ControlClass*           m_pControlObj;

    condition_variable      m_getDataFromFileCV;
    condition_variable      m_getDataFromDBCV;

    bool                    m_bGetDataFromFileStart;
    bool                    m_bGetDataFromDBStart;
    mutex                   m_mutex;
};

void TestControlCompute();


#endif // !_TIME_CLOCK_H_
