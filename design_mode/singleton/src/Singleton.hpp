#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <iostream>
using std::lock_guard;
using std::mutex;
using std::cout;
using std::endl;

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
        static void Initialize(Args&&... args)
        {
            m_unique_instance.reset(new T(std::forward<Args>(args)...));
        }

        template<typename... Args>
        static T& Instance(Args&&... args)
        {
            std::call_once(m_once_flag,  &Singleton<T>::Initialize<Args...>, std::forward<Args>(args)...);

            return *m_unique_instance;
        }  

        static T& GetInstance()
        {
            if (m_unique_instance == nullptr)
                throw std::logic_error("the instance is not init, please initialize the instance first");

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
