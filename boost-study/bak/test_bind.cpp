#include "test_bind.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>
using std::cout;
using std::endl;




class Add
{
public:
    // typedef int result_type;
    int operator()(int x, int y)
    {
        cout << "x: " << x << ", y: " << y << endl;
        return x + y;
    }

    int add(int x, int y)
    {
        cout << "x: " << x << ", y: " << y << endl;
        return x + y;
    }
};

class Number
{
public:
    Number() {cout << "contructor" << endl;}
    Number(int value): value_(value) { cout << "constructor: " << value_ << endl;}
    Number(Number& number) { value_ = number.get_value(); }

    int get_value(){return value_;}

private:
    int value_;
};

int add(int x, int y)
{
    cout << "x: " << x << ", y: " << y << endl;
    return x + y;
}

int add_class(Number x, Number y)
{
    cout << "x: " << x.get_value() << ", y: " << y.get_value() << endl;
    return x.get_value() + y.get_value();
}


void basic_test()
{
    Add add_obj;
    boost::function<int(int, int)> add_basic_func = boost::bind(add, _1, _2);

    auto add_class_func = boost::bind(&Add::add, &add_obj, _1, _2);

    auto add_func_obj = boost::bind<int>(Add(), _2, _1);

    cout << add_basic_func(1,2) << endl;
    cout << add_class_func(1,2) << endl;
    cout << add_func_obj(1,2) << endl;


}

void more_test()
{
    //  如何在绑定时，函数参数设置为拷贝；
    Number x(10);
    Number y(20);
    auto test1 = boost::bind(add_class, boost::ref(x), boost::ref(y));
    cout << test1() << endl;

    // auto test2 = boost::bind(add_class, x, y);
    // cout << test2() << endl;

    auto test3 = boost::bind(add_class, _1, _2);
    cout << test3(x, y) << endl;
}

class DemoClass
{
private:
    typedef boost::function<void(int)> func_t;
    func_t func_;
    int n_;

public:
    DemoClass(int n): n_(n){}

    template<typename CallbackFunc>
    void accept(CallbackFunc func) { func_ = func; }

    void run() { func_(n_);}

};

class CallbackObj
{
public:
    CallbackObj(int n):n_(n) {}

    void operator()(int value) 
    {
        n_ += value;
        cout << "n: " << n_ << endl;
    }

    void call_back_func(int value)
    {
        n_ += value;
        cout << "n: " << n_ << endl;        
    }

private:
    int n_;

};

void test_function()
{
    DemoClass demo_obj(2);
    CallbackObj callback_obj(10);

    auto func = boost::bind<void>(CallbackObj(2), _1);

    // demo_obj.accept(boost::ref(callback_obj));

    demo_obj.accept(boost::bind(&CallbackObj::call_back_func, &callback_obj, _1));

    demo_obj.accept(boost::bind<void>(callback_obj, _1));

    demo_obj.run();

    demo_obj.run();
}

void TestBind()
{
    // basic_test();

    // more_test();

    test_function();
}