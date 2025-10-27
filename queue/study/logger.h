#pragma once
#ifndef UTE_API_CODE
#if !defined(SPDLOG_ACTIVE_LEVEL)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#endif

namespace tech {
    inline void Nothing() {

    }

}


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
    static int log_level;
};



#define LOG_TRACE(...)      \
    do { if (logger::log_level <= SPDLOG_LEVEL_TRACE) {SPDLOG_LOGGER_TRACE(logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_DEBUG(...)      \
    do { if (logger::log_level <= SPDLOG_LEVEL_DEBUG) {SPDLOG_LOGGER_DEBUG(logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_INFO(...)       \
    do { if (logger::log_level <= SPDLOG_LEVEL_INFO) {SPDLOG_LOGGER_INFO(logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_WARN(...)       \
    do { if (logger::log_level <= SPDLOG_LEVEL_WARN) {SPDLOG_LOGGER_WARN(logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_ERROR(...)      \
    do { if (logger::log_level <= SPDLOG_LEVEL_ERROR) {SPDLOG_LOGGER_ERROR(logger::async_logger, __VA_ARGS__);}}while(0)
#define LOG_CRITICAL(...)   \
    do { if (logger::log_level <= SPDLOG_LEVEL_CRITICAL) {SPDLOG_LOGGER_CRITICAL(logger::async_logger, __VA_ARGS__);}}while(0)

#endif
