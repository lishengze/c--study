#include "quant_func.h"
#include "struct.h"


bool IsQuantWriteEnd(MetaData& metaData, ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoWriteCount, int64& iCurCount) {
    if (eProcStatus != ProcessStatus::Running) return true;

    if (metaData.iWorkSecs == 0 && metaData.iWriteBlockCount > 0 && iCurCount >= metaData.iWriteBlockCount/metaData.iWriteThreadCount ) return true;

    return false;   
}

bool IsQuantReadEnd(que_proc_buf& workQueue, MetaData& metaData, ProcessStatus& eProcStatus, std::atomic<unsigned long long>& ulAtoReadCount) {
    if (ProcessStatus::Running != eProcStatus && ProcessStatus::WriteEnd != eProcStatus) return true;

    if (metaData.iWriteBlockCount > 0 && ulAtoReadCount >= metaData.iWriteBlockCount && metaData.iWorkSecs == 0 ) return true;

    // if (workQueue.get_used() == 0) return true;

    return false;   
}


/*
 * 读取线程函数 - 消费者线程（模式2）
 * 功能：读取指定位置的数据，用于多消费者分别读取不同区域的场景
 * 参数：arg - 指向存储读取位置的指针
 * 返回值：NULL - 线程结束
 */
void *read_thread_func_quant_pos(ProcessStatus& eProcStatus, int64& readPos, que_proc_buf& workQueue, 
                                std::atomic<unsigned long long>& ulAtoReadCount, 
                                TestOutput& testOutput,int iCpuID, 
                                std::mutex& LogMutex, MetaData metaData)
{
    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Pos Read thread Initing \n", LogMutex);
    BindCpuID(iCpuID,metaData.iNumaNode, "Quant Read Pos ");
    // 等待启动信号
    while(eProcStatus == ProcessStatus::Initing 
        || (metaData.iTestType == (int)(TestType::Read) && eProcStatus != ProcessStatus::WriteEnd)) {
        // std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Pos Read thread Working  \n", LogMutex);
    char tc;                       // 用于验证数据一致性的字符
    int64 count = 0;               // 读取计数器
    int32 len = 0;                 // 单次读取长度
    char *pbuf;                    // 指向队列数据的指针
    readPos = workQueue.get_read_pos();  // 初始化读取位置
    
    testOutput.ulReadStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    DataBlockFixed stFixedBlock;
    DataBlockFixed* pFixedBlock = &stFixedBlock;

    unsigned long long ulPushTime = 0;
    unsigned long long ulAfterPopTime = 0;
    unsigned long long ulBeforePopTime = 0;

    TEST_LOG_DETAIL_THREADS("[Start] POS Read thread start,read_pos="+std::to_string(readPos), LogMutex);
    
    while(!IsQuantReadEnd(workQueue, metaData, eProcStatus, ulAtoReadCount)) {
        // 从指定位置读取数据
        while((len = workQueue.read_get(pbuf,readPos)) > 0){

            if (metaData.iTestType == (int)(TestType::Read)) {
                ulBeforePopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }

            if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                ulPushTime = *((unsigned long long*)pbuf);
                ulAfterPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }
            else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)){
                // stFixedBlock = *((DataBlockFixed*)pbuf);
                memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);
                ulAfterPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();                
                ulPushTime = stFixedBlock.push_time_;               

                // if (1 == metaData.iCheckDetailValue) {                        
                //     if (stFixedBlock.data_[stFixedBlock.array_size_ - 1] != (stFixedBlock.array_size_ - 1) % 128) {
                //         TEST_LOG_ERROR_THREADS (" Read Data Error: stFixedBlock.data_[" 
                //                                 + std::to_string(stFixedBlock.array_size_ - 1) + "] = "
                //                                 + std::to_string(int(stFixedBlock.data_[stFixedBlock.array_size_ - 1])) 
                //                                 + "\n",  LogMutex);
                //     }
                // }                      
            } else {
                DataBlock* pBlock = (DataBlock*)pbuf;
                ulAfterPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                ulPushTime = pBlock->push_time_;             
            }          

             if (metaData.iWorkSecs == 0) {
                testOutput.vecCostTime[ulAtoReadCount] = ulAfterPopTime - ulPushTime;
            } else {
                testOutput.vecCostTime.push_back(ulAfterPopTime - ulPushTime);
            }


            if (metaData.iTestType == (int)TestType::Read) {
                testOutput.vecReadAfterPopTimeList[ulAtoReadCount] = (ulAfterPopTime);  
                testOutput.vecReadBeforePopTimeList[ulAtoReadCount] = (ulBeforePopTime);
            }              

            readPos = workQueue.next_pos(readPos,len);  // 更新读取位置
            if(metaData.iReadThreadCount == 1)
                workQueue.read_cmt_pos(readPos);  // 提交读取位置（单消费者模式）
            
            count++;
            ulAtoReadCount++;
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制读取速率        
    }

    testOutput.ulReadEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    TEST_LOG_DETAIL_THREADS("[END] POS Read thread end,count="+std::to_string(count) 
                + ", ulAtoReadCount="+std::to_string(ulAtoReadCount) + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                + ", workQueue.get_used()="+std::to_string(workQueue.get_used())+ ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime ) , LogMutex);
    
    return NULL;
}

/*
 * 提交读取位置线程函数
 * 功能：协调多个读取线程的进度，提交最小的已读取位置
 */
void *read_thread_func_quant_cmt(ProcessStatus& eProcStatus, que_proc_buf& workQueue, vector<int64>& vecReadPos, 
                                     std::mutex& LogMutex, MetaData metaData)
{
    // 计算线程休眠时间
    int32 tus = metaData.iReadThreadCount/2;
    if(tus == 0)
        tus = 1;
    if(metaData.iReadThreadCount == 1)
        return NULL;  // 单消费者模式下不需要此线程
    
    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Write thread Initing \n", LogMutex);

    // 等待启动信号
    while(eProcStatus == ProcessStatus::Initing ) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    
    TEST_LOG_DETAIL_THREADS("[Start] Quant Queue CMT Read thread Working\n", LogMutex);
    
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
        // workQueue.read_cmt_mth(tpos);  // 使用多线程安全模式提交
        workQueue.read_cmt_pos(tpos);    // 提交最小读取位置
        // comm_utils::sleep_us(tus);  // 可选：控制提交频率
        
    }while(workQueue.get_used() > 0 && eProcStatus == ProcessStatus::Running);  // 标志为1或队列非空时继续运行
    
    unsigned long long  ulEndNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    TEST_LOG_DETAIL_THREADS("[END] CMT Read thread , commit_po: " + std::to_string(tpos) 
                            + ", eProcStatus: " + std::to_string((int)(eProcStatus)) 
                            + ", get_used: " + std::to_string(workQueue.get_used())
                            + ", endTime:" + NanoToMicroString(ulEndNanosecs) , LogMutex);
    
    return NULL;
}

void write_thread_func_quant(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
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
    
        
    int64 count = 0;  // 当前线程, 写入计数器
    char *pbuf;       // 指向队列缓冲区的指针
    int64 tpos;       // 写入位置

    unsigned long long ulBeforePushTimes = 0;
    unsigned long long ulAfterPushTimes = 0;


    testOutput.ulWriteStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    TEST_LOG_DETAIL_THREADS("[START] Quant Queue Write thread Working \n", LogMutex);


    while(!IsQuantWriteEnd(metaData,eProcStatus, ulAtoWriteCount, count)) {

        int iBufferSize = 8;
        char* pSrcBuffer = nullptr;
        DataBlockFixedPtr pFixedBlock = nullptr;
        DataBlockPtr  pVirtualBlock = nullptr;

        if (metaData.iFixedBlock == 1) {
            pFixedBlock = GetDataBlockFixed();
            iBufferSize = pFixedBlock->size_;
        } else if (metaData.iFixedBlock == 2) {
            iBufferSize = 8;
        }else {
            pVirtualBlock = GetRandomDataBlock();
            if(pVirtualBlock == nullptr){
                continue;
            }        
            iBufferSize = pVirtualBlock->size_;
        }

        // 根据写入线程数量选择不同的写入模式
        if(metaData.iWriteThreadCount == 1){

            // 单生产者模式 - 使用普通写入接口
            do{

                if (metaData.iTestType == (int)TestType::Write) {
                    ulBeforePushTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();                    
                }
                // 获取写入位置和缓冲区指针
                tpos = workQueue.write_get(pbuf, iBufferSize);
                if(tpos > 0)  // 成功获取到写入位置 
                {  
                    break;                
                }
                                    
                // // 每30000000次循环打印一次队列已满信息（避免频繁打印影响性能）
                // if((i/30000000) == 0){
                //     // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                //     i = 0;
                // }
                // i++;
            }while(eProcStatus == ProcessStatus::Running);  // 当标志为1时继续尝试

            if(tpos >0){                

                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    unsigned long long ulPushTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    *((unsigned long long*)pbuf) = ulPushTime;
                    // *((unsigned long long*)pbuf) = ulBeforePushTimes;
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {
                    //POD类型的数据，可以直接拷贝;
                    pFixedBlock->push_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    // pFixedBlock->push_time_ = ulBeforePushTimes;

                    memcpy(pbuf,pFixedBlock.get(),iBufferSize); 
                } else {
                    // 此次拷贝的结构体含有虚函数，无法通过memcpy 直接进行拷贝, 因此需要调用CopyDataBlockToBuffer函数, 拷贝的最后一步，会自动更新时间戳
                    pVirtualBlock->push_time_ = ulBeforePushTimes;
                    CopyDataBlockToBuffer(pbuf, pVirtualBlock.get()); 
                }
                
                workQueue.write_cmt(tpos,iBufferSize);  // 提交写入

                if (metaData.iTestType == (int)TestType::Write) {
                    ulAfterPushTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    testOutput.vecWriteAfterPushTimeList[count] = (ulAfterPushTimes);  
                    testOutput.vecWriteBeforePushTimeList[count] = (ulBeforePushTimes);
                }                
                ++count; 
                ++ulAtoWriteCount;
            }

            // printf("Write thread get used=%d\n",workQueue.get_used());
            // printf("Single Thread Write count: %ld,len=%u\n", count, pBlock->size_);
        }
        else{
            // 多生产者模式 - 使用多线程安全写入接口


            do{
                // 获取写入位置和缓冲区指针（多线程安全版本）
                if (metaData.iTestType == (int)TestType::Write) {
                    ulBeforePushTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                }

                tpos = workQueue.write_get_mth(pbuf,iBufferSize);
                if(tpos > 0)  // 成功获取到写入位置
                {
                    break;                
                }
                
                // // 每30000000次循环打印一次队列已满信息
                // if((i/30000000) == 0){
                //     // printf("write queue have fulled,count=%ld,c=%c\n",count,tc);
                //     i = 0;
                // }
                // i++;

            }while(eProcStatus == ProcessStatus::Running);  // 当标志为1时继续尝试
            
            if(tpos >0){
                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    unsigned long long ulPushTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    *((unsigned long long*)pbuf) = ulPushTime;
                    // *((unsigned long long*)pbuf) = ulBeforePushTimes;
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)){ 
                    //POD类型的数据，可以直接拷贝;
                    pFixedBlock->push_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    // pFixedBlock->push_time_ = ulBeforePushTimes;
                    memcpy(pbuf,pFixedBlock.get(),iBufferSize); 
                } else {
                    // 此次拷贝的结构体含有虚函数，无法通过memcpy 直接进行拷贝, 因此需要调用CopyDataBlockToBuffer函数, 拷贝的最后一步，会自动更新时间戳
                    // pVirtualBlock->push_time_ = ulBeforePushTimes;
                    CopyDataBlockToBuffer(pbuf, pVirtualBlock.get()); 
                }
                workQueue.write_cmt_mth(tpos,iBufferSize);  // 提交写入（多线程安全版本）

                if (metaData.iTestType == (int)TestType::Write) {
                    ulAfterPushTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    testOutput.vecWriteAfterPushTimeList[count] = (ulAfterPushTimes);  
                    testOutput.vecWriteBeforePushTimeList[count] = (ulBeforePushTimes);
                } 

                ++count;
                ++ulAtoWriteCount;
            }
        }
        
        if (metaData.iSleepTimeUs > 0) {
            comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制写入速率
        }        
    } 
    
    
    if (metaData.iTestType == (int)(TestType::Read)) {
        eProcStatus = ProcessStatus::WriteEnd;
    } 

    testOutput.ulWriteEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    TEST_LOG_DETAIL_THREADS( "[END] Quant Write Thread ulAtoWriteCount=" + std::to_string(ulAtoWriteCount) 
                + ", count=" + std::to_string(count) + ", eProcStatus: " + std::to_string((int)(eProcStatus)) 
                + ", workQueue.get_used(): " + std::to_string(workQueue.get_used()) 
                + ", endTime:" + NanoToMicroString(testOutput.ulWriteEndTime ) + "\n", LogMutex);
}

void read_thread_func_quant_simple(ProcessStatus& eProcStatus, que_proc_buf& workQueue,
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
    int64 count = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    DataBlockFixed stFixedBlock;
    DataBlockFixed* pFixedBlock = &stFixedBlock;
    unsigned long long ulBeforePopTimes = 0;
    unsigned long long ulAfterPopTimes = 0;
    unsigned long long ulPushTime = 0;
    unsigned long long ulPopTime = 0;    

    testOutput.ulReadStartTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    TEST_LOG_DETAIL_THREADS("Quant Queue Simple Read Thread Working "
                            + NanoToMicroString(testOutput.ulReadStartTime) +"\n", LogMutex);

    while(!IsQuantReadEnd(workQueue, metaData, eProcStatus, ulAtoReadCount)) {

        if(metaData.iReadThreadCount == 1){
            // 单消费者模式 - 直接读取并提交

            if (metaData.iTestType == (int)TestType::Read) {
                ulBeforePopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }

            while((len = workQueue.read_get(pbuf)) > 0){
                ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    ulPushTime = *((unsigned long long*)pbuf);
                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {         
                    //  ulPushTime = ((DataBlockFixed*)pbuf)->push_time_;   
                    // stFixedBlock = *((DataBlockFixed*)pbuf);
                    memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);
                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    ulPushTime = stFixedBlock.push_time_;     
                    
                    // if (1 == metaData.iCheckDetailValue) {                        
                    //     if (stFixedBlock.data_[stFixedBlock.array_size_ - 1] != (stFixedBlock.array_size_ - 1) % 128) {
                    //         TEST_LOG_ERROR_THREADS (" Read Data Error: stFixedBlock.data_[" 
                    //                                 + std::to_string(stFixedBlock.array_size_ - 1) + "] = "
                    //                                 + std::to_string(int(stFixedBlock.data_[stFixedBlock.array_size_ - 1])) 
                    //                                 + "\n",  LogMutex);
                    //     }
                    // }                    
                } else {
                    DataBlock* pBlock = (DataBlock*)pbuf;
                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    ulPushTime = pBlock->push_time_;
                } 

                if (metaData.iWorkSecs == 0) {
                    testOutput.vecCostTime[ulAtoReadCount] = (ulPopTime - ulPushTime);
                } else {
                    testOutput.vecCostTime.push_back(ulPopTime - ulPushTime);
                }

            
                workQueue.read_cmt();  // 提交读取（单消费者模式）

                if (metaData.iTestType == (int)TestType::Read) {
                    ulAfterPopTimes = ulPopTime;
                    testOutput.vecReadAfterPopTimeList[ulAtoReadCount] = (ulAfterPopTimes);
                    testOutput.vecReadBeforePopTimeList[ulAtoReadCount] = (ulBeforePopTimes);
                }   

                count++;
                ulAtoReadCount++;

                if (IsQuantReadEnd(workQueue, metaData, eProcStatus, ulAtoReadCount)) break;
            }
        }
        else{
            // 多消费者模式 - 使用弹出接口
            if (metaData.iTestType == (int)TestType::Read) {
                ulBeforePopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();                
            }

            while((len = workQueue.read_pop(tcache,sizeof(tcache))) > 0){
                ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    ulPushTime = *((unsigned long long*)pbuf);
                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                } else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {

                    // stFixedBlock = *((DataBlockFixed*)pbuf);
                    memcpy(&stFixedBlock, pbuf, ((DataBlockFixed*)pbuf)->size_);

                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    ulPushTime = stFixedBlock.push_time_;

                    if (1 == metaData.iCheckDetailValue) {                        
                        if (pFixedBlock->data_[pFixedBlock->array_size_ - 1] != (pFixedBlock->array_size_ - 1) % 128) {
                            TEST_LOG_ERROR_THREADS (" Read Data Error: pFixedBlock->data_[" + std::to_string(pFixedBlock->array_size_ - 1) + "] = "
                                                    + std::to_string(int(pFixedBlock->data_[pFixedBlock->array_size_ - 1])) + "\n",  LogMutex);
                        }
                    }                 
                } else {
                    DataBlock* pBlock = (DataBlock*)pbuf;
                    ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    ulPushTime = pBlock->push_time_;
                }   

                if (metaData.iTestType == (int)TestType::Read) {
                    ulAfterPopTimes = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    testOutput.vecReadAfterPopTimeList.push_back(ulAfterPopTimes);
                    testOutput.vecReadBeforePopTimeList.push_back(ulBeforePopTimes);
                }

                if (metaData.iWorkSecs == 0) {
                    testOutput.vecCostTime[ulAtoReadCount] = (ulPopTime - ulPushTime);
                } else {
                    testOutput.vecCostTime.push_back(ulPopTime - ulPushTime);
                }

                count++;
                
                ulAtoReadCount++;

                if (IsQuantReadEnd(workQueue, metaData, eProcStatus, ulAtoReadCount)) break;
            }
        }
        
        // comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制读取速率
        
    }
    testOutput.iReadCount_ = count;
    testOutput.ulReadEndTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    TEST_LOG_DETAIL_THREADS("[END] Simple Read Thread ulAtoReadCount: " + std::to_string(ulAtoReadCount) 
                + ",count: " + std::to_string(count) + ", eProcStatus: " + std::to_string((int)(eProcStatus))
                + ", workQueue.get_used(): " + std::to_string(workQueue.get_used()) 
                + ", dataCount: " +  std::to_string(testOutput.vecCostTime.size())
                + ", endTime:" + NanoToMicroString(testOutput.ulReadEndTime) + "\n", LogMutex);
}

