#pragma once

#include "global_declare.h"

using CallerObj = std::function<void()>;

class ThreadPoolSimple
{
    public:

    class ThreadWorker
    {
        ThreadWorker(ThreadPoolSimple* pool, int pool_id):
            pool_{pool}, pool_id_{pool_id}
        {

        }

        void work()
        {
            if (!pool_->shut_down())
            {
                while (true)
                {
                    CallerObj caller;
                    {
                        std::unique_lock<std::mutex> lock(caller_mutex_);

                        caller_condition_.wait(lock, [&]()
                        {
                            return !caller_queue_.empty();
                        });

                        // CallerObj

                        caller = caller_queue_.front();

                        caller_queue_.pop();
                    }

                    caller();
                }
            }
        }


        ThreadPoolSimple* pool_{nullptr};
        int               pool_id_{-1};

        std::queue<CallerObj>      caller_queue_;

        std::mutex                 caller_mutex_;

        std::condition_variable    caller_condition_;  
    };

    bool shut_down()
    {
        bool result;

        return result;
    }
};



void TestThreadPool();