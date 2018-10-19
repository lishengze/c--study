#include "time_func.h"
#include <stdio.h>
#include <signal.h>

void alarm_timer(int sig)
{
    if(SIGALRM == sig)
    {
            printf("timer\n");
            alarm(1);       //重新继续定时1s
    }

    return ;
}

void test_alarm() 
{
    printf("Test_Alarm Start!");
    signal(SIGALRM, alarm_timer);
    alarm(1);       //触发定时器

    getchar();
    
    printf("Test_Alarm End!");
}

void test_main()
{
    test_alarm();
}