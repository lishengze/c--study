#pragma once

#include <iostream>

#include "struct.h"
#include "mpmc_queue.h"

void write_thread_func_mpmc( std::atomic<unsigned long long>& ulAtoWriteCount, MetaData metaData);

void read_thread_func_mpmc(std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData);