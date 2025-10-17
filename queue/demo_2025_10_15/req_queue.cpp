#include "req_queue.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "mpmc_queue.h"
#include "external_message.h"

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


bool ReqQueueManager::Init(const char* cstrSharedMemName) {
    // 打开已有的共享内存对象
    int shm_fd = shm_open(cstrSharedMemName, O_RDWR, 0);
    if (shm_fd == -1) {
        // perror("shm_open failed");
        return false;
    }
    
    // 获取共享内存大小
    struct stat stat_buf;
    if (fstat(shm_fd, &stat_buf) == -1) {
        // perror("fstat failed"); todo 增加日志输出
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED) {
        // perror("mmap failed"); todo 增加日志输出
        close(shm_fd);
        return false;
    }
    
    close(shm_fd);
    pReqQueue_ = static_cast<tech::mpmc_queue<UteMsg>*>(addr);    

    return true;
}


void ReqQueueManager::SendMsg(int iMsgID, const char* pMsgBuf, const int iMsgLen, unsigned long long ulStrategyKey) {
    if (!pReqQueue_) {
        pReqQueue_->push(iMsgID, iMsgLen, ulStrategyKey, pMsgBuf); // 在 enqueue 时会调用 UteMsg 的构造函数
    }
}

void ReqQueueManager::Release() {
    if (!pReqQueue_) {
        // todo 增加日志输出
        return;
    }

    if (!uiMemorySize_) {
        // todo 增加日志输出
        return;
    }

    if (munmap(pReqQueue_, uiMemorySize_) == -1) {
        // perror("munmap failed");
        // todo 增加日志输出
        return;
    }
        
}