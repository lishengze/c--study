
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "fte_func.h"
#include "mpmc_queue.h"

void write_thread_func_mpmc( std::atomic<unsigned long long>& ulAtoWriteCount, MetaData metaData) {
    
    // unsigned long long count = 0;

}

void read_thread_func_mpmc(std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData) {

}