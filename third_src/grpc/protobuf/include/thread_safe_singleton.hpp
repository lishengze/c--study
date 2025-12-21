#pragma once

#include <mutex>
#include <thread>
#include <memory>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using std::lock_guard;
using std::mutex;
using std::cout;
using std::endl;

template<class T>
class ThreadSafeSingleton
{
    public:

        template<typename... Args>
        static boost::shared_ptr<T>& DoubleCheckInstance(Args&&... args)
        {
            if (nullptr == m_unique_instance)
            {
                std::lock_guard<std::mutex> lk(m_mutex);

                if (nullptr == m_unique_instance)
                {
                    // m_unique_instance.reset(new T(std::forward<Args>(args)...));

                    m_unique_instance = boost::make_shared<T>(std::forward<Args>(args)...);
                }
            }
            return m_unique_instance;
        }

        template<typename... Args>
        static void Initialize(Args&&... args)
        {
            m_unique_instance.reset(new T(std::forward<Args>(args)...));
        }

        template<typename... Args>
        static boost::shared_ptr<T>& Instance(Args&&... args)
        {
            std::call_once(m_once_flag,  &ThreadSafeSingleton<T>::Initialize<Args...>, std::forward<Args>(args)...);

            return m_unique_instance;
        }  

        static boost::shared_ptr<T>& GetInstance()
        {
            if (m_unique_instance == nullptr)
                throw std::logic_error("the instance is not init, please initialize the instance first");

            return m_unique_instance;
        }        

private:
    ThreadSafeSingleton(void);
    ThreadSafeSingleton(const ThreadSafeSingleton&);
    ThreadSafeSingleton& operator = (const ThreadSafeSingleton&);

    static boost::shared_ptr<T>   m_unique_instance;
    static mutex                m_mutex;
    static std::once_flag       m_once_flag;

};


template<class T> boost::shared_ptr<T> ThreadSafeSingleton<T>::m_unique_instance = nullptr;

template<class T> mutex ThreadSafeSingleton<T>::m_mutex;

template<class T> std::once_flag ThreadSafeSingleton<T>::m_once_flag;
