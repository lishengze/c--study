#pragma once

#include "share_comm_external_message.h"
#include "share_comm_mpmc_queue.h"

#include "logger.h"

using namespace share_common;

#include <string>

class TradeOutput {
public:
    TradeOutput():{}

    bool Init();

    bool Start();

    bool Stop();

    
    virtual ~TradeOutput() {}

private:


};