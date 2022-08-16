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


atomic<string*> g_s_data{nullptr};
int g_d_data{0};

void producer_con()
{
    int g_d_data = 100;
    string* ptr = new string("hello");
    g_s_data.store(ptr, std::memory_order_release);
}

void consumer_con()
{
    string* cur_data{nullptr};

    while(!(cur_data = g_s_data.load(std::memory_order_consume))); // A, 第一个读；

    cout << "cur_data: " << *cur_data << endl;  // B, cur_data 和 g_s_data 关联, 所以 cur_data 一定是在 g_s_data.load 之后执行；
    cout << "g_d_data: " << g_d_data << endl;  // C, g_d_data 和 g_s_data 没有关联，所以不一定 在 A 之后执行; 对于 X86, TSO ;
}

void test_consume()
{
    thread consumer_thread = thread(consumer_con);
    thread producer_thread = thread(producer_con);

    consumer_thread.join();
    producer_thread.join();
}

atomic_bool x{false};
atomic_bool y{false};
atomic_int z{0};

void load_x() 
{
    x.store(true, std::memory_order_seq_cst);
}

void load_y()
{
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y()
{
    while( !x.load(std::memory_order_seq_cst));

    if (y.load(std::memory_order_seq_cst)) z++; // 不用 seq_cst acquire 就够了; 不让下面的提前就够了；
}

void read_y_then_x()
{
    while( !y.load(std::memory_order_seq_cst)); // 不用 seq_cst acquire 就够了; 不让下面的提前就够了；

    if (x.load(std::memory_order_seq_cst)) z++;    
}

void test_seq_cst()
{

}

volatile int g_v_data{0};
int g_test_v_count{1000000};

void minus_func()
{
    for (int i=0; i<g_test_v_count; ++i) ++g_v_data;
}

void add_func()
{
    for (int i=0; i<g_test_v_count; ++i) --g_v_data;
}

void test_volatile()
{
    thread minus_thread = thread(minus_func);
    thread add_thread = thread(add_func);

    minus_thread.join();
    add_thread.join();

    cout << "g_v_data: " << g_v_data << endl;
}

void TestMain() 
{
    // test_relaxed();

    // test_consume();

    test_volatile();
}