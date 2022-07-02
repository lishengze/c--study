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
	cout << "<L> " << t << endl;
    PrintTest(std::forward<T>(t));
    PrintTest(t);
}

template <class T>
void Print(T&& t)
{
	cout << "<R> " << t << endl;
    PrintTest(std::forward<T>(t));
    PrintTest(t);    
}


//既可以对左值引用，也可以对右值引用。但要注意，引用以后，这个t值它本质上是一个左值
template <class T>
void func( T&& t)
{
    cout << "[R] " << t << endl; 
	Print(t);//一定是左值，因为t此时已经是一个具名的变量
	Print(std::move(t));//move(t)是右值
	Print(std::forward<T>(t));//右值引用 forward(t)按照参数原来的类型转发
}

template <class T>
void func( T& t)
{
    cout << "[R] " << t << endl; 
	Print(t);//一定是左值，因为t此时已经是一个具名的变量
	Print(std::move(t));//move(t)是右值
	Print(std::forward<T>(t));//右值引用 forward(t)按照参数原来的类型转发
}

// template <class T>
// void func(T& t)
// {
// 	cout << "[L] " << t << endl;
// 	Print(t);//一定是左值，因为t此时已经是一个具名的变量
// 	Print(std::move(t));//move(t)是右值
// 	Print(std::forward<T>(t));//forward(t)按照参数原来的类型转发    
// }

void func2(int&& i) {
    cout << "Raw Right Ref: " << i << endl;
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
	// func(std::forward<int>(y)); // 对于确定的左值或其引用 -- 返回的是右值

    int const && a = 10;
    // a  = 100;
	return 0;
}

int TestRightSimple() {
    int i = 0;
    int&& a = 10;

    func2(100);

    // func2(a);

    func2(std::move(i));
    func2(std::forward<int>(i));
}

void test_move_inner() {
    int a = 10;
    int&& b = std::move(a);
    cout << "a: " << a << endl;
    cout << "b: " << b << endl;

    std::string s_a = "Hello";
    std::string&& s_b = std::move(s_a);
    cout << "s_a: " << s_a << endl;

    s_a = "aaa";
    cout << "s_a: " << s_a << endl;
    cout << "s_b: " << s_b << endl;

    std::unique_ptr<std::string> s_c = std::make_unique<std::string>("ccccc");

    cout << "s_c: " << *s_c << endl;

    std::unique_ptr<std::string> s_c_2 = std::move(s_c);
    cout << "s_c_2: " << *s_c_2 << endl;
    // cout << "s_c: " << *s_c << endl;

    std::string* ss_a = new string("ssss");
    cout << "ss_a: " << *ss_a << endl;
    std::string* && ss_b = std::move(ss_a);
    cout << "ss_b: " << *ss_b <<endl;
    *ss_a = "tttt";
    cout << "ss_a: " << *ss_a << endl;
    

}

void TestRightMain() {
    // TestRawRightValue();

    // BasicTest();

    // TestRight();

    // TestRightSimple();

    test_move_inner();
}