#pragma once

#include <sys/time.h>
#include <sstream>
#include <iomanip>
#include "global_declare.h"

const long MILLISECONDS_PER_SECOND = 1000;
const long MICROSECONDS_PER_MILLISECOND = 1000;
const long NANOSECONDS_PER_MICROSECOND = 1000;

const long MICROSECONDS_PER_SECOND =
        MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;
const long NANOSECONDS_PER_MILLISECOND =
        NANOSECONDS_PER_MICROSECOND * MICROSECONDS_PER_MILLISECOND;
const long NANOSECONDS_PER_SECOND =
        NANOSECONDS_PER_MILLISECOND * MILLISECONDS_PER_SECOND;

const int SECONDS_PER_MINUTE = 60;
const int MINUTES_PER_HOUR = 60;
const int HOURS_PER_DAY = 24;
const int SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;

const long MILLISECONDS_PER_MINUTE =
        MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE;
const long NANOSECONDS_PER_MINUTE = NANOSECONDS_PER_SECOND * SECONDS_PER_MINUTE;
const long NANOSECONDS_PER_HOUR = NANOSECONDS_PER_SECOND * SECONDS_PER_HOUR;
const long NANOSECONDS_PER_DAY = NANOSECONDS_PER_HOUR * HOURS_PER_DAY;

/**
 * parse struct tm to nano time
 * @param _tm ctime struct
 * @return nano time in long
 */
inline long ToNanoSecondFromStruct(struct tm _tm) {
    return mktime(&_tm) * NANOSECONDS_PER_SECOND;
}

inline long ToSecondFromStruct(struct tm _tm)
{
    return mktime(&_tm);
}

/**
 * dump long time to string with format
 * @param nano nano time in long
 * @param format eg: %Y%m%d-%H:%M:%S
 * @return string-formatted time
 */
inline std::string ToSecondStr(long nano, const char* format="%Y-%m-%d %H:%M:%S") {
    if (nano <= 0)
        return std::string("NULL");
    nano /= NANOSECONDS_PER_SECOND;
    struct tm* dt = {0};
    char buffer[30];
    dt = gmtime(&nano);
    strftime(buffer, sizeof(buffer), format, dt);
    return std::string(buffer);
}

/**
 * dump long time to struct tm
 * @param nano nano time in long
 * @return ctime struct
 */
inline struct tm ToSecondStruct(long nano) {
    time_t sec_num = nano / NANOSECONDS_PER_SECOND;
    return *gmtime(&sec_num);
}

/**
 * util function to utilize NanoTimer
 * @return current nano time in long (unix-timestamp * 1e9 + nano-part)
 */
inline long NanoTime() {
    std::chrono::high_resolution_clock::time_point curtime = std::chrono::high_resolution_clock().now();
    long orin_nanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(curtime.time_since_epoch()).count();
    return orin_nanosecs;
//    return NanoTimer::getInstance()->getNano();
}

/*

*/
inline std::string NanoTimeStr() {
    long nano_time = NanoTime();
    string time_now = ToSecondStr(nano_time, "%Y-%m-%d %H:%M:%S");
    time_now += "." + std::to_string(nano_time % NANOSECONDS_PER_SECOND);
    return time_now;
}

inline std::string SecTimeStr(std::string time_format="%Y-%m-%d %H:%M:%S") {
    long nano_time = NanoTime();
    string time_now = ToSecondStr(nano_time, time_format.c_str());
    return time_now;
}

/*
 * linux timespec
 */
inline timespec GetTimespec() {
    timespec ts{ 0, 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts;
}
