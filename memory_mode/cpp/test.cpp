#include "test.h"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

atomic<int> relaxed_count{0};

// int relaxed_count{0};

void compute_count()
{
    for (int i=0;i < 1000; ++i)
    {
        relaxed_count.fetch_add(1, std::memory_order_relaxed);
        // relaxed_count+= 1;
    }
}

void test_relaxed()
{
    int thread_count = 20000;

    std::vector<thread> thread_vec;

    for(int i=0; i<thread_count; ++i)
    {
        thread_vec.emplace_back(compute_count);
    }

    for(int i=0; i<thread_vec.size(); ++i)
    {
        if (thread_vec[i].joinable())
        {
            thread_vec[i].join();
        }
    }

    if (relaxed_count != 1000 * thread_count) 
    {
        cout << "compute error: " << relaxed_count << endl;
    }
    else
    {
        cout << "compute right: " << relaxed_count << endl;
    }
}

void TestMain() 
{
    test_relaxed();
}