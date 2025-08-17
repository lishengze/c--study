#ifndef _TIME_CLOCK_H_
#define _TIME_CLOCK_H_

#include <thread>
#include <mutex>
#include <condition_variable>

using std::mutex;
using std::condition_variable;


class TimeClock
{
    public:

        void start();

        void work();

        void setStopWorkFlag(bool value);

    private:
        unsigned int     m_tickMSecs;
        std::mutex       m_mutex;
        bool             m_bStopWork;
};


#endif // !_TIME_CLOCK_H_
