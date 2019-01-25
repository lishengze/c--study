#ifndef _SINGLETON_HOLDER_H_
#define _SINGLETON_HOLDER_H_

#include <mutex>
#include <thread>
using std::lock_guard;
using std::mutex;

template<class userClass>
class SingletonHolder
{
    public:
        SingletonHolder();
        
        userClass* Instance();

        userClass* m_Instance;

    private:
        
        mutex           m_mutex;
};

template<class userClass>
SingletonHolder<userClass>::SingletonHolder()
{
    m_Instance = nullptr;
}

template<class userClass>
userClass* SingletonHolder<userClass>::Instance()
{
    if (m_Instance == nullptr)
    {
        m_mutex.lock();

        if (m_Instance == nullptr)
        {
            m_Instance = new userClass();
        }

        m_mutex.unlock();
    }
    return m_Instance;
}

#endif // !_SINGLETON_HOLDER_H_
