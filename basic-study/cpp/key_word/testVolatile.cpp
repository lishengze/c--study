#include "testVolatile.h"
#include "../print.h"

// #pragma pack(1)

void testVolatile1() {
    int a = 5;
    int b = 10;
    b = a;
    cout << a << b << endl;
}

void testVolatile2() {
    volatile int a = 5;
    int b = 10;
    b = a;
    cout << a << b << endl;
}

class Base1
{
    public:
        short  a;
        double    b;
        char   c;

        virtual void test() {};
};

void testSizeof()
{


    Base1 base1Obj;

    int* pointer;
    cout << "pointer.size: " << sizeof(pointer) << endl;
    cout << "base1Obj.size: " << sizeof(base1Obj) << endl;
}