#include "ute_server.h"
#include "logger.h"
#include "share_comm_util.h"

using namespace share_common;


int main()
{
    std::string loggerName = "ute_server_"+ SecTimeStr("%Y%m%d") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    LOG_INFO("Write Server Test Start");    

    TestWriteMain();

    return 0;    
}   