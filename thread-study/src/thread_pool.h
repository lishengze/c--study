#pragma once

#include "global_declare.h"

using CallerObj = std::function<void()>;
using threadPtr = boost::shared_ptr<std::thread>;

class ThreadPoolSimple
{
    public:
        ThreadPoolSimple(int thread_count):
        thread_count_{thread_count}
        {
            cout << "thread_count: " << thread_count_ << endl;
            init_work_thread();
        }

        ~ThreadPoolSimple()
        {
            block();
        }

    class ThreadWorker
    {
        public:

        ThreadWorker(ThreadPoolSimple* pool, int pool_id):
            pool_{pool}, pool_id_{pool_id}
        {

        }

        void work()
        {
            while (pool_->is_shut_down_ == false)
            {
                CallerObj caller;
                {
                    std::unique_lock<std::mutex> lock(pool_->caller_mutex_);

                    pool_->caller_condition_.wait(lock, [&]()
                    {
                        return !pool_->caller_queue_.empty();
                    });

                    // CallerObj
                    
                    caller = pool_->caller_queue_.front();

                    pool_->caller_queue_.pop();
                }

                cout << "Thread " << pool_id_ << " execute caller" << endl;
                caller();
                cout << endl;
            }
        }


        ThreadPoolSimple* pool_{nullptr};
        int               pool_id_{-1};


    };

    using ThreadWorkerPtr = boost::shared_ptr<ThreadWorker>;

    void init_work_thread()
    {
        for (int i = 0; i < thread_count_; ++i)
        {
            ThreadWorkerPtr cur_worker = boost::make_shared<ThreadWorker>(this, i);
            threadPtr cur_thread = boost::make_shared<std::thread>(&ThreadWorker::work, cur_worker.get());

            thread_list_.push_back(cur_thread);
            thread_worker_list_.push_back(cur_worker);
        }


    }

    void block()
    {
        for (auto& thread_atom: thread_list_)
        {
            if (thread_atom->joinable())
            {
                thread_atom->join();
            }
        }
    }

    void shut_down()
    {
        caller_condition_.notify_all();
        is_shut_down_ = true;
    }

    void post_call(CallerObj& caller)
    {
        std::unique_lock<std::mutex> lock(caller_mutex_);

        caller_queue_.push(caller);

        cout << "caller_queue_.size: " << caller_queue_.size() << endl;

        caller_condition_.notify_one();
    }


    std::queue<CallerObj>      caller_queue_;

    std::list<threadPtr>       thread_list_;

    std::list<ThreadWorkerPtr> thread_worker_list_;

    std::mutex                 caller_mutex_;

    std::condition_variable    caller_condition_;   

    bool                       is_shut_down_{false};   

    unsigned int               thread_count_{0};


};



void TestThreadPool();