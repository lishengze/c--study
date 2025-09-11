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





/*
 * 写入线程函数 - 生产者线程
 * 功能：持续向队列写入数据，根据配置使用普通模式或多线程安全模式
 * 参数：arg - 线程标识参数，此处为写入数据的填充字符
 * 返回值：NULL - 线程结束
 */
void *write_thread_func(int& iStopFlag, que_proc_info* pQueProcInfo, int32 iWriteCount, int iReadCount)
{
    // 计算线程休眠时间，根据写入线程数量动态调整
    int32 tus = iWriteCount/2;
    if(tus == 0){
        tus = 1;
    }
    // 若只有一个读取线程，增加写入线程休眠时间，降低队列压力
    if(iReadCount == 1)
        tus++;
    
    // 等待启动信号(iStopFlag != 0)
    while(iStopFlag == 0);
    
    // // 获取线程标识字符
    // char tc = *((char *)arg);
    // // 分配写入数据缓冲区并初始化为指定字符
    // char *tv = new char[g_wr_que_len];
    // for(int32 i=0;i<g_wr_que_len;i++){
    //     tv[i] = tc;
    // }
    // tv[g_wr_que_len-1] = '\0';  // 确保字符串结束符
    
    // printf("write thread start,c=%c\n",tc);
    
    int64 count = 0;  // 写入计数器
    int32 i= 0;       // 循环计数器
    char *pbuf;       // 指向队列缓冲区的指针
    int64 tpos;       // 写入位置
    do{
        i = 0;
        DataBlockPtr  pBlock = GetRandomDataBlock(); 
        if(pBlock == nullptr){
            continue;
        }
        
        // 根据写入线程数量选择不同的写入模式
        if(iWriteCount == 1){
            // 单生产者模式 - 使用普通写入接口
            do{
                
                // 获取写入位置和缓冲区指针
                tpos = pQueProcInfo->write_get(pBlock.get(), pBlock->size_);
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
                // memcpy(pbuf,tv,g_wr_que_len);  // 实际应用中需要复制数据
                pQueProcInfo->write_cmt(tpos,g_wr_que_len);  // 提交写入
                count++;
            }
        }
        else{
            // 多生产者模式 - 使用多线程安全写入接口
            do{
                // 获取写入位置和缓冲区指针（多线程安全版本）
                tpos = pQueProcInfo->write_get_mth(pbuf,g_wr_que_len);
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
                // memcpy(pbuf,tv,g_wr_que_len);  // 实际应用中需要复制数据
                pQueProcInfo->write_cmt_mth(tpos,g_wr_que_len);  // 提交写入（多线程安全版本）
                count++;
            }
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制写入速率
        
    }while(iStopFlag == 1);  // 当标志为1时继续运行，为2时退出
    
    // printf("write thread end,count=%ld,c=%c\n",count,tc);
    
    // delete[] tv;  // 释放缓冲区
    return NULL;
}


int test_atomic(unsigned int iMemSize, unsigned int iWriteCount, unsigned int iReadCount) {
    cout << "test_atomic" << endl;

    // 初始化共享内存相关;
    unsigned long  ulQueSize = (iMemSize<<20);  // 转换为字节(MB -> B)    

    // 计算所需共享内存大小
    unsigned long  ulShareSize = que_proc_buf::need_buf_size(ulQueSize);
    ulShareSize += sizeof(que_proc_info);  // 加上队列元数据大小
    
    void *pSharedMem = NULL;

    int ret = comm_utils::map_shm(pSharedMem,"test_proc_que",ulShareSize,0);
    printf("mmap que,ret=%d\n",ret);
    if(ret < 0) {
        return ret;  // 共享内存创建失败
    }

    // 初始化队列
    que_proc_info* pQueProcInfo = (que_proc_info *)pSharedMem;
    que_proc_buf curQue;
    curQue.init_shm(pQueProcInfo,((char *)pSharedMem) + sizeof(que_proc_info), ulQueSize,ret,1024,0);
    curQue.start(QUE_RECOVE_TYPE_RESTART);  // 启动队列，设置重启恢复模式

    std::vector<threadPtr> vecWriteTheads;
    vecWriteTheads.reserve(iWriteCount);

    std::vector<threadPtr> vecReadTheads;
    vecReadTheads.reserve(iReadCount);    

    int iStopFlag = 0;

    /* 开启写线程 */
    
    /* 开启读线程 */
    

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
}

void TestMain() {
    printf("TestMain\n");
}