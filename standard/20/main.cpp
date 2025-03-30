#include <iostream>
#include <coroutine>
#include <future>
#include <thread>

#include "coroutine/test_coro.h"
#include "module/test_module.h"

// // 协程包装器
// template<typename T>
// struct Task {
//     struct promise_type;
//     using handle_type = std::coroutine_handle<promise_type>;

//     Task(handle_type h) : coro(h) {}
//     ~Task() {
//         if (coro) coro.destroy();
//     }

//     T get() {
//         coro.resume();
//         return coro.promise().result.get_future().get();
//     }

//     struct promise_type {
//         std::promise<T> result;

//         Task get_return_object() {
//             return {handle_type::from_promise(*this)};
//         }

//         std::suspend_never initial_suspend() { return {}; }
//         std::suspend_never final_suspend() noexcept { return {}; }

//         void unhandled_exception() {
//             result.set_exception(std::current_exception());
//         }

//         template<typename U>
//         void return_value(U&& value) {
//             result.set_value(std::forward<U>(value));
//         }
//     };

//     handle_type coro;
// };

// // 模拟异步操作
// Task<int> async_operation() {
//     co_return std::async([] {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         return 42;
//     }).get();
// }

int main() {
    // TestCoro();

    TestModule();

    return 0;
}