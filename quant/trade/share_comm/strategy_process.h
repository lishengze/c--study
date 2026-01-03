#pragma once
#include "share_comm_external_message.h"


class StrategyProcess { 
public:
    StrategyProcess() {
        strategy_dll_map_.reserve(10);
    }

    bool Init();

    bool Start();

    bool Stop();

    bool CheckFund(const OrderReq& order_req);

    bool CheckStock(const OrderReq& order_req);

    bool SendOrderReq(const OrderReq& order_req) {
        // LOG_INFO("SendOrderReq, order_req: \n{}", order_req.str());
        std::cout << "SendOrderReq, order_req: \n" << order_req.str() << std::endl;
        return true;        
    }

    ~StrategyProcess() {

    }

    void OnMarketSrcData(const MarketData& market_data);

private:  
    std::unordered_map<std::string, TradeUnitDllInfoPtr> strategy_dll_map_;
};