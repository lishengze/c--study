#include "queue_manager.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"
#include "bits.h"

namespace share_common 
{

bool QueueManager::Init(ReadGetRspCallbackMessageFuncType StraegyOnMessageFunc,
                        WorkerType workerType, const char* cstrSharedMemName,  
                        bool bIsCreateSharedMemory, bool bIsReqOrderQueue) {

    LOG_INFO("Init QueueManager, workerType = {}, bIsCreateSharedMemory = {}, cstrSharedMemName = {}, bIsReqOrderQueue = {}", 
                (int)(workerType), bIsCreateSharedMemory, cstrSharedMemName, bIsReqOrderQueue);

    bIsReqOrderQueue_ = bIsReqOrderQueue;
    workerType_ = workerType;
    bIsCreateSharedMemory_ = bIsCreateSharedMemory;
    strSharedMemName_ = cstrSharedMemName;

    StraegyOnMessageFunc_ = StraegyOnMessageFunc;

    if (!StraegyOnMessageFunc_) {
        LOG_ERROR("StraegyOnMessageFunc_ is null");
        return false;
    }

    if (!bIsCreateSharedMemory) {
        if (!AttachShareMemory(cstrSharedMemName)) return false;
    } else {
        if (!CreateShareMemory(cstrSharedMemName)) return false;
    }

    if (workerType_ == Consumer) {
        StartConsumerThread();
    }

    return true;
}

// 当前场景下只监听，从UTE到策略端的回报;
void QueueManager::StartConsumerThread() {
    LOG_DEBUG("StartConsumerThread: {}, bIsInShareMemory_: {}", strSharedMemName_, bIsInShareMemory_);
    if (!pWriteMsgMpmcQueue_) {
        LOG_ERROR("pWriteMsgMpmcQueue_ is nullptr");
        return;
    }

    if (bIsInShareMemory_ && !pWriteMsgMpmcShareSlots_) {
        LOG_ERROR("pWriteMsgMpmcShareSlots_ is nullptr");
        return;  
    }

    shptrConsumerThread_ = std::make_shared<std::thread>([this]() {
        while (bIsRunning_) {
            WriteMsg msg;

            if (bIsInShareMemory_) {
                pWriteMsgMpmcQueue_->pop_share(pWriteMsgMpmcShareSlots_, msg);
                LOG_DEBUG("Msg From Server, iMsgID:{}, iMsgLen:{}", msg.iMsgID,  msg.iMsgLen);
                StraegyOnMessageFunc_(msg.iMsgID, msg.strMsgBuf, msg.iMsgLen); 
            } else {
                pWriteMsgMpmcQueue_->pop(msg); 
                StraegyOnMessageFunc_(msg.iMsgID, msg.strMsgBuf, msg.iMsgLen); 
            }

            // sleep(1); // todo 测试专用;
        }
        LOG_INFO("Waiting  Queue Data Is Over");
        
    });

    if (!shptrConsumerThread_) {
        LOG_ERROR("create consumer thread failed");
        return;
    }
 
}

bool QueueManager::Init(WorkerType workerType, const char* cstrSharedMemName,  
                        bool bIsCreateSharedMemory, unsigned long long ulFileKey,
                        bool bIsReqOrderQueue ) {

    LOG_INFO("Init QueueManager, workerType = {}, bIsCreateSharedMemory = {}, cstrSharedMemName = {}, bIsReqOrderQueue={}",
     (int)(workerType), bIsCreateSharedMemory, cstrSharedMemName, bIsReqOrderQueue);

    bIsReqOrderQueue_ = bIsReqOrderQueue;
    workerType_ = workerType;
    bIsCreateSharedMemory_ = bIsCreateSharedMemory;
    strSharedMemName_ = cstrSharedMemName;
    ulFileKey_ = ulFileKey;

    if (!bIsCreateSharedMemory) {
        // 共享内存名称不为空时，尝试打开已有的共享内存
        if (strcmp(cstrSharedMemName, "") != 0) {
            return AttachShareMemory(cstrSharedMemName); // 对于 服务端进程没有这种场景;
        } else {
            return InitQueueWithoutSharedMemory(); // 创建内存中的无所队列 -- 对于UTE进程，创建转发 API请求回报的无锁队列；
        }
    } else {
        return CreateShareMemory(cstrSharedMemName); // 创建新的共享内存 -- 对于UTE进程，创建接受策略进程发送的请求的无锁队列 和 用于回报的共享内存以及相关队列;
    }

    return true;
}


bool QueueManager::InitQueueWithoutSharedMemory() {
    LOG_INFO("InitQueueWithoutSharedMemory Start!");

    bIsAttachSharedMemory_ = false;
    bIsCreateSharedMemory_ = false;

    // if (bIsReqOrderQueue_) {

    //     pReqOrderMpmcQueue_ = new mpmc_queue<TradeOrderReq>();

    //     if (!pReqOrderMpmcQueue_) {
    //         LOG_ERROR("create queue failed");
    //         return false;
    //     }
    //     if (!pReqOrderMpmcQueue_->create(uiQueueBlockCount_)) {
    //         LOG_ERROR("queue create  failed");
    //         return false;
    //     }
    // } else 
    
    {
        pWriteMsgMpmcQueue_ = new mpmc_queue<WriteMsg>();

        if (!pWriteMsgMpmcQueue_) {
            LOG_ERROR("create queue failed");
            return false;
        }
        if (!pWriteMsgMpmcQueue_->create(uiQueueBlockCount_)) {
            LOG_ERROR("queue create  failed");
            return false;
        }
    }


    return true;
}

bool QueueManager::AttachShareMemory(const char* cstrSharedMemName) {
    // LOG_INFO("AttachShareMemory: {}", cstrSharedMemName);

    bIsAttachSharedMemory_ = true;
    bIsInShareMemory_ = true;
    // 打开已有的共享内存对象
    int shm_fd = shm_open(cstrSharedMemName, O_RDWR, 0);
    if (shm_fd == -1) {
        LOG_WARN("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    
    // 获取共享内存大小
    struct stat stat_buf;
    if (fstat(shm_fd, &stat_buf) == -1) {
        LOG_WARN("fstat {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    uiMemorySize_ = stat_buf.st_size; // 记录共享内存大小
    void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_POPULATE, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_WARN("mmap {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }

    close(shm_fd);

    // if (bIsReqOrderQueue_) {
    //     pReqOrderMpmcQueue_ = static_cast<mpmc_queue<TradeOrderReq>*>(addr);   

    //     if (!pReqOrderMpmcQueue_) {
    //         LOG_WARN("pReqOrderMpmcQueue_ is null");
    //         return false;
    //     }
        
    //     // 手动将slot 映射到外部的内存地址中 -- 共享内存版本,这一步导致了很多的问题，导致无法进行服务端对slot 的解锁出错了。
    //     pReqOrderMpmcQueue_->slot_attach(pReqOrderMpmcShareSlots_, static_cast<void*>((char*)addr + sizeof(mpmc_queue<TradeOrderReq>) + 32));
    // } else 
    
    {
        pWriteMsgMpmcQueue_ = static_cast<mpmc_queue<WriteMsg>*>(addr);   

        if (!pWriteMsgMpmcQueue_) {
            LOG_WARN("pWriteMsgMpmcQueue_ is null");
            return false;
        }
        
        // 手动将slot 映射到外部的内存地址中 -- 共享内存版本,这一步导致了很多的问题，导致无法进行服务端对slot 的解锁出错了。
        pWriteMsgMpmcQueue_->slot_attach(pWriteMsgMpmcShareSlots_, static_cast<void*>((char*)addr + sizeof(mpmc_queue<WriteMsg>) + 32));



    }

    return true;
}

bool QueueManager::CreateShareMemory(const char* cstrSharedMemName) {
    LOG_INFO("CreateShareMemory: {}", cstrSharedMemName);
    bIsCreateSharedMemory_ = true;
    bIsInShareMemory_ = true;

    int shm_fd = shm_open(cstrSharedMemName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    


    if (bIsReqOrderQueue_) {

        // unsigned int uiElementSlotBlocksSize = (roundup_pow_of_two(uiQueueBlockCount_) + 2) * sizeof( element_slot<TradeOrderReq, false>) ;

        // // 设置共享内存大小
        // uiMemorySize_ = sizeof(mpmc_queue<TradeOrderReq>) + uiElementSlotBlocksSize + 128; // 给共享内存留足够的空间;
        // if (ftruncate(shm_fd, uiMemorySize_) == -1) {
        //     LOG_ERROR("ftruncate {} failed ", cstrSharedMemName);
        //     close(shm_fd);
        //     return false;
        // }
        
        // // 映射共享内存
        // void* addr = mmap(NULL, uiMemorySize_, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_POPULATE, shm_fd, 0);
        // if (addr == MAP_FAILED) {
        //     LOG_ERROR("mmap {} failed ", cstrSharedMemName);
        //     close(shm_fd);
        //     return false;
        // }


        // // 在共享内存中构造队列对象
        // pReqOrderMpmcQueue_ = new (addr) mpmc_queue<TradeOrderReq>();
        // if (!pReqOrderMpmcQueue_) {
        //     LOG_ERROR("new pReqOrderMpmcQueue_ Failed!");
        //     return false;
        // }
        
        // // 使用自定义内存分配器初始化队列
        // // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；
        // if (!pReqOrderMpmcQueue_->create_shared(uiQueueBlockCount_, pReqOrderMpmcShareSlots_, 
        //                                     static_cast<void*>((char*)addr + sizeof(mpmc_queue<TradeOrderReq>) + 32))) { 

        //     LOG_ERROR("queue create_shared  failed");
        //     return false;
        // }


    } else {

        unsigned int uiElementSlotBlocksSize = (roundup_pow_of_two(uiQueueBlockCount_) + 2) * sizeof( element_slot<WriteMsg, false>) ;

        // 设置共享内存大小
        uiMemorySize_ = sizeof(mpmc_queue<WriteMsg>) + uiElementSlotBlocksSize + 128; // 给共享内存留足够的空间;
        if (ftruncate(shm_fd, uiMemorySize_) == -1) {
            LOG_ERROR("ftruncate {} failed ", cstrSharedMemName);
            close(shm_fd);
            return false;
        }
        
        // 映射共享内存
        void* addr = mmap(NULL, uiMemorySize_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (addr == MAP_FAILED) {
            LOG_ERROR("mmap {} failed ", cstrSharedMemName);
            close(shm_fd);
            return false;
        }


        // 在共享内存中构造队列对象
        pWriteMsgMpmcQueue_ = new (addr) mpmc_queue<WriteMsg>();
        if (!pWriteMsgMpmcQueue_) {
            LOG_ERROR("new pWriteMsgMpmcQueue_ Failed!");
            return false;
        }

        
        // 使用自定义内存分配器初始化队列

        if (!pWriteMsgMpmcQueue_->create_shared(uiQueueBlockCount_, pWriteMsgMpmcShareSlots_, 
                                            static_cast<void*>((char*)addr + sizeof(mpmc_queue<WriteMsg>) + 32))) { // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；
            LOG_ERROR("queue create_shared  failed");
            return false;
        }
    } 



    close(shm_fd);

    return true;
}



void QueueManager::Release() {

    if (bIsReleased_) {
        LOG_WARN("Queue Has Been Released!");
        return;
    }
    bIsReleased_ = true;

    LOG_INFO("Releasing Queue!");

    // 等待消费队列资源释放;
    if (shptrConsumerThread_ && shptrConsumerThread_->joinable()) {
        shptrConsumerThread_->join();
    }       


    bIsRunning_ = false;  // 直接结束事件循环队列;


    // if (bIsReqOrderQueue_) {
    //     if (!pReqOrderMpmcQueue_) {
    //         LOG_ERROR("pReqOrderMpmcQueue_ is null");
    //         return;
    //     }

    //     /// 若是映射了共享内存，则需要解除内存映射
    //     if (bIsAttachSharedMemory_ && munmap(pReqOrderMpmcQueue_, uiMemorySize_) == -1) {

    //         LOG_ERROR("munmap {} failed ", strSharedMemName_);
    //         return;
    //     }

    //     /// 若是创建了共享内存，则需要删除共享内存对象
    //     if (bIsCreateSharedMemory_) {
    //         shm_unlink(strSharedMemName_.c_str());
    //     }

    //     /// 若是未映射共享内存，则需要手动释放内存
    //     if (!bIsAttachSharedMemory_) {
    //         if (pReqOrderMpmcQueue_) delete pReqOrderMpmcQueue_;
    //     }        

    //     if (pReqOrderMpmcQueue_) pReqOrderMpmcQueue_ = nullptr;
    // } else 
    
    {
        if (!pWriteMsgMpmcQueue_) {
            LOG_ERROR("pWriteMsgMpmcQueue_ is null");
            return;
        }

        /// 若是映射了共享内存，则需要解除内存映射
        if (bIsAttachSharedMemory_ && munmap(pWriteMsgMpmcQueue_, uiMemorySize_) == -1) {

            LOG_ERROR("munmap {} failed ", strSharedMemName_);
            return;
        }

        /// 若是创建了共享内存，则需要删除共享内存对象
        if (bIsCreateSharedMemory_) {
            shm_unlink(strSharedMemName_.c_str());
        }

        /// 若是未映射共享内存，则需要手动释放内存
        if (!bIsAttachSharedMemory_) {
            if (pWriteMsgMpmcQueue_) delete pWriteMsgMpmcQueue_;
        }        

        if (pWriteMsgMpmcQueue_) pWriteMsgMpmcQueue_ = nullptr;
    }


}

/// @brief 通过共享内存进行进程间通讯使用
/// @param iMsgID 
/// @param pMsgBuf 
/// @param iMsgLen 
/// @param ulReadKey 
void QueueManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulReadKey) {
    LOG_DEBUG("iMsgID:{}, iMsgLen:{},ulReadKey:{},bIsInShareMemory_:{}", iMsgID,iMsgLen,ulReadKey, bIsInShareMemory_);
    if (bIsInShareMemory_) {
        pWriteMsgMpmcQueue_->push_share(pWriteMsgMpmcShareSlots_, iMsgID, iMsgLen, ulReadKey, pMsgBuf); // 在 enqueue 时会调用 UteWrite 的构造函数
    } else {
        pWriteMsgMpmcQueue_->push(iMsgID, iMsgLen, ulReadKey, pMsgBuf); 
    }
    
}

/// @brief 通过共享内存进行进程间通讯使用
/// @param iMsgID 
/// @param pMsgBuf 
/// @param iMsgLen 
/// @param ulReadKey 
void QueueManager::SendMsgShare(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulReadKey) {
    pWriteMsgMpmcQueue_->push_share(pWriteMsgMpmcShareSlots_, iMsgID, iMsgLen, ulReadKey, pMsgBuf); // 在 enqueue 时会调用 UteWrite 的构造函数
}


// 这个是用户内部通讯的队列请求;
void QueueManager::SendMsg(int iMsgID, const char* pMsgBuf, unsigned int  iMsgLen, int iMsgSrcType, void* pMsgHandler) {
    if (SHARE_COMM_LIKELY(pWriteMsgMpmcQueue_)) {
        LOG_DEBUG("iMsgID = {}, iMsgLen = {}, iMsgSrcType = {}", iMsgID, iMsgLen, iMsgSrcType);
        pWriteMsgMpmcQueue_->push(iMsgID, iMsgLen, iMsgSrcType, pMsgHandler, pMsgBuf);         
    } else {
        LOG_ERROR("pWriteMsgMpmcQueue_ is null");
    }
}

// 这个是用户内部通讯的队列请求;
void QueueManager::SendReqOrder(const char* pBuffer) {
    // if (SHARE_COMM_LIKELY(pWriteMsgMpmcQueue_)) {
    //     LOG_DEBUG("iMsgID = {}, iMsgLen = {}, iMsgSrcType = {}", iMsgID, iMsgLen, iMsgSrcType);
    //     pReqOrderMpmcQueue_->push(iMsgID, iMsgLen, iMsgSrcType, pMsgHandler, pMsgBuf);         
    // } else {
    //     LOG_ERROR("pWriteMsgMpmcQueue_ is null");
    // }

    // pReqOrderMpmcQueue_->push_share(pReqOrderMpmcShareSlots_, pBuffer); // 在 enqueue 时会调用 UteWrite 的构造函数
}



bool QueueManager::TryPop(WriteMsg& msg) {
    if (bIsInShareMemory_) {
        return pWriteMsgMpmcQueue_->TryPopShare(pWriteMsgMpmcShareSlots_, msg);
    } else {
        return pWriteMsgMpmcQueue_->trypop(msg); 
    }    
}

bool QueueManager::TryPopShare(WriteMsg& msg) {
    return pWriteMsgMpmcQueue_->TryPopShare(pWriteMsgMpmcShareSlots_, msg);  
}

// bool QueueManager::TryPopReqOrder(TradeOrderReq& msg) {
//     // return pReqOrderMpmcQueue_->TryPopShare(pReqOrderMpmcShareSlots_, msg);  
// }




} // namespace share_common