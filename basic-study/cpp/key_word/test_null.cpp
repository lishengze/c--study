#include "test_null.h"
#include "../global_def.h"


void func(int a) { 
    cout << "func_int: " << a << endl;
}

void func(char* b) {
    cout << "func_char*: " << b << endl;
}

void Test1() {
    // func(NULL); // call of overloaded ‘func(NULL)’ is ambiguous, 隐式的转换指针或整形;
    func(nullptr);

    // bool t = nullptr;
    // int  t1 = nullptr;

    std::nullptr_t ptr = nullptr;

    if (ptr) {
        cout << "ptr trans to bool " << endl;
    }
}

void TestKeyNULL() {
    Test1();
}