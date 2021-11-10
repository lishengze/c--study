#include "log.h"

void BridgeLog::start()
{
    try
    {
        init_logger();
    }
    catch(const std::exception& e)
    {
        std::cerr << __FILE__ << ":"  << __FUNCTION__ <<"."<< __LINE__ << " " <<  e.what() << '\n';
    }
    
}

void BridgeLog::init_logger()
{
    try
    {
        // log4cplus::tstring file_pattern = LOG4CPLUS_TEXT("%D{%Y.%m.%d %H:%M:%S} [%F-%L] %-5p - %m %n");

        log4cplus::tstring file_pattern = LOG4CPLUS_TEXT("%D{%Y.%m.%d %H:%M:%S} %-5p - %m %n");

        common_logger_ = boost::make_shared<log4cplus::Logger>(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT ("common")));
        common_logger_->setLogLevel(log4cplus::TRACE_LOG_LEVEL);

        string time_str = utrade::pandora::SecTimeStr("%Y-%m-%d_%H:%M:%S");

        string log_dir = work_dir_ + program_name_ + "_log/";

        trace_appender_ = new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT(log_dir + time_str + "_trace.log"), 100*1024*1024, 2, true, false);
        trace_appender_->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(file_pattern)));
        trace_appender_->setThreshold(log4cplus::TRACE_LOG_LEVEL);

        info_appender_ = new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT(log_dir + time_str +  "_info.log"), 100*1024*1024, 2, true, true);
        info_appender_->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(file_pattern)));
        info_appender_->setThreshold(log4cplus::INFO_LOG_LEVEL);

        warn_appender_ = new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT(log_dir + time_str + "_warn.log"), 100*1024*1024, 2, true, true);
        warn_appender_->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(file_pattern)));
        warn_appender_->setThreshold(log4cplus::WARN_LOG_LEVEL);          

        common_logger_->addAppender(trace_appender_);
        common_logger_->addAppender(info_appender_);     
        common_logger_->addAppender(warn_appender_);      

        debug_logger_ = boost::make_shared<log4cplus::Logger>(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT ("debug")));
        debug_logger_->setLogLevel(log4cplus::DEBUG_LOG_LEVEL);

        debug_appender_ = new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT(log_dir + time_str +  "_debug.log"), 100*1024*1024, 2, true, true);
        debug_appender_->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(file_pattern)));
        debug_appender_->setThreshold(log4cplus::DEBUG_LOG_LEVEL);
        debug_logger_->addAppender(debug_appender_);

    }
    catch(const std::exception& e)
    {
        std::cerr << __FILE__ << ":"  << __FUNCTION__ <<"."<< __LINE__ << " " <<  e.what() << '\n';
    }
}
