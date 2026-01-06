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

#include "comm_define.h"
#include "market_manager.h"
#include "market_data_manager.h"

#include "thread_pool.h"

using namespace share_common;
using std::shared_ptr;

MarketManager::MarketManager() {
}

bool MarketManager::Init() {

    if (!CONFIG_MANAGER_INSTANCE->Init()) return false;

    THREAD_POOL_SIMPLE->Init(CONFIG_MANAGER_INSTANCE->GetIntValue("WorkMode", "MarketComputeCpuCount",4));

    if (!market_receiver_.Init()) return false;

    if (!market_output_.Init()) return false;    

    if (!market_data_manager_.Init()) return false;

    KlineVectorCallbackFuncType callback_func = std::bind(&MarketOutput::OutputVecKline, &market_output_, std::placeholders::_1);
    market_data_manager_.SetKlineCallback(callback_func);

    KlineVectorCallbackFuncType kline_vector_callback_func = std::bind(&MarketDataManager::ProcessVecKline, &market_data_manager_, std::placeholders::_1);
    market_receiver_.SetKlineVectorCallback(kline_vector_callback_func);

    LOG_INFO("MarketManager Init Success");

    return true;
}

bool MarketManager::Start() { 

    if (!market_receiver_.Start()) return false;

    if (!market_data_manager_.Start()) return false;    

    if (!market_output_.Start()) return false;

    LOG_INFO("MarketManager Start Success");

    return true;

}

// bool MarketManager::StartListenSrcMarketData() {
//     LOG_INFO("StartListenSrcMarketData Start");

//     shptrGetSrcMarketDataThread_ = std::make_shared<std::thread>([this]() {
//         while (bIsRunning_) {
//             MarketData msg;

//             if (ptr_src_market_data_queue_->trypop(msg)) {
//                 LOG_INFO("StartListenSrcMarketData, pop market_data:\n{}", msg.str());
//                 data_compute_.OnMarketSrcData(msg);
//             } else {
//                 LOG_INFO("StartListenSrcMarketData, ptr_src_market_data_queue_ try_pop failed");
//             }
            
//             std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            
//         }
//         LOG_INFO("Waiting  Queue Data Is Over");
        
//     });

//     if (!shptrGetSrcMarketDataThread_) {
//         LOG_ERROR("create consumer thread failed");
//         return false;
//     }    

//     return true;
// }
