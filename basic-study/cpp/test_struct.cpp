#include "test_struct.h"
#include <iostream>
#include<stdio.h>
using namespace std;
// #pragma pack(4)

class TestA {
    public:
    // virtual void print() {}
    int b;
    bool a;
    bool c;
    
}; // sizeof (TestA) == 3

struct A
{
	int a;
	union{
		short a;
		long b;
	}c;
};

struct B {
	char c;
    // int d;
    // char e;
    // char f;
	A a;
	int d;
};


void test_address() {
    B obj = B();

    printf("obj: %p, c: %p, a: %p, d: %p;\n", &obj, &obj.c, &obj.a, &obj.d);
}

void TestSizeof () {
    cout << "sizeof (B): " << sizeof(B) << endl;
    cout << "sizeof (A): " << sizeof(A) << endl;

    cout << "sizeof(long) " << sizeof (long) << endl;
    cout << "sizeof(short) " << sizeof (short) << endl;


}


void TestStructMain() {
    test_address();
    TestSizeof();
}