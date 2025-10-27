#pragma once

#include "test_ute.h"
#include "test_strategy.h"
#include "test_external_msg.h"
#include "test_lock_file.h"
#include "test_queue.h"

#include "logger.h"
#include "share_comm_util.h"

using namespace share_common;


inline void TestMain() {
    std::string loggerName = "test_"+ SecTimeStr("%Y%m%d_%H%M%S") + ".log";
    logger::init(loggerName);
    logger::set_level(spdlog::level::debug);

    LOG_INFO("Share Common Test Start");

    test_ute();
    test_strategy();
    test_external_msg();
    test_lock_file();
    test_queue();
}