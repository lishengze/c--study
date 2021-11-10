#pragma once

#include "../bridge_declare.h"

#include "pandora/util/thread_safe_singleton.hpp"
#include "pandora/log/base_log.h"

#include "../utils/tools.h"

class BridgeLog: public BaseLog 
{
public:

    BridgeLog():BaseLog()
    {

    }

    virtual ~BridgeLog() { }

    virtual void init_logger() override;

    virtual void start() override; 
};

#define LOG utrade::pandora::ThreadSafeSingleton<BridgeLog>::DoubleCheckInstance()

#define B_LOG_TRACE(info) LOG->log_trace_(LOG_HEADER + info)
#define B_LOG_DEBUG(info) LOG->log_debug_(LOG_HEADER + info)
#define B_LOG_INFO(info) LOG->log_info_(LOG_HEADER + info)
#define B_LOG_WARN(info) LOG->log_warn_(LOG_HEADER + info)
#define B_LOG_ERROR(info) LOG->log_error_(LOG_HEADER + info)
#define B_LOG_FATAL(info) LOG->log_fatal_(LOG_HEADER + info)


