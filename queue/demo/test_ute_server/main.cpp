#include "ute_server.h"
#include "logger.h"
#include "util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "ute_server_"+ SecTimeStr("%Y%m%d_%H%M%S") + ".log";
    share_common::logger::init(loggerName);
    share_common::logger::set_level(spdlog::level::debug);

    LOG_INFO("UTE Server Test Start");    

    return 0;    
}   