#include "test_func.h"
#include <iostream>
using namespace std;

class Base 
{
    public:
        void Print1() {
            cout << "Base Print1" << endl;
        }

        void Print2(int a) { 
            cout << "Base Print2 " << endl;
        }

};

class Child:public Base 
{
    public:
        void Print1() {
            cout << "Child Print1" << endl;
        }

        void Print2(bool a) { 
            cout << "Child Print2 " << a << endl;
        }
};

void TestRewrite() {
    Child c1;
    c1.Print1();
    c1.Print2(1);
    c1.Print2(false);
}

void TestFuncMain() {
    TestRewrite();
}