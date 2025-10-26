#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "util.h"
#include "struct.h"

/// @brief 多写多读的固定容量的无锁队列
///核心问题：解决多线程写的场景下的，线程安全的更新 写入索引和 读取索引 和 容量判断问题

template<typename T>
class MPMCQueue {

    public:
        MPMCQueue(unsigned int uiCapacity) : uiCapacity_(uiCapacity) {
            pBuffer_ = new T[uiCapacity_];
        }

        ~MPMCQueue() {
            delete[] pBuffer_;
        }

        /// @brief 入队操作
        /// @param tValue 要入队的值
        /// @return true 入队成功
        /// @return false 入队失败
        bool enqueue(const T& tValue) {
            unsigned int uiCurrentCount;
            do {
                uiCurrentCount = atouiCount_.load(std::memory_order_acquire);
                if (uiCurrentCount >= uiCapacity_) {
                    return false;
                }                
            } while(!atouiCount_.compare_exchange_weak(uiCurrentCount, uiCurrentCount + 1, 
                                                        std::memory_order_release, std::memory_order_acquire));    
                                                        // 入队时判断队列是否已满


            unsigned int uiCurrentWriteIndex = atouiWriteIndex_.load(std::memory_order_relaxed);
            do {
                uiCurrentWriteIndex = atouiWriteIndex_.load(std::memory_order_relaxed);
            }while (!atouiWriteIndex_.compare_exchange_weak(uiCurrentWriteIndex, (uiCurrentWriteIndex+1) % uiCapacity_, 
                                                        std::memory_order_release, std::memory_order_acquire));

            pBuffer_[uiCurrentWriteIndex] = tValue;

            return true;
        }

        /// @brief 出队操作
        /// @param tValue 出队的值
        /// @return true 出队成功
        /// @return false 出队失败
        bool dequeue(T& tValue) {
            unsigned int uiCurrentCount;
            do {
                uiCurrentCount = atouiCount_.load(std::memory_order_acquire);
                if (uiCurrentCount <= 0) {
                    return false;
                }                
            } while(!atouiCount_.compare_exchange_weak(uiCurrentCount, uiCurrentCount - 1, 
                                                        std::memory_order_release, std::memory_order_acquire));    

            unsigned int uiCurrentReadIndex = atouiReadIndex_.load(std::memory_order_relaxed);
            do {
                uiCurrentReadIndex = atouiReadIndex_.load(std::memory_order_relaxed);
            }while (!atouiReadIndex_.compare_exchange_weak(uiCurrentReadIndex, (uiCurrentReadIndex+1) % uiCapacity_, 
                                                        std::memory_order_release, std::memory_order_acquire));

            tValue = pBuffer_[uiCurrentReadIndex];

            return true;
        }

    private:

    unsigned int uiCapacity_;       // 队列的容量
    std::atomic<unsigned int> atouiCount_{0};       // 队列中当前元素的数量
    std::atomic<unsigned int> atouiWriteIndex_{0};       // 写入索引
    std::atomic<unsigned int> atouiReadIndex_{0};       // 读取索引

    T* pBuffer_;
};

std::vector<unsigned long long> gMpmc1VecTime;

void TestMpmc_2() {
    MPMCQueue<Block1> queue(10000);

    
    int iTestCount = 10000;
    gMpmc1VecTime.resize(iTestCount, 0);
    for (int i = 0; i < iTestCount; ++i) {
        gMpmc1VecTime[i] = 0;
    }

    // 消费者线程
    std::thread consumer([&queue, iTestCount]() {
        Block1 stOutputBlock1;
        for (int i = 0; i < iTestCount; ++i) {
            while(!queue.dequeue(stOutputBlock1)) {}
            // cout << "dequeue: push_time =  " << NanoToNanoString(stOutputBlock1.ulTime) << endl;
            gMpmc1VecTime[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - stOutputBlock1.ulTime;
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

    std::cout << GetSimpleTimeData(gMpmc1VecTime) << std::endl;

}


