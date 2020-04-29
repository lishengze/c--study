#include "test.h"
#include "refreSingleton.h"
#include "hungrySingleton.h"
#include "simpleClass.h"
#include "singletonHolder.h"

#include <list>
#include <thread>
#include <string>
#include <iostream>

using std::list;
using std::thread;
using std::string;
using std::cout;
using std::endl;

class UserClass
{
    public:
        UserClass()
        {
            cout << "UserClass Constructor"<< endl;
        }

        ~UserClass()
        {
            cout << "UserClass Destructor" << endl;
        }
};

class TestSingleton
{
    public:
        TestSingleton(string singleType="RefreSingle");

        void createThread();

        void testCreateMultiSingle();

    private:
        mutex       m_mutex;
        string      m_singleType;
};

TestSingleton::TestSingleton(string singleType):
    m_singleType(singleType)
{
    testCreateMultiSingle();
}

void TestSingleton::createThread()
{
    // printf("value: %d, threadID: %d \n", RefreSingle::Instance().getValue());

    m_mutex.lock();

    printf("ThreadID: %d \n", std::this_thread::get_id());

    m_mutex.unlock();

    if (m_singleType == "RefreSingle")
    {
        RefreSingle::Instance();
    }

    if (m_singleType == "HungrySingleton")
    {
        HungrySingleton::Instance();
    }    

    if (m_singleType == "SingletonHolder")
    {
        SimpleClass::Instance();
    }

    if (m_singleType == "Singleton")
    {
        // Singleton<UserClass>::GetInstance();

        Singleton<UserClass>::DoubleCheckInstance();
    }
}

void TestSingleton::testCreateMultiSingle()
{
    int threadNumb = 5;
    list<thread*> threadList;
    for (int i = 0; i < threadNumb; ++i)
    {
        // printf("i: %d \n", i);
        thread* testThread = new thread(&TestSingleton::createThread, this);
        threadList.push_back(testThread);
    }

    for (list<thread*>::const_iterator it = threadList.begin();
        it != threadList.end(); ++it)
    {
        if ((*it)->joinable())
        {
            (*it)->join();
        }
    }

    for (list<thread*>::const_iterator it = threadList.begin();
        it != threadList.end(); ++it)
    {
        if (*it != nullptr )
        {
            delete *it;
        }
    }
}

void testRefreSingle()
{
    // string singleType = "SingletonHolder";

    // string singleType = "SingletonHolder";

    // string singleType = "SingletonHolder";

    string singleType = "Singleton";

    TestSingleton testObj(singleType);
}

void testMain()
{
    testRefreSingle();    
}