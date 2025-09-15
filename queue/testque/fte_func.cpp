
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
bool IsFteWriteEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData, unsigned long long& ulStartWriteTimeNanosecs) {
    if (ProcessStatus::Running != eProcStatus) return true;

    if (metaData.iWriteBlockCount > 0 && ulAtoWriteCount++ < metaData.iWriteBlockCount) return true;

    if (metaData.iWriteSecs > 0) {
        unsigned long long ulEndTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (ulEndTimeNanosecs - ulStartWriteTimeNanosecs >= metaData.iWriteSecs * NANO_PER_SECOND) {
            return true;
        }
    }

    return false;
}

/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteReadEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount, MetaData& metaData, unsigned long long& ulStartReadTimeNanosecs) {
    if (ProcessStatus::Running != eProcStatus) return true;

    if (metaData.iWriteBlockCount > 0 && ulAtoReadCount++ < metaData.iWriteBlockCount) return true;

    if (metaData.iWriteSecs > 0 || metaData.iReadSecs > 0) {
        unsigned long long ulEndTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (ulEndTimeNanosecs - ulStartReadTimeNanosecs >= std::max(metaData.iWriteSecs, metaData.iReadSecs) * NANO_PER_SECOND) {
            return true;
        }
    }

    return false;
}


template <>
void write_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Write Thread DataBlockFixed Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init
    unsigned long long ulStartWriteTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    while(IsFteWriteEnd(eProcStatus, ulAtoWriteCount, metaData, ulStartWriteTimeNanosecs)) {
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

    unsigned long long ulStartWriteTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    while(IsFteWriteEnd(eProcStatus, ulAtoWriteCount, metaData, ulStartWriteTimeNanosecs)) {
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

    // unsigned long long ulStartReadTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    // while(IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData, ulStartReadTimeNanosecs) ) {
    //     DataBlockFixed dataBlock;
    //     queue.pop(dataBlock);
    //     unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    //     vecCostTime.push_back(ulCurTimeNanosecs - dataBlock.push_time_);
    // }

    TEST_LOG_DETAIL("[END] MPMC Read Thread DataBlockFixed End ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+" **********************\n");
}

template <>
void read_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                            std::vector<unsigned long long>& vecCostTime, MetaData& metaData) {

    TEST_LOG_DETAIL("[START] MPMC Read Thread unsigned long long Start ***********************\n");
    while(eProcStatus == ProcessStatus::NotInit ); // wait for init
    // unsigned long long ulStartReadTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    // while(IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData, ulStartReadTimeNanosecs) ) {
    //     unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    //     unsigned long long ulPushTimeNanosecs = 0;
    //     queue.pop(ulPushTimeNanosecs);
    //     vecCostTime.push_back(ulCurTimeNanosecs - ulPushTimeNanosecs);
    // }

    TEST_LOG_DETAIL("[END] MPMC Read Thread unsigned long long End ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)+" **********************\n");
}