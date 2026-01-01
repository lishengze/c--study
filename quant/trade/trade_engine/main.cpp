#include "trade_manager.h"
#include "logger.h"
#include "share_comm_util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "trade_"+ SecTimeStr("%Y%m%d") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    LOG_INFO("Trade Server Start");    

    TradeManager tradeManager;
    if (!tradeManager.Init()) {
        LOG_ERROR("TradeManager Init Failed");
        return -1;
    }
    if (!tradeManager.Start()) {
        LOG_ERROR("TradeManager Start Failed");
        return -1;
    }

    return 0;    
}   