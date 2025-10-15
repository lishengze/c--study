#include "spsc_1.h"
#include "test.h"

#include <thread>
#include <iostream>
#include <chrono>

int test_spsc_1() {
    SPSCQueue<int> queue(100);  // 创建容量为100的SPSC队列
    
    // 生产者线程
    std::thread producer([&queue]() {
        for (int i = 0; i < 1000; ++i) {
            // 等待队列有空间
            while (!queue.enqueue(i)) {
                std::this_thread::yield();
            }
            if (i % 100 == 0) {
                std::cout << "Produced: " << i << std::endl;
            }
        }
    });
    
    // 消费者线程
    std::thread consumer([&queue]() {
        int value;
        for (int i = 0; i < 1000; ++i) {
            // 等待队列有数据
            while (!queue.dequeue(value)) {
                std::this_thread::yield();
            }
            if (i % 100 == 0) {
                std::cout << "Consumed: " << value << std::endl;
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    return 0;
}