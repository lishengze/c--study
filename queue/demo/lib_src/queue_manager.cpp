#include "queue_manager.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "mpmc_queue.h"
#include "external_message.h"
#include "bits.h"

#include "strategy_message_manager.h"

namespace share_common 
{

// // 在共享内存中创建队列
// bool create_shared_queue(const char* name, uint32_t size)
// {
//     // 打开或创建共享内存对象
//     int shm_fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
//     if (shm_fd == -1) {
//         perror("shm_open failed");
//         return false;
//     }
    
//     // 设置共享内存大小
//     size_t shm_size = sizeof(mpmc_queue<T>) + ...; // 计算完整大小
//     if (ftruncate(shm_fd, shm_size) == -1) {
//         perror("ftruncate failed");
//         close(shm_fd);
//         return false;
//     }
    
//     // 映射共享内存
//     void* addr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
//     if (addr == MAP_FAILED) {
//         perror("mmap failed");
//         close(shm_fd);
//         return false;
//     }
    
//     // 在共享内存中构造队列对象
//     mpmc_queue<T>* queue = new (addr) mpmc_queue<T>();
    
//     // 使用自定义内存分配器初始化队列
//     queue->create_shared(size, addr + sizeof(mpmc_queue<T>));
    
//     close(shm_fd);
//     return true;
// }

// // 在mpmc_queue类中添加
// bool create_shared(uint32_t size, void* buffer)
// {
//     size = size ? size : 1;
//     size = roundup_pow_of_two(size + 1);
    
//     // 直接使用传入的共享内存地址作为slots_
//     slots_ = static_cast<slot_type*>(buffer);
    
//     // 初始化所有元素槽（如果需要）
//     for (uint32_t i = 0; i < size; ++i) {
//         new (&slots_[i]) slot_type();
//     }
    
//     mask_ = size - 1;
//     bit_mask_ = __builtin_ctz((uint64_t)size);
//     stride_ = 1;
//     push_ticket_ = 0;
//     pop_ticket_ = 0;
    
//     return true;
// }

// mpmc_queue<T>* open_shared_queue(const char* name)
// {
//     // 打开已有的共享内存对象
//     int shm_fd = shm_open(name, O_RDWR, 0);
//     if (shm_fd == -1) {
//         perror("shm_open failed");
//         return nullptr;
//     }
    
//     // 获取共享内存大小
//     struct stat stat_buf;
//     if (fstat(shm_fd, &stat_buf) == -1) {
//         perror("fstat failed");
//         close(shm_fd);
//         return nullptr;
//     }
    
//     // 映射共享内存
//     void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
//     if (addr == MAP_FAILED) {
//         perror("mmap failed");
//         close(shm_fd);
//         return nullptr;
//     }
    
//     close(shm_fd);
//     return static_cast<mpmc_queue<T>*>(addr);
// }

// void close_shared_queue(mpmc_queue<T>* queue, const char* name)
// {
//     // 计算共享内存大小
//     size_t shm_size = ...; // 与创建时相同的大小
    
//     // 解除内存映射
//     if (munmap(queue, shm_size) == -1) {
//         perror("munmap failed");
//     }
    
//     // 删除共享内存对象（通常由创建进程完成）
//     // shm_unlink(name);
// }


bool QueueManager::Init(StrategyMessageManager* pStrategyMessageManager, WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory) {
    LOG_INFO("Init QueueManager, workerType = {}, bIsCreateSharedMemory = {}, cstrSharedMemName = {}", (int)(workerType), bIsCreateSharedMemory, cstrSharedMemName);
    workerType_ = workerType;
    bIsCreateSharedMemory_ = bIsCreateSharedMemory;
    strSharedMemName_ = cstrSharedMemName;
    pStrategyMessageManager_ = pStrategyMessageManager;

    if (!pStrategyMessageManager_) {
        LOG_ERROR("pStrategyMessageManager_ is null");
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
void QueueManager::StartConsumerThread() {
    LOG_INFO("StartConsumerThread: {}", strSharedMemName_);
    shptrConsumerThread_ = std::make_shared<std::thread>([this]() {
        while (true) {
            UteMsg msg;
            if (pMpmcQueue_->pop(msg)) {
                // 传输消息
                pStrategyMessageManager_->m_pfnOnMessage(msg.iMsgID, msg.strMsgBuf, msg.iMsgLen); 
            }

            sleep(1); // todo 测试专用;
        }
    });

    if (!shptrConsumerThread_) {
        LOG_ERROR("create consumer thread failed");
        return;
    }
}

bool QueueManager::Init(WorkerType workerType, const char* cstrSharedMemName,  bool bIsCreateSharedMemory, unsigned long long ulFileKey ) {
    LOG_INFO("Init QueueManager, workerType = {}, bIsCreateSharedMemory = {}, cstrSharedMemName = {}", (int)(workerType), bIsCreateSharedMemory, cstrSharedMemName);

    workerType_ = workerType;
    bIsCreateSharedMemory_ = bIsCreateSharedMemory;
    strSharedMemName_ = cstrSharedMemName;
    ulFileKey_ = ulFileKey;

    if (!bIsCreateSharedMemory) {
        // 共享内存名称不为空时，尝试打开已有的共享内存
        if (strcmp(cstrSharedMemName, "") != 0) {
            return AttachShareMemory(cstrSharedMemName); // 尝试打开已有的共享内存 -- 对于UTE进程，需要映射对应的策略进程的共享内存；
        } else {
            return InitQueueWithoutSharedMemory(); // 创建内存中的无所队列 -- 对于UTE进程，创建转发 API请求回报的无锁队列；
        }
    } else {
        return CreateShareMemory(cstrSharedMemName); // 创建新的共享内存 -- 对于UTE进程，创建接受策略进程发送的请求的无锁队列；
    }

    return true;
}


bool QueueManager::InitQueueWithoutSharedMemory() {
    LOG_INFO("InitQueueWithoutSharedMemory Start!");

    bIsAttachSharedMemory_ = false;
    bIsCreateSharedMemory_ = false;

    pMpmcQueue_ = new share_common::mpmc_queue<UteMsg>();

    if (!pMpmcQueue_) {
        LOG_ERROR("create queue failed");
        return false;
    }
    if (!pMpmcQueue_->create(uiQueueBlockCount_)) {
        LOG_ERROR("queue create  failed");
        return false;
    }

    return true;
}

bool QueueManager::AttachShareMemory(const char* cstrSharedMemName) {
    LOG_INFO("AttachShareMemory: {}", cstrSharedMemName);

    bIsAttachSharedMemory_ = true;
    // 打开已有的共享内存对象
    int shm_fd = shm_open(cstrSharedMemName, O_RDWR, 0);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    
    // 获取共享内存大小
    struct stat stat_buf;
    if (fstat(shm_fd, &stat_buf) == -1) {
        LOG_ERROR("fstat {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    uiMemorySize_ = stat_buf.st_size; // 记录共享内存大小
    void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_ERROR("mmap {} failed ", cstrSharedMemName);
        close(shm_fd);
        return false;
    }

    LOG_DEBUG("pMpmcQueue_ attach uiMemorySize_: {}", uiMemorySize_);
    
    close(shm_fd);
    pMpmcQueue_ = static_cast<share_common::mpmc_queue<UteMsg>*>(addr);   

    if (!pMpmcQueue_) {
        LOG_ERROR("pMpmcQueue_ is null");
        return false;
    }

    pMpmcQueue_->slot_attach(addr + sizeof(share_common::mpmc_queue<UteMsg>) + 128); // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；

    // LOG_DEBUG("pMpmcQueue_->size = {}", pMpmcQueue_->mask_ + 1);

    // LOG_DEBUG("pMpmcQueue_->size = {},slot_address: {}", pMpmcQueue_->mask_ + 1, (void*)(pMpmcQueue_->slots_));

    //     //测试专用 - 写入测试数据;
    // UteMsg testPushMsg(kPktStrategyEnd, 0, 8888, "");
    // if (pMpmcQueue_->trypush(testPushMsg) ){
    //     LOG_DEBUG("[SUCCESS] trypush msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPushMsg.iMsgID, testPushMsg.iMsgLen, testPushMsg.ulStrategyKey);
    // } else {
    //     LOG_ERROR("[FAILED] trypush msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPushMsg.iMsgID, testPushMsg.iMsgLen, testPushMsg.ulStrategyKey);
    // }

    // UteMsg testPopMsg;
    // if (pMpmcQueue_->trypop(testPopMsg)) {
    //     LOG_DEBUG("[SUCCESS] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // } else {
    //     LOG_ERROR("[FAILED] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // }


    return true;
}

bool QueueManager::CreateShareMemory(const char* cstrSharedMemName) {
    LOG_INFO("CreateShareMemory: {}", cstrSharedMemName);
    bIsCreateSharedMemory_ = true;
    int shm_fd = shm_open(cstrSharedMemName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", cstrSharedMemName);
        return false;
    }
    
    unsigned int uiElementSlotBlocksSize = (share_common::roundup_pow_of_two(uiQueueBlockCount_) + 2) * sizeof( element_slot<UteMsg, false>) ;

    // 设置共享内存大小
    uiMemorySize_ = sizeof(share_common::mpmc_queue<UteMsg>) + uiElementSlotBlocksSize + 1024; // 给共享内存留足够的空间;
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

    LOG_DEBUG("pMpmcQueue_ create uiMemorySize_: {}", uiMemorySize_);
    
    // 在共享内存中构造队列对象
    pMpmcQueue_ = new (addr) share_common::mpmc_queue<UteMsg>();
    
    // 使用自定义内存分配器初始化队列
    // pMpmcQueue_->create(uiQueueBlockCount_);

    if (!pMpmcQueue_->create_shared(uiQueueBlockCount_, addr + sizeof(share_common::mpmc_queue<UteMsg>) + 128)) { // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；
        LOG_ERROR("queue create_shared  failed");
        return false;
    }

    // 测试专用 - 写入测试数据;
    UteMsg testPushMsg(kPktStrategyEnd, 0, 8888, "");
    if (pMpmcQueue_->trypush(testPushMsg) ){
        LOG_DEBUG("[SUCCESS] trypush msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPushMsg.iMsgID, testPushMsg.iMsgLen, testPushMsg.ulStrategyKey);
    } else {
        LOG_ERROR("[FAILED] trypush msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPushMsg.iMsgID, testPushMsg.iMsgLen, testPushMsg.ulStrategyKey);
    }


    // // LOG_DEBUG("pMpmcQueue_->size = {},slot_address: {}", pMpmcQueue_->mask_ + 1, (void*)(pMpmcQueue_->slots_));
    
    close(shm_fd);

    // UteMsg testPopMsg;
    // if (pMpmcQueue_->trypop(testPopMsg)) {
    //     LOG_DEBUG("[SUCCESS] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // } else {
    //     LOG_ERROR("[FAILED] trypop msg: iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", testPopMsg.iMsgID, testPopMsg.iMsgLen, testPopMsg.ulStrategyKey);
    // }

    return true;
}


void QueueManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulStrategyKey) {
    if (pMpmcQueue_) {
        LOG_DEBUG("iMsgID = {}, iMsgLen = {}, ulStrategyKey = {}", iMsgID, iMsgLen, ulStrategyKey);
        pMpmcQueue_->push(iMsgID, iMsgLen, ulStrategyKey, pMsgBuf); // 在 enqueue 时会调用 UteMsg 的构造函数
    } else {
        LOG_ERROR("pMpmcQueue_ is null");
    }
}

void QueueManager::SendMsg(int iMsgID, const char* pMsgBuf, unsigned int  iMsgLen, int iMsgSrcType, void* pMsgHandler) {
    if (pMpmcQueue_) {
        LOG_DEBUG("iMsgID = {}, iMsgLen = {}, iMsgSrcType = {}", iMsgID, iMsgLen, iMsgSrcType);
        pMpmcQueue_->push(iMsgID, iMsgLen, iMsgSrcType, pMsgHandler, pMsgBuf); // 在 enqueue 时会调用 UteMsg 的构造函数
    } else {
        LOG_ERROR("pMpmcQueue_ is null");
    }
}


void QueueManager::Release() {
    if (!pMpmcQueue_) {
        LOG_ERROR("pMpmcQueue_ is null");
        return;
    }

    /// 若是映射了共享内存，则需要解除内存映射
    if (bIsAttachSharedMemory_ && munmap(pMpmcQueue_, uiMemorySize_) == -1) {

        LOG_ERROR("munmap {} failed ", strSharedMemName_);
        return;
    }

    /// 若是创建了共享内存，则需要删除共享内存对象
    if (bIsCreateSharedMemory_) {
        shm_unlink(strSharedMemName_.c_str());
    }

    /// 若是未映射共享内存，则需要手动释放内存
    if (!bIsAttachSharedMemory_) {
        if (pMpmcQueue_) delete pMpmcQueue_;
    }
        
}

bool QueueManager::trypop(UteMsg& msg) {

    return pMpmcQueue_->trypop(msg);
}

} // namespace share_common