
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "fte_func.h"
#include "mpmc_queue.h"
#include "test_log.h"

using namespace std;


///////////////////////////////////////////////////////  开始极简测试代码 ////////////////////////////////////////////////////////


/**********************************************************************************
测试入队相关性能
**********************************************************************************/
// 脉冲式写入,用于测试 SPSC 写入-取出延迟 ;
void write_mpmc_spsc_no_sleep(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                            std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput, int iCpuID, 
                            std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS("MPMC Write SPSC Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode, "MPMC Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // TEST_LOG_DEBUG_THREADS("MPMC Write  "+ NanoToMicroString(testOutput.ulWriteStartTime ) +" Working ***********************\n", LogMutex);

    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);

    TEST_LOG_DEBUG_THREADS("MPMC Write SPSC Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +" ***********************\n", LogMutex);

    unsigned long long MaxCount = metaData.iWriteBlockCount/metaData.iWriteThreadCount;

    testOutput.ulWriteStartTime = get_monotonic_ns();

    while(testOutput.iWriteCount_++ < MaxCount) {                                
        dataBlock.push_time_  = get_monotonic_ns();        
        // dataBlock.index_ = testOutput.iWriteCount_;
        queue.push(dataBlock);
    }

    testOutput.ulWriteEndTime = get_monotonic_ns();

    ulAtoWriteCount += testOutput.iWriteCount_;

    std::lock_guard<std::mutex> lock(mtx);

    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    TEST_LOG_DETAIL_THREADS("[END] MPMC Write DataCount: "
                    + std::to_string(testOutput.iWriteCount_ - 1) 
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulWriteEndTime) 
                    + " ***********************\n", LogMutex);
}

// 匀速写入,用于测试 SPSC 写入-取出延迟 ;
void write_mpmc_spsc_with_sleep(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                                            std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount,
                                            TestOutput& testOutput, int iCpuID, 
                                            std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS("MPMC Write Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode, "MPMC Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);


    testOutput.ulWriteStartTime = get_monotonic_ns();
    TEST_LOG_DEBUG_THREADS("MPMC Write Working "+ NanoToMicroString(testOutput.ulWriteStartTime ) +" ***********************\n", LogMutex);
    
    unsigned long long MaxCount = metaData.iWriteBlockCount/metaData.iWriteThreadCount;
    while(eProcStatus == ProcessStatus::Running) { 
                        
        // dataBlock.index_ = testOutput.iWriteCount_++;
        testOutput.iWriteCount_++;
        dataBlock.push_time_  = get_monotonic_ns();        
        queue.push(dataBlock);

        std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));            
    }

    std::lock_guard<std::mutex> lock(mtx);

    testOutput.ulWriteEndTime = get_monotonic_ns();

    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    // TEST_LOG_DEBUG_THREADS("TryPush Failed Counts: " + std::to_string(ulPushedFailedCount), LogMutex)

    TEST_LOG_DETAIL_THREADS("[END] MPMC Write DataCount: "
                    + std::to_string(testOutput.iWriteCount_) 
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulWriteEndTime) 
                    + " ***********************\n", LogMutex);
}

// 脉冲式写入 - 只测试压满队列的平均耗时 
void write_mpmc_push(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                    std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount,
                    TestOutput& testOutput, int iCpuID, 
                    std::mutex& LogMutex, MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS("MPMC Write Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode, "MPMC Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);


    testOutput.ulWriteStartTime = get_monotonic_ns();
    // TEST_LOG_DEBUG_THREADS("MPMC Write  "+ NanoToMicroString(testOutput.ulWriteStartTime ) +" Working ***********************\n", LogMutex);

    unsigned long long MaxCount = metaData.iWriteBlockCount/metaData.iWriteThreadCount;

    while(testOutput.iWriteCount_++ < MaxCount) {                              
        queue.push(dataBlock);
        // testOutput.iWriteCount_++;        
        // cout << "testOutput.iWriteCount_: " << testOutput.iWriteCount_ << endl;
    }

    testOutput.ulWriteEndTime = get_monotonic_ns();

    ulAtoWriteCount += testOutput.iWriteCount_;

    std::lock_guard<std::mutex> lock(mtx);

    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    LOG_RST("MPMC  Test Write DataCount: " + std::to_string(testOutput.iWriteCount_-1)
                        + ", start: " + NanoToMicroString(testOutput.ulWriteStartTime)
                        + ", end: " + NanoToMicroString(testOutput.ulWriteEndTime)
                        + ",ave: " + std::to_string((testOutput.ulWriteEndTime - testOutput.ulWriteStartTime)/testOutput.iWriteCount_)
                        + "\n");

    TEST_LOG_DEBUG_THREADS("MPMC Test Write DataCount: " + std::to_string(testOutput.iWriteCount_)
                        + ", start: " + NanoToMicroString(testOutput.ulWriteStartTime)
                        + ", end: " + NanoToMicroString(testOutput.ulWriteEndTime)
                        + ",ave: " + std::to_string((testOutput.ulWriteEndTime - testOutput.ulWriteStartTime)/testOutput.iWriteCount_), 
                        LogMutex);
}

/**********************************************************************************
测试出队相关性能
**********************************************************************************/


/// 只测试队列写满后, 队列出队的性能表现, 记录取出开始结束时间，计算平均耗时;
void read_mpmc_pop(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                    std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                    TestOutput& testOutput, int iCpuID, 
                    std::mutex& LogMutex,  MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS(" MPMC Read Pop Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"MPMC Read ");
    DataBlockFixed dataBlock;

    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing 
    || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        if (ulWaitCount++%1000000 == 0) {
            TEST_LOG_DETAIL_THREADS("MPMC Read Pop Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        }
    }
    testOutput.iReadCount_ = 0;
    // TEST_LOG_DEBUG_THREADS("MPMC Read Working "+ NanoToMicroString(testOutput.ulReadStartTime ) +" ***********************\n", LogMutex);


    testOutput.ulReadStartTime = get_monotonic_ns();

    while(testOutput.iReadCount_++ < metaData.iWriteBlockCount) {
        queue.pop(dataBlock);
    }

    testOutput.ulReadEndTime = get_monotonic_ns();

    LOG_RST("MPMC  Test Read: " + NanoToMicroString(testOutput.ulReadStartTime)  
                        + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                        + ", count: " + std::to_string(testOutput.iReadCount_-1)
                        + ", ave: " + std::to_string( (testOutput.ulReadEndTime - testOutput.ulReadStartTime)/ metaData.iWriteBlockCount )  
                        + "\n")

    TEST_LOG_DETAIL_THREADS("[END]  DataCount: " + std::to_string(testOutput.iReadCount_-1)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                    +" **********************\n", LogMutex);
}


/// 测试-脉冲式写入, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_mpmc_spsc_no_sleep(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                    std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                    TestOutput& testOutput, int iCpuID, 
                    std::mutex& LogMutex,  MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS("MPMC Read  SPSC Initing ***********************\n", LogMutex);
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

    testOutput.iReadCount_ = 0;
    testOutput.vecCostTime[0] = 0; // 将 内存数据 调入到 cache 中;

    TEST_LOG_DEBUG_THREADS("MPMC Read  SPSC Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +" ***********************\n", LogMutex);

    testOutput.ulReadStartTime = get_monotonic_ns();

    while(testOutput.iReadCount_++ < metaData.iWriteBlockCount) {     
    
        // ulBeforePopTimes = get_monotonic_ns();

        queue.pop(dataBlock);
        ulAfterPopTimes = get_monotonic_ns();
        testOutput.vecCostTime[testOutput.iReadCount_-1] = (ulAfterPopTimes - dataBlock.push_time_);

        // if (testOutput.iReadCount_ < 5 || testOutput.iReadCount_ > metaData.iWriteBlockCount - 6) {
        //     TEST_LOG_DETAIL_THREADS("PushTime: " + NanoToMicroString(dataBlock.push_time_) 
        //                 // + ", " + std::to_string(dataBlock.index_)
        //                 + ", BeforePop: " + NanoToMicroString(ulBeforePopTimes)
        //                 + ", AfterPop: " + NanoToMicroString(ulAfterPopTimes)
        //                 + ", Delta: " + std::to_string(ulAfterPopTimes - dataBlock.push_time_), LogMutex);              
        // }        
    }
    
    // testOutput.iReadCount_ = ulAtoReadCount;

    testOutput.ulReadEndTime = get_monotonic_ns();

    // LOG_RST("Test Pop Ana Start: " + NanoToMicroString(testOutput.ulReadStartTime)  + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
    //                     + ",ave: " + std::to_string( (testOutput.ulReadEndTime - testOutput.ulReadStartTime)/ ulAtoReadCount )  )

    TEST_LOG_DETAIL_THREADS("[END] MPMC Read  DataCount: "
                    + std::to_string(testOutput.iReadCount_-1)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                    +" **********************\n", LogMutex);
}

/// 测试-匀速写入, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_mpmc_spsc_with_sleep(ProcessStatus& eProcStatus, tech::mpmc_queue<DataBlockFixed>& queue, 
                    std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
                    TestOutput& testOutput, int iCpuID, 
                    std::mutex& LogMutex,  MetaData& metaData) {

    TEST_LOG_DETAIL_THREADS(" MPMC Read SPSC Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"MPMC Read ");
    DataBlockFixed dataBlock;
    unsigned long long ulPopFailCount = 0;
    unsigned long long ulBeforePopTimes = 0;
    unsigned long long ulAfterPopTimes = 0;

    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing) {
        // if (ulWaitCount++%1000000 == 0) {
        //     TEST_LOG_DETAIL_THREADS("MPMC Read Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        // }
    }

    testOutput.iReadCount_ = 0;
    testOutput.vecCostTime[0] = 0; // 将 内存数据 调入到 cache 中;

    TEST_LOG_DEBUG_THREADS("MPMC Read  Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +" ***********************\n", LogMutex);

    testOutput.ulReadStartTime = get_monotonic_ns();

    while(eProcStatus == ProcessStatus::Running) {            
        if(queue.trypop(dataBlock)) {
            ulAfterPopTimes = get_monotonic_ns();
            testOutput.vecCostTime.push_back(ulAfterPopTimes - dataBlock.push_time_);
            testOutput.iReadCount_++;   
        }
        // cout << "testOutput.iReadCount_: "  << testOutput.iReadCount_ << ", eProcStatus: "  << (int)eProcStatus << endl;
    }

    testOutput.ulReadEndTime = get_monotonic_ns();

    TEST_LOG_DETAIL_THREADS("[END] MPMC Read  DataCount: "
                    + std::to_string(testOutput.iReadCount_)
                    +", eProcStatus: "+std::to_string((int)eProcStatus)
                    + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                    +" **********************\n", LogMutex);
}
