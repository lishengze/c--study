#include "test.h"
#include <iostream>
#include <stdio.h>
#include <memory>
#include <thread>
#include <array>
#include <vector>

using namespace std;


#include "mutils.h"
#include "comm_sys.h"
#include "que_proc_buf.h"
#include "struct.h"
#include "base_util.h"
#include <algorithm>

using threadPtr = std::shared_ptr<std::thread>;

using namespace lb_common;


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


void AnaRst(std::vector<DataBlockPtr>& vecPushBlocks, std::vector<DataBlockPtr>& vecPopBlocks, MetaData) {
    unsigned long long ulMin = 3600000000000; // 1小时
    unsigned long long ulMax = 0;
    unsigned long long ulAve = 0;
    unsigned long long ulSum = 0;
    std::vector<unsigned long long> vecTime;
    for (auto& pBlock : vecPopBlocks) {
        unsigned long long ulTime = pBlock->pop_time_ - pBlock->push_time_;
        // printf("pop_time=%lld, push_time=%lld, time=%lld\n", pBlock->pop_time_, pBlock->push_time_, ulTime);
        if(ulTime < ulMin)
            ulMin = ulTime;
        if(ulTime > ulMax)
            ulMax = ulTime;
        ulSum += ulTime;
        vecTime.push_back(ulTime);
    }

    std::sort(vecTime.begin(), vecTime.end());

    ulAve = ulSum / vecPopBlocks.size();

    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];
    
    printf("min=%lld,max=%lld,ave=%lld,50%%=%lld,75%%=%lld,90%%=%lld\n",ulMin,ulMax,ulAve,ul50,ul75,ul90);   


}


/*
 * 读取线程函数 - 消费者线程（模式2）
 * 功能：读取指定位置的数据，用于多消费者分别读取不同区域的场景
 * 参数：arg - 指向存储读取位置的指针
 * 返回值：NULL - 线程结束
 */
void *read_thread_func_quota_pos(int& iStopFlag, int64& readPos, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPopBlocks, MetaData metaData)
{
    // 计算线程休眠时间
    int32 tus = metaData.iReadThreadCount/2;
    if(tus == 0)
        tus = 1;
    
    // 等待启动信号
    while(iStopFlag == 0);
    
    char tc;                       // 用于验证数据一致性的字符
    int64 count = 0;               // 读取计数器
    int32 len = 0;                 // 单次读取长度
    char *pbuf;                    // 指向队列数据的指针
    readPos = queProBuf.get_read_pos();  // 初始化读取位置
    
    // printf("read pos thread start,read_pos=%ld\n",readPos);
    
    do{
        // 从指定位置读取数据
        while((len = queProBuf.read_get(pbuf,readPos)) > 0){
            DataBlock* pBlock = (DataBlock*)pbuf;
            pBlock->pop_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            // vecPopBlocks.push_back(GetCopyBlock(pBlock));
            // assert(len == g_wr_que_len);  // 验证读取长度
            /* 数据验证代码（注释掉以提高性能）
            tc = pbuf[0];
            for(int32 j=1;j<len-2;j++){
                assert(tc == pbuf[j]);
            }
            */
            readPos = queProBuf.next_pos(readPos,len);  // 更新读取位置
            if(metaData.iReadThreadCount == 1)
                queProBuf.read_cmt_pos(readPos);  // 提交读取位置（单消费者模式）
            
            count++;
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制读取速率
        
    }while(iStopFlag == 1 || queProBuf.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("read pos thread end,count=%ld,read_pos=%ld\n",count,readPos);
    
    return NULL;
}

/*
 * 提交读取位置线程函数
 * 功能：协调多个读取线程的进度，提交最小的已读取位置
 */
void *read_thread_func_quota_cmt(int& iStopFlag, que_proc_buf& queProBuf, vector<int64>& vecReadPos, std::vector<DataBlockPtr>& vecPopBlocks, MetaData metaData)
{
    // 计算线程休眠时间
    int32 tus = metaData.iReadThreadCount/2;
    if(tus == 0)
        tus = 1;
    if(metaData.iReadThreadCount == 1)
        return NULL;  // 单消费者模式下不需要此线程
    
    // 等待启动信号
    while(iStopFlag == 0);
    
    printf("commit pos thread start\n");
    
    int32 i= 0;
    int64 tpos = 0;
    
    do{
        tpos = vecReadPos[0];
        // 找出所有读取线程中的最小读取位置
        for(i=1;i<metaData.iReadThreadCount;i++){
            int64 t= vecReadPos[i];
            if(tpos > t){
                tpos = t;
            }
        }
        // queProBuf.read_cmt_mth(tpos);  // 使用多线程安全模式提交
        queProBuf.read_cmt_pos(tpos);    // 提交最小读取位置
        // comm_utils::sleep_us(tus);  // 可选：控制提交频率
        
    }while(iStopFlag == 1 || queProBuf.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("commit pos thread end,commit_pos=%ld\n",tpos);
    
    return NULL;
}

bool IsWriteEnd(MetaData& metaData, bool isStopFlag, int writeIndex, unsigned long long ulStartNanoTime) {
    if (!isStopFlag) return true;

    if (metaData.iWriteSecs > 0) {  // 写入时间限制模式
        unsigned long long ulCurTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (ulCurTime - ulStartNanoTime >= metaData.iWriteSecs * 1000000000) {
            return true;
        }
    }

    if (metaData.iWriteBlockCount > 0) { //写入数量限制模式
        if (writeIndex >= metaData.iWriteBlockCount) {
            return true;
        }
    }

    return false;   
}

void write_thread_func_quta(int& iStopFlag, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPushBlocks, MetaData metaData)
{
    // std::cout << "[START] write_thread_func_quta , metaData=" << metaData.str() << std::endl;

    // 计算线程休眠时间，根据写入线程数量动态调整
    int32 tus = metaData.iWriteThreadCount/2;
    if(tus == 0){
        tus = 1;
    }
    // 若只有一个读取线程，增加写入线程休眠时间，降低队列压力
    if(metaData.iReadThreadCount == 1)
        tus++;
    
    // 等待启动信号(iStopFlag != 0)
    while(iStopFlag == 0);

    printf("[START] Write thread start\n");
        
    int64 count = 0;  // 写入计数器
    int32 i= 0;       // 循环计数器
    char *pbuf;       // 指向队列缓冲区的指针
    int64 tpos;       // 写入位置
    unsigned long long ulStartNanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


    do{
        i = 0;
        DataBlockPtr  pBlock = GetRandomDataBlock();
        if(pBlock == nullptr){
            continue;
        }
        
        // 根据写入线程数量选择不同的写入模式
        if(metaData.iWriteThreadCount == 1){
            // 单生产者模式 - 使用普通写入接口
            do{
                
                // 获取写入位置和缓冲区指针
                tpos = queProBuf.write_get(pbuf, pBlock->size_);
                if(tpos > 0)  // 成功获取到写入位置
                    break;
                
                // 每30000000次循环打印一次队列已满信息（避免频繁打印影响性能）
                if((i/30000000) == 0){
                    // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                    i = 0;
                }
                i++;
            }while(iStopFlag == 1);  // 当标志为1时继续尝试

            if(tpos >0){
                vecPushBlocks.push_back(pBlock);
                // memcpy(pbuf,pBlock.get(),pBlock->size_);  // 实际应用中需要复制数据

                // 此次拷贝的结构体含有虚函数，无法通过memcpy 直接进行拷贝, 因此需要调用CopyDataBlockToBuffer函数, 拷贝的最后一步，会自动更新时间戳
                CopyDataBlockToBuffer(pbuf, pBlock.get()); 
                queProBuf.write_cmt(tpos,pBlock->size_);  // 提交写入
                count++;
            }

            // printf("Write thread get used=%d\n",queProBuf.get_used());
            // printf("Single Thread Write count: %ld,len=%u\n", count, pBlock->size_);
        }
        else{
            // 多生产者模式 - 使用多线程安全写入接口
            do{
                // 获取写入位置和缓冲区指针（多线程安全版本）
                tpos = queProBuf.write_get_mth(pbuf,pBlock->size_);
                if(tpos > 0)  // 成功获取到写入位置
                    break;
                
                // 每30000000次循环打印一次队列已满信息
                if((i/30000000) == 0){
                    // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                    i = 0;
                }
                i++;
            }while(iStopFlag == 1);  // 当标志为1时继续尝试
            
            if(tpos >0){
                vecPushBlocks.push_back(pBlock);                
                // memcpy(pbuf,pBlock.get(),pBlock->size_);  // 实际应用中需要复制数据
                // CopyDataBlockToBuffer(pbuf, pBlock.get()); // 拷贝的最后一步，会自动更新时间戳
                queProBuf.write_cmt_mth(tpos,pBlock->size_);  // 提交写入（多线程安全版本）
                count++;
            }
        }
        
        comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制写入速率
        
    } while(!IsWriteEnd(metaData, iStopFlag, count, ulStartNanoTime));  // 当标志为1时继续运行，为2时退出
    
    unsigned long long ulEndNanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "[END] Write Thread Count=" << count << ",time=" << (ulEndNanoTime - ulStartNanoTime)  << " nanoseconds" << endl;

    iStopFlag = 2;  // 结束信号,写线程结束，读线程也就结束了；

    // printf("write thread end,count=%ld,c=%c\n",count,tc);
    
    // delete[] tv;  // 释放缓冲区
}

void read_thread_func_quota_simple(int& iStopFlag, que_proc_buf& queProBuf,  std::vector<DataBlockPtr>& vecPopBlocks, MetaData metaData)
{
    // std::cout << "[START] read_thread_func_quota_simple ,metaData=" << metaData.str() << std::endl;

    // 计算线程休眠时间，根据读取线程数量动态调整
    // int32 tus = metaData.iReadThreadCount/2;
    // if(tus == 0)
    //     tus = 1;
    
    // 等待启动信号
    while(iStopFlag == 0);
    
    printf("[START] Read thread start\n");
    
    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    int64 count = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    do{
        if(metaData.iReadThreadCount == 1){

            // printf("Read thread get used=%d\n", queProBuf.get_used());

            // 单消费者模式 - 直接读取并提交
            while((len = queProBuf.read_get(pbuf)) > 0){
                DataBlock* pBlock = (DataBlock*)pbuf;
                pBlock->pop_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                vecPopBlocks.push_back(GetCopyBlock(pBlock));

                // assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = pbuf[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == pbuf[j]);
                }
                */
                queProBuf.read_cmt();  // 提交读取（单消费者模式）
                count++;
                // std::cout << "Single Thread Read count: " << count << " len: " << len << std::endl;
            }

            // std::cout << "[Read Failed] read_thread_func_quota_simple,count=" << count << std::endl;
        }
        else{
            // 多消费者模式 - 使用弹出接口
            while((len = queProBuf.read_pop(tcache,sizeof(tcache))) > 0){
                DataBlock* pBlock = (DataBlock*)tcache;
                pBlock->pop_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                vecPopBlocks.push_back(GetCopyBlock(pBlock));

                // assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = tcache[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == tcache[j]);
                }
                */
                count++;
            }
        }
        
        // comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制读取速率
        
    }while(iStopFlag == 1 || queProBuf.get_used()>0);  // 标志为1或队列非空时继续运行
    
    printf("[END] Read Thread Count=%ld\n",count);
}


int test_atomic(MetaData& metaData) {
    cout << "test_atomic" << endl;

    // 初始化共享内存相关;
    unsigned long  ulQueSize = (metaData.iMemMBSize<<20);  // 转换为字节(MB -> B)    

    // 计算所需共享内存大小
    unsigned long  ulShareSize = que_proc_buf::need_buf_size(ulQueSize);
    ulShareSize += sizeof(que_proc_info);  // 加上队列元数据大小
    
    void *pSharedMem = NULL;

    int ret = comm_utils::map_shm(pSharedMem,"test_proc_que",ulShareSize,0);
    printf("Create shared memory mmap que,ret=%d\n",ret);
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

    int iStopFlag = 1;        

    /* 开启读线程 */
    for (int i = 0; i < metaData.iReadThreadCount; i++) {
        if(metaData.iReadType == 0){
            // 创建普通读取线程
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quota_simple, std::ref(iStopFlag), std::ref(queProBuf),  std::ref(vecPopBlocks), metaData);
            if (nullptr == pThread) {
                printf("create read simple thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }
        else{
            // 创建位置模式读取线程
            threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quota_pos, std::ref(iStopFlag), std::ref(vecReadPos[i]), std::ref(queProBuf),  std::ref(vecPopBlocks), metaData);
            if (nullptr == pThread) {
                printf("create read pos thread failed\n");
                continue;
            }
            vecReadTheads.push_back(pThread);
        }
    }

    if (metaData.iReadType > 0) {
        // 创建提交线程
        threadPtr pThread = std::make_shared<std::thread>(read_thread_func_quota_cmt, std::ref(iStopFlag), std::ref(queProBuf),  std::ref(vecReadPos), std::ref(vecPopBlocks), metaData);    
        if (nullptr == pThread) {
            printf("create commit thread failed\n");
        }
        vecReadTheads.push_back(pThread);
    }

    /* 开启写线程 */
    for (int i = 0; i < metaData.iWriteThreadCount; i++) {
        threadPtr pThread = std::make_shared<std::thread>(write_thread_func_quta, std::ref(iStopFlag), std::ref(queProBuf),  std::ref(vecPushBlocks), metaData);
        if (nullptr == pThread) {
            printf("create write thread failed\n");
            continue;
        }
        vecWriteTheads.push_back(pThread);
    }
    
    // iStopFlag = 1;

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

    AnaRst(vecPushBlocks, vecPopBlocks,metaData);

    // 释放共享内存
    // comm_utils::close_shm(pSharedMem,"test_proc_que",ulShareSize);

    printf("[END] Thread All end,que_wrcmt=%ld,que_rdcmt=%ld,que_uesd=%ld\n",
    queProBuf.get_write_pos(),
    queProBuf.get_read_pos(),
    queProBuf.get_used());
    

    return 0;
}

// 测试一个基础场景；
void Test1() {
    std::string sConfigFileName = GetWorkDir() + "config.json";
    std::cout << "sConfigFileName: " << sConfigFileName << std::endl;
    njson fileJson;
    Error error;
    if ((error = GetJsonFromFile(fileJson, sConfigFileName)).IsFailed()) {
        std::cout << "GetJsonFromFile failed, error: " << error.Str() << std::endl;
    }

    MetaData metaData;
    string sErrMsg;    
    if (fileJson.is_array()) {
        for (njson::iterator it = fileJson.begin(); it != fileJson.end(); ++it) {           
            njson jsAtom = *it;
            if (!metaData.InitFromJson(jsAtom, sErrMsg)) {
                std::cout << "InitFromJson failed, error: " << sErrMsg << std::endl;
            }
            std::cout << "metaData: " << metaData.str() << std::endl;

            test_atomic(metaData);
        }
    }
    // MetaData metaData;
    // metaData.iMemMBSize = fileJson["mem_mb_size"].GetInt();
    // metaData.iWriteThreadCount = fileJson["write_thread_count"].GetInt();
    // metaData.iReadThreadCount = fileJson["read_thread_count"].GetInt();
    // metaData.iMemMBSize = 2; //10MB
    // metaData.iWriteThreadCount = 1;
    // metaData.iReadThreadCount = 1;
    // metaData.iWriteBlockCount = 1000;
    // metaData.iWriteSecs = 0;
    // metaData.iReadType = 0;
    // metaData.iSleepTimeUs = 10000;
    
}

void TestMain() {
    printf("Test Queue Main\n");

    Test1();
}