#include "market_receiver.h"
#include "config_manager.h"
#include "logger.h"
#include "share_comm_external_message.h"


using namespace share_common;

bool MarketReceiver::Init() {
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
                std::this_thread::sleep_for(std::chrono::seconds(CONFIG_MANAGER_INSTANCE->GetIntValue("WorkMode", "DataFreqSec", 5)));
                
        }
    });
    }

    return true;
}

bool MarketReceiver::Stop() {
    return true;
}