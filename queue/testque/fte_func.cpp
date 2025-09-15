
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "fte_func.h"
#include "mpmc_queue.h"
#include "test_log.h"


/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteWriteEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData) {
    if (ProcessStatus::Running != eProcStatus) return true;

    if (ulAtoWriteCount++ < metaData.iWriteBlockCount && metaData.iWorkSecs == 0 && metaData.iWriteBlockCount > 0) return true;

    return false;
}

/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteReadEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount, MetaData& metaData) {
    if (ProcessStatus::Running != eProcStatus) return true;

    if (ulAtoReadCount++ < metaData.iWriteBlockCount && metaData.iWriteBlockCount > 0 && metaData.iWorkSecs == 0) return true;

    return false;
}


template <>
void write_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Write Thread DataBlockFixed Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init

    while(!IsFteWriteEnd(eProcStatus, ulAtoWriteCount, metaData)) {
            DataBlockFixed dataBlock;
            dataBlock.size_ = sizeof(DataBlockFixed);
            dataBlock.push_time_  = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            queue.push(dataBlock);

            TEST_LOG_DETAIL("DataBlockFixed write ulAtoWriteCount: "+ std::to_string(ulAtoWriteCount) +" ***********************\n");

            if (metaData.iSleepTimeUs > 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));
            }
    }

    std::lock_guard<std::mutex> lock(mtx);
    TEST_LOG_DETAIL("[END] MPMC Write Thread DataBlockFixed End ulAtoWriteCount: "
                    + std::to_string(ulAtoWriteCount) 
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+ " ***********************\n");
}

template <>
void write_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Write Thread unsigned long long Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init

    while(!IsFteWriteEnd(eProcStatus, ulAtoWriteCount, metaData)) {
            unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            queue.push(ulCurTimeNanosecs);

            if (metaData.iSleepTimeUs > 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));
            }
    }

    std::lock_guard<std::mutex> lock(mtx);

    TEST_LOG_DETAIL("[END] MPMC Write Thread unsigned long long End ulAtoWriteCount: "
                    + std::to_string(ulAtoWriteCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+" ***********************\n");
}

template <>
void read_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                            std::vector<unsigned long long>& vecCostTime, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Read Thread DataBlockFixed Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init

    while(!IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData) ) {
        DataBlockFixed dataBlock;
        queue.pop(dataBlock);
        unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        vecCostTime.push_back(ulCurTimeNanosecs - dataBlock.push_time_);
    }

    TEST_LOG_DETAIL("[END] MPMC Read Thread DataBlockFixed End ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+" **********************\n");
}

template <>
void read_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                            std::vector<unsigned long long>& vecCostTime, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Read Thread unsigned long long Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init

    while(!IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData) ) {
        unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        unsigned long long ulPushTimeNanosecs = 0;
        queue.pop(ulPushTimeNanosecs);
        vecCostTime.push_back(ulCurTimeNanosecs - ulPushTimeNanosecs);
    }

    TEST_LOG_DETAIL("[END] MPMC Read Thread unsigned long long End ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+" **********************\n");
}