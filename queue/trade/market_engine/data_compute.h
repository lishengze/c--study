#pragma once

class DataCompute { 
public:
    DataCompute() {

    }

    bool Init();

    bool Start();

    void OnMarketData(const MarketData& market_data);

    bool Stop();

    ~DataCompute() {

    }
};