#pragma once

#include "global_declare.h"

class A    //定义一个类A，类中有3个虚函数
{
public:
	int x;
	int y;
	virtual void f(){ cout << "A f() called !" << endl; };
	virtual void f1(){ cout << "A f1() called !" << endl; };
	virtual void f2(){ cout << "A f2() called !" << endl; };
};

class B:public A    //定义一个类A，类中有3个虚函数
{
public:
	int x;
	int y;
	virtual void f(){ cout << "B f() called !" << endl; };
	virtual void f1(){ cout << "B f1() called !" << endl; };
	virtual void f2(){ cout << "B f2() called !" << endl; };
};


void test_vp_main();