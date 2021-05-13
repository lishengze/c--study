#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include "thread_safe_singleton.hpp"
#include "time_util.h"

using std::string;
using std::cout;
using std::endl;


#define LOG ThreadSafeSingleton<Log>::DoubleCheckInstance()

#define LOG_ERROR(msg) LOG->log(msg, "Error")
#define LOG_INFO(msg) LOG->log(msg, "Info ")
#define LOG_DEBUG(msg) LOG->log(msg, "Debug")

class Log
{
    public:
        Log()
        {
            log_file_.open(file_name_, ios_base::ate | ios_base::out);
        }

        void log(string msg, string flag)
        {
            std::lock_guard<std::mutex> lk(mutex_);
            
            cout << SecTimeStr() << " " << flag << ": " << msg << endl;

            // if (!log_file_.is_open())
            // {
            //     log_file_.open(file_name_, ios_base::ate | ios_base::out | ios_base::app);
            // }

            // log_file_ << flag << ": " << msg << "\n";
            // log_file_.close();
        }

        ~Log()
        {

        }

    private:
        string              file_name_{"test.log"};
        std::ofstream       log_file_;
        std::mutex          mutex_;
};