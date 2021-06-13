#pragma once

#include "global_declare.h"

class A    //定义一个类A，类中有3个虚函数
{
public:
	int x;
	int y;
	virtual void f(){ cout << "A f() called ! " << std::this_thread::get_id() << endl; };
	virtual void f1(){ cout << "A f1() called !" << endl; };
	virtual void f2(){ cout << "A f2() called !" << endl; };

    virtual ~A()
    {
        cout << "~A" << endl;
    }    
};

class B:public A    //定义一个类A，类中有3个虚函数
{
public:
	int x;
	int y;
	virtual void f(){ cout << "B f() called ! " << std::this_thread::get_id() <<endl; };
	virtual void f1(){ cout << "B f1() called !" << endl; };
	virtual void f2(){ cout << "B f2() called !" << endl; };

    virtual ~B()
    {
        cout << "~B" << endl;
    }
};

class Server
{
public:

    void regiser_A(A* a) {
        a_ = a;
    }

    void start();

    void thread_main();

    ~Server()
    {
        if (thread_)
        {
            if (thread_->joinable())
            {
                thread_->join();
            }
        }
    }

    A* a_{nullptr};
    std::thread*         thread_{nullptr};
};


void test_vp_main();