#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "util.h"
#include "struct.h"
#include "logger.h"

/// @brief 多写多读的固定容量的无锁队列
///核心问题：解决多线程写的场景下的，线程安全的更新 写入索引和 读取索引 和 容量判断问题

template<typename T>
class MPMCQueue {

    public:
        MPMCQueue(unsigned int uiCapacity) : uiCapacity_(uiCapacity),atouiReadIndex_(0),atouiWriteIndex_(0),atouiCount_(0) {
            pBuffer_ = new T[uiCapacity_](); // 添加括号进行值初始化
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

            // 获取当前写入索引，但暂不更新
            unsigned int uiCurrentWriteIndex = atouiWriteIndex_.load(std::memory_order_relaxed);
            unsigned int uiNextWriteIndex;
            
            // // 等待获取写入位置
            // do {
            //     uiNextWriteIndex = (uiCurrentWriteIndex + 1) % uiCapacity_;
            // } while (!atouiWriteIndex_.compare_exchange_weak(uiCurrentWriteIndex, uiNextWriteIndex,
            //                                             std::memory_order_relaxed, std::memory_order_relaxed));

            // 等待获取写入位置
            do {
                uiNextWriteIndex = (uiCurrentWriteIndex + 1) % uiCapacity_;
            } while (!atouiWriteIndex_.compare_exchange_strong(uiCurrentWriteIndex, uiNextWriteIndex));                                                        

            // 写入数据到缓冲区
            pBuffer_[uiCurrentWriteIndex] = tValue;
            
            // 使用内存屏障确保数据写入对其他线程可见，并且在索引更新之后
            std::atomic_thread_fence(std::memory_order_release);

            return true;
        }

        bool dequeue(T& tValue) {
            unsigned int uiCurrentCount;
            do {
                uiCurrentCount = atouiCount_.load(std::memory_order_acquire);
                if (uiCurrentCount <= 0) {
                    return false;
                }
            } while(!atouiCount_.compare_exchange_weak(uiCurrentCount, uiCurrentCount - 1,
                                                        std::memory_order_release, std::memory_order_acquire));

            // 获取当前读取索引，但暂不更新
            unsigned int uiCurrentReadIndex = atouiReadIndex_.load(std::memory_order_relaxed);
            unsigned int uiNextReadIndex;
            
            // // 等待获取读取位置
            // do {
            //     uiNextReadIndex = (uiCurrentReadIndex + 1) % uiCapacity_;
            // } while (!atouiReadIndex_.compare_exchange_weak(uiCurrentReadIndex, uiNextReadIndex,
            //                                             std::memory_order_relaxed, std::memory_order_relaxed));

            // 等待获取读取位置
            do {
                uiNextReadIndex = (uiCurrentReadIndex + 1) % uiCapacity_;
            } while (!atouiReadIndex_.compare_exchange_strong(uiCurrentReadIndex, uiNextReadIndex));


            // 使用内存屏障确保读取到最新的数据，并且在索引更新之后
            std::atomic_thread_fence(std::memory_order_acquire);
            
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

void TestMpmc_1() {
    MPMCQueue<Block1> queue(10000);

    
    int iTestCount = 1000;
    gMpmc1VecTime.resize(iTestCount, 0);
    for (int i = 0; i < iTestCount; ++i) {
        gMpmc1VecTime[i] = 0;
    }

    // 消费者线程
    std::thread consumer([&queue, iTestCount]() {
        int iEmptyCount = 0;
        Block1 stOutputBlock1;
        int iValidCount = 0;
        for (int i = 0; i < iTestCount; ++i) {
            while(!queue.dequeue(stOutputBlock1)) {}
            // LOG_DEBUG("dequeue: push_time =  {}", NanoToNanoString(stOutputBlock1.ulTime));
            if (stOutputBlock1.ulTime == 0) {
                iEmptyCount++;
            } else {
                gMpmc1VecTime[iValidCount++] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - stOutputBlock1.ulTime;
            }            

        }

        LOG_INFO("dequeue: empty_count = {}", iEmptyCount);
    });    


    // 生产者线程
    std::thread producer([&queue, iTestCount]() {
        Block1 stInputBlock1;
        for (int i = 0; i < iTestCount; ++i) {
            stInputBlock1.ulTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            // LOG_DEBUG("enqueue: push_time =  {}", NanoToNanoString(stInputBlock1.ulTime));
            while(!queue.enqueue(stInputBlock1)) {
                stInputBlock1.ulTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    });
    
    // std::this_thread::sleep_for(std::chrono::microseconds(100));


    consumer.join();
    producer.join();

    LOG_INFO("{}", GetSimpleTimeData(gMpmc1VecTime));

}