#include "strategy_process.h"
#include "logger.h"
#include "share_comm_external_message.h"

using namespace share_common;

bool StrategyProcess::Init() {
    return true;
}

bool StrategyProcess::Start() {
    return true;
}

void StrategyProcess::OnMarketSrcData(const MarketData& market_data) {
    LOG_INFO("OnMarketSrcData, market_data: {}", market_data.str());
}

bool StrategyProcess::Stop() {
    return true;
}