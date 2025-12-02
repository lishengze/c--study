#include "test.h"
#include "quant_func.h"
#include "fte_func.h"
#include "struct.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <sched.h>    // for sched_setaffinity, CPU_SET
#include <cmath>
#include <unistd.h>


using threadPtr = std::shared_ptr<std::thread>;



/*
主要测试四种场景;
1. 固定数量-脉冲式入队时 - 入队到出队的时间延迟;
2. 固定数量-匀速入队 - 入队到出队的时间延迟;
3. 固定数量 - 只测试单线程压满队列队列的总耗时以及平均耗时；
4. 固定数量 - 队列满后，单线程出队的总耗时以及平均耗时；
*/

unsigned int GetQueueSize(MetaData& metaData) {
    unsigned int uiQueueSize = 4000;  // 默认的队列长度

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWorkSecs == 0) {
        double dBlockSize = 8;
        if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
            dBlockSize = sizeof(DataBlockFixed);
        }
        uiQueueSize = metaData.iWriteBlockCount * 1.2;
    }
    return uiQueueSize;
}

void test_mpmc_simple_both(MetaData& metaData) {
    if (metaData.iWriteBlockCount <= 0 && metaData.iWorkSecs <= 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
        return;
    }    

    tech::mpmc_queue<DataBlockFixed> dataBlockFixedQueue;
    unsigned int uiQueueSize = GetQueueSize(metaData);
    dataBlockFixedQueue.create(uiQueueSize);

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput readTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);
    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);

    threadPtr pReadThread = nullptr;
    threadPtr pWriteThread = nullptr;


    if (metaData.iWorkSecs > 0) {
        pReadThread = std::make_shared<std::thread>(read_mpmc_spsc_with_sleep, 
                                                    std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                    std::ref(mtx), std::ref(ulAtoReadCount),
                                                    std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                    std::ref(LogMutex), std::ref(metaData));

        if (nullptr == pReadThread) {
            TEST_LOG_FAIL("create read thread failed\n");
            return ;
        }   

        pWriteThread =  std::make_shared<std::thread>(write_mpmc_spsc_with_sleep,
                                                        std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                        std::ref(mtx), std::ref(ulAtoWriteCount),
                                                        std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                        std::ref(LogMutex), std::ref(metaData));
                    
        if (nullptr == pWriteThread) {
            TEST_LOG_FAIL("create write thread failed\n");
            return ;
        } 
                            
    } else if (metaData.iWriteBlockCount > 0) {
        pReadThread = std::make_shared<std::thread>(read_mpmc_spsc_no_sleep, std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                    std::ref(mtx), std::ref(ulAtoReadCount),
                                                                    std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                                    std::ref(LogMutex), std::ref(metaData));

        if (nullptr == pReadThread) {
            TEST_LOG_FAIL("create read thread failed\n");
            return ;
        }   

        // std::this_thread::sleep_for(std::chrono::seconds(3));   

        pWriteThread =  std::make_shared<std::thread>(write_mpmc_spsc_no_sleep, std::ref(eProcStatus), 
                                                    std::ref(dataBlockFixedQueue),  
                                                    std::ref(mtx), std::ref(ulAtoWriteCount),
                                                    std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                    std::ref(LogMutex), std::ref(metaData));
                    
        if (nullptr == pWriteThread) {
            TEST_LOG_FAIL("create write thread failed\n");
            return ;
        } 
    }

    if (metaData.iWorkSecs > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(metaData.iWorkSecs));
        eProcStatus = ProcessStatus::Stop;
        TEST_LOG_DETAIL(std::string("Work ALL END, eProcStatus: ") + std::to_string((int)(eProcStatus)));
    }        

    if (pReadThread && pReadThread->joinable()) {
        pReadThread->join();
    }

    if (pWriteThread && pWriteThread->joinable()) {
        pWriteThread->join();
    }    


    unsigned long long ulStartTime = std::min(readTestOutput.ulReadStartTime, writeTestOutput.ulWriteStartTime);
    unsigned long long ulEndTime = std::max(readTestOutput.ulReadEndTime, writeTestOutput.ulWriteEndTime);

    std::string strAnaStr = GetAnaRst(readTestOutput.vecCostTime, metaData, 
                                      ulStartTime,
                                      ulEndTime, "MPMC_Queue");

    LOG_RST(strAnaStr);      
}

void test_mpmc_simple_read(MetaData& metaData) {
    if (metaData.iWriteBlockCount <= 0 ) {
        TEST_LOG_ERROR("iWriteBlockCount 0\n");
        return;
    }    

    tech::mpmc_queue<DataBlockFixed> dataBlockFixedQueue;
    unsigned int uiQueueSize = GetQueueSize(metaData);
    dataBlockFixedQueue.create(uiQueueSize);

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput readTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);
    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);


    threadPtr pWriteThread =  std::make_shared<std::thread>(write_mpmc_spsc_no_sleep, std::ref(eProcStatus), 
                                                std::ref(dataBlockFixedQueue),  
                                                std::ref(mtx), std::ref(ulAtoWriteCount),
                                                std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                std::ref(LogMutex), std::ref(metaData));
                
    if (nullptr == pWriteThread) {
        TEST_LOG_FAIL("create write thread failed\n");
        return ;
    } 

    threadPtr pReadThread = std::make_shared<std::thread>(read_mpmc_pop, std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                std::ref(mtx), std::ref(ulAtoReadCount),
                                                                std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                                std::ref(LogMutex), std::ref(metaData));

    if (nullptr == pReadThread) {
        TEST_LOG_FAIL("create read thread failed\n");
        return ;
    }   

    if (pReadThread->joinable()) {
        pReadThread->join();
    }

    if (pWriteThread->joinable()) {
        pWriteThread->join();
    }        
}

void test_mpmc_simple_write(MetaData& metaData) {
    if (metaData.iWriteBlockCount <= 0 ) {
        TEST_LOG_ERROR("iWriteBlockCount 0\n");
        return;
    }    

    tech::mpmc_queue<DataBlockFixed> dataBlockFixedQueue;
    unsigned int uiQueueSize = GetQueueSize(metaData);
    dataBlockFixedQueue.create(uiQueueSize);

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);


    threadPtr pWriteThread =  std::make_shared<std::thread>(write_mpmc_push, 
                                                std::ref(eProcStatus), 
                                                std::ref(dataBlockFixedQueue),  
                                                std::ref(mtx), std::ref(ulAtoWriteCount),
                                                std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                std::ref(LogMutex), std::ref(metaData));
                
    if (nullptr == pWriteThread) {
        TEST_LOG_FAIL("create write thread failed\n");
        return ;
    } 

    if (pWriteThread->joinable()) {
        cout << "****** JOIN *******" << endl;
        pWriteThread->join();
    }          
    
}

void test_mpmc_simple(MetaData& metaData) {
    if (metaData.iTestType == (int)(TestType::Both)) {

        test_mpmc_simple_both(metaData);

    } else if (metaData.iTestType == (int)(TestType::Read)) {

        test_mpmc_simple_read(metaData);

    } else if (metaData.iTestType == (int)(TestType::Write)) {

        test_mpmc_simple_write(metaData);

    }
}

void test_quant_simple_both(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test Quant Simple SPSC Start****************\n");
    if (metaData.iWriteBlockCount == 0 && metaData.iWorkSecs == 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
    }

    // 初始化共享内存相关;
    unsigned long long ulQueSize = 128 * 1024 * 1024;

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWriteBlockCount >= 0) {
        ulQueSize = metaData.iWriteBlockCount * 128 * 1.4;
    }

    unsigned long long ulMemorySize = que_proc_buf::need_buf_size(ulQueSize) + sizeof(que_proc_info);
    void * pSharedMem = malloc(ulMemorySize);
    if (NULL == pSharedMem) {
        return;
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf dataBlockFixedQueue;
    dataBlockFixedQueue.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize, 1, 1024, 0);
    dataBlockFixedQueue.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式    

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput readTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);
    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);

    threadPtr pReadThread = nullptr;
    threadPtr pWriteThread = nullptr;


    if (metaData.iWorkSecs > 0) {
        pReadThread = std::make_shared<std::thread>(read_quant_spsc_with_sleep, std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                    std::ref(ulAtoReadCount),
                                                                    std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                                    std::ref(LogMutex), std::ref(metaData));

        if (nullptr == pReadThread) {
            TEST_LOG_FAIL("create read thread failed\n");
            return ;
        }      

        

        pWriteThread =  std::make_shared<std::thread>(write_quant_spsc_with_sleep, std::ref(eProcStatus), 
                                                    std::ref(dataBlockFixedQueue),  
                                                    std::ref(ulAtoWriteCount),
                                                    std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                    std::ref(LogMutex), std::ref(metaData));
                    
        if (nullptr == pWriteThread) {
            TEST_LOG_FAIL("create write thread failed\n");
            return ;
        } 

    
                            
    } else if (metaData.iWriteBlockCount > 0) {
        pReadThread = std::make_shared<std::thread>(read_quant_spsc_no_sleep, std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                    std::ref(ulAtoReadCount),
                                                                    std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                                    std::ref(LogMutex), std::ref(metaData));

        if (nullptr == pReadThread) {
            TEST_LOG_FAIL("create read thread failed\n");
            return ;
        }   

        // std::this_thread::sleep_for(std::chrono::seconds(3));     

        pWriteThread =  std::make_shared<std::thread>(write_quant_spsc_no_sleep, std::ref(eProcStatus), 
                                                    std::ref(dataBlockFixedQueue),  
                                                    std::ref(ulAtoWriteCount),
                                                    std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                    std::ref(LogMutex), std::ref(metaData));                    
        if (nullptr == pWriteThread) {
            TEST_LOG_FAIL("create write thread failed\n");
            return ;
        } 
    }

    // 如果未设置写入的块数，则等待指定的时间
    if (metaData.iWorkSecs > 0) {
        // TEST_LOG_DETAIL(std::string("Start Work ") + std::to_string(metaData.iWorkSecs));
        std::this_thread::sleep_for(std::chrono::seconds(metaData.iWorkSecs));
        eProcStatus = ProcessStatus::Stop;

        // TEST_LOG_DETAIL(std::string("Work ALL END, eProcStatus: ") + std::to_string((int)(eProcStatus)));
    }  

    if (pReadThread && pReadThread->joinable()) {
        pReadThread->join();
    }

    if (pWriteThread && pWriteThread->joinable()) {
        pWriteThread->join();
    }          

    unsigned long long ulStartTime = std::min(readTestOutput.ulReadStartTime, writeTestOutput.ulWriteStartTime);
    unsigned long long ulEndTime = std::max(readTestOutput.ulReadEndTime, writeTestOutput.ulWriteEndTime);

    std::string strAnaStr = GetAnaRst(readTestOutput.vecCostTime, metaData, 
                                      ulStartTime,
                                      ulEndTime, "Quant_Queue");
    LOG_RST(strAnaStr);        
}

void test_quant_simple_read(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test Quant Simple Read Start****************\n");
    if (metaData.iWriteBlockCount == 0 && metaData.iWorkSecs == 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
    }

    // 初始化共享内存相关;
    unsigned long long ulQueSize = 128 * 1024 * 1024;

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWriteBlockCount >= 0) {
        ulQueSize = metaData.iWriteBlockCount * 128 * 1.4;
    }

    unsigned long long ulMemorySize = que_proc_buf::need_buf_size(ulQueSize) + sizeof(que_proc_info);
    void * pSharedMem = malloc(ulMemorySize);
    if (NULL == pSharedMem) {
        return;
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf dataBlockFixedQueue;
    dataBlockFixedQueue.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize, 1, 1024, 0);
    dataBlockFixedQueue.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式    

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput readTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);
    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);

    threadPtr pReadThread = nullptr;
    threadPtr pWriteThread = nullptr;

    pReadThread = std::make_shared<std::thread>(read_quant_pop, std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                std::ref(ulAtoReadCount),
                                                                std::ref(readTestOutput),metaData.vecReadCpuList[0], 
                                                                std::ref(LogMutex), std::ref(metaData));

    if (nullptr == pReadThread) {
        TEST_LOG_FAIL("create read thread failed\n");
        return ;
    }   

    pWriteThread =  std::make_shared<std::thread>(write_quant_spsc_no_sleep, std::ref(eProcStatus), 
                                                std::ref(dataBlockFixedQueue),  
                                                std::ref(ulAtoWriteCount),
                                                std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                std::ref(LogMutex), std::ref(metaData));
                
    if (nullptr == pWriteThread) {
        TEST_LOG_FAIL("create write thread failed\n");
        return ;
    } 

    if (pReadThread->joinable()) {
        pReadThread->join();
    }

    if (pWriteThread->joinable()) {
        pWriteThread->join();
    }        
}

void test_quant_simple_write(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test Quant Simple Write Start****************\n");
    if (metaData.iWriteBlockCount == 0 && metaData.iWorkSecs == 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
    }

    // 初始化共享内存相关;
    unsigned long long ulQueSize = 128 * 1024 * 1024;

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWriteBlockCount >= 0) {
        ulQueSize = metaData.iWriteBlockCount * 128 * 1.4;
    }

    unsigned long long ulMemorySize = que_proc_buf::need_buf_size(ulQueSize) + sizeof(que_proc_info);
    void * pSharedMem = malloc(ulMemorySize);
    if (NULL == pSharedMem) {
        return;
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf dataBlockFixedQueue;
    dataBlockFixedQueue.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize, 1, 1024, 0);
    dataBlockFixedQueue.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式    

    ProcessStatus eProcStatus = ProcessStatus::Running;
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    std::mutex LogMutex;
    std::mutex mtx;

    TestOutput writeTestOutput(metaData.iWriteBlockCount, metaData.iWorkSecs, metaData.iSleepTimeUs);

    threadPtr pWriteThread = nullptr;

    pWriteThread =  std::make_shared<std::thread>(write_quant_push, std::ref(eProcStatus), 
                                                std::ref(dataBlockFixedQueue),  
                                                std::ref(ulAtoWriteCount),
                                                std::ref(writeTestOutput),metaData.vecWriteCpuList[0], 
                                                std::ref(LogMutex), std::ref(metaData));
                
    if (nullptr == pWriteThread) {
        TEST_LOG_FAIL("create write thread failed\n");
        return ;
    } 

    if (pWriteThread->joinable()) {
        pWriteThread->join();
    }        
}

void test_quant_simple(MetaData& metaData) {
    TEST_LOG_DETAIL("Test Quant Start!");
    if (metaData.iTestType == (int)(TestType::Both)) {

        test_quant_simple_both(metaData);

    } else if (metaData.iTestType == (int)(TestType::Read)) {

        test_quant_simple_read(metaData);

    } else if (metaData.iTestType == (int)(TestType::Write)) {

        test_quant_simple_write(metaData);

    }
}

void TestSimple() {
   std::string sConfigFileName = GetWorkDir() + "config.json";
    // std::cout << "sConfigFileName: " << sConfigFileName << std::endl;
    njson fileJson;
    Error error;
    if ((error = GetJsonFromFile(fileJson, sConfigFileName)).IsFailed()) {
        TEST_LOG_FAIL("GetJsonFromFile failed, error: " + error.Str() + "\n");
        return;
    }

    MetaData metaData;
    string sErrMsg;    
    njson jsCaseList = fileJson["CaseList"];
    if (jsCaseList.is_array()) {
        for (njson::iterator it = jsCaseList.begin(); it != jsCaseList.end(); ++it) {           
            njson jsAtom = *it;
            if (!metaData.InitFromJson(jsAtom, sErrMsg)) {
                TEST_LOG_ERROR( "InitFromJson failed, error: " + sErrMsg + "\n");
                continue;
            }
            TEST_LOG_DETAIL(metaData.str());
            for (int i = 0; i < metaData.iTestCount; ++i) {

                if (metaData.iQueueType == 0) {
                    test_mpmc_simple(metaData);
                    test_quant_simple(metaData);
                }
                else if (metaData.iQueueType == 1) {
                    test_quant_simple(metaData);
                } else if (metaData.iQueueType == 2) {
                    test_mpmc_simple(metaData);
                }
            }
        }
    } else {
        TEST_LOG_ERROR("Test CaseList is not array\n");
    }
}