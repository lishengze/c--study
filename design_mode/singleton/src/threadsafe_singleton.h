#pragma once

#include <memory>
#include <mutex>
#include <functional>
#include "global_declare.h"

template <typename T>
class SingletonThreadSafe
{
public:
    template<typename... Args>
    static std::shared_ptr<T> Instance(Args&&... args)
    {
        if(m_pInstance == nullptr)
        {
            cout << "Instance" << endl;
            m_pInstance = std::make_shared<T>(std::forward<Args>(args)...);
        }            

        return m_pInstance;
    }

    template<typename... Args>
    static std::shared_ptr<T> GetInstance(Args&&... args)
    {
        if (m_pInstance == nullptr)
        {
            cout << "GetInstance" << endl;
            // std::call_once(m_create_instance_flag, &SingletonThreadSafe<T>::Instance<Args&&...>, std::forward<Args>(args)...);

            std::call_once(m_create_instance_flag, &(SingletonThreadSafe<T>::Instance<Args&&...>), args...);
        }
        return m_pInstance;
    }

    // static void DestroyInstance()
    // {
    //     delete m_pInstance;
    //     m_pInstance = nullptr;
    // }

private:
        
    SingletonThreadSafe(void);
    virtual ~SingletonThreadSafe(void);
    SingletonThreadSafe(const SingletonThreadSafe&);
    SingletonThreadSafe& operator=(const SingletonThreadSafe&);

    static std::once_flag       m_create_instance_flag;        
    static std::shared_ptr<T>   m_pInstance;
};

template <typename T> std::shared_ptr<T> SingletonThreadSafe<T>::m_pInstance = nullptr;
template <typename T> std::once_flag SingletonThreadSafe<T>::m_create_instance_flag;