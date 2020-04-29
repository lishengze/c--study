#pragma once
#include <functional>
#include <vector>

#include "DataDefine.h"

// 行情与交易数据 更新的格式;

class DataPublisher
{

public:
    void import_data();

    void trans_data();

    void update_data()
    {
        // for (TradeDataPtr)
    }

private:
    std::function<void(MarketDataPtr)>  on_market;
    std::function<void(TradeDataPtr)>   on_trade;

    std::vector<TradeDataPtr>   m_cur_tradedata_vec;
    MarketDataPtr               m_cur_makertdata;
};