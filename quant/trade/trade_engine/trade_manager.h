#pragma once

#include "strategy_process.h"
#include "trade_output.h"
#include "msg_receiver.h"

#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"

#include "logger.h"

#include <memory>
#include <string>

using namespace std;

using namespace share_common;

class TradeManager {
public:
    TradeManager() ;

    bool Init() ;

    bool Start();



    ~TradeManager() {
    }

private:
    StrategyProcess strategy_process_;
    TradeOutput trade_output_;
    MsgReceiver msg_receiver_;
};