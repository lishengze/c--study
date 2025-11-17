#include "time_part.h"
#include <chrono>
#include <iostream>

using namespace std;
/*
perf stat -e L1-dcache-loads,L1-dcache-load-misses L2-cache-loads,L2-cache-load-misses,L3-cache-loads,L3-cache-load-misses ./cache_test
perf stat -e L1-dcache-loads,L1-dcache-load-misses L2-cache-loads,L2-cache-load-misses,L3-cache-loads,L3-cache-load-misses ./cache_test

perf stat -e cache-misses,cache-references ./cache_test

perf stat -e L1-dcache-loads,L1-dcache-load-misses ./cache_test  > result.log

perf stat -e L1-dcache-loads,L1-dcache-load-misses ./cache_test 

valgrind --tool=cachegrind ./cache_test

perf record -e \
  L1-dcache-loads,L1-dcache-load-misses \
  -g  
  ./cache_test

*/

int gObjInf[800000] = {0};

void test_global_variable() {
    int i = 0;
    // unsigned long long start = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
    unsigned long long ulSum = 0;

    for (i = 0; i < 800000; i++) {
        gObjInf[i] = i;
    }

    // int* pILocal = gObjInf;
    // for (i = 0; i < 800000; i++) {
    //     pILocal[i] = i + 1;
    // }


}

void TestTimePart() {
    cout << "TestTimePart" << endl;

    test_global_variable();
}