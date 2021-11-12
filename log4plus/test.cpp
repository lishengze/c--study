#include "test.h"

#include <log4cplus/log4cplus.h>

void test_file_appener()
{
log4cplus::Initializer initializer;
 
    //第1步：创建ConsoleAppender和FileAppender(参数app表示内容追加到文件)
    log4cplus::SharedAppenderPtr consoleAppender(new log4cplus::ConsoleAppender);
    log4cplus::SharedAppenderPtr fileAppender(new log4cplus::FileAppender(
                                                  LOG4CPLUS_TEXT("log.txt"),
                                                  std::ios_base::app
                                                  )
                                              );
 
    //第2步：设置Appender的名称和输出格式
    //ConsoleAppender使用SimpleLayout
    //FileAppender使用PatternLayout
    consoleAppender->setName(LOG4CPLUS_TEXT("console"));
    log4cplus::tstring simple_pattern = LOG4CPLUS_TEXT("%D{%Y.%m.%d %H:%M:%S} %-5p - %m %n");
    consoleAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(simple_pattern)));


    fileAppender->setName(LOG4CPLUS_TEXT("file"));
    fileAppender->setThreshold(log4cplus::WARN_LOG_LEVEL);

    // log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D{%m/%d/%y %H:%M:%S,%Q} [%t] %-5p %c - %m [%l]%n");


    log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D{%Y.%m.%d %H:%M:%S} [%F-%L] %-5p - %m %n");
    fileAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)));
 
    //第3步：获得一个Logger实例，并设置其日志输出等级阈值
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT ("test"));
    logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
 
    //第4步：为Logger实例添加ConsoleAppender和FileAppender
    logger.addAppender(consoleAppender);
    logger.addAppender(fileAppender);
 
    //第5步：使用宏将日志输出
    LOG4CPLUS_TRACE(logger, LOG4CPLUS_TEXT("Hello world"));
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("Hello world"));
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Hello world"));
    LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT("Hello world"));
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Hello world"));
    LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT("Hello world"));
    
}

void TestMain()
{    
    cout << "Test Log4Plus " << endl;

    test_file_appener();
}
