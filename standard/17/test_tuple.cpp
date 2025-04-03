#include "test_tuple.h"

#include <tuple>
#include <iostream>
using namespace std;

void test_tuple1() {
    std::tuple t1(1, 2.5, 'a');

    cout << std::get<0>(t1) << endl;
    cout << std::get<1>(t1) << endl;
    cout << std::get<2>(t1) << endl;

    auto [a, b, c] = t1; // 结构化绑定
    cout << a << endl;
    cout << b << endl;    
    cout << c << endl;

    std::tuple t2(3, 4.5, 'b');
    auto t3 = std::tuple_cat(t1, t2);
    cout << std::get<0>(t3) << endl;
    cout << std::get<1>(t3) << endl;
    cout << std::get<2>(t3) << endl;
    cout << std::get<3>(t3) << endl;
    cout << std::get<4>(t3) << endl;
    cout << std::get<5>(t3) << endl;
}

void test_tuple2() {
}

void TestTuple() {
    test_tuple1();
}