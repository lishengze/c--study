#pragma once
#include "share_comm_external_message.h"

class DataCompute { 
public:
    DataCompute() {

    }

    bool Init();

    bool Start();

    void OnMarketSrcData(const KlineAtom& market_data);

    bool Stop();

    void SetMarketDataCallbackFunc(const KlineAtomCallbackFuncType& market_data_callback_func) {
        market_data_callback_func_ = market_data_callback_func;
    }

    ~DataCompute() {

    }

private:
    KlineAtomCallbackFuncType market_data_callback_func_;    
};