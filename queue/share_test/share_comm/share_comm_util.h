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

#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <iostream>
#include <numa.h>


using std::string;
using std::cout;
using std::endl;
using std::fstream;
using std::map;
using std::set;
using std::vector;

// #include "json.hpp"

// using njson = nlohmann::json;

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

inline long SecTime() {
    std::chrono::high_resolution_clock::time_point curtime = std::chrono::high_resolution_clock().now();
    return std::chrono::duration_cast<std::chrono::seconds>(curtime.time_since_epoch()).count();
}


inline string ToSecondStr(long nano, string time_format="%Y-%m-%d") {
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

inline std::string SecTimeStr(std::string time_format="%Y-%m-%d") {
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

inline bool BindCpuID(int iCpuID, int iNumaNode, string strMetaInfo="") {

    pid_t tid = syscall(SYS_gettid);

    if (iNumaNode > 0) {
        // if (numa_available() < 0) {
        //     LOG_WARN(" System does not support numa!");
        // } else {
        //     int iTotalNodes = numa_num_configured_nodes();
        //     if (iNumaNode < 0 || iNumaNode >= iTotalNodes) {
        //         LOG_WARN(" iNumaNode is illegal, MaxNumaNode is:{}, TargetNumaNode:{} ", iTotalNodes, iNumaNode);
        //     } else {
        //         numa_set_preferred(iNumaNode);
        //     }

        // }
    }


    // numa_set_preferred(iNumaNode);

    // struct bitmask tmp;
	// tmp.maskp = (unsigned long *)buffer;
	// tmp.size = buffer_len * 8;
	// return numa_node_to_cpus(node, &tmp);

    // if (!numa_bitmask_isbitset(numa_node_to_cpus(iNumaNode), iCpuID)) {
    //     TEST_LOG_DETAIL(strMetaInfo + " Thread: " + std::to_string(tid) 
    //                     + ", Try Bind NumaNode: " + std::to_string(iNumaNode)
    //                     + ", Bind CPU: " + std::to_string(iCpuID)  + "Failed");
    // }

    // std::cout << "Thread: " << tid  << " Start Running, Try Bind: " << iCpuID << std::endl;
    // TEST_LOG_DETAIL(strMetaInfo + " Thread: " + std::to_string(tid) + ",  Try Bind CPU: " + std::to_string(iCpuID));

    if (iCpuID > 0) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(iCpuID, &mask);

        int ret = sched_setaffinity(tid, sizeof(mask), &mask);
        if (ret == -1) {
            LOG_WARN(" Bind CPU:{} to thread:{} Failed", iCpuID, tid);
            return false;
        }

        // TEST_LOG_WARN(strMetaInfo + " Bind CPU: " + std::to_string(iCpuID) + " to thread: " + std::to_string(tid) + " Sucess!");

        cpu_set_t get_mask;
        CPU_ZERO(&get_mask);
        sched_getaffinity(tid, sizeof(mask), &mask);
        for (int i = 0; i < CPU_SETSIZE; ++i) {
            if (CPU_ISSET(i, &get_mask)) {
                if (CPU_ISSET(i, &get_mask)) {
                    LOG_INFO(" Bind CPU:{} to thread:{} SUCCESS!", iCpuID, tid);
                    break;
                }
            }
        }
    }

    return true;
}

}