#pragma once

#include <iostream>
#include <coroutine>

using namespace std;

void TestCoro1();

inline void TestCoro() {
    TestCoro1();
}