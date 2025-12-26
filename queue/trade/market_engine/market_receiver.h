#pragma once

class MarketReceiver {
public:
    MarketReceiver() {}

    bool Init();

    bool Start();

    bool Stop();

    virtual ~MarketReceiver() {}

};