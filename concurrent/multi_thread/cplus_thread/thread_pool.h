#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <list>
#include <future>
#include <functional>
#include <vector>
using namespace std;

using CallerObj = std::function<void()>;
using threadPtr = std::shared_ptr<std::thread>;

enum class SYNC_MODE {
    MUTEX = 0,
    ATOMIC = 1
};


class ThreadPoolSimple
{
    public:
        ThreadPoolSimple(unsigned int thread_count):
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

        ThreadWorker(ThreadPoolSimple* pool, unsigned int pool_id):
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
        unsigned int      pool_id_{0};


    };

    using ThreadWorkerPtr = std::shared_ptr<ThreadWorker>;

    void init_work_thread()
    {
        for (int i = 0; i < thread_count_; ++i)
        {
            ThreadWorkerPtr cur_worker = std::make_shared<ThreadWorker>(this, i);
            threadPtr cur_thread = std::make_shared<std::thread>(&ThreadWorker::work, cur_worker.get());

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
        // Encapsulate it into a shared pointer in order to be able to copy construct
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func); // [3]
        // Warp packaged task into void function
        std::function<void()> warpper_func = [task_ptr]()
        {
            (*task_ptr)();
        };  // [4]
        // 队列通用安全封包函数，并压入安全队列
        caller_queue_.push(warpper_func);
        // 唤醒一个等待中的线程
        caller_condition_.notify_one();
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


class ThreadPoolUsr
{
    public:
        ThreadPoolUsr(unsigned int thread_count = 0, SYNC_MODE sync_mode = SYNC_MODE::MUTEX): thread_count_{thread_count}, sync_mode_{sync_mode}
        {
            
            if (thread_count_ == 0) {
                thread_count_ = std::thread::hardware_concurrency();
            }
            cout << "thread_count: " << thread_count_ << endl;

            for (int i = 0; i < thread_count_; ++i)
            {
                threadPtr cur_thread = std::make_shared<std::thread>(&ThreadPoolUsr::work_loop_by_mutex, this);
                threads_.push_back(cur_thread);
            }
        }

        // 构建每个线程的工作循环，并启动线程；
        // 循环中不断从任务队列中取出任务，并执行；
        // 循环中不断检查是否需要退出；
        void work_loop_by_mutex() {
            while (is_shut_down_ == false) {
                CallerObj task = nullptr;

                {
                    // 互斥锁保护任务队列；
                    std::unique_lock<std::mutex> lock(caller_mutex_);

                    caller_condition_.wait(lock, [&](){
                        return !caller_queue_.empty();
                    });

                    task = std::move(caller_queue_.front());  // 使用移动语义，避免拷贝构造；
                    caller_queue_.pop_front();

                    lock.unlock();
                }

                if (task) {
                    // cout << "thread [" << std::this_thread::get_id() << "] start execute task" << endl;
                    task();                    
                } else {
                    cout << "thread " << std::this_thread::get_id() << " exit" << endl;
                }
            }
        }

        void work_loop_by_atomic() {
            while (is_shut_down_ == false) {
            
                
            
            }
        }
    
        /// @brief Submit a function to be executed asynchronously by the pool
        /// @tparam F -- 函数类型
        /// @tparam Args -- 函数参数类型
        template <typename F, typename... Args>
        void submit(F &&f, Args &&...args) {
            if (is_shut_down_) {
                cout << "ThreadPoolUsr is shut down, ignore submit" << endl;
                return;
            }

            // 原始任务（如 std::bind 的结果）被存储在 std::shared_ptr<std::function<void()>> 中，引用计数管理生命周期；
            auto task = std::make_shared<CallerObj>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            auto task_simple = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

            if (sync_mode_ == SYNC_MODE::MUTEX) {
                // cout << "mutex submit" << endl;
                std::lock_guard<std::mutex> lock(caller_mutex_);

                // caller_queue_.push_back([task](){
                //     (*task)(); // 确保在 lambda 执行前，task 指向的任务对象不会被销毁（引用计数至少为 1）；
                // });

                caller_queue_.push_back(task_simple);

                caller_condition_.notify_one();

            } else if (sync_mode_ == SYNC_MODE::ATOMIC) {
                // 原子操作
                cout << "atomic submit" << endl;
            }
        }


        ~ThreadPoolUsr()
        {
            // 停止当前工作;
            is_shut_down_ = true;

            caller_condition_.notify_all();


            // 通知所有线程退出
            for (auto& thread_atom: threads_) {
                if (thread_atom->joinable()) {
                    thread_atom->join();
                }
            }
        }
    
    
        unsigned int                thread_count_;
        std::vector<threadPtr>      threads_;
        std::deque<CallerObj>       caller_queue_;
        std::mutex                  caller_mutex_;
        std::condition_variable     caller_condition_;   
        bool is_shut_down_          = false;
        SYNC_MODE sync_mode_;
};


void TestThreadPool();