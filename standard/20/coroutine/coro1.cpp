#include "test_coro.h"

#include <iostream>
#include <coroutine>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
using namespace std;

class IntReader {
public:
    IntReader(int current) : current_(current) {}

    bool await_ready() {
        cout << "IntReader await_ready: current_ = " << current_ << endl;
        return false;
    }

    void await_suspend(coroutine_handle<> handle) {
        cout << "IntReader await_suspend start: current_ = " << current_ << endl;

        std::thread([handle]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "[Async] operation completed." << std::endl;
            // 恢复协程执行
            // handle.resume(); // 这里会导致死锁, 因为当前线程和协程在同一个线程上;
        }).detach();// 这里detach线程, 不会阻塞主线程;
        cout << "IntReader await_suspend end: current_ = " << current_ << endl;

        // handle.resume();
    }

    int await_resume() {
        cout << "IntReader await_resume: current_ = " << current_ << endl;
        return current_;
    }


private:
    int current_;
};

class Task {
    public:
        Task(int value) : value_(value) {
            cout << "Task constructor" << endl;
        }
        class promise_type {
            public:
            promise_type() {
                cout << "promise_type constructor" << endl;
            }

            // get_return_object() is called to create the coroutine_handle
            // that will be used to control the coroutine execution.
            Task get_return_object() {
                cout << "promise_type get_return_object" << endl;
                // return Task{value_}; 
                return Task{ std::coroutine_handle<promise_type>::from_promise(*this)};// 这里保存了协程句柄;

            }

            // void return_void() {
            //     cout << "promise_type return_void" << endl;
            // }

            void return_value(int value) {
                cout << "promise_type return_value" << endl;
                value_ = value;
            }

            // initial_suspend() is called to create the initial suspend_never
            // object that will be used to suspend the coroutine execution.
            auto initial_suspend() {
                cout << "promise_type initial_suspend" << endl;
                return std::suspend_never{};
                // return std::suspend_always{}; // 挂起当前协程, 直到其他协程恢复;
            }

            // final_suspend() is called to create the final suspend_never
            // object that will be used to suspend the coroutine execution.
            auto final_suspend() noexcept {
                cout << "promise_type final_suspend" << endl;
                // return std::suspend_never{};// 挂起当前协程, 直到其他协程恢复;
                return std::suspend_always{}; // 挂起当前协程, 直到其他协程恢复;
            }

            // unhandled_exception() is called to handle an exception that
            // was thrown from the coroutine execution.
            void unhandled_exception() {
                cout << "promise_type unhandled_exception" << endl;
            }


            int value_;

        };

        ~Task() {
            // if (coro_handle) coro_handle.destroy(); // 不能重复销毁
            if (coro_handle && coro_handle.done() == true) {
                coro_handle.destroy(); // 销毁协程句柄;
                cout << "Task destructor: coroutine done, destroy" << endl;
            } else {
                cout << "Task destructor: coroutine not done, not destroy" << endl;
            }
        }        

        explicit Task(std::coroutine_handle<promise_type> handle) : coro_handle(handle) {
            cout << "Task constructor coroutine_handle" << endl;
        }    
        
        explicit Task(int value, std::coroutine_handle<promise_type> handle) : value_(value), coro_handle(handle) {
            cout << "Task constructor coroutine_handle: value_ = " << value_ << endl;
        }       
        
        Task(const Task&) = delete;
        Task& operator=( Task& other) {
            cout << "Task Operator= " << endl;
            if (this!= &other) {
                coro_handle = other.coro_handle;
                other.coro_handle = nullptr;
            }
            return *this;
        }
        Task(Task&& other) noexcept : coro_handle(other.coro_handle) {
            cout << "Task move constructor move" << endl;
            other.coro_handle = nullptr;
        }
        Task& operator=(Task&& other) noexcept {
            cout << "Task Operator= move" << endl;
            if (this!= &other) {
                coro_handle = other.coro_handle;
                other.coro_handle = nullptr;
            }
            return *this;
        }
            
    public:
        int value_;
        std::coroutine_handle<promise_type> coro_handle; // handle for the coroutine
};


// 就是一个协程函数;
Task PrintData() {
    cout << "\n--------PrintData" << endl;

    IntReader reader1(10);
    cout << "before co_await" << endl;
    int value = co_await reader1;
    cout << "reader1 end!, value = " << value << "\n" << endl;

    IntReader reader2(11);
    value += co_await reader2;
    cout << "reader2 end!, value = " << value << "\n" << endl;
    
    IntReader reader3(12);
    value += co_await reader3;    
    cout << "reader3 end!, value = " << value << "\n" << endl;

    // cout << "value = " << value << endl;

    co_return value;
}



void TestCoro1() {
    auto task = PrintData();
    cout << "before co_await" << endl;
    int result = task.value_;
    cout << "after co_await, result = " << result << endl;

    // std::this_thread::sleep_for(std::chrono::seconds(3));

    while( task.coro_handle.done() == false ) {
        cout << "\n-----------\nbefore resume" << endl;
        task.coro_handle.resume(); 
        cout << "after resume" << endl;
    }
    

    // auto handle = std::coroutine_handle<Task::promise_type>::from_promise(task.promise_);
}

struct Generator {
    struct promise_type {
        int current_value;
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
    };

    std::coroutine_handle<promise_type> coro_handle; // handle for the coroutine

    // Constructor and destructor are required by the standard
    explicit Generator(std::coroutine_handle<promise_type> handle) : coro_handle(handle) {
        cout << "Generator constructor" << endl;
    }
    ~Generator() {
        if (coro_handle) coro_handle.destroy();
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : coro_handle(other.coro_handle) {
        other.coro_handle = nullptr;
    }
    Generator& operator=(Generator&& other) noexcept {
        if (this!= &other) {
            coro_handle = other.coro_handle;
            other.coro_handle = nullptr;
        }
        return *this;
    }

    bool move_next() {
        if (!coro_handle.done()) { // coroutine is not done yet,协程还在挂起;
            cout << "move_next: current_value = " << coro_handle.promise().current_value << endl;
            coro_handle.resume(); // resume the coroutine,恢复当前协程;
            return true;
        }
        return false;
    }
    int current_value() const {
        return coro_handle.promise().current_value;
    }
};

Generator counter() {
    for (int i = 0; i < 5; ++i) {
        co_yield i;
    }
    co_return;
}

void TestCoro2() {
    auto gen = counter();
    while (gen.move_next()) {
        cout << "current_value = " << gen.current_value() << endl;
    }
}

// 用于表示文件读取结果的结构体
struct FileReadResult {
    bool ready;
    std::string line;
};

// 实现awaitable接口的类，用于控制协程在文件读取时的挂起和恢复
// struct FileReadAwaiter {
//     FileReadResult& result;
//     std::ifstream& file;
//     FileReadAwaiter(FileReadResult& res, std::ifstream& f) : result(res), file(f) {}
//     bool await_ready() const noexcept { return result.ready; }
//     void await_suspend(std::coroutine_handle<> handle) const noexcept {
//         // 启动一个新线程模拟异步文件读取操作
//         std::thread([this, handle]() {
//             if (std::getline(file, result.line)) {
//                 result.ready = true;
//             }
//             handle.resume();
//         }).detach();
//     }
//     void await_resume() const noexcept {}
// };

// // 协程函数，用于异步读取文件
// std::coroutine_handle<> readFile() {
//     std::ifstream file("example.txt");
//     if (file.is_open()) {
//         FileReadResult result = { false };
//         while (true) {
//             // 使用co_await等待文件读取完成，协程在此处挂起，不阻塞线程
//             // co_await FileReadAwaiter(result, file); 
//             if (result.ready) {
//                 std::cout << result.line << std::endl;
//             } else {
//                 break;
//             }
//         }
//         file.close();
//     } else {
//         std::cerr << "无法打开文件" << std::endl;
//     }
//     co_return;
// }

int TestCoro3() {
    // auto handle = readFile();
    // 这里可以继续执行其他任务，而不必等待文件读取完成
    // 假设其他任务耗时 1 秒
    // std::this_thread::sleep_for(std::chrono::seconds(1)); 
    // handle.resume();
    return 0;
}