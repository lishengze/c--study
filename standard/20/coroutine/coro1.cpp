#include "test_coro.h"

#include <iostream>
#include <coroutine>
#include <chrono>
#include <thread>
using namespace std;

class IntReader {
public:
    IntReader(int current) : current_(current) {}

    bool await_ready() {
        return false;
    }

    void await_suspend(coroutine_handle<> handle) {
        cout << "IntReader await_suspend start: current_ = " << current_ << endl;

        std::chrono::seconds delay(3);
        std::this_thread::sleep_for(delay);

        cout << "IntReader await_suspend end: current_ = " << current_ << endl;

        handle.resume();
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
                return Task{value_}; 
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
            }

            // final_suspend() is called to create the final suspend_never
            // object that will be used to suspend the coroutine execution.
            auto final_suspend() noexcept {
                cout << "promise_type final_suspend" << endl;
                return std::suspend_never{};
            }

            // unhandled_exception() is called to handle an exception that
            // was thrown from the coroutine execution.
            void unhandled_exception() {
                cout << "promise_type unhandled_exception" << endl;
            }


            int value_;

        };

    public:
        int value_;
};



Task PrintData() {
    cout << "TestCoro1" << endl;

    IntReader reader1(10);
    int value = co_await reader1;

    IntReader reader2(11);
    value += co_await reader2;
    
    IntReader reader3(12);
    value += co_await reader3;    

    cout << "value = " << value << endl;

    co_return value;
}

void TestCoro1() {
    auto task = PrintData();
    cout << "before co_await" << endl;
    int result = task.value_;
    cout << "after co_await, result = " << result << endl;
}