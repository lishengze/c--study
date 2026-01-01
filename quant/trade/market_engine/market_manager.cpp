#include "market_manager.h"
#include "config_manager.h"
#include "logger.h"
#include "share_comm_util.h"

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


using namespace share_common;
using std::shared_ptr;

MarketManager::MarketManager():ptr_src_market_data_queue_{nullptr}, iQueueSize_{4096},bIsRunning_{true} {
}

bool MarketManager::Init() {

    CONFIG_MANAGER_INSTANCE->Init();


    // if (!InitSrcMarketDataQueue()) return false;

    if (!market_receiver_.Init()) return false;

    ptr_src_market_data_queue_ = market_receiver_.GetSrcMarketDataQueue();

    market_receiver_.SetSrcMarketDataQueue(ptr_src_market_data_queue_);

    if (!market_output_.Init()) return false;

    if (!data_compute_.Init()) return false;

    MarketDataCallbackFuncType callback_func = std::bind(&MarketOutput::OutputMarketData, &market_output_, std::placeholders::_1);
    data_compute_.SetMarketDataCallbackFunc(callback_func);

    LOG_INFO("MarketManager Init Success");

    return true;
}

// bool MarketManager::InitSrcMarketDataQueue() {
//     LOG_INFO("InitSrcMarketDataQueue Start");

//     ptr_src_market_data_queue_ = new mpmc_queue<MarketData>();

//     if (!ptr_src_market_data_queue_->create(iQueueSize_)) {
//         LOG_ERROR("queue create  failed");
//         return false;
//     }

//     MarketData market_data;

//     for (int i = 0; i < 10; ++i) {
//         market_data.SetRandomData();

//         if (!ptr_src_market_data_queue_->trypush(market_data)) {
//             LOG_ERROR("InitSrcMarketDataQueue, ptr_src_market_data_queue_ try_push failed");
//             return false;
//         } else {
//             LOG_INFO("InitSrcMarketDataQueue, ptr_src_market_data_queue_ try_push success");
//         }
//     }

                
//     return true;
// }


bool MarketManager::Start() { 
    if (!StartListenSrcMarketData()) return false;

    if (!market_receiver_.Start()) return false;

    if (!data_compute_.Start()) return false;    

    if (!market_output_.Start()) return false;

    LOG_INFO("MarketManager Start Success");

    return true;

}

bool MarketManager::StartListenSrcMarketData() {
    LOG_INFO("StartListenSrcMarketData Start");

    shptrGetSrcMarketDataThread_ = std::make_shared<std::thread>([this]() {
        while (bIsRunning_) {
            MarketData msg;

            if (ptr_src_market_data_queue_->trypop(msg)) {
                LOG_INFO("StartListenSrcMarketData, pop market_data:\n{}", msg.str());
                data_compute_.OnMarketSrcData(msg);
            } else {
                LOG_INFO("StartListenSrcMarketData, ptr_src_market_data_queue_ try_pop failed");
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            
        }
        LOG_INFO("Waiting  Queue Data Is Over");
        
    });

    if (!shptrGetSrcMarketDataThread_) {
        LOG_ERROR("create consumer thread failed");
        return false;
    }    

    return true;
}
