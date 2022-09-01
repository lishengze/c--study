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

void test_func1() {
    struct s1 { 
        int i1;
        long long l1;
        char c1;
        short int s1;
    };

    struct s2 { 
        int  i1;
        char c1;
        short int s1;
        long long l1;
    };
    s2 data;
    cout << sizeof(data) << endl;

    s1 data2;
    cout << sizeof(data2) << endl;

    cout << sizeof(short int) << endl;
}

template<class T>
T test_func2(T x, T y) {
    return x;
}

void test3() {
    int x=2;
    int y;

    switch(x){
        case 1:
            y=1;
        case 2:
            y=2;
        case 3:
            y=3;
            break;                        
    }
    cout << y << endl;
}

void test4() {
    char a[] = "It is mine";
    char *p = "It is mine";
    cout << *(a+1)<< endl;

    p ="It is mine aaaa";

    a[11] = 'a';
}

void TestStructMain() {
    test_address();
    TestSizeof();
}