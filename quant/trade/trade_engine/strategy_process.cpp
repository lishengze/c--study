#include "strategy_process.h"
#include "logger.h"
#include "share_comm_external_message.h"
#include <string>
using std::string;
using namespace share_common;

bool StrategyProcess::Init() {
    string lib_name = "./libstrategy_impl_1.so";
    TradeUnitDllInfoPtr pTradeUnitDllInfo = std::make_shared<TradeUnitDllInfo>(lib_name);

    if (!pTradeUnitDllInfo->LoadDll()) {
        return false;
    }
    pTradeUnitDllInfo->RegisterAppMain(this);
    strategy_dll_map_[lib_name] = pTradeUnitDllInfo;

    return true;
}

bool StrategyProcess::Start() {
    return true;
}

void StrategyProcess::OnMarketSrcData(const MarketData& market_data) {
    LOG_INFO("OnMarketSrcData, market_data: \n{}", market_data.str());

    for (auto& it : strategy_dll_map_) {
        TradeUnitDllInfoPtr pTradeUnitDllInfo = it.second;
        pTradeUnitDllInfo->ProcessMarketData(const_cast<MarketData*>(&market_data));
    }
}

bool StrategyProcess::Stop() {
    return true;
}

bool StrategyProcess::CheckFund(const OrderReq& order_req) {
    return true;
}

bool StrategyProcess::CheckStock(const OrderReq& order_req) {
    return true;
}
