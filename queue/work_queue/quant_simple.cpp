#include "quant_func.h"
#include "struct.h"


///////////////////////////////////////////////////////  开始极简测试代码 ////////////////////////////////////////////////////////

/**********************************************************************************
测试入队相关性能
**********************************************************************************/
// 脉冲式写入,用于测试 SPSC 写入-取出延迟 ;
void write_quant_spsc_no_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("Quant Write SPSC Initing ***********************\n", LogMutex);
    // 等待启动信号(eProcStatus != 0)
    BindCpuID(iCpuID,metaData.iNumaNode, "Quant Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
            
    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);

    TEST_LOG_DEBUG_THREADS("Quant Write SPSC Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +"***********************\n", LogMutex);
    testOutput.ulWriteStartTime = get_monotonic_ns();

    while(testOutput.iWriteCount_++ < metaData.iWriteBlockCount)  {
        dataBlock.push_time_  = get_monotonic_ns();        
        workQueue.write((char*)(&dataBlock),dataBlock.size_);  // 提交写入      
    } 

    testOutput.ulWriteEndTime = get_monotonic_ns();
    
    //// 供测试 取出 平均耗时使用;
    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    TEST_LOG_DETAIL_THREADS("[END] Quant Write DataCount: " + std::to_string(testOutput.iWriteCount_-1)
                        + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                        + ", endTime: " + NanoToMicroString(testOutput.ulWriteEndTime), 
                        LogMutex);
}

// 匀速写入,用于测试 SPSC 写入-取出延迟 ;
void write_quant_spsc_with_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Write thread Initing \n", LogMutex);
    // 等待启动信号(eProcStatus != 0)
    BindCpuID(iCpuID,metaData.iNumaNode, "Quant Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
            
    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);

    TEST_LOG_DEBUG_THREADS("Quant Write Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +"***********************\n", LogMutex);


    testOutput.ulWriteStartTime = get_monotonic_ns();

    while(eProcStatus == ProcessStatus::Running) {

        testOutput.iWriteCount_++;   
        dataBlock.push_time_  = get_monotonic_ns();        

        workQueue.write((char*)(&dataBlock),dataBlock.size_);  // 提交写入
        

        std::this_thread::sleep_for(std::chrono::microseconds(metaData.iSleepTimeUs));            
    } 
    
    //// 供测试 取出 平均耗时使用;
    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    testOutput.ulWriteEndTime = get_monotonic_ns();


    TEST_LOG_DETAIL_THREADS("[END] Write Count: " + std::to_string(testOutput.iWriteCount_)
                        + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                        + ", endTime: " + NanoToMicroString(testOutput.ulWriteEndTime)+ "\n", 
                        LogMutex);
}


// 脉冲式写入 - 只测试压满队列的平均耗时 
void write_quant_push(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                            std::atomic<unsigned long long>& ulAtoWriteCount,
                            TestOutput& testOutput,int iCpuID, 
                            std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Write thread Initing \n", LogMutex);
    // 等待启动信号(eProcStatus != 0)
    BindCpuID(iCpuID,metaData.iNumaNode, "Quant Write ");
    while(eProcStatus == ProcessStatus::Initing ) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
            
    testOutput.iWriteCount_ = 0;
    DataBlockFixed dataBlock;
    dataBlock.size_ = sizeof(DataBlockFixed);
    unsigned long long MaxCount = metaData.iWriteBlockCount/metaData.iWriteThreadCount;

    // TEST_LOG_DEBUG_THREADS("[START] Quant Queue Write thread Working "+ NanoToMicroString(testOutput.ulWriteStartTime)  +"\n", LogMutex);
    testOutput.ulWriteStartTime = get_monotonic_ns();

    while(testOutput.iWriteCount_++ < MaxCount)  {
        workQueue.write((char*)(&dataBlock),dataBlock.size_);  // 提交写入
        // testOutput.iWriteCount_++;   
    } 
    

    testOutput.ulWriteEndTime = get_monotonic_ns();

    //// 供测试 取出 平均耗时使用;
    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 


    LOG_RST("Quant Test Write DataCount: " + std::to_string(testOutput.iWriteCount_-1)
                        + ", start: " + NanoToMicroString(testOutput.ulWriteStartTime)
                        + ", end: " + NanoToMicroString(testOutput.ulWriteEndTime)
                        + ",ave: " + std::to_string((testOutput.ulWriteEndTime - testOutput.ulWriteStartTime)/testOutput.iWriteCount_)
                        + "\n");

    TEST_LOG_DEBUG_THREADS("MPMC testOutput.iWriteCount_: " + std::to_string(testOutput.iWriteCount_)
                        + ", start: " + NanoToMicroString(testOutput.ulWriteStartTime)
                        + ", end: " + NanoToMicroString(testOutput.ulWriteEndTime)
                        + ",ave: " + std::to_string((testOutput.ulWriteEndTime - testOutput.ulWriteStartTime)/testOutput.iWriteCount_), 
                        LogMutex);
    
}
/**********************************************************************************
测试出队相关性能
**********************************************************************************/

/// 只测试队列写满后, 队列出队的性能表现, 记录取出开始结束时间，计算平均耗时;
void read_quant_pop(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("Initing\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"Quant Read Simple ");
    // 等待启动信号
    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing 
        || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        if (ulWaitCount++%1000000 == 0) {
            TEST_LOG_DETAIL_THREADS("Quant Read Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        }
    }

    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    testOutput.iReadCount_ = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    DataBlockFixed stFixedBlock;

    testOutput.ulReadStartTime = get_monotonic_ns();

    // TEST_LOG_DEBUG_THREADS("Working " + NanoToMicroString(testOutput.ulReadStartTime) +"\n", LogMutex);

    while(testOutput.iReadCount_ < metaData.iWriteBlockCount) {

        while((len = workQueue.read_get(pbuf)) > 0){
            memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);
            workQueue.read_cmt();  // 提交读取（单消费者模式）
            testOutput.iReadCount_++;
            if (testOutput.iReadCount_ >= metaData.iWriteBlockCount) break;
        }
    }
            
    testOutput.ulReadEndTime = get_monotonic_ns();

    LOG_RST("Quant Test Read: " + NanoToMicroString(testOutput.ulReadStartTime)  
                        + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) 
                        + ", count: " + std::to_string(testOutput.iReadCount_)
                        + ", ave: " + std::to_string( (testOutput.ulReadEndTime - testOutput.ulReadStartTime)/ metaData.iWriteBlockCount )
                        + "\n"  )

    TEST_LOG_DETAIL_THREADS("[END] DataCount: " + std::to_string(testOutput.iReadCount_) 
                + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                + ", workQueue.get_used(): " + std::to_string(workQueue.get_used()) 
                + ", dataCount: " +  std::to_string(testOutput.vecCostTime.size())
                + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) + "\n", LogMutex);
}

/// 测试-脉冲式写入, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_quant_spsc_no_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("Quant Read  SPSC Initing ***********************\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"Quant Read Simple ");
    // 等待启动信号
    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing 
        || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        if (ulWaitCount++%1000000 == 0) {
            TEST_LOG_DETAIL_THREADS("Quant Read Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        }
    }

    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    testOutput.iReadCount_ = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    DataBlockFixed stFixedBlock;  
    unsigned long long ulBeforePopTimes = 0;
    unsigned long long ulAfterPopTimes = 0;    

    TEST_LOG_DEBUG_THREADS("Quant Read  SPSC Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +"***********************\n", LogMutex);

    testOutput.ulReadStartTime = get_monotonic_ns();

    while(testOutput.iReadCount_ < metaData.iWriteBlockCount) {

        while((len = workQueue.read_get(pbuf)) > 0){
            // ulBeforePopTimes = get_monotonic_ns();

            memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);
            ulAfterPopTimes = get_monotonic_ns();
            testOutput.vecCostTime[testOutput.iReadCount_] = (ulAfterPopTimes - stFixedBlock.push_time_);

            // if (testOutput.iReadCount_ < 5 || testOutput.iReadCount_ > metaData.iWriteBlockCount - 6) {
            //     TEST_LOG_DETAIL_THREADS("PushTime: " + NanoToMicroString(stFixedBlock.push_time_) 
            //                         + ", BeforePop: " + NanoToMicroString(ulBeforePopTimes)
            //                         + ", AfterPop: " + NanoToMicroString(ulAfterPopTimes)
            //                 + ", Delta: " + std::to_string(ulAfterPopTimes - stFixedBlock.push_time_), LogMutex);              
            // }

            workQueue.read_cmt();  // 提交读取（单消费者模式）

            // testOutput.iReadCount_++;
            if (testOutput.iReadCount_++ >= metaData.iWriteBlockCount) break;
        }
    }        
    
    testOutput.ulReadEndTime = get_monotonic_ns();

    TEST_LOG_DETAIL_THREADS("[END] Quant Read  DataCount: " + std::to_string(testOutput.iReadCount_) 
                + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) + "\n", LogMutex);
}

/// 测试-匀速写入, 入队到出队的 延迟表现， 在每次出队时记录出队时间,并记录时间差;
void read_quant_spsc_with_sleep(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
                                     std::atomic<unsigned long long>& ulAtoReadCount, 
                                    TestOutput& testOutput,int iCpuID,  
                                    std::mutex& LogMutex, MetaData metaData)
{    
    TEST_LOG_DETAIL_THREADS("Quant Queue Simple Read Thread Initing\n", LogMutex);
    BindCpuID(iCpuID, metaData.iNumaNode,"Quant Read Simple ");
    // 等待启动信号
    unsigned long long ulWaitCount = 1;
    while(eProcStatus == ProcessStatus::Initing 
        || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        if (ulWaitCount++%1000000 == 0) {
            TEST_LOG_DETAIL_THREADS("Quant Read Waiting ProcStatus: "+ std::to_string(int(eProcStatus)) +" ***********************\n", LogMutex);
        }
    }

    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    testOutput.iReadCount_ = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    DataBlockFixed stFixedBlock;
    unsigned long long ulPopTime = 0;    

    TEST_LOG_DEBUG_THREADS("Quant Read  Working "
                            + NanoToMicroString(get_monotonic_ns()) 
                            +"***********************\n", LogMutex);

    testOutput.ulReadStartTime = get_monotonic_ns();

    while(eProcStatus == ProcessStatus::Running) {

        while((len = workQueue.read_get(pbuf)) > 0){
            
            memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);
            ulPopTime = get_monotonic_ns();
            testOutput.vecCostTime.push_back(ulPopTime - stFixedBlock.push_time_);

            // if (ulPopTime - stFixedBlock.push_time_ == 0) {
            //     cout << NanoToMicroString(ulPopTime) << ", " << NanoToMicroString(stFixedBlock.push_time_) << endl;
            // }

            workQueue.read_cmt();  // 提交读取（单消费者模式）

            testOutput.iReadCount_++;
            if (eProcStatus != ProcessStatus::Running) break;
        }
    }        
    
    testOutput.ulReadEndTime = get_monotonic_ns();

    TEST_LOG_DETAIL_THREADS("[END] Read  Count: " + std::to_string(testOutput.iReadCount_ - 1) 
                + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) + "\n", LogMutex);
}
