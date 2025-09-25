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