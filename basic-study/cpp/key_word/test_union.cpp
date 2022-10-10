#include "test_union.h"
#include "../global_def.h"


void test_rewrite() {
    union TestUO {
        int a;
        double b;
    };
    TestUO test_uo;
    test_uo.a = 1;
    cout << "test_uo.a : " << test_uo.a << endl;

    test_uo.b = 10.1;
    cout << "test_uo.b : " << test_uo.b << endl;
    cout << "test_uo.a : " << test_uo.a << endl;
}


void TestLittleBig() {
    union TestUO {
        unsigned char a;
        unsigned long b;
    };

    TestUO test_uo;
    test_uo.b = 0x12345678;

    cout << "test_uo.a : " << int(test_uo.a) << endl;

    if (test_uo.a == 0x12) {
        cout << "System is Big!" << endl;
    } else if (test_uo.a == 0x78) {
        cout << "System is Little!" << endl;
    } else {
        cout << "Something is wrong!" << endl;
    }
}

void TestUnion() {
    cout << "----- TestUnion -----" << endl;
    // test_rewrite();

    TestLittleBig();
}