#include "market_output.h"

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <utility>
#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"
#include "bits.h"

#include "logger.h"



bool MarketOutput::Init() {

    InitShareMarketDataQueue();

    return true;
}

bool MarketOutput::Start() {
    return true;
}

bool MarketOutput::Stop() {
    return true;
}

void MarketOutput::OutputMarketData(const MarketData& market_data) {
    
    // if (market_data_callback_func_) {
    //     market_data_callback_func_(market_data);
    // }

    if (ptr_share_market_data_queue_) {
        ptr_share_market_data_queue_->push(market_data);
    }
}

bool MarketOutput::InitShareMarketDataQueue() {

    ptr_share_market_data_queue_ = new mpmc_queue<MarketData>();

    if (!ptr_share_market_data_queue_->create(iQueueSize_)) {
        LOG_ERROR("queue create  failed");
        return false;
    }

    int shm_fd = shm_open(strSharedMemName_.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", strSharedMemName_);
        return false;
    }
    
    unsigned int uiElementSlotBlocksSize = (roundup_pow_of_two(iQueueSize_) + 2) * sizeof( element_slot<MarketData, false>) ;

    // 设置共享内存大小
    unsigned int uiMemorySize_ = sizeof(mpmc_queue<MarketData>) + uiElementSlotBlocksSize + 128; // 给共享内存留足够的空间;
    if (ftruncate(shm_fd, uiMemorySize_) == -1) {
        LOG_ERROR("ftruncate {} failed ", strSharedMemName_);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    void* addr = mmap(NULL, uiMemorySize_, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_POPULATE, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_ERROR("mmap {} failed ", strSharedMemName_);
        close(shm_fd);
        return false;
    }


    // 在共享内存中构造队列对象
    ptr_share_market_data_queue_ = new mpmc_queue<MarketData>(addr);
    if (!ptr_share_market_data_queue_) {
        LOG_ERROR("new pMarketDataMpmcQueue_ Failed!");
        return false;
    }
    
    // 使用自定义内存分配器初始化队列
    // 手动将slot 映射到外部的内存地址中 -- 共享内存版本；
    if (!ptr_share_market_data_queue_->create_shared(iQueueSize_, ptr_share_market_data_queue_slot_, 
                                        static_cast<void*>((char*)addr + sizeof(mpmc_queue<MarketData>) + 32))) { 

        LOG_ERROR("ptr_share_market_data_queue_ create_shared  failed");
        return false;
    }


    return true;
}