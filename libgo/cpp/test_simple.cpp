#include "test_simple.h"
#include "coroutine.h"

#include "pandora/util/time_util.h"

void go_func0 () 
{
    while(true)
    {
        // co_sleep(3000);

        std::this_thread::sleep_for(std::chrono::seconds(2));

        cout << "go_func0: " << std::this_thread::get_id()  << ", " << utrade::pandora::NanoTimeStr() <<  endl;
    }
    
}

void go_func1 () 
{
    // while(true)
    // {
    //     // co_sleep(3000);

    //     std::this_thread::sleep_for(std::chrono::seconds(3));

    //     go go_func0;

    //     cout << "go_func1: " << std::this_thread::get_id()  << ", " << utrade::pandora::NanoTimeStr() <<  endl;
    // }


        std::this_thread::sleep_for(std::chrono::seconds(3));

        go go_func0;

        cout << "go_func1: " << std::this_thread::get_id()  << ", " << utrade::pandora::NanoTimeStr() <<  endl;    
    
}

void go_func2() 
{
    while(true)
    {
        // co_sleep(3000);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        cout << "go_func2: " << std::this_thread::get_id()  << ", " << utrade::pandora::NanoTimeStr() <<  endl;
    }
}

void test1() { 

    cout << "test1.main_thread:  " << std::this_thread::get_id() << endl;

    go go_func1;
    go go_func2;

    // std::thread sched_thread = std::thread([](){ co_sched.Start(0, 1024);});
    // sched_thread.detach();

    co_sched.Start();

    cout << "test1 over" << endl;
}

void go_func(int i ) {
    cout << "go_func1: " << std::this_thread::get_id()  << ", " << utrade::pandora::NanoTimeStr() <<  endl;
}

void test_thread() {
    int coroutine_count = 2;
    for (int i = 0; i < coroutine_count; ++i) {
        go std::bind(go_func, i);
    }

    co_sched.Start();

    cout << "test_thread over" << endl;    
}

void TestSimple()
{
    test1();

    // test_thread();
}