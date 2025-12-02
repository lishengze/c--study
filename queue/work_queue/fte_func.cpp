
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "fte_func.h"
#include "mpmc_queue.h"
#include "test_log.h"

using namespace std;


/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteWriteEnd(ProcessStatus& eProcStatus, MetaData& metaData, int iCurCount) {
    if (ProcessStatus::Running != eProcStatus) return true;

    if (metaData.iWorkSecs == 0 && metaData.iWriteBlockCount > 0 && iCurCount >= metaData.iWriteBlockCount/metaData.iWriteThreadCount ) return true;

    // std::cout << "eProcStatus: " << (int)eProcStatus << ", ulAtoWriteCount: " << ulAtoWriteCount << ", iWorkSecs: " << metaData.iWorkSecs << ", iWriteBlockCount: " << metaData.iWriteBlockCount << std::endl;

    return false;
}

/// @brief 判断是否写入结束: 写入的数量, 写入的时间;
/// @param ulAtoWriteCount 写入的块数
/// @param metaData 元数据
/// @param ulStartWriteTimeNanosecs 写入开始时间
/// @return true 写入结束
/// @return false 写入未结束
bool IsFteReadEnd(ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount, MetaData& metaData) {
    if (ProcessStatus::Running != eProcStatus && ProcessStatus::WriteEnd != eProcStatus) return true;

    if (ulAtoReadCount >= metaData.iWriteBlockCount && metaData.iWriteBlockCount > 0 && metaData.iWorkSecs == 0) return true;

    return false;
}


template <>
void write_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                                            std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount,
                                            TestOutput& testOutput, int iCpuID, 
                                            std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS("MPMC Write Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode, "MPMC Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    testOutput.ulWriteStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    TEST_LOG_DEBUG_THREADS("MPMC Write  "+ NanoToMicroString(testOutput.ulWriteStartTime ) +" Working ***********************\n", LogMutex);

    testOutput.iWriteCount_ = 0;
    unsigned long long ulPushedFailedCount = 0;

    unsigned long long ulBeforePushTimes = 0;
    unsigned long long ulAfterPushTimes = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);

    while(!IsFteWriteEnd(eProcStatus, metaData, testOutput.iWriteCount_)) {
            
            
            dataBlock.push_time_  = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            if (metaData.iTestType == (int)(TestType::Write) || metaData.iTestType == (int)TestType::Detail
            // || true
            ) {
                ulBeforePushTimes = dataBlock.push_time_;
            }
        
            queue.push(dataBlock);

            if (metaData.iTestType == (int)(TestType::Write) || metaData.iTestType == (int)TestType::Detail
            // || true            
            ) {
                ulAfterPushTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                testOutput.vecWriteAfterPushTimeList[testOutput.iWriteCount_] = (ulAfterPushTimes);    
                testOutput.vecWriteBeforePushTimeList[testOutput.iWriteCount_] = (ulBeforePushTimes); 
            }     

            ulAtoWriteCount++;
            testOutput.iWriteCount_++;

            // if (!queue.trypush(dataBlock) ) {
            //     ulPushedFailedCount++;
            // } else {
            //     ulAtoWriteCount++;
            //     iCurCount++;
            // }

            if (metaData.iSleepTimeUs > 0) {
                std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));
            }
    }

    std::lock_guard<std::mutex> lock(mtx);

    testOutput.ulWriteEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    TEST_LOG_DEBUG_THREADS("testOutput.iWriteCount_: " + std::to_string(testOutput.iWriteCount_)
    + ", fristtime: " + NanoToMicroString(testOutput.vecWriteBeforePushTimeList[0])
    + ", lasttime: " + NanoToMicroString(testOutput.vecWriteBeforePushTimeList[testOutput.iWriteCount_-1]), LogMutex);

    // TEST_LOG_DEBUG_THREADS("TryPush Failed Counts: " + std::to_string(ulPushedFailedCount), LogMutex)

    TEST_LOG_DETAIL_THREADS("[END] MPMC Write ulAtoWriteCount: "
                    + std::to_string(ulAtoWriteCount) 
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulWriteEndTime) 
                    + " ***********************\n", LogMutex);
}


template <>
void read_thread_func_mpmc<DataBlockFixed>(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                                            std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                                            TestOutput& testOutput, int iCpuID, 
                                            std::mutex& LogMutex,  MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS(" MPMC Read Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"MPMC Read ");
    DataBlockFixed dataBlock;
    unsigned long long ulPopFailCount = 0;
    unsigned long long ulBeforePopTimes = 0;
    unsigned long long ulAfterPopTimes = 0;

    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing 
    || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        if (ulWaitCount++%1000000 == 0) {
            TEST_LOG_DETAIL_THREADS("MPMC Read Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        }
    }

    
    testOutput.ulReadStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    TEST_LOG_DEBUG_THREADS("MPMC Read Working "+ NanoToMicroString(testOutput.ulReadStartTime ) +" ***********************\n", LogMutex);

    // TEST_LOG_DEBUG_THREADS("testOutput.iReadCount_: " + std::to_string(testOutput.iReadCount_), LogMutex);

    while(!IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData) ) {
                
        if (metaData.iTestType == (int)TestType::Read || metaData.iTestType == (int)TestType::Detail 
            // || true
        ) {
            ulBeforePopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        if (queue.trypop(dataBlock)) {

            ulAfterPopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            // testOutput.vecCostTime[ulAtoReadCount] = (ulAfterPopTimes - dataBlock.push_time_);
            
            if (metaData.iWorkSecs > 0) {
                testOutput.vecCostTime.push_back(ulAfterPopTimes - dataBlock.push_time_); 
                testOutput.iReadCount_++;
            } else { // 按照数量进行压测, vecCostTime 里的空间已经申请好;
                testOutput.vecCostTime[ulAtoReadCount] = (ulAfterPopTimes - dataBlock.push_time_);
                testOutput.iReadCount_++;
            }

            if (metaData.iTestType == (int)TestType::Read || metaData.iTestType == (int)TestType::Detail 
                // || true
            ) {
                testOutput.vecReadAfterPopTimeList[ulAtoReadCount] = (ulAfterPopTimes);  
                testOutput.vecReadBeforePopTimeList[ulAtoReadCount] = (ulBeforePopTimes);
            }  

            ulAtoReadCount++;      

            // if (1 == metaData.iCheckDetailValue) {
            //     if (dataBlock.data_[dataBlock.array_size_ - 1] != (dataBlock.array_size_ - 1) % 128) {
            //         TEST_LOG_ERROR_THREADS (" Read Data Error: dataBlock.data_[" + std::to_string(dataBlock.array_size_ - 1) + "] = "
            //                                 + std::to_string(int(dataBlock.data_[dataBlock.array_size_ - 1]))+ ", endTime:" + NanoToMicroString(ulEndNanosecs)  + "\n",  LogMutex);
            //     }
            // }
        } else {
            ++ulPopFailCount;
        }

        queue.pop(dataBlock);
        ulAtoReadCount++;

    }

    // TEST_LOG_DEBUG_THREADS("testOutput.iReadCount_: " + std::to_string(testOutput.iReadCount_), LogMutex);

    testOutput.ulReadEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    LOG_RST("\n\nTest Pop Ana Start: " + NanoToMicroString(testOutput.ulReadStartTime)  + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                        + ",ave: " + std::to_string( (testOutput.ulReadEndTime - testOutput.ulReadStartTime)/ ulAtoReadCount )  )

    TEST_LOG_DETAIL_THREADS("[END] MPMC Read  ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                    +" **********************\n", LogMutex);
}

template <>
void write_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, 
                                                std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoWriteCount, 
                                                TestOutput& testOutput,int iCpuID,  
                                                std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DEBUG_THREADS("MPMC Write  Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode, "MPMC Write ");
    testOutput.iWriteCount_ = 0;
    unsigned long long ulPushedFailedCount = 0;

    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    testOutput.ulWriteStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    TEST_LOG_DEBUG_THREADS("MPMC Write  Working Time: "+ NanoToNanoString(testOutput.ulWriteStartTime) +"  ***********************\n", LogMutex);

    do {
        unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        testOutput.vecWriteBeforePushTimeList.push_back(ulCurTimeNanosecs);

        int index = queue.push(ulCurTimeNanosecs);
        ulAtoWriteCount++;
        testOutput.iWriteCount_++;

        ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        // testOutput.vecWriteSlotList.push_back(index);
        testOutput.vecWriteAfterPushTimeList.push_back(ulCurTimeNanosecs); //core;

        // if (!queue.trypush(ulCurTimeNanosecs) ) {
        //     ulPushedFailedCount++;
        // } else {
        //     ulAtoWriteCount++;
        //     iCurCount++;
        // }

        if (metaData.iSleepTimeUs > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));
        }
    }  while(!IsFteWriteEnd(eProcStatus, metaData, testOutput.iWriteCount_));

    std::lock_guard<std::mutex> lock(mtx);

    testOutput.ulWriteEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    // // TEST_LOG_DEBUG_THREADS("TryPush Failed Counts: " + std::to_string(ulPushedFailedCount), LogMutex);

    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 


    TEST_LOG_DETAIL_THREADS("[END] MPMC Write  End ulAtoWriteCount: "
                    + std::to_string(ulAtoWriteCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus) 
                    + ", dataCount: " + std::to_string(testOutput.vecWriteBeforePushTimeList.size())
                    + ", endTime:" + NanoToMicroString(testOutput.ulWriteEndTime ) 
                    +" ***********************\n", LogMutex);
}


template <>
void read_thread_func_mpmc<unsigned long long>(ProcessStatus& eProcStatus, tech::mpmc_queue<unsigned long long>& queue, 
                                                std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                                                TestOutput& testOutput,int iCpuID, 
                                                std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DEBUG_THREADS("MPMC Read  Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"MPMC Read ");

    unsigned long long ulPopFailCount = 0;
    unsigned long long ulPushTimeNanosecs = 0;
    unsigned long long ulAfterPopTimes = 0;
    unsigned long long ulBeforePopTimes = 0;

    while(eProcStatus == ProcessStatus::Initing 
        || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    testOutput.ulReadStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    TEST_LOG_DEBUG_THREADS("MPMC Read  Working  Time: "+ NanoToNanoString(testOutput.ulReadStartTime) 
                +"***********************\n", LogMutex);

    do {
        if (metaData.iTestType == (int)TestType::Read || metaData.iTestType == (int)TestType::Detail) {
            ulBeforePopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        if (queue.trypop(ulPushTimeNanosecs)) {
            ulAfterPopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            if (metaData.iWorkSecs > 0) {
                testOutput.vecCostTime.push_back(ulAfterPopTimes - ulPushTimeNanosecs); 
                testOutput.iReadCount_++;
            } else { // 按照数量进行压测, vecCostTime 里的空间已经申请好;
                testOutput.vecCostTime[ulAtoReadCount] = (ulAfterPopTimes - ulPushTimeNanosecs); 
                testOutput.iReadCount_++;
            }
            
            if (metaData.iTestType == (int)(TestType::Read) || metaData.iTestType == (int)(TestType::Detail)) {
                testOutput.vecReadAfterPopTimeList[ulAtoReadCount] = (ulAfterPopTimes);  
                testOutput.vecReadBeforePopTimeList[ulAtoReadCount] = (ulBeforePopTimes);
            }

            ulAtoReadCount++;   
        } else {
            ulPopFailCount++;
        }

        // int index = queue.pop(ulPushTimeNanosecs);
        // unsigned long long ulCurTimeNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        // testOutput.vecCostTime.push_back(ulCurTimeNanosecs - ulPushTimeNanosecs);
        // ulAtoReadCount++;   

        // testOutput.vecReadSlotList.push_back(index);
        // testOutput.vecReadAfterPopTimeList.push_back(ulCurTimeNanosecs);
        // vecPopCostTimes.push_back(ulCurTimeNanosecs - ulBeforePopTimes);

        // queue.pop(ulPushTimeNanosecs);
        // testOutput.vecCostTime.push_back(ulCurTimeNanosecs - ulPushTimeNanosecs);
        // ulAtoReadCount++;        

    }while(!IsFteReadEnd(eProcStatus, ulAtoReadCount, metaData));

    testOutput.ulReadEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    // TEST_LOG_DEBUG_THREADS("TryPop Failed Counts: " + std::to_string(ulPopFailCount), LogMutex);


    TEST_LOG_DETAIL_THREADS("[END] MPMC Read Thread  ulAtoReadCount: "
                    + std::to_string(ulAtoReadCount)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", dataCount: "+std::to_string(testOutput.vecReadAfterPopTimeList.size())
                    + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                    +" **********************\n", LogMutex);
}
