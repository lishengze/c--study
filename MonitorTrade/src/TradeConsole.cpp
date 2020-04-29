#include "TradeConsole.h"

void TradeConsole::init_depth_data()
{
    emit_update_data_signal();
}

void TradeConsole::init_process_event_thread()
{
    m_process_event_thread = std::make_shared<std::thread>(&TradeConsole::process_event, this);

    if (m_process_event_thread->joinable())
    {
        m_process_event_thread->join();
    }
}

void TradeConsole::launch()
{
    init_depth_data();

    init_process_event_thread();
}

void TradeConsole::on_market(MarketDataPtr market_data)
{

}

void TradeConsole::on_trade(TradeDataPtr trade_data)
{

}

// 处理所有事件
// 事件队列在不停的 插入和删除；
void TradeConsole::process_event()
{
    
}

void TradeConsole::insert_event(TIME_TYPE time_stamp, EventBasePtr new_event)
{
    m_event_map.emplace(time_stamp, new_event);
}

