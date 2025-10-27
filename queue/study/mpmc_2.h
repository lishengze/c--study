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
struct Element {

    Element(): bIsWriteen{false} {
        
    }

    T value_;
    std::atomic<bool> bIsWriteen;     // 这个slot是否写入值了。

    void Enqueue(T& value) {

        while(bIsWriteen.load(std::memory_order_acquire));  // 等待这个位置被空出来;当前是写的状态，代表已经满了。

        new (&value_) T(value); // 在当前元素的位置以传入的值重新初始化当前的元素;

        bIsWriteen.store(true, std::memory_order_release);

        return;
    }

    bool Dequeue(T& value) {

        while(!bIsWriteen.load(std::memory_order_acquire)); // 

        value = value_;

        bIsWriteen.store(false, std::memory_order_release); // 当前位置已经空出来;

        return true;
    }
};

template<typename T>
class MPMCQueue_2 {

    public:
        MPMCQueue_2(unsigned int uiCapacity) : uiCapacity_(uiCapacity),atouiReadIndex_(0),atouiWriteIndex_(0),atouiCount_(0) {
            pBuffer_ = new Element<T>[uiCapacity_](); // 添加括号进行值初始化
        }

        ~MPMCQueue_2() {
            delete[] pBuffer_;
        }

        /// @brief 入队操作
        /// @param tValue 要入队的值
        /// @return true 入队成功
        /// @return false 入队失败
        bool enqueue(const T& tValue) {
            unsigned int uiCurWriteIndex = atouiWriteIndex_++ % uiCapacity_;

            pBuffer_[uiCurWriteIndex].Enqueue(tValue);

            return true;
        }

        bool dequeue(T& tValue) {
            unsigned int uiCurReadIndex = atouiReadIndex_++ % uiCapacity_;

            pBuffer_[uiCurReadIndex].Dequeue(tValue);

            return true;
        }

    private:

    unsigned int uiCapacity_;       // 队列的容量
    std::atomic<unsigned int> atouiCount_{0};       // 队列中当前元素的数量
    std::atomic<unsigned int> atouiWriteIndex_{0};       // 写入索引
    std::atomic<unsigned int> atouiReadIndex_{0};       // 读取索引

    Element<T>* pBuffer_;
};



std::vector<unsigned long long> gMpmc2VecTime;

void TestMpmc_2() {
    MPMCQueue_2<Block1> queue(10000);

    
    int iTestCount = 1000;
    gMpmc2VecTime.resize(iTestCount, 0);
    for (int i = 0; i < iTestCount; ++i) {
        gMpmc2VecTime[i] = 0;
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
                gMpmc2VecTime[iValidCount++] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - stOutputBlock1.ulTime;
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

    LOG_INFO("{}", GetSimpleTimeData(gMpmc2VecTime));

}