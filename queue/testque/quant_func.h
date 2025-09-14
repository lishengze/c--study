#pragma once

#include <iostream>
#include <stdio.h>
#include <memory>
#include <thread>
#include <array>
#include <vector>
#include <atomic>
#include <algorithm>

#include "mutils.h"
#include "comm_sys.h"
#include "que_proc_buf.h"
#include "struct.h"
#include "base_util.h"



#include "struct.h"
#include "test_log.h"

using namespace std;
using namespace lb_common;

void *read_thread_func_quant_pos(int& iStopFlag, int64& readPos, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPopBlocks, 
                                std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData);


void *read_thread_func_quant_cmt(int& iStopFlag, que_proc_buf& queProBuf, vector<int64>& vecReadPos, std::vector<DataBlockPtr>& vecPopBlocks, MetaData metaData);

void write_thread_func_quant(int& iStopFlag, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPushBlocks, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData metaData);

void read_thread_func_quant_simple(int& iStopFlag, que_proc_buf& queProBuf,  std::vector<DataBlockPtr>& vecPopBlocks, 
                                    std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData);