#include "EventHandler.h"

// 1. 根据最新的market, trade 信息处理订单；
void EventOrder::execute()
{

}

// 1. Send Update Signal
void EventUpdate::execute()
{
    m_trade_console->emit_update_data_signal();
}

// 1. Call On Timer Interface;
// 2. Add Next Timer Event;
void EventTimer::execute()
{

}