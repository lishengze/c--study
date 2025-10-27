#include "spsc_1.h"
#include "spsc_2.h"
#include "mpmc_1.h"

#include "logger.h"
#include "util.h"

int main() {
    std::string loggerName = "startegy_message_manager_"+ SecTimeStr("%Y%m%d") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    // TestSpscQueue_2();
    TestMpmc_2();
    return 0;
}