#pragma once

#include <iostream>
#include <chrono>
#include <mutex>

#include "struct.h"
#include "mpmc_queue.h"
#include "base_util.h"

/*
void test_func(uint32_t size)
{
    auto start = std::chrono::high_resolution_clock::now();
    mpmc_queue<uint32_t> mpmc_que;
    mpmc_que.create(size);
    thread t1([&](){
        uint32_t i = 0;
        for (; i < tech::roundup_pow_of_two(size) * 3 + (uint32_t)100; ++i) 
        {
            mpmc_que.push(i);
        }
        BOOST_CHECK(i & tech::roundup_pow_of_two(size));
        });

    thread t2([&](){
        uint32_t read_data = 0;
        for (uint32_t i = 0; i < tech::roundup_pow_of_two(size) * 3 + (uint32_t)100; ++i) 
        {
            mpmc_que.pop(read_data);
            BOOST_CHECK(i == read_data);
        }
        });
    
    if (t1.joinable())    t1.join();
    if (t2.joinable())    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    BOOST_TEST_MESSAGE("mpmc_queue<uint32_t> cost " << duration.count() << " us.");
}

void test_func_futex(uint32_t size)
{
    
    auto start = std::chrono::high_resolution_clock::now();
    mpmc_queue<uint32_t, true> mpmc_que;
    mpmc_que.create(size);
    thread t1([&](){
        uint32_t i = 0;
        for (; i < tech::roundup_pow_of_two(size) * 3 + (uint32_t)100; ++i) 
        {
            mpmc_que.push(i);
        }
        BOOST_CHECK(i & tech::roundup_pow_of_two(size));
        });

    thread t2([&](){
        uint32_t read_data = 0;
        for (uint32_t i = 0; i < tech::roundup_pow_of_two(size) * 3 + (uint32_t)100; ++i) 
        {
            mpmc_que.pop(read_data);
            BOOST_CHECK(i == read_data);
        }
        });
    
    if (t1.joinable())    t1.join();
    if (t2.joinable())    t2.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    BOOST_TEST_MESSAGE("mpmc_queue<uint32_t, true> cost " << duration.count() << " us.");
}

template<typename T>
void pro_func(int id, T& queue, std::atomic<uint32_t>& pro_count, uint32_t max_count)
{
        auto i = 0;
        while(1)
        {
            auto tmp = pro_count++;
            if (tmp < max_count)
            {
                queue.push(tmp);
                i++;
            }
            else
            {
                break;
            }
        }
        std::lock_guard<std::mutex> lock(cout_mutex);
        BOOST_TEST_MESSAGE("thread " << id << " push " << i << " elements.");
}


*/
/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteWriteEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoWriteCount, 
                    MetaData& metaData, unsigned long long& ulStartWriteTimeNanosecs);

template <typename T>
void write_thread_func_mpmc(ProcessStatus& eProcStatus, tech::mpmc_queue<T>& queue, std::mutex& mtx, 
                            std::atomic<unsigned long long>& ulAtoWriteCount, 
                            TestOutput& testOutput, int iCpuID,  
                            std::mutex& LogMutex, MetaData& metaData) {

}

template <>
void write_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                                            std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, 
                                            TestOutput& testOutput, int iCpuID,  
                                            std::mutex& LogMutex, MetaData& metaData) ;
template <>
void write_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue,
                                                std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, 
                                                TestOutput& testOutput, int iCpuID, 
                                                std::mutex& LogMutex, MetaData& metaData) ;

/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteReadEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount, 
                    MetaData& metaData, unsigned long long& ulStartReadTimeNanosecs);

/*
template <typename T>
void con_func(int id, vector<uint32_t>& vec, T& queue, std::atomic<uint32_t>& con_count, uint32_t max_count)
{
        auto i = 0;
        auto index = 0;
        while(1)
        {
            index = con_count++;
            if (index < max_count)
            {
                uint32_t tmp;
                queue.pop(tmp);
                vec.push_back(tmp);
                i++;
            }
            else
            {
                break;
            }
        }
        std::lock_guard<std::mutex> lock(cout_mutex);
        BOOST_TEST_MESSAGE("thread " << id << " pop " << i << " elements, max_con_count = " <<  max_count << ", index = " << index << ".");
}
*/
template <typename T>
void read_thread_func_mpmc(ProcessStatus& eProcStatus, tech::mpmc_queue<T>& queue, 
                            std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                            TestOutput& testOutput, int iCpuID, 
                            std::mutex& LogMutex, MetaData& metaData) {

}

template <>
void read_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                                            std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                                            TestOutput& testOutput,int iCpuID,
                                            std::mutex& LogMutex, MetaData& metaData);

template <>
void read_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, 
                                                std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                                                TestOutput& testOutput, int iCpuID, 
                                                std::mutex& LogMutex, MetaData& metaData);