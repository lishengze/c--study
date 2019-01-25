#include "simpleClass.h"
#include "singletonHolder.h"
#include <thread>
#include <stdio.h>
using std::thread;

static SingletonHolder<SimpleClass> g_shSimpleClass;

SimpleClass::SimpleClass()
{
    m_value = 0;
    printf("SimpleClass m_value: %d, thread: %d \n", m_value,  std::this_thread::get_id());    
}

SimpleClass* SimpleClass::Instance()
{
    static SimpleClass* result = g_shSimpleClass.Instance();

    return result;
}