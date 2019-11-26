#include "test_func.h"
#include "test_static.h"
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

void test_static_func() 
{
    cout <<"Ori m_value: " << TestStaticClassFunc::Instance().getValue() << endl;

    TestStaticClassFunc::Instance().addValue(10);

    cout << "After add 10, m_value: "  << TestStaticClassFunc::Instance().getValue() << endl;
}

void test_main() 
{
    test_static_func();
}