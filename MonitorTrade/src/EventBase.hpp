#pragma once
#include <memory>

#include "DataDefine.h"

class TradeConsole;

class EventBase
{
    public:
        EventBase(int event_type, TradeConsole* trade_console): 
                m_event_type(event_type), m_trade_console(trade_console)
        {}

        virutal void execute() = 0;

        virtual ~EventBase() {}

    private:

        int                 m_event_type{ORDER_EVENT};

        TradeConsole*       m_trade_console;
};

using EventBasePtr = std::shared_ptr<EventBase>;