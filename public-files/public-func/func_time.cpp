#include "func_time.h"
#include <ctime>
#include <stdio.h>
using std::time_t;
using std::tm;
using std::localtime;
using std::to_string;

int getCurrTimeSecs()
{
    time_t currTime = time(nullptr);
    tm * tmTime = localtime(&currTime);
    int secs = tmTime->tm_hour*60*60 + tmTime->tm_min*60 + tmTime->tm_sec;
    return secs;
}

string getCurrDateTimeStr()
{
    time_t currTime = time(nullptr);
    tm * tmTime = localtime(&currTime);
    string result = to_string(1900 + tmTime->tm_year) +  to_string(tmTime->tm_mon+1) + to_string(tmTime->tm_mday)
                  + " " + to_string(tmTime->tm_hour) + ":"  + to_string(tmTime->tm_min) + ":" + to_string(tmTime->tm_sec);
    return result;
}

long long getCurrDatetime()
{
    time_t currTime = time(nullptr);
    tm * tmTime = localtime(&currTime);
    long long result = (1900 + tmTime->tm_year)*10000000000 +  (tmTime->tm_mon)*100000000 + (tmTime->tm_mday)*1000000
                     + (tmTime->tm_hour)*10000 +  (tmTime->tm_min)*100 + (tmTime->tm_sec);
    return result;    
}

int getCurrDate()
{
    time_t currTime = time(nullptr);
    tm * tmTime = localtime(&currTime);
    int result = (1900 + tmTime->tm_year)*10000 +  (tmTime->tm_mon+1)*100 + tmTime->tm_mday;
    return result;
}

TestFuncTimeMain::TestFuncTimeMain()
{
    test_getCurrDateTimeStr();

    test_getCurrTimeSecs();
}

void TestFuncTimeMain::test_getCurrDateTimeStr()
{
    string currDatetimeStr = getCurrDateTimeStr();
    printf("currDatetimeStr: %s \n", currDatetimeStr.c_str());
}

void TestFuncTimeMain::test_getCurrTimeSecs()
{
    int datetimeSecs = getCurrTimeSecs();
    printf("datetimeSecs: %d \n", datetimeSecs);
}

void testFuncTimeMain()
{
    TestFuncTimeMain testObj;
}