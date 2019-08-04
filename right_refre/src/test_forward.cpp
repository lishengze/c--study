#include "test_forward.h"
#include <utility>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;


void printT(int& t)
{
    cout << "lvalue" << endl;
}

template<typename T>
void printT(T&& t)
{
    cout << "rvalue" << endl;
}


template<typename T>
void testForward(T&& value)
{
    printT(value);
    printT(std::forward<T>(value));
    printT(std::move(value));
}

void TestForward()
{
    testForward(1);
    int x = 1;
    testForward(x);
    testForward(std::forward<int>(x)); // 问题？
}