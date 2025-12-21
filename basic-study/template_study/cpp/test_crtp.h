#include "global_declare.hpp"

template<typename T>
class Base
{
    public:
        void foo()
        {
            static_cast<T*>(this) -> inner_foo();
        }

        void bar()
        {
            static_cast<T*>(this)->inner_bar();
        }
};

class Derived1:public Base<Derived1>
{
    public:
        void inner_foo() 
        {
            cout << "Derived1 foo" << endl;
        }

        void inner_bar() 
        {
            cout << "Derived1 bar" << endl;
        }        
};

class Derived2:public Base<Derived2>
{
    public:
        void inner_foo() 
        {
            cout << "Derived2 foo" << endl;
        }

        void inner_bar() 
        {
            cout << "Derived2 bar" << endl;
        }        
};

// 父类，子类的引用可以实现隐式转换？
template<typename T>
void help_foo(Base<T>& obj) 
{
    obj.foo();
}

void TestCRTP();
