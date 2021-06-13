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

    // 完美包装： 能够完美的传递参数类型，包括引用，右值引用; 能够自动推导返回值类型；
    // 异步执行任务;
    // 函数返回值及时返回;

    // Submit a function to be executed asynchronously by the pool
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> // [1]
    {
        // Create a function with bounded parameter ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); // [2] 连接函数和参数定义，特殊函数类型，避免左右值错误
​
        // Encapsulate it into a shared pointer in order to be able to copy construct
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func); // [3]
​
        // Warp packaged task into void function
        std::function<void()> warpper_func = [task_ptr]()
        {
            (*task_ptr)();
        };  // [4]
​
        // 队列通用安全封包函数，并压入安全队列
        // caller_queue_.enqueue(warpper_func);
​
        // 唤醒一个等待中的线程
        // m_conditional_lock.notify_one();  ⑤
​
        // 返回先前注册的任务指针
        return task_ptr->get_future();
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