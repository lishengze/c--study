#include "market_manager.h"
#include "logger.h"
#include "share_comm_util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "market_"+ SecTimeStr("%Y%m%d") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    LOG_INFO("Market Start");    


    MarketManager marketManager;
    if (!marketManager.Init()) {
        LOG_ERROR("MarketManager Init Failed");
        return -1;
    }
    if (!marketManager.Start()) {
        LOG_ERROR("MarketManager Start Failed");
        return -1;
    }
    
    return 0;    
}   