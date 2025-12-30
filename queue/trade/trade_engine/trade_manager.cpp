#include "market_manager.h"
#include "config_manager.h"

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

TradeManager::TradeManager():ptr_src_market_data_queue_{nullptr}, iQueueSize_{0} {
    iQueueSize_ = CONFIG_MANAGER_INSTANCE->GetIntValue("TradeManager", "QueueSize", 10000);
}

bool TradeManager::Init() {

    CONFIG_MANAGER_INSTANCE->Init();


    if (!InitSrcMarketDataQueue()) return false;

    if (!market_receiver_.Init()) return false;

    if (!market_output_.Init()) return false;

    if (!data_compute_.Init()) return false;

    return true;
}

bool TradeManager::InitSrcMarketDataQueue() {

    ptr_src_market_data_queue_ = new mpmc_queue<MarketData>();

    if (!ptr_src_market_data_queue_->create(iQueueSize_)) {
        LOG_ERROR("queue create  failed");
        return false;
    }

    return true;
}


bool TradeManager::Start() { 

    if (!market_receiver_.Start()) return false;

    if (!data_compute_.Start()) return false;    

    MarketDataCallbackFuncType callback_func = std::bind(&MarketOutput::OutputMarketData, &market_output_, std::placeholders::_1);

    data_compute_.SetMarketDataCallbackFunc(callback_func);

    if (!market_output_.Start()) return false;


    if (!StartListenSrcMarketData()) return false;

    return true;

}

bool TradeManager::StartListenSrcMarketData() {

    shptrGetSrcMarketDataThread_ = std::make_shared<std::thread>([this]() {
        while (bIsRunning_) {
            MarketData msg;
            ptr_src_market_data_queue_->pop(msg); 

            data_compute_.OnMarketSrcData(msg);

            // sleep(1); // todo ≤‚ ‘◊®”√;
        }
        LOG_INFO("Waiting  Queue Data Is Over");
        
    });

    if (!shptrGetSrcMarketDataThread_) {
        LOG_ERROR("create consumer thread failed");
        return false;
    }    

    return true;
}
