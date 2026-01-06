#pragma once
#ifndef UTE_API_CODE
#if !defined(SPDLOG_ACTIVE_LEVEL)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#endif

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace tech {
    inline void Nothing() {

    }

}

namespace share_common
{

#ifndef UTE_API_CODE
/*
 * async logger
 *
 * define SPDLOG_ACTIVE_LEVEL 6 // Turn off logger during compilation
 */
class logger
{
public:
    static void init(std::string name);

    static void set_level(int level);

    static void stop();

public:
    static std::shared_ptr<spdlog::logger> async_logger;
    static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;
    static int log_level;
};



#define LOG_TRACE(...)      \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_TRACE) {SPDLOG_LOGGER_TRACE(share_common::logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_DEBUG(...)      \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_DEBUG) {SPDLOG_LOGGER_DEBUG(share_common::logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_INFO(...)       \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_INFO) {SPDLOG_LOGGER_INFO(share_common::logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_WARN(...)       \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_WARN) {SPDLOG_LOGGER_WARN(share_common::logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_ERROR(...)      \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_ERROR) {SPDLOG_LOGGER_ERROR(share_common::logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_CRITICAL(...)   \
    do { if (share_common::logger::log_level <= SPDLOG_LEVEL_CRITICAL) {SPDLOG_LOGGER_CRITICAL(share_common::logger::async_logger, __VA_ARGS__);}}while(0)

#endif

}   // namespace share_common
