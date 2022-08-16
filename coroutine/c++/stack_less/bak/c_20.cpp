

void func() {

    if (!awaiter.await_ready()) {

    using handle_t = std::experimental::coroutine_handle<P>;
    using await_suspend_result_t = decltype(awaiter.await_suspend(handle_t::from_promise(p)));

    }
    // <suspend-coroutine>

    if constexpr (std::is_void_v<await_suspend_result_t>) {
        awaiter.await_suspend(handle_t::from_promise(p));
        // <return-to-caller-or-resumer>
    } else {
        static_assert(std::is_same_v<await_suspend_result_t, bool>, "await_suspend() must return 'void' or 'bool'.");
    }

    if (awaiter.await_suspend(handle_t::from_promise(p))) {
        // <return-to-caller-or-resumer>
    }

        // <resume-point>
    return awaiter.await_resume();
}




