#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <iostream>
using namespace std;

#define MILLI_PER_SECOND 1000
#define MICRO_PER_MILLI 1000
#define NANO_PER_MICRO 1000

#define MICRO_PER_SECOND (MILLI_PER_SECOND * MICRO_PER_MILLI)
#define NANO_PER_SECOND (MICRO_PER_SECOND * NANO_PER_MICRO)
#define NANO_PER_MILLI (NANO_PER_MICRO * MICRO_PER_MILLI)


inline std::string GetSimpleTimeData(std::vector<unsigned long long>& vecTime) {
    if (vecTime.size()  == 0) return  "";

    std::sort(vecTime.begin(), vecTime.end());

    unsigned long long ulMin = vecTime[0];
    unsigned long long ulMax = vecTime[vecTime.size()-1];

    unsigned long long ul25 = vecTime[std::floor(vecTime.size()*25/100)];
    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];

    std::string sDelayTimeInfo = "dataCount: " + std::to_string(vecTime.size()) 
                        +  ", min=" + std::to_string(ulMin) + ", max=" + std::to_string(ulMax)
                        + ", 25%=" + std::to_string(ul25) + ", 50%=" + std::to_string(ul50) 
                        + ", 75%=" + std::to_string(ul75) + ", 90%=" + std::to_string(ul90)
                        + "\n";
    return sDelayTimeInfo;
}

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

inline std::string NanoToMicroString(unsigned long long ulNanosecs) {
    unsigned long long microSecs = ulNanosecs / 1000;

    time_t totalSecs = static_cast<time_t>(microSecs/1000000);
    int leftMicroSecs = static_cast<int>(microSecs%1000000);

    std::tm localTm = *std::localtime(&totalSecs);
    std::stringstream ss;
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", &localTm);

    ss << timeBuffer << "." << std::setw(6) << std::setfill('0') << leftMicroSecs;

    return ss.str();
}

inline std::string NanoToNanoString(unsigned long long ulNanosecs) {
    // unsigned long long microSecs = ulNanosecs / 1000;

    time_t totalSecs = static_cast<time_t>(ulNanosecs/NANO_PER_SECOND);
    unsigned long long leftMicroSecs = static_cast<int>(ulNanosecs%NANO_PER_SECOND);

    std::tm localTm = *std::localtime(&totalSecs);
    std::stringstream ss;
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", &localTm);

    ss << timeBuffer << "." << std::setw(9) << std::setfill('0') << leftMicroSecs;

    std::stringstream ss2;
    ss2 << std::setw(9) << std::setfill('0') << leftMicroSecs;

    std::string tmp = ss2.str();

    std::string rst = tmp.substr(0,3) + "," + tmp.substr(3,3) + "," + tmp.substr(6,3);


    return rst;
}

    