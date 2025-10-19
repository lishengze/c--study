#include "strategy_client.h"
#include "logger.h"
#include "util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "strategy_client_"+ SecTimeStr("%Y%m%d") + ".log";
    share_common::logger::init(loggerName);
    share_common::logger::set_level(spdlog::level::debug);

    LOG_INFO("Strategy Client Test Start");    

    TestStrategyClient();

    return 0;    
}   