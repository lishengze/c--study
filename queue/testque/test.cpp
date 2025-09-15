#include "test.h"
#include "quant_func.h"
#include "fte_func.h"
#include "struct.h"




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


void AnaRst(std::vector<unsigned long long>& vecTime, MetaData metaData) {
    unsigned long long ulMin = 3600000000000; // 1小时
    unsigned long long ulMax = 0;
    unsigned long long ulAve = 0;
    unsigned long long ulSum = 0;
    for (auto& ulTime : vecTime) {
        ulSum += ulTime;
    }

    std::sort(vecTime.begin(), vecTime.end());

    ulMin = vecTime[0];
    ulMax = vecTime[vecTime.size()-1];
    ulAve = ulSum / vecTime.size();

    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];
    
    std::string strMsg =string("*********Test Meta Info:\n") + metaData.str() 
                                + "Test Static Info:\ncount=" + std::to_string(vecTime.size()) 
                                + ",min=" + std::to_string(ulMin) + ",max=" + std::to_string(ulMax) + ",ave=" + std::to_string(ulAve) 
                                + ",50%=" + std::to_string(ul50) + ",75%=" + std::to_string(ul75) + ",90%=" + std::to_string(ul90) 
                                + "\n*********";
   
    // printf("min=%lld,max=%lld,ave=%lld,50%%=%lld,75%%=%lld,90%%=%lld\n",ulMin,ulMax,ulAve,ul50,ul75,ul90);   

    LOG_RST(strMsg);

}




int test_quant(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test Quant Start****************\n");
    // 初始化共享内存相关;
    unsigned long  ulQueSize = (metaData.iMemMBSize<<20);  // 转换为字节(MB -> B)    

    // 计算所需内存大小
    unsigned long  ulMemorySize = que_proc_buf::need_buf_size(ulQueSize);

    TEST_LOG_DETAIL("Memory Size=" + std::to_string(ulMemorySize) + "B, Que Size=" + std::to_string(ulQueSize) + " B");

    ulMemorySize += sizeof(que_proc_info);  // 加上队列元数据大小
    
    
    void *pSharedMem = NULL;

    int ret = comm_utils::map_shm(pSharedMem,"test_proc_que",ulMemorySize,0);
    TEST_LOG_DETAIL("Create shared memory mmap que,ret=" + std::to_string(ret));
    if(ret < 0) {
        return ret;  // 共享内存创建失败
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf queProBuf;
    queProBuf.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize,ret,1024,0);
    queProBuf.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式

    std::vector<threadPtr> vecWriteTheads;
    vecWriteTheads.reserve(metaData.iWriteThreadCount);

    std::vector<threadPtr> vecReadTheads;
    vecReadTheads.reserve(metaData.iReadThreadCount);   
    
    std::vector<int64> vecReadPos;
    vecReadPos.reserve(metaData.iReadThreadCount);

    std::vector<DataBlockPtr> vecPushBlocks;
    vecPushBlocks.reserve(metaData.iWriteBlockCount);
    std::vector<DataBlockPtr> vecPopBlocks;
    vecPopBlocks.reserve(metaData.iWriteBlockCount);

    std::vector<unsigned long long> vecCostTime;
    vecCostTime.reserve(metaData.iWriteBlockCount);

    int iStopFlag = 1;        

    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器

    /* 开启读线程 */
    for (int i = 0; i < metaData.iReadThreadCount; i++) {
        if(metaData.iReadType == 0){
            // 创建普通读取线程
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_simple, std::ref(iStopFlag), std::ref(queProBuf),  
                                                                std::ref(vecPopBlocks), std::ref(vecCostTime), std::ref(ulAtoReadCount), metaData);
            if (nullptr == pThread) {
                printf("create read simple thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }
        else{
            // 创建位置模式读取线程
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_pos, std::ref(iStopFlag), std::ref(vecReadPos[i]), std::ref(queProBuf),  
                                                    std::ref(vecPopBlocks), std::ref(vecCostTime), std::ref(ulAtoReadCount), metaData);
            if (nullptr == pThread) {
                printf("create read pos thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }
    }

    if (metaData.iReadType > 0) {
        // 创建提交线程
        threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quant_cmt, std::ref(iStopFlag), std::ref(queProBuf),  std::ref(vecReadPos), std::ref(vecPopBlocks), metaData);    
        if (nullptr == pThread) {
            printf("create commit thread failed\n");
        }
        vecReadTheads.push_back(pThread);
    }

    /* 开启写线程 */
    for (int i = 0; i < metaData.iWriteThreadCount; i++) {
        threadPtr pThread = std::make_shared<std::thread>(write_thread_func_quant, std::ref(iStopFlag), std::ref(queProBuf),  
                                std::ref(vecPushBlocks), std::ref(ulAtoWriteCount), metaData);
        if (nullptr == pThread) {
            printf("create write thread failed\n");
            continue;
        }
        vecWriteTheads.push_back(pThread);
    }
    
    iStopFlag = 1;

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

    AnaRst(vecCostTime,metaData);

    // 释放共享内存
    // comm_utils::close_shm(pSharedMem,"test_proc_que",ulMemorySize);

    TEST_LOG_DETAIL("[END] Thread All end, WriteCpt:"+ std::to_string(queProBuf.get_write_pos()) +", ReadCmt:"
                    + std::to_string(queProBuf.get_read_pos()) +", QueUsed:" + std::to_string(queProBuf.get_used()) +"\n");

    return 0;
}

// template <typename T>
// void write_thread_func_mpmc(ProcessStatus& eProcStatus, tech::mpmc_queue<T>& queue, std::mutex& mtx, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData& metaData)

// template <typename T>
// void read_thread_func_mpmc(ProcessStatus& eProcStatus, tech::mpmc_queue<T>& queue, std::mutex& mtx,  std::atomic<unsigned long long>& ulAtoReadCount, 
//                             std::vector<unsigned long long>& vecCostTime, MetaData& metaData)

int test_mpmc(MetaData& metaData) {
    TEST_LOG_DETAIL("****************Test MPMC Start****************\n");

    unsigned long  ulQueSize = (metaData.iMemMBSize<<10);  // 转换为字节(MB -> B)    

    // 计算所需内存大小
    unsigned long  ulMemorySize = que_proc_buf::need_buf_size(ulQueSize);

    std::vector<threadPtr> vecWriteTheads;
    vecWriteTheads.reserve(metaData.iWriteThreadCount);

    std::vector<threadPtr> vecReadTheads;
    vecReadTheads.reserve(metaData.iReadThreadCount);   
    
    std::vector<unsigned long long> vecCostTime;
    vecCostTime.reserve(metaData.iWriteBlockCount);

    ProcessStatus eProcStatus = ProcessStatus::NotInit;          
    std::atomic<unsigned long long> ulAtoReadCount(0);  // 原子计数器
    std::atomic<unsigned long long> ulAtoWriteCount(0);  // 原子计数器
    std::mutex mtx;

    unsigned int uiQueueSize = 1000;

    // tech::mpmc_queue<int> queue;
    // queue.create(uiQueueSize);

    if (metaData.iFixedBlock == 1) {
        tech::mpmc_queue<DataBlockFixed> queue;
        queue.create(uiQueueSize);

        /* 开启读线程 */
        for (int i = 0; i < metaData.iReadThreadCount; i++) {
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_mpmc<DataBlockFixed>, std::ref(eProcStatus), std::ref(queue),  
                                                                std::ref(mtx), std::ref(ulAtoReadCount), std::ref(vecCostTime), std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create read simple thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }


        /* 开启写线程 */
        for (int i = 0; i < metaData.iWriteThreadCount; i++) {
            threadPtr pThread = std::make_shared<std::thread>(write_thread_func_mpmc<DataBlockFixed>, std::ref(eProcStatus), std::ref(queue),  
                                    std::ref(mtx), std::ref(ulAtoWriteCount), std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create write thread failed\n");
                continue;
            }
            vecWriteTheads.push_back(pThread);
        }        
    }
    else if (metaData.iFixedBlock == 2) {
        tech::mpmc_queue<unsigned long long> queue;
        queue.create(uiQueueSize);

        /* 开启读线程 */
        for (int i = 0; i < metaData.iReadThreadCount; i++) {
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_mpmc<unsigned long long>, std::ref(eProcStatus), std::ref(queue),  
                                                                std::ref(mtx), std::ref(ulAtoReadCount), std::ref(vecCostTime), std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create read simple thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }


        /* 开启写线程 */
        for (int i = 0; i < metaData.iWriteThreadCount; i++) {
            threadPtr pThread = std::make_shared<std::thread>(write_thread_func_mpmc<unsigned long long>, std::ref(eProcStatus), std::ref(queue),  
                                    std::ref(mtx), std::ref(ulAtoWriteCount), std::ref(metaData));
            if (nullptr == pThread) {
                TEST_LOG_DETAIL("create write thread failed\n");
                continue;
            }
            vecWriteTheads.push_back(pThread);
        }           
    }

    
    eProcStatus = ProcessStatus::Running;

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

    AnaRst(vecCostTime,metaData);


    TEST_LOG_DETAIL("[END] MPMC Thread All end, \n");    

    return 1;
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
    if (fileJson.is_array()) {
        for (njson::iterator it = fileJson.begin(); it != fileJson.end(); ++it) {           
            njson jsAtom = *it;
            if (!metaData.InitFromJson(jsAtom, sErrMsg)) {
                TEST_LOG_ERROR( "InitFromJson failed, error: " + sErrMsg + "\n");
                continue;
            }
            TEST_LOG_DETAIL("Test Meta Info:\n" + metaData.str());

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
}