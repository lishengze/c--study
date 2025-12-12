#pragma once

#include <stdint.h>
#include <stdio.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;


// struct Order1 {
//     char a1;
//     char a2[24];
//     char a3[15];
//     double a4;
//     char a5;
//     char a6;
//     int a7;
//     int a8;
//     char a9;
// };

// struct Order2 {
//     char a1;
//     char a2[24];
//     char a3[15];
//     double a4;
//     char a5;
//     char a6;
//     int a7;
//     int a8;
//     char a9;
// }__attribute__((packed));

// std::atomic<int> gY(0);
// std::atomic<int> gX(0);
// int gR1 = 0;
// int gR2 = 0;

// void thread_func1() {
//     gR1 = gY.load(std::memory_order_relaxed);
//     gX.store(gR1, std::memory_order_relaxed);
// }

// void thread_func2() {
//     gR2 = gX.load(std::memory_order_relaxed);
//     gY.store(42, std::memory_order_relaxed);
// }

// void test_thread_func() {
//     std::thread t1(thread_func1);
//     std::thread t2(thread_func2);
// }

// struct MyGurad {
//     MyGurad(std::recursive_mutex& lock): mLock(lock) {
//         mLock.lock();
//         cout << "lock" << endl;
        
//     }
//     ~MyGurad() {
//         mLock.unlock();
//         cout << "unlock" << endl;
//     }
// private:
//     std::recursive_mutex& mLock;
// };

// void f1(std::recursive_mutex& lock) {
//     MyGurad g(lock);
//     cout << "f1 run" << endl;
// }

// void f2(std::recursive_mutex& lock) {
//     MyGurad g(lock);
//     f1(lock);
//     cout << "f2 run" << endl;
// }

// int test() {
//     std::recursive_mutex lock;
//     f2(lock);
//     return 0;
// }

// void test2() {
//     fork() && fork() && fork() && sleep(10);
//     printf("hello world\n");
//     exit(0);
// }

// int test_main() {
//     test2();
//     return 0;
// }

/*

有这样的三个属性
a1: 取值为 0,1 ，只需要一个bit
a2: 取值为 0,1,2,3,只需要2bit
a3: 0-1000000; 只需要21bit

决定用一个 4B 的 int 来表示这三个属性

a1: 0-1, 占最低位 1bit
a2: 0-3, 占a1 旁边的 2bit
a3: 0-1000000, 占剩下的 29bit

完善下面赋值的接口代码:

int set_a1(int& dstValue, int a1) {
    dstValue = (dstValue & ~(0x1)) | (a1 & 0x1);
}

int set_a2(int& dstValue, int a2) {
    dstValue = (dstValue & ~(0x3 << 1)) | ((a2 & 0x3) << 1);
}

int set_a3(int& dstValue, int a3) {
    dstValue = (dstValue & ~(0x7FFFFFFF << 3)) | ((a3 & 0x7FFFFFFF) << 3);
}




*/