#pragma once

#include "thread_pool.h"
#include <string>

#include<chrono>
#include <atomic>

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

void TestThreadPoolUsr()
{
    cout << "TestThreadPool " << endl;
    test_simple_thread_pool();
}

std::atomic<int> g_count{0};

void AddGlobalCount(int& count) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // g_count.fetch_add(count);
    std::string msg = "AddGlobalCount " + std::to_string(count) + ", Ori: " + std::to_string(g_count) + ", New: " + std::to_string(g_count.fetch_add(count)) + "\n";
    cout << msg << endl;
}

void test_usr_thread_pool()
{
    ThreadPoolUsr pool;

    int test_count = 10;
    for (int i = 0; i < test_count; ++i)
    {
        pool.submit(AddGlobalCount, i);
    }
    

}

void TestThreadPool()
{
    test_usr_thread_pool();
    
}