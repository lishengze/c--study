#pragma once

#include <iostream>
#include <stdio.h>
#include <memory>
#include <thread>
#include <array>
#include <vector>
#include <atomic>
#include <algorithm>
#include <mutex>

#include "mutils.h"
#include "comm_sys.h"
#include "que_proc_buf.h"
#include "struct.h"
#include "base_util.h"



#include "struct.h"
#include "test_log.h"

using namespace std;
using namespace lb_common;

bool IsQuantWriteEnd(MetaData& metaData, ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoWriteCount, unsigned int  iCurCount) ;

bool IsQuantReadEnd(que_proc_buf& workQueue, MetaData& metaData, ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount);

void *read_thread_func_quant_pos(ProcessStatus& eProcStatus, int64& readPos, que_proc_buf& workQueue, 
                                std::atomic<unsigned long long>& ulAtoReadCount, 
                                TestOutput& testOutput, int iCpuID, 
                                std::mutex& LogMutex, MetaData metaData);

void *read_thread_func_quant_cmt(ProcessStatus& eProcStatus, que_proc_buf& workQueue, vector<int64>& vecReadPos, 
                                    std::mutex& LogMutex, MetaData metaData);

void write_thread_func_quant(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData);

void read_thread_func_quant_simple(ProcessStatus& eProcStatus, que_proc_buf& workQueue, 
                                    std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID, 
                                    std::mutex& LogMutex, MetaData metaData);

void write_quant_spsc_no_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData);                              


// 记录压入时间,用于测试 写入-取出延迟, 中间有睡眠;
void write_quant_spsc_with_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData);                                  

// 只测试压入的平均耗时 - 压入过程中不记录每个数据块的入队时间;
void write_quant_push(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData);                

/// 只测试队列取出的性能表现, 记录取出开始结束时间，计算平均耗时;
void read_quant_pop(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData);          

/// 测试-固定写入数量场景, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_quant_spsc_no_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData);  

/// 测试-固定写入时间，匀速发单的场景, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_quant_spsc_with_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData) ;                                                                                            