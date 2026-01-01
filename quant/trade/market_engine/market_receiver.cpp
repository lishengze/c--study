#include "market_receiver.h"
#include "config_manager.h"
#include "logger.h"
#include "share_comm_external_message.h"


using namespace share_common;

bool MarketReceiver::Init() {

    if (!InitSrcMarketDataQueue()) {
        LOG_ERROR("InitSrcMarketDataQueue Failed");
        return false;
    }

    return true;
}

void MarketReceiver::SendMarketDataToQueue(const MarketData& market_data) {
    LOG_INFO("SendMarketDataToQueue:\nmarket_data:{}", market_data.str());

    if (ptr_src_market_data_queue_ != nullptr) {

        if (!ptr_src_market_data_queue_->trypush(market_data)) {
            LOG_INFO("SendMarketDataToQueue, ptr_src_market_data_queue_ try_push failed");
        } else {
            LOG_INFO("SendMarketDataToQueue, ptr_src_market_data_queue_ try_push success");
        }
    } else {
        LOG_ERROR("SendMarketDataToQueue, ptr_src_market_data_queue_ is null");
    }
}

bool MarketReceiver::InitSrcMarketDataQueue() {
    LOG_INFO("InitSrcMarketDataQueue Start");

    ptr_src_market_data_queue_ = new mpmc_queue<MarketData>();

    if (!ptr_src_market_data_queue_->create(4096)) {
        LOG_ERROR("queue create  failed");
        return false;
    }

    MarketData market_data;

    // for (int i = 0; i < 10; ++i) {
    //     market_data.SetRandomData();

    //     if (!ptr_src_market_data_queue_->trypush(market_data)) {
    //         LOG_ERROR("InitSrcMarketDataQueue, ptr_src_market_data_queue_ try_push failed");
    //         return false;
    //     } else {
    //         LOG_INFO("InitSrcMarketDataQueue, ptr_src_market_data_queue_ try_push success");
    //     }
    // }

                
    return true;
}



bool MarketReceiver::Start() {


    if (CONFIG_MANAGER_INSTANCE->GetStringValue("WorkMode", "Mode", "Test") == "Test") {
        ptr_thread_ = std::make_shared<std::thread>([this]() {
            // 接收线程逻辑
            while (true) {
                // 从源市场行情数据队列中获取数据
                MarketData market_data;
                market_data.SetRandomData();
                SendMarketDataToQueue(market_data);
                std::this_thread::sleep_for(std::chrono::seconds(
                                            CONFIG_MANAGER_INSTANCE->GetIntValue("WorkMode", "DataFreqSec", 5)));                
        }
    });
    } else {
        LOG_INFO("MarketReceiver Start Failed, WorkMode is not Test");
        return true;
    }

    return true;
}

bool MarketReceiver::Stop() {
    return true;
}