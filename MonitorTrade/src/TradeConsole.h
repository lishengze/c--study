#pragma once

#include "DataDefine.h"

#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include <map>
#include <thread>
#include "EventBase.hpp"

class TradeConsole
{

public:
    void init_depth_data();

    void init_process_event_thread();

    void launch();    

    void process_event();

    void on_market(MarketDataPtr market_data);

    void on_trade(TradeDataPtr trade_data);

    void insert_event(TIME_TYPE time_stamp, EventBasePtr new_event);

    void emit_update_data_signal()
    {
        update_data_signal();
    }

    std::vector<TradeDataPtr>& get_trade_data_vec() { return m_cur_tradedata_vec;}
    MarketDataPtr& get_market_data() { return  m_cur_makertdata;}

private:
    typedef boost::signals2::signal<void()>  update_signal_t;
    typedef update_signal_t::slot_type       update_data_signal;

    std::multimap<TIME_TYPE, EventBasePtr>   m_event_map;

    std::shared_ptr<std::thread>             m_process_event_thread;

    std::vector<TradeDataPtr>                m_cur_tradedata_vec;

    MarketDataPtr                            m_cur_makertdata;    
};