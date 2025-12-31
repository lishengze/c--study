#include "msg_receiver.h"
#include "config_manager.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"
#include "bits.h"


bool MsgReceiver::Init() {

    if (!InitMarketDataQueue()) {
        LOG_ERROR("InitMarketDataQueue failed");
        return false;
    }
    return true;
}

bool MsgReceiver::Start() {
    if (!StartReceiveMarketData()) {
        LOG_ERROR("StartReceiveMarketData failed");
        return false;
    }   
    return true;
}

bool MsgReceiver::Stop() {
    return true;
}


bool MsgReceiver::InitMarketDataQueue() {

    // 打开共享内存
    std::string strSharedMemName = CONFIG_MANAGER_INSTANCE->GetStringValue("MsgReceiver", "SharedMemName", "MarketData.queue");
    int shm_fd = shm_open(strSharedMemName.c_str(), O_RDWR, 0);
    if (shm_fd == -1) {
        LOG_ERROR("shm_open {} failed ", strSharedMemName);
        return false;
    }
    
    // 获取共享内存大小
    struct stat stat_buf;
    if (fstat(shm_fd, &stat_buf) == -1) {
        LOG_ERROR("fstat {} failed ", strSharedMemName);
        close(shm_fd);
        return false;
    }
    
    // 映射共享内存
    unsigned int uiMemorySize_ = stat_buf.st_size; // 记录共享内存大小
    void* addr = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_POPULATE, shm_fd, 0);
    if (addr == MAP_FAILED) {
        LOG_ERROR("mmap {} failed ", strSharedMemName);
        close(shm_fd);
        return false;
    }

    close(shm_fd);

    ptr_share_market_data_queue_ = static_cast<mpmc_queue<MarketData>*>(addr);   

    if (!ptr_share_market_data_queue_) {
        LOG_ERROR("ptr_share_market_data_queue_ is null");
        return false;
    }
    
    // 手动将slot 映射到外部的内存地址中 -- 共享内存版本,这一步导致了很多的问题，导致无法进行服务端对slot 的解锁出错了。
    ptr_share_market_data_queue_->slot_attach(ptr_share_market_data_queue_slot_, static_cast<void*>((char*)addr + sizeof(mpmc_queue<MarketData>) + 32));


    return true;
        
}

bool MsgReceiver::StartReceiveMarketData() {
    // 启动接收市场数据线程
    shptrGetSrcMarketDataThread_ = std::make_shared<std::thread>([this]() {
        while (bIsRunning_) {
            MarketData market_data;
            if (ptr_share_market_data_queue_->pop(market_data)) {
                // 处理市场数据
                market_data_callback_func_(market_data);
            }
        }
    });
    
    if (!shptrGetSrcMarketDataThread_) {
        LOG_ERROR("shptrGetSrcMarketDataThread_ is null");
        return false;
    }

    return true;
}
