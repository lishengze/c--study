#include "test_struct.h"
#include <iostream>
using namespace std;

class TestA {
    public:
    // virtual void print() {}
    int b;
    bool a;
    bool c;
    
}; // sizeof (TestA) == 3

void TestSizeof () {
    cout << "sizeof (TestA): " << sizeof(TestA) << endl;
}

void TestStructMain() {
    TestSizeof();
}