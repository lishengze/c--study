#pragma once

class MarketOutput {
public:
    MarketOutput() {}

    bool Init();

    bool Start();

    bool Stop();
    
    virtual ~MarketOutput() {}

};