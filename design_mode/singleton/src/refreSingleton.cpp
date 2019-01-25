#include "refreSingleton.h"
#include "stdio.h"
#include <thread>
#include <list>
#include <mutex>
using std::mutex;
using std::list;
using std::thread;

RefreSingle::RefreSingle()
{
    m_value = 0;
    printf("RefreSingle m_value: %d, thread: %d \n", m_value,  std::this_thread::get_id());
}

RefreSingle& RefreSingle::Instance()
{
    static RefreSingle object;
    return object;
}

int RefreSingle::getValue()
{
    return m_value;
}
