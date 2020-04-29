#pragma once

#include <memory>

#define ORDER_EVENT         0
#define ON_TIMER_EVENT      1
#define UPDATE_DATA_EVENT   2

using TIME_TYPE = unsigned long long;

struct MarketData
{
    /* data */
};

struct TradeData
{
    /* data */
};

using MarketDataPtr = std::shared_ptr<MarketData>;
using TradeDataPtr = std::shared_ptr<TradeData>;

