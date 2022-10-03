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

void TestUnion() {
    cout << "----- TestUnion -----" << endl;
    test_rewrite();
}