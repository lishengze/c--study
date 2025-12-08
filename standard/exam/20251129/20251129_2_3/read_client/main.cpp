#include "strategy_client.h"
#include "logger.h"
#include "share_comm_util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "strategy_client_"+ SecTimeStr("%Y%m%d") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    LOG_INFO("Read Client Test Start");    

    TestReadMain();

    return 0;    
}   