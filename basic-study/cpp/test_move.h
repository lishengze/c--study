#pragma once

#include "data_struct.h"
#include <iostream>
using namespace std;

void TestRawRightValue() {
    A tmp = GetA(false);
}

template<class T>
void TestRef(T& a) {
    std::cout << "L: " << a << std::endl;
}

template<class T>
void TestRef(T&& a) {
    std::cout << "R: " << a << std::endl;
}

void BasicTest() {
    int&& a = 10;
    TestRef(1);
    TestRef(a); // 单独使用 a 已经具名了，成了左值;
    TestRef(std::forward<int>(a));

    A b = A("base");
    A&& tmp = std::move(b);
    tmp.get_value();

    // TestRef(tmp); // 单独使用 a 已经具名了，成了左值;
    // TestRef(std::forward<A>(tmp));    
    // auto b = a;
    // TestRef(b);
}


#include <iostream>
using namespace std;

template <class T>
void PrintTest(T& t)
{
	cout << "Test L" << t << endl;
}

template <class T>
void PrintTest(T&& t)
{
	cout << "Test R" << t << endl;
}


template <class T>
void Print(T& t)
{
	cout << "L" << t << endl;
    PrintTest(std::forward<T>(t));
}

template <class T>
void Print(T&& t)
{
	cout << "R" << t << endl;
}


//既可以对左值引用，也可以对右值引用。但要注意，引用以后，这个t值它本质上是一个左值
template <class T>
void func(T&& t)
{
	Print(t);//一定是左值，因为t此时已经是一个具名的变量
	Print(std::move(t));//move(t)是右值
	Print(std::forward<T>(t));//forward(t)按照参数原来的类型转发
}

int TestRight()
{
	cout << "-- func(1)" << endl;
	func(1);//右值
	int x = 10;
	int y = 20;
	cout << "-- func(x)" << endl;
	func(x); // x本身是左值
	cout << "-- func(std::forward<int>(y))" << endl;
	func(std::forward<int>(y)); //
	return 0;
}


void TestRightMain() {
    // TestRawRightValue();

    // BasicTest();

    TestRight();
}