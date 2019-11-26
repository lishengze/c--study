#include "time_func.h"
#include <stdio.h>
#include <signal.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include "time_util.h"

using std::cout;
using std::endl;
using std::vector;

void alarm_timer(int sig)
{
    // if(SIGALRM == sig)
    // {
    //         printf("timer\n");
    //         alarm(1);       //重新继续定时1s
    // }

    return ;
}

void test_alarm() 
{
    // printf("Test_Alarm Start!");
    // signal(SIGALRM, alarm_timer);
    // alarm(1);       //触发定时器

    // getchar();
    
    printf("Test_Alarm End!");
}

void get_curtime()
{
    int testNumb = 100;
    long delta_max = -10000000;
    long delta_min = 10000000;
    double delta_mean = 0;
    for (int i=0; i < testNumb; ++i)
    {
        std::chrono::high_resolution_clock::time_point curtime = std::chrono::high_resolution_clock().now();
        long orin_nanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(curtime.time_since_epoch()).count();

        long joint_nanosecs = NanoTimer::getInstance()->getNano();
        double delta_millisecs = (orin_nanosecs - joint_nanosecs) / 1000;
        cout << "origin nanosecs: " << orin_nanosecs << ", "
            << "joint nanosecs: " << joint_nanosecs << ", "
            << "delta microsecs: " << delta_millisecs << endl;

        delta_max = delta_max < delta_millisecs ? delta_millisecs : delta_max;
        delta_min = delta_min > delta_millisecs ? delta_millisecs : delta_min;
        delta_mean += delta_millisecs;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    cout << "delta_max: " << delta_max << ", "
         << "delta_min: " << delta_min << ", "
         << "delta_ave: " << delta_mean / testNumb << endl;


    // std::time_t now_c = std::chrono::high_resolution_clock::to_time_t(curtime);
    // std::cout << "time is: "
    //           << std::put_time(std::localtime(&now_c), "%F %T") << '\n';         
}

void test_nano_timer()
{
    cout <<"GetNano: " << NanoTimer::getInstance()->getNano() << endl;
}

void test_main()
{
    get_curtime();
    // test_nano_timer();
    // test_alarm();
}