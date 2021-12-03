#include "test_forward.h"
#include <utility>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class TestRe
{
    public:
        TestRe(int x): value_{x} 
        {
            cout << "construct" << endl;
        }

        TestRe(TestRe&& src)
        {
            cout << "copy construct" << endl;
        }        

        ~TestRe()
        {
            cout << "Deconstruct " << endl;
        }

    private:
        int  value_{0};
};


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
    // cout << "testForward(1);" << endl;
    // testForward(1);
    // cout << endl;

    int y = 1;
    TestRe x(y);

    // cout << "testForward(x);" << endl;
    // testForward(x);
    // cout << endl;

    cout << "testForward(std::forward<TestRe>(x));" << endl;
    testForward(std::forward<TestRe>(x)); 
    cout << endl;

    cout << "testForward(std::move(x));" << endl;
    testForward(std::move(x)); 
    cout << endl;    
}