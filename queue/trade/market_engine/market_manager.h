#pragma once

#include "data_compute.h"
#include "market_output.h"
#include "market_receiver.h"

class MarketManager {
public:
    MarketManager() {

    }
    ~MarketManager() {
        
    }

private:
    DataCompute data_compute_;
    MarketOutput market_output_;
    MarketReceiver market_receiver_;

};