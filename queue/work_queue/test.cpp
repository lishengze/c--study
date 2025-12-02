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
* 测试概述
* 1. 基本测试：测试队列的基本功能，包括入队、出队、队列长度、队列是否为空、队列是否已满等。
* 2. 边界测试：测试队列的边界情况，包括队列长度为0、队列长度为最大值、队列已满、队列为空等。
* 3. 并发测试：测试队列在多线程环境下的并发读写。
* 4. 性能测试：测试队列在不同场景下的性能，包括单线程读写、多线程读写、读写效率、队列满载、队列空载等。
* 5. 异常测试：测试队列在异常情况下的行为，包括入队时队列已满、出队时队列为空等。
* 6. 内存泄漏测试：测试队列在使用过程中是否会导致内存泄漏。
* 7. 线程安全测试：测试队列在多线程环境下是否线程安全，包括入队、出队、队列长度、队列是否为空、队列是否已满等。
* 基本参数：
*  数据块大小： 列12个大小， 随机选择写入的大小在 4B ~ 8KB 之间；
*  队列长度： 列10个长度， 随机选择队列长度在1000到10000之间；
*  读写线程数:  列10个线程数， 不使用超线程；
*  共享内存大小: 10-100MB 也就是队列的容量;
*/




int test_quant(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test Quant Start****************\n");
    if (metaData.iWriteBlockCount == 0 && metaData.iWorkSecs == 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
    }

    // 初始化共享内存相关;
    unsigned long  ulQueSize = (metaData.iMemMBSize<<20);  // 转换为字节(MB -> B)    

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWorkSecs == 0) {
        double dBlockSize = 1024;
        // if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {
        //     dBlockSize = sizeof(DataBlockFixed);
        // }
        ulQueSize = metaData.iWriteBlockCount * dBlockSize * 1.4;
    }

    // 计算所需内存大小,取 2 的幂次方;
    unsigned long  ulMemorySize = que_proc_buf::need_buf_size(ulQueSize);

    TEST_LOG_DETAIL("Memory Size=" + std::to_string(ulMemorySize) + "B, Que Size=" + std::to_string(ulQueSize) + " B");

    ulMemorySize += sizeof(que_proc_info);  // 加上队列元数据大小
    
    
    // void *pSharedMem = NULL;    
    // int ret = comm_utils::map_shm(pSharedMem,"test_proc_que",ulMemorySize,0);
    // TEST_LOG_DETAIL("Create shared memory mmap que,ret=" + std::to_string(ret));
    // if(ret < 0) {
    //     return ret;  // 共享内存创建失败
    // }

    int ret = 1;
    void * pSharedMem = malloc(ulMemorySize);
    if (NULL == pSharedMem) {
        return -1;
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf queProBuf;
    queProBuf.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize, ret, 1024, 0);
    queProBuf.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式

    std::vector<threadPtr> vecWriteTheads;
    vecWriteTheads.reserve(metaData.iWriteThreadCount);

    std::vector<threadPtr> vecReadTheads;
    vecReadTheads.reserve(metaData.iReadThreadCount);   
    
    std::vector<int64> vecReadPos;
    vecReadPos.reserve(metaData.iReadThreadCount);

    // std::vector<DataBlockPtr> vecPushBlocks;
    // vecPushBlocks.reserve(metaData.iWriteBlockCount);
    // std::vector<DataBlockPtr> vecPopBlocks;
    // vecPopBlocks.reserve(metaData.iWriteBlockCount);

    // std::vector<unsigned long long> vecCostTime;
    // vecCostTime.reserve(metaData.iWriteBlockCount);

    // for (int i = 0; i < metaData.iWriteBlockCount; ++i) {
    //     vecCostTime.push_back(0);
    // }    

    // std::vector<unsigned long long> vecStartTime;
    // vecStartTime.reserve(metaData.iWriteThreadCount + metaData.iReadThreadCount);

    // std::vector<unsigned long long> vecEndTime;
    // if (metaData.iReadType > 0) {
    //     vecEndTime.reserve(metaData.iWriteThreadCount + metaData.iReadThreadCount + 1);   // 多一个写入提交线程;
    // } else {
    //     vecEndTime.reserve(metaData.iWriteThreadCount + metaData.iReadThreadCount);  
    // }
      

    std::vector<TestOutput> vecWriteTestOutput;
    vecWriteTestOutput.reserve(metaData.iWriteThreadCount);
    for (int i = 0; i <metaData.iWriteThreadCount; ++i ) {
        vecWriteTestOutput.push_back(TestOutput( metaData.iWriteBlockCount / metaData.iWriteThreadCount, metaData.iWorkSecs));
    }

    std::vector<TestOutput> vecReadTestOutput;
    vecReadTestOutput.reserve(metaData.iReadThreadCount);
    for (int i = 0; i <metaData.iReadThreadCount; ++i ) {
        vecReadTestOutput.push_back(TestOutput( metaData.iWriteBlockCount / metaData.iWriteThreadCount, metaData.iWorkSecs));
    }      

    ProcessStatus eProcStatus = ProcessStatus::Initing;        

    std::atomic<unsigned long long> ulAtoReadCount(0);  // 总共读取的数量;
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 总共写入的数量;

    std::mutex LogMutex;

    int iThreadIndex = 0;

    // if(metaData.iTestType != (int)(TestType::Read)) {
    //     eProcStatus = ProcessStatus::Running;
    // }

    eProcStatus = ProcessStatus::Running;

    // 不是只测写入;
    if (metaData.iTestType != (int)TestType::Write) {
        /* 开启读线程 */
        for (int i = 0; i < metaData.iReadThreadCount; i++) {
            if(metaData.iReadType == 0){
                // 创建普通读取线程
                int iCurWorkCPU = metaData.vecReadCpuList[i%metaData.vecReadCpuList.size()];
                threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_simple, 
                                                        std::ref(eProcStatus), std::ref(queProBuf),  
                                                         std::ref(ulAtoReadCount), std::ref(vecReadTestOutput[i]),
                                                         iCurWorkCPU, std::ref(LogMutex), metaData);
                if (nullptr == pThread) {
                    printf("create read simple thread failed\n");
                    continue;
                }
                iThreadIndex++;
                vecReadTheads.push_back(pThread);
            }
            else{
                // 创建位置模式读取线程
                int iCurWorkCPU = metaData.vecReadCpuList[i%metaData.vecReadCpuList.size()];
                threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_pos, 
                                                        std::ref(eProcStatus),  std::ref(vecReadPos[i]), std::ref(queProBuf),  
                                                        std::ref(ulAtoReadCount), 
                                                        std::ref(vecReadTestOutput[i]), iCurWorkCPU, std::ref(LogMutex), metaData);
                if (nullptr == pThread) {
                    printf("create read pos thread failed\n");
                    continue;
                }
                iThreadIndex++;
                vecReadTheads.push_back(pThread);
            }
        }

        if (metaData.iReadType > 0) {
            // 创建提交线程
            // int iCurWorkCPU = metaData.vecReadCpuList[metaData.vecReadCpuList.size()-1];
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_cmt, 
                                                                std::ref(eProcStatus), std::ref(queProBuf),  
                                                                std::ref(vecReadPos), 
                                                                std::ref(LogMutex), metaData);    
            if (nullptr == pThread) {
                printf("create commit thread failed\n");
            }
            iThreadIndex++;
            vecReadTheads.push_back(pThread);
        }
    }

    // sleep(3);


    /* 开启写线程 */
    for (int i = 0; i < metaData.iWriteThreadCount; i++) {
        int iCurWorkCPU = metaData.vecWriteCpuList[i%metaData.vecWriteCpuList.size()];
        threadPtr pThread = std::make_shared<std::thread>(write_thread_func_quant, 
                                                            std::ref(eProcStatus), std::ref(queProBuf),  
                                                            std::ref(ulAtoWriteCount), 
                                                            std::ref(vecWriteTestOutput[i]), iCurWorkCPU, 
                                                            std::ref(LogMutex), metaData);
        if (nullptr == pThread) {
            printf("create write thread failed\n");
            continue;
        }
        iThreadIndex++;
        vecWriteTheads.push_back(pThread);
    }


    // eProcStatus = ProcessStatus::Running;

    // 如果未设置写入的块数，则等待指定的时间
    if (metaData.iWorkSecs > 0) {
        TEST_LOG_DETAIL(std::string("Start Work ") + std::to_string(metaData.iWorkSecs));
        std::this_thread::sleep_for(std::chrono::seconds(metaData.iWorkSecs));
        eProcStatus = ProcessStatus::Stop;
        TEST_LOG_DETAIL(std::string("Word END"));
    }

    for (auto pThread : vecWriteTheads) {
        if (pThread->joinable()) {
            pThread->join();
        }
    }

    for (auto pThread : vecReadTheads) {
        if (pThread->joinable()) {
            pThread->join();
        }
    }

    LOG_RST(string("\n***************** Test Result Quant Queue ****************\n") + metaData.str() );

    std::string strTestoutputStr = GetAnaTestOutputRst (vecWriteTestOutput,vecReadTestOutput, metaData.iTestType);
    LOG_RST(strTestoutputStr);

    unsigned long long ulStartTime = 0;
    unsigned long long ulEndTime = 0;
    unsigned long long ulCostTime = 0;
    GetStartEndTimeFromWriteRead(vecWriteTestOutput, vecReadTestOutput, ulStartTime, ulEndTime);

    if (metaData.iTestType == (int)TestType::Both || metaData.iTestType == (int)TestType::Detail) {
        std::vector<unsigned long long> vecCostTime;
        GetPushPopDelayVecCostTime(vecReadTestOutput, vecCostTime);        
        std::string strAnaStr = GetAnaRst(vecCostTime,metaData, ulStartTime, ulEndTime, "Quant_Queue");
        LOG_RST(strAnaStr);    
    }


    TEST_LOG_DETAIL("[END] "+ NanoToMicroString(ulEndTime) +" Quant Thread All end \n");    


    // 释放共享内存
    // comm_utils::close_shm(pSharedMem,"test_proc_que",ulMemorySize);

    if (NULL != pSharedMem) {
        free(pSharedMem);
    }

    return 0;
}

int test_mpmc(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test MPMC Start****************\n");

    if (metaData.iWriteBlockCount == 0 && metaData.iWorkSecs == 0) {
        TEST_LOG_ERROR("Test MPMC failed, both write block count and write secs is 0\n");
    }

    std::vector<threadPtr> vecWriteTheads;
    vecWriteTheads.reserve(metaData.iWriteThreadCount);

    std::vector<threadPtr> vecReadTheads;
    vecReadTheads.reserve(metaData.iReadThreadCount);   

    std::vector<TestOutput> vecWriteTestOutput;
    vecWriteTestOutput.reserve(metaData.iWriteThreadCount);
    for (int i = 0; i <metaData.iWriteThreadCount; ++i ) {
        vecWriteTestOutput.push_back(TestOutput( metaData.iWriteBlockCount / metaData.iWriteThreadCount, metaData.iWorkSecs));
    }

    std::vector<TestOutput> vecReadTestOutput;
    vecReadTestOutput.reserve(metaData.iReadThreadCount);
    for (int i = 0; i <metaData.iReadThreadCount; ++i ) {
        vecReadTestOutput.push_back(TestOutput( metaData.iWriteBlockCount / metaData.iWriteThreadCount, metaData.iWorkSecs));
    }

    int iThreadIndex = 0;

    ProcessStatus eProcStatus = ProcessStatus::Initing;          
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器
    std::mutex mtx;

    unsigned int uiQueueSize = 4000;  // 默认的队列长度

    // 表示进行数量读写的并发量的压力测试, 給队列申请足够的写入空间;
    if (metaData.iWorkSecs == 0) {
        double dBlockSize = 8;
        if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
            dBlockSize = sizeof(DataBlockFixed);
        }
        uiQueueSize = metaData.iWriteBlockCount * 1.2;
    }

    // TEST_LOG_DETAIL("MPMC QUEUE SIZE: " + std::to_string(uiQueueSize));

    tech::mpmc_queue<DataBlockFixed> dataBlockFixedQueue;
    tech::mpmc_queue<unsigned long long> ulQueue;

    std::mutex LogMutex;


    unsigned long long ulStartNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    if (metaData.iFixedBlock == 1) {
        
        dataBlockFixedQueue.create(uiQueueSize);

        if(metaData.iTestType != (int)(TestType::Read)) {
            eProcStatus = ProcessStatus::Running;
        }
        

        // 不是只测写入
        if (metaData.iTestType != (int)TestType::Write) {
            /* 开启读线程 */
            for (int i = 0; i < metaData.iReadThreadCount; i++) {

                int iCurWorkCPU = metaData.vecReadCpuList[i%metaData.vecReadCpuList.size()];

                threadPtr pThread = std::make_shared<std::thread>(read_thread_func_mpmc<DataBlockFixed>, 
                                                                    std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                    std::ref(mtx), std::ref(ulAtoReadCount),
                                                                    std::ref(vecReadTestOutput[i]),iCurWorkCPU, 
                                                                    std::ref(LogMutex), std::ref(metaData));
                iThreadIndex++;
                if (nullptr == pThread) {
                    TEST_LOG_DETAIL("create read simple thread failed\n");
                    continue;
                }
                vecReadTheads.push_back(pThread);
            }

        }

        /* 开启写线程 */
        for (int i = 0; i < metaData.iWriteThreadCount; i++) {

            int iCurWorkCPU = metaData.vecWriteCpuList[i%metaData.vecWriteCpuList.size()];

            threadPtr pThread = std::make_shared<std::thread>(write_thread_func_mpmc<DataBlockFixed>, 
                                                                std::ref(eProcStatus), std::ref(dataBlockFixedQueue),  
                                                                std::ref(mtx), std::ref(ulAtoWriteCount), 
                                                                std::ref(vecWriteTestOutput[i]), iCurWorkCPU, 
                                                                std::ref(LogMutex),  std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create write thread failed\n");
                continue;
            }
            iThreadIndex++;
            vecWriteTheads.push_back(pThread);
        }
    }
    else if (metaData.iFixedBlock == 2) {
        ulQueue.create(uiQueueSize);

        if(metaData.iTestType != (int)(TestType::Read)) {
            eProcStatus = ProcessStatus::Running;
        }

        // 不是只测写入
        if (metaData.iTestType != (int)TestType::Write) {
            /* 开启读线程 */
            for (int i = 0; i < metaData.iReadThreadCount; i++) {

                int iCurWorkCPU = metaData.vecReadCpuList[i%metaData.vecReadCpuList.size()];

                threadPtr pThread = std::make_shared<std::thread>(read_thread_func_mpmc<unsigned long long>, 
                                                                    std::ref(eProcStatus), std::ref(ulQueue),  
                                                                    std::ref(mtx), std::ref(ulAtoReadCount),
                                                                    std::ref(vecReadTestOutput[i]),iCurWorkCPU, 
                                                                    std::ref(LogMutex),  std::ref(metaData));
                if (nullptr == pThread) {
                    TEST_LOG_DETAIL("create read simple thread failed\n");
                    continue;
                }
                iThreadIndex++;
                vecReadTheads.push_back(pThread);
            }
        }

        // sleep(3);

        /* 开启写线程 */
        for (int i = 0; i < metaData.iWriteThreadCount; i++) {

            int iCurWorkCPU = metaData.vecWriteCpuList[i%metaData.vecWriteCpuList.size()];

            threadPtr pThread = std::make_shared<std::thread>(write_thread_func_mpmc<unsigned long long>, 
                                                                std::ref(eProcStatus), std::ref(ulQueue),  
                                                                std::ref(mtx), std::ref(ulAtoWriteCount), 
                                                                std::ref(vecWriteTestOutput[i]), iCurWorkCPU, 
                                                                std::ref(LogMutex),  std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create write thread failed\n");
                continue;
            }
            iThreadIndex++;
            vecWriteTheads.push_back(pThread);
        }          
    }

    
    eProcStatus = ProcessStatus::Running;


    // 如果未设置写入的块数，则等待指定的时间
    if (metaData.iWorkSecs > 0) {
        TEST_LOG_DETAIL(std::string("Start Work ") + std::to_string(metaData.iWorkSecs));
        std::this_thread::sleep_for(std::chrono::seconds(metaData.iWorkSecs));
        eProcStatus = ProcessStatus::Stop;

        TEST_LOG_DETAIL(std::string("Work ALL END, eProcStatus: ") + std::to_string((int)(eProcStatus)));
    }    


    for (auto pThread : vecReadTheads) {
            if (pThread->joinable()) {
                pThread->join();
            }
    }    
        
    for (auto pThread : vecWriteTheads) {
        if (pThread->joinable()) {
            pThread->join();
        }
    }

    // std::sort(vecStartTime.begin(), vecStartTime.end());
    // std::sort(vecEndTime.begin(), vecEndTime.end());

    // unsigned long long ulEndNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    // unsigned long long costNanosecs = ulEndNanosecs - ulStartNanosecs;

    // LOG_RST(string("\n***************** Test Result MPMC Queue ****************\n") + metaData.str() );
    // std::string strTestoutputStr = GetAnaTestOutputRst (vecWriteTestOutput, vecReadTestOutput, metaData.iTestType);
    // LOG_RST(strTestoutputStr);

    // if (metaData.iTestType == (int)TestType::Both || metaData.iTestType == (int)TestType::Detail) {
    //     unsigned long long ulStartTime = 0;
    //     unsigned long long ulEndTime = 0;
    //     GetStartEndTimeFromWriteRead(vecWriteTestOutput, vecReadTestOutput, ulStartTime, ulEndTime);

    //     std::vector<unsigned long long> vecCostTime;
    //     GetPushPopDelayVecCostTime(vecReadTestOutput, vecCostTime);
    //     std::string strAnaStr = GetAnaRst(vecCostTime,metaData, ulStartTime, ulEndTime, "MPMC_Queue");
    //     LOG_RST(strAnaStr);    
    // }


    TEST_LOG_DETAIL("[END] MPMC Thread All \n");    

    return 1;
}

// =============================================
// 定义 128 字节数据结构，前 8 字节为时间戳
// =============================================
struct Data {
        uint64_t timestamp;  // 前8字节：push 时记录的时间戳（纳秒）
        char payload[120];   // 后120字节：任意数据（无意义，测试用）
};

static_assert(sizeof(Data) == 128, "Data struct must be exactly 128 bytes");

// // =============================================
// // 获取当前时间的单调时钟（纳秒级），使用 clock_gettime
// // 返回：uint64_t，纳秒时间戳
// // =============================================
// inline uint64_t get_monotonic_ns() {
//         timespec ts;
//         if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
//                 perror("clock_gettime failed");
//                 return 0;
//         }
//         return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL + ts.tv_nsec;
// }

// =============================================
// 设置当前线程的 CPU 亲和性（绑定到某个逻辑 CPU）
// 参数：cpu_id 为 0,1,2,...（逻辑核编号）
// 返回：true 表示成功，false 表示失败
// =============================================
bool set_thread_affinity(int cpu_id) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpu_id, &cpuset);

        // 0 表示当前线程
        if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
                perror("[ERROR] sched_setaffinity failed");
                return false;
        }
        return true;
}


// =============================================
// 测试函数：1P1C，带延迟统计 + CPU 亲和性绑定
// =============================================
void test_mpmc_queue_with_affinity()
{
        using namespace std;
        using namespace tech;

        mpmc_queue<Data> queue; // ⚠️ 请替换为你的真实 mpmc_queue
        const int kNumIterations = 30000; // 建议 >=10万次，用于统计
        queue.create(kNumIterations);

        vector<uint64_t> latency_samples;
        latency_samples.reserve(kNumIterations);

        // 生产者线程
        auto producer = [&]() {
                // 绑定到 CPU 19
                if (!set_thread_affinity(8)) {
                        cerr << "[Producer] Failed to bind to CPU 19, but continuing anyway..." << endl;
                } else {
                        cout << "[Producer] Bound to CPU 19" << endl;
                }

                for (int i = 0; i < kNumIterations; ++i)
                {
                        Data d;
                        d.timestamp = get_monotonic_ns(); // 记录 push 时间戳

                        queue.push(d);

                }
        };

       // 消费者线程
        auto consumer = [&]() {
                // 绑定到 CPU 17
                if (!set_thread_affinity(9)) {
                        cerr << "[Consumer] Failed to bind to CPU 17, but continuing anyway..." << endl;
                } else {
                        cout << "[Consumer] Bound to CPU 17" << endl;
                }

                int count = 0;
                while (count < kNumIterations)
                {
                        Data d;
                        queue.pop(d);

                        uint64_t timestamp_ns = d.timestamp;
                        uint64_t now_ns = get_monotonic_ns();

                        int64_t latency_ns = static_cast<int64_t>(now_ns) - static_cast<int64_t>(timestamp_ns);

                latency_samples.push_back(static_cast<uint64_t>(latency_ns));


                        ++count;
                }
        };

        // 启动线程
        thread t2(consumer);
        // sleep(3);

        thread t1(producer);

        t1.join();
        t2.join();

        // =========================================
        // 统计分析：min/max/avg/p50/p75
        // =========================================
        if (latency_samples.empty())
        {
                cout << "No latency samples." << endl;
                return;
        }

        size_t n = latency_samples.size();
        sort(latency_samples.begin(), latency_samples.end());

        uint64_t sum = 0;
        uint64_t min_val = latency_samples.front();
        uint64_t max_val = latency_samples.back();

        for (uint64_t v : latency_samples)
                sum += v;
        double avg_val = static_cast<double>(sum) / n;

        // 分位数计算
        auto get_percentile = [&](double p) -> uint64_t {
                double pos = p * (n - 1);
                size_t idx = static_cast<size_t>(pos);
                double frac = pos - idx;
                if (idx + 1 >= n)
                        return latency_samples[idx];
                uint64_t v1 = latency_samples[idx];
                uint64_t v2 = latency_samples[idx + 1];
                return static_cast<uint64_t>(v1 + frac * (v2 - v1));
        };

        uint64_t p50 = get_percentile(0.50);
        uint64_t p75 = get_percentile(0.75);

        // =========================================
        // 打印统计结果
        // =========================================

    std::stringstream ss2;

    ss2 << "\n========== Latency Statistics (C++11 + clock_gettime) ==========\n";
    ss2 << "Total samples:     " << n << "\n";
    ss2 << "Min latency:       " << min_val << " ns (~" << min_val / 1000 << " µs)\n";
    ss2 << "Max latency:       " << max_val << " ns (~" << max_val / 1000 << " µs)\n";
    ss2 << "Avg latency:       " << fixed << setprecision(2) << avg_val
            << " ns (~" << setprecision(2) << avg_val / 1000 << " µs)\n";
    ss2 << "p50 (median):      " << p50 << " ns (~" << p50 / 1000 << " µs)\n";
    ss2 << "p75:               " << p75 << " ns (~" << p75 / 1000 << " µs)\n";
    ss2 << "================================================================\n";

    std::string tmp = ss2.str();
    LOG_RST(tmp);

}



void TestMain() {

   

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
            // TEST_LOG_DETAIL("Test Meta Info:\n" + metaData.str());
            

            for (int i = 0; i < metaData.iTestCount; ++i) {
                test_mpmc_queue_with_affinity();

                if (metaData.iQueueType == 0) {
                    test_mpmc(metaData);
                    test_quant(metaData);
                }
                else if (metaData.iQueueType == 1) {
                    test_quant(metaData);
                } else if (metaData.iQueueType == 2) {
                    test_mpmc(metaData);
                }
            }
        }
    } else {
        TEST_LOG_ERROR("Test CaseList is not array\n");
    }
}