#include <chrono>
#include "logger.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace share_common
{
    std::shared_ptr<spdlog::logger> logger::async_logger = nullptr;
    int logger::log_level = -1;
    
    void logger::init(std::string name)
    {
        if (nullptr != async_logger) {
            return; // already init
        }
        async_logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
            "async_file_logger", 
            name, 
            (std::size_t)1024 * 1024 * 1024 * 3, 
            1000); 

        async_logger->set_pattern("[%H:%M:%S] %P %t [%l] %s [%!] %# | %v");
        async_logger->set_level(spdlog::level::debug);
        async_logger->flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(1));
    }

    void logger::set_level(int level)
    {
        if (log_level > 0) {
            return; // already set
        }
        log_level = level;
        async_logger->set_level((spdlog::level::level_enum)level);
    }

    void logger::stop()
    {
        async_logger->flush();
    }
} // namespace share_common
