#include "condition_study.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "func_math.h"
using std::mutex;
using std::condition_variable;
using std::vector;
using std::thread;
using std::unique_lock;
using std::lock_guard;


class ConsumeProduce
{
public:
    ConsumeProduce();

    void Produce(int procudeValue=1);

    void Consume(int consumeValue=1); 

private:
    mutex               m_mutex;
    condition_variable  m_cv;
    int                 m_value;
};

ConsumeProduce::ConsumeProduce():m_value(0)
{

}


void ConsumeProduce::Produce(int procudeValue)
{
    printf("[BP] procudeValue: %d, m_value: %d \n", procudeValue, m_value);
    lock_guard<mutex> lg(m_mutex);
    m_value += procudeValue;
    
    printf("[P] procudeValue: %d, m_value: %d \n", procudeValue, m_value);

    m_cv.notify_all();
}

void ConsumeProduce::Consume(int consumeValue)
{
    printf("[BC] consumeValue: %d, m_value: %d \n", consumeValue, m_value);
    unique_lock<mutex> lk(m_mutex);
    while(m_value-consumeValue < 0)
    {
        m_cv.wait(lk);
    }
    m_value -= consumeValue;

    printf("[C] consumeValue: %d, m_value: %d \n", consumeValue, m_value);
}

void testConsumeProduce()
{
    int allNumb = 10;
    int procudeValue = 1;
    int consumeValue = 1;

    vector<thread> threadVec;
    ConsumeProduce cpObj;

    for (int i = 0; i < allNumb; ++i)
    {
        if (randBool())
        {
            thread produceObj = thread(&ConsumeProduce::Produce, &cpObj, procudeValue);
            threadVec.push_back(produceObj);
        }
        else
        {
            thread consumeObj = thread(&ConsumeProduce::Consume, &cpObj, consumeValue);
            threadVec.push_back(consumeObj);
        }
    }

    // for (vector<thread>::const_iterator it = threadVec.begin();
    // it != threadVec.end(); ++it)
    // {
    //     if (it->joinable())
    //     {
    //         (*it).join();
    //     }
    // }    

    // for (int i = 0; i < threadVec.size(); ++i)
    // {
    //     if (threadVec[i].joinable())
    //     {
    //         threadVec[i].join();
    //     }
    // }

}
