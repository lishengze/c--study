#include "testVolatile.h"
#include "print.h"

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