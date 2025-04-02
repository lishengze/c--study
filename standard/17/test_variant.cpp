#include "test_variant.h"
#include <iostream>
#include <variant>
#include <string>   
using namespace std;

struct MyData
{
    int iValue_;
    string sValue_;
    /* data */
};


void test_var1() {
    std::variant<MyData,int, double, std::string> var;
    var = 10;
    // std::visit([&](auto&& arg) { std::cout << arg << std::endl; }, var); // output: 10
    cout << var.index() << endl; // output: 0
    var = 3.14;
    // std::visit([&](auto&& arg) { std::cout << arg << std::endl; }, var); // output: 3.14
    cout << var.index() << endl; // output: 1
    var = "hello world";
    // std::visit([&](auto&& arg) { std::cout << arg << std::endl; }, var); // output: hello world
    cout << var.index() << endl; // output: 2

    MyData mydata;
    mydata.iValue_ = 100;
    mydata.sValue_ = "mydata";
    var = mydata;
    // std::visit([&](auto&& arg) {  }, var); // output: 100 mydata
    cout << var.index() << endl; // output: 3
}

void TestVariant()
{
    test_var1();
}