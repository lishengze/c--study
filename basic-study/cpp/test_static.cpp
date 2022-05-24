#include "test_static.h"
#include <iostream>
using namespace std;

int g_value;
static int gs_value;

class TestS {

public: 
    static int cs_value;
};

int TestS::cs_value;

void test_static_1() {
    static int ls_i = 0;
    cout << "test_static_1: " << &ls_i << ", " << ++ls_i << endl;
}

void test_static_2() {
    static int ls_i = 0;
    cout << "test_static_2: " << &ls_i << ", " << ++ls_i << endl;
}

void TestStaticMemory() {
    static int ls_value;

    cout << "g_value: " << &g_value << "\n"
         << "gs_value: " << &gs_value << "\n"
         << "TestS::cs_value: " << &TestS::cs_value << "\n"
         << "ls_value: " << &ls_value << "\n"
         << endl;

    test_static_1();
    test_static_2();
}



void TestStaticMain()
{
    TestStaticMemory();
}