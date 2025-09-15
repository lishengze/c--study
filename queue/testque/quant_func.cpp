#include "quant_func.h"
#include "struct.h"

/*
 * 读取线程函数 - 消费者线程（模式2）
 * 功能：读取指定位置的数据，用于多消费者分别读取不同区域的场景
 * 参数：arg - 指向存储读取位置的指针
 * 返回值：NULL - 线程结束
 */
void *read_thread_func_quant_pos(int& iStopFlag, int64& readPos, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPopBlocks, 
                                std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData)
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
    
     TEST_LOG_DETAIL("[Start] POS Read thread start,read_pos="+std::to_string(readPos));
    
    do{
        if (ulAtoReadCount >= metaData.iWriteBlockCount) break; // 防止其他线程已经读取完所有数据
        // 从指定位置读取数据
        while((len = queProBuf.read_get(pbuf,readPos)) > 0){
            unsigned long long ulPushTime = 0;
            unsigned long long ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    ulPushTime = *((unsigned long long*)pbuf);
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)){
                DataBlockFixed* pFixedBlock = (DataBlockFixed*)pbuf;
                // printf("read push_time=%lld\n",pFixedBlock->push_time_);
                ulPushTime = pFixedBlock->push_time_;                    
            } else {
                DataBlock* pBlock = (DataBlock*)pbuf;
                ulPushTime = pBlock->push_time_;
            }                
            // vecPopBlocks.push_back(GetCopyBlock(pBlock));
            vecCostTime.push_back(ulPopTime - ulPushTime);

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
            if (++ulAtoReadCount >= metaData.iWriteBlockCount) break;
        }
        
        // comm_utils::sleep_us(tus);  // 可选：控制读取速率
        
    } while(queProBuf.get_used()>0 || ulAtoReadCount < metaData.iWriteBlockCount);  // 标志为1或队列非空时继续运行
    
    TEST_LOG_DETAIL("[END] POS Read thread end,count="+std::to_string(count) 
                + ", ulAtoReadCount="+std::to_string(ulAtoReadCount) 
                + ", queProBuf.get_used()="+std::to_string(queProBuf.get_used()));
    
    return NULL;
}

/*
 * 提交读取位置线程函数
 * 功能：协调多个读取线程的进度，提交最小的已读取位置
 */
void *read_thread_func_quant_cmt(int& iStopFlag, que_proc_buf& queProBuf, vector<int64>& vecReadPos, std::vector<DataBlockPtr>& vecPopBlocks, MetaData metaData)
{
    // 计算线程休眠时间
    int32 tus = metaData.iReadThreadCount/2;
    if(tus == 0)
        tus = 1;
    if(metaData.iReadThreadCount == 1)
        return NULL;  // 单消费者模式下不需要此线程
    
    // 等待启动信号
    while(iStopFlag == 0);
    
    TEST_LOG_DETAIL("[Start] CMT Read thread start\n");
    
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
        
    }while(queProBuf.get_used()>0);  // 标志为1或队列非空时继续运行
    
    TEST_LOG_DETAIL("[END] CMT Read thread , commit_pos="+std::to_string(tpos));
    
    return NULL;
}

bool IsWriteEnd(MetaData& metaData, bool isStopFlag, int writeIndex, unsigned long long ulStartNanoTime) {
    if (isStopFlag != 1) return true;

    if (metaData.iWriteSecs > 0) {  // 写入时间限制模式
        unsigned long long ulCurTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        if (ulCurTime - ulStartNanoTime >= metaData.iWriteSecs * 1000000000) {
            return true;
        }
    }

    // if (metaData.iWriteBlockCount > 0) { //写入数量限制模式
    //     if (writeIndex >= metaData.iWriteBlockCount) {
    //         return true;
    //     }
    // }

    return false;   
}

void write_thread_func_quant(int& iStopFlag, que_proc_buf& queProBuf, std::vector<DataBlockPtr>& vecPushBlocks, std::atomic<unsigned long long>& ulAtoWriteCount, MetaData metaData)
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
        
    int64 count = 0;  // 当前线程, 写入计数器
    int32 i= 0;       // 循环计数器
    char *pbuf;       // 指向队列缓冲区的指针
    int64 tpos;       // 写入位置
    unsigned long long ulStartNanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();


    do{
        if (ulAtoWriteCount >= metaData.iWriteBlockCount) break;

        i = 0;
        int iBufferSize = 0;
        char* pSrcBuffer = nullptr;
        DataBlockFixedPtr pFixedBlock = nullptr;
        DataBlockPtr  pVirtualBlock = nullptr;
        if (metaData.iFixedBlock == 1) {
             pFixedBlock = GetDataBlockFixed();
            iBufferSize = pFixedBlock->size_;
        } else {
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
                
                // 获取写入位置和缓冲区指针
                tpos = queProBuf.write_get(pbuf, iBufferSize);
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
                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    unsigned long long ulPushTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    *((unsigned long long*)pbuf) = ulPushTime;
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {
                    //POD类型的数据，可以直接拷贝;
                    pFixedBlock->push_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    memcpy(pbuf,pFixedBlock.get(),iBufferSize); 
                    // printf("write push_time=%lld\n",pFixedBlock->push_time_);
                } else {
                    // 此次拷贝的结构体含有虚函数，无法通过memcpy 直接进行拷贝, 因此需要调用CopyDataBlockToBuffer函数, 拷贝的最后一步，会自动更新时间戳
                    CopyDataBlockToBuffer(pbuf, pVirtualBlock.get()); 
                }
                
                queProBuf.write_cmt(tpos,iBufferSize);  // 提交写入
                ++count;
                if (++ulAtoWriteCount >= metaData.iWriteBlockCount) break;
            }

            // printf("Write thread get used=%d\n",queProBuf.get_used());
            // printf("Single Thread Write count: %ld,len=%u\n", count, pBlock->size_);
        }
        else{
            // 多生产者模式 - 使用多线程安全写入接口
            do{
                // 获取写入位置和缓冲区指针（多线程安全版本）
                tpos = queProBuf.write_get_mth(pbuf,iBufferSize);
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
                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    unsigned long long ulPushTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    *((unsigned long long*)pbuf) = ulPushTime;
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)){ 
                    //POD类型的数据，可以直接拷贝;
                    pFixedBlock->push_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    memcpy(pbuf,pFixedBlock.get(),iBufferSize); 
                    // printf("write push_time=%lld\n",pFixedBlock->push_time_);
                } else {
                    // 此次拷贝的结构体含有虚函数，无法通过memcpy 直接进行拷贝, 因此需要调用CopyDataBlockToBuffer函数, 拷贝的最后一步，会自动更新时间戳
                    CopyDataBlockToBuffer(pbuf, pVirtualBlock.get()); 
                }
                queProBuf.write_cmt_mth(tpos,iBufferSize);  // 提交写入（多线程安全版本）
                ++count;
                if (++ulAtoWriteCount >= metaData.iWriteBlockCount) break;
            }
        }
        
        if (metaData.iSleepTimeUs > 0) {
            comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制写入速率
        }        
    } while(!IsWriteEnd(metaData, iStopFlag, count, ulStartNanoTime));  // 当标志为1时继续运行，为2时退出
    
    unsigned long long ulEndNanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    TEST_LOG_DETAIL( "[END] Write Thread ulAtoWriteCount=" + std::to_string(ulAtoWriteCount) + ",count=" + std::to_string(count) + ",time=" + std::to_string((ulEndNanoTime - ulStartNanoTime)/1000)  + " micros");

    // iStopFlag = 2;  // 结束信号,写线程结束，读线程也就结束了；
}

void read_thread_func_quant_simple(int& iStopFlag, que_proc_buf& queProBuf,  std::vector<DataBlockPtr>& vecPopBlocks, 
                                    std::vector<unsigned long long>& vecCostTime, std::atomic<unsigned long long>& ulAtoReadCount, MetaData metaData)
{
    // std::cout << "[START] read_thread_func_quant_simple ,metaData=" << metaData.str() << std::endl;

    // 计算线程休眠时间，根据读取线程数量动态调整
    // int32 tus = metaData.iReadThreadCount/2;
    // if(tus == 0)
    //     tus = 1;
    
    // 等待启动信号
    while(iStopFlag == 0);
    
    TEST_LOG_DETAIL("[START] Simple Read Thread start\n");
    
    char tcache[4096];  // 读取数据缓冲区
    char tc;             // 用于验证数据一致性的字符
    int64 count = 0;     // 读取计数器
    int32 len = 0;       // 单次读取长度
    char *pbuf;          // 指向队列数据的指针
    
    do{
        if (ulAtoReadCount >= metaData.iWriteBlockCount) break;

        if(metaData.iReadThreadCount == 1){
            // 单消费者模式 - 直接读取并提交
            while((len = queProBuf.read_get(pbuf)) > 0){
                unsigned long long ulPushTime = 0;
                unsigned long long ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

                if (metaData.iFixedBlock == (int)(BlockType::FixedPOD)) {
                    ulPushTime = *((unsigned long long*)pbuf);
                }
                else if (metaData.iFixedBlock == (int)(BlockType::FixedStruct)) {
                    DataBlockFixed* pFixedBlock = (DataBlockFixed*)pbuf;
                    // printf("read push_time=%lld\n",pFixedBlock->push_time_);
                    ulPushTime = pFixedBlock->push_time_;                    
                } else {
                    DataBlock* pBlock = (DataBlock*)pbuf;
                    ulPushTime = pBlock->push_time_;
                }                
                // vecPopBlocks.push_back(GetCopyBlock(pBlock));
                vecCostTime.push_back(ulPopTime - ulPushTime);

                // assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = pbuf[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == pbuf[j]);
                }
                */
                queProBuf.read_cmt();  // 提交读取（单消费者模式）
                count++;
                if (++ulAtoReadCount >= metaData.iWriteBlockCount) break;
                // std::cout << "Single Thread Read count: " << count << " len: " << len << std::endl;
            }

            // printf("Read thread get used=%d, count:%d, vecCostTime.size: %d\n", queProBuf.get_used(), count, vecCostTime.size());

            // std::cout << "[Read Failed] read_thread_func_quant_simple,count=" << count << std::endl;
        }
        else{
            // 多消费者模式 - 使用弹出接口
            while((len = queProBuf.read_pop(tcache,sizeof(tcache))) > 0){
                unsigned long long ulPushTime = 0;
                unsigned long long ulPopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

                if (metaData.iFixedBlock == 1) {
                    DataBlock* pBlock = (DataBlock*)pbuf;
                    ulPushTime = pBlock->push_time_;
                } else {
                    DataBlockFixed* pFixedBlock = (DataBlockFixed*)pbuf;
                    // printf("read push_time=%lld\n",pFixedBlock->push_time_);
                    ulPushTime = pFixedBlock->push_time_;
                }                
                // vecPopBlocks.push_back(GetCopyBlock(pBlock));
                vecCostTime.push_back(ulPopTime - ulPushTime);

                // assert(len == g_wr_que_len);  // 验证读取长度是否符合预期
                /* 数据验证代码（注释掉以提高性能）
                tc = tcache[0];
                for(int32 j=1;j<len-2;j++){
                    assert(tc == tcache[j]);
                }
                */
                count++;
                if (++ulAtoReadCount >= metaData.iWriteBlockCount) break;
            }
        }
        
        // comm_utils::sleep_us(metaData.iSleepTimeUs);  // 可选：控制读取速率
        
    }while(queProBuf.get_used()>0 || iStopFlag == 1);  // 标志为1或队列非空时继续运行
    
    TEST_LOG_DETAIL("[END] Simple Read Thread ulAtoReadCount: " + std::to_string(ulAtoReadCount) 
                + ",count: " + std::to_string(count) 
                + ", vecCostTime.size: " + std::to_string(vecCostTime.size()));
}

