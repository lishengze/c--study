#include "HungrySingleton.h"
#include <mutex>
#include <thread>
using std::lock_guard;
using std::mutex;

HungrySingleton* HungrySingleton::m_sInstance = nullptr;
mutex HungrySingleton::m_mutex;

HungrySingleton* HungrySingleton::Instance()
{
    if (nullptr == m_sInstance)
    {
        lock_guard<mutex> lg(m_mutex);

        if (nullptr == m_sInstance)
        {
            m_sInstance = new HungrySingleton();
        }

    }
    return m_sInstance;
}

HungrySingleton::HungrySingleton()
{
    m_value = 0;
    printf("HungrySingleton m_value: %d, thread: %d \n", m_value,  std::this_thread::get_id());
}

