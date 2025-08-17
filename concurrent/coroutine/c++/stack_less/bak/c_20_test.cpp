std::coroutine_handle<> g_handle;

struct BaseSwapTestCoro

{

    struct awaiter {

        bool await_ready() { return false; }

        void await_suspend(std::coroutine_handle<> h) { g_handle = h; }

        void await_resume() {}

    };



    struct promise_type
    {
        BaseSwapTestCoro get_return_object() { return {}; }

        std::suspend_never initial_suspend() { return {}; }

        std::suspend_never final_suspend() noexcept { return {}; }

        void unhandled_exception() {}

        void return_void() {}
    };

};


BaseSwapTestCoro SomeFunc() {
    LOG(0, "in coroutine: before await");

    co_await BaseSwapTestCoro::awaiter();

    LOG(0, "in coroutine: after await");
}



TEST(base, swap) {
    SomeFunc();

    LOG(0, "in main: before resume");

    g_handle.resume();

    LOG(0, "in main: after resume");
}