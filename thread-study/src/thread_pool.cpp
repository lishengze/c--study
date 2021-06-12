#pragma once

#include "thread_pool.h"

void print_data(string msg)
{
    cout << "print_data: " << msg << endl;
}


void test_simple_thread_pool()
{   
    cout << "test_simple_thread_pool " << endl;
    int thread_count = 3;

    ThreadPoolSimple simple_pool(thread_count);

    int test_count = 10;

    for (int i = 0; i < test_count; ++i)
    {
        CallerObj func = std::bind(print_data, std::to_string(i));

        simple_pool.post_call(func);
    }
}

void TestThreadPool()
{
    cout << "TestThreadPool " << endl;
    test_simple_thread_pool();
}