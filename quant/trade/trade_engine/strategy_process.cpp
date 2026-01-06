#include "strategy_process.h"
#include "config_manager.h"
#include "logger.h"
#include "share_comm_external_message.h"
#include <string>
using std::string;
using namespace share_common;

bool StrategyProcess::Init() {
    string lib_name = "libstrategy_impl_1";

    my_set<my_string>& setStrategySet_ = CONFIG_MANAGER_INSTANCE->GetStrategySet();
    for (auto& it : setStrategySet_) {

        string lib_name = it;

        TradeUnitDllInfoPtr pTradeUnitDllInfo = std::make_shared<TradeUnitDllInfo>(lib_name, ".");

        if (!pTradeUnitDllInfo->LoadDll()) {
            LOG_WARN("LoadDll failed, lib_name: {}", lib_name);
            
        }

        if (pTradeUnitDllInfo->RegisterAppMain(this) == ErrSuccess) {
            strategy_dll_map_[lib_name] = pTradeUnitDllInfo;
        } else {
            LOG_WARN("RegisterAppMain failed, lib_name: {}", lib_name);            
        }
    }
    


    return true;
}

bool StrategyProcess::Start() {
    return true;
}

void StrategyProcess::OnMarketSrcData(const KlineAtom& market_data) {
    LOG_INFO("OnMarketSrcData, market_data: \n{}", market_data.str());

    for (auto& it : strategy_dll_map_) {
        TradeUnitDllInfoPtr pTradeUnitDllInfo = it.second;
        pTradeUnitDllInfo->ProcessKlineAtom(const_cast<KlineAtom*>(&market_data));
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
