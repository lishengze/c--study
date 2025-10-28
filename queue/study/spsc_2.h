#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "util.h"
#include "struct.h"
#include "logger.h"

using namespace std;

// 原子变量的作用: 确保多线程环境下的原子操作,避免竞态条件;
// 内存序的作用: 1. 可见性，顺序性;

template<typename T>
class SPSCQueue_2 {
    public:
    SPSCQueue_2(size_t capacity) : _capacity(capacity), _buffer(new T[capacity]) {
        _head.store(0);
        _tail.store(0);
    }
     ~SPSCQueue_2() {
        delete[] _buffer;
    }

    bool enqueue(const T& item) {
        size_t tail = _tail.load(std::memory_order_relaxed); // 不存在另外线程的写操作;
        size_t next_tail = (tail + 1) % _capacity;


        size_t head = _head.load(std::memory_order_acquire); // acquire semantics, ensure that the head is up-to-date,读的下标修改一定能被看到;
        
        if (next_tail == head) {
            return false; // 代表已满
        }

        _buffer[tail] = item;
        _tail.store(next_tail, std::memory_order_release);  // 保证写的下标修改对其他线程可见;
        return true;
    }

    bool dequeue(T& item) {
    
        size_t head = _head.load(std::memory_order_relaxed);
        size_t tail = _tail.load(std::memory_order_acquire); // acquire semantics, ensure that the tail is up-to-date,写的下标修改一定能被看到;

        if (head == tail) {
            return false;
        }
        item = std::move(_buffer[head]);

        size_t next_head = (head + 1) % _capacity;
        _head.store(next_head, std::memory_order_release); // release semantics, ensure that the head is visible to other threads,写的下标修改对其他线程可见;

        return true;
    }


    private:
    std::atomic<size_t> _head;  // 读的下标
    std::atomic<size_t> _tail;  // 写的下标
    const size_t _capacity;
    T* _buffer;
};

std::vector<unsigned long long> gVecTime;

void TestSpscQueue_2() {
    LOG_INFO("---------- TEST SPSCQueue_2 START ---------");
    
    SPSCQueue_2<Block1> queue(10000);

    
    int iTestCount = 10000;
    gVecTime.resize(iTestCount, 0);
    for (int i = 0; i < iTestCount; ++i) {
        gVecTime[i] = 0;
    }

    // 消费者线程
    std::thread consumer([&queue, iTestCount]() {
        Block1 stOutputBlock1;
        for (int i = 0; i < iTestCount; ++i) {
            while(!queue.dequeue(stOutputBlock1)) {}
            // cout << "dequeue: push_time =  " << NanoToNanoString(stOutputBlock1.ulTime) << endl;
            gVecTime[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - stOutputBlock1.ulTime;
        }
    });    


    // 生产者线程
    std::thread producer([&queue, iTestCount]() {
        Block1 stInputBlock1;
        for (int i = 0; i < iTestCount; ++i) {
            stInputBlock1.ulTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            while(!queue.enqueue(stInputBlock1)) {
                stInputBlock1.ulTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    });
    
    // std::this_thread::sleep_for(std::chrono::microseconds(100));


    consumer.join();
    producer.join();

    LOG_INFO("{}", GetSimpleTimeData(gVecTime) );
}


