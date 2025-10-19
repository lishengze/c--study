#pragma once

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <chrono>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::fstream;
using std::map;
using std::set;
using std::vector;

#include "json.hpp"

using njson = nlohmann::json;

#define ILL_FILE 0
#define REG_FILE 1
#define DIR_FILE 2

#define CMP_UPLOAD  1 
#define CMP_DEFAULT 0 

#define MILLI_PER_SECOND 1000
#define MICRO_PER_MILLI 1000
#define NANO_PER_MICRO 1000

#define MICRO_PER_SECOND (MILLI_PER_SECOND * MICRO_PER_MILLI)
#define NANO_PER_SECOND (MICRO_PER_SECOND * NANO_PER_MICRO)
#define NANO_PER_MILLI (NANO_PER_MICRO * MICRO_PER_MILLI)

namespace share_common
{

inline long NanoTime() {
    std::chrono::high_resolution_clock::time_point curtime = std::chrono::high_resolution_clock().now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(curtime.time_since_epoch()).count();
}


inline string ToSecondStr(long nano, string time_format="%Y-%m-%d %H:%M:%S") {
    if (nano <=0) {
        return "NULL";
    }

    nano /= NANO_PER_SECOND;
    struct  tm* dt ={0};
    char buffer[30] = {0};
    dt = gmtime(&nano);
    strftime(buffer, sizeof(buffer), time_format.c_str(), dt);

    return std::string(buffer);
}

inline std::string SecTimeStr(std::string time_format="%Y-%m-%d %H:%M:%S") {
    long nano_time = NanoTime();
    return ToSecondStr(nano_time, time_format);
}

}