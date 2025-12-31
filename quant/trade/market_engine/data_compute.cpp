#include "data_compute.h"
#include "logger.h"
using namespace share_common;

bool DataCompute::Init() {
    return true;
}

bool DataCompute::Start() {
    return true;
}

void DataCompute::OnMarketSrcData(const MarketData& market_data) {
    LOG_INFO("OnMarketSrcData, market_data: {}", market_data.str());

    market_data_callback_func_(market_data);
    
}

bool DataCompute::Stop() {
    return true;
}