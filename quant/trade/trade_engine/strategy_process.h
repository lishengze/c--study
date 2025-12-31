#pragma once
#include "share_comm_external_message.h"

class StrategyProcess { 
public:
    StrategyProcess() {

    }



    bool Init();

    bool Start();

    bool Stop();

    ~StrategyProcess() {

    }

    void OnMarketSrcData(const MarketData& market_data);

private:  
};