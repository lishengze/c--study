#ifndef _SINGLETON_HOLDER_H_
#define _SINGLETON_HOLDER_H_

#include <mutex>
#include <thread>
#include <memory>
#include <iostream>

using std::lock_guard;
using std::mutex;
using std::cout;
using std::endl;

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

/*

1. 
*/

template<class T>
class Singleton
{
    public:

        template<typename... Args>
        static T& DoubleCheckInstance(Args&&... args)
        {
            if (nullptr == m_unique_instance)
            {
                std::lock_guard<std::mutex> lk(m_mutex);

                if (nullptr == m_unique_instance)
                {
                    m_unique_instance.reset(new T(std::forward<Args>(args)...));
                }
            }
            return *m_unique_instance;
        }

        template<typename... Args>
        static void Instance(Args&&... args)
        {
            m_unique_instance.reset(new T(std::forward<Args>(args)...));
        }

        template<typename... Args>
        static T& GetInstance(Args&&... args)
        {
            std::call_once(m_once_flag,  &Singleton<T>::Instance<Args...>, std::forward<Args>(args)...);

            return *m_unique_instance;
        }        

private:
    Singleton(void);
    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);

    static std::unique_ptr<T>   m_unique_instance;
    static mutex                m_mutex;
    static std::once_flag       m_once_flag;

};


template<class T> std::unique_ptr<T> Singleton<T>::m_unique_instance = nullptr;

template<class T> mutex Singleton<T>::m_mutex;

template<class T> std::once_flag Singleton<T>::m_once_flag;

#endif // !_SINGLETON_HOLDER_H_
