#include "test_double.h"
#include <stdio.h>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <string>
using std::cout;
using std::endl;

using boost::multiprecision::number;
using boost::multiprecision::cpp_dec_float;
// typedef number<cpp_dec_float<50> > cpp_dec_float_50;



using boost::multiprecision::cpp_dec_float_50;
typedef cpp_dec_float_50 DOUBLE;

typedef long long int64;

void test_double()
{
    printf("test double! \n");
    // cpp_dec_float_50 v1("5726.867366095");    // 为防止精度丢失，只能以字符串形式赋值，
    // cpp_dec_float_50 v2("5837.754018494999"); // 否则就是把错误的值赋给了对象，初值就错了
    // const int64 N = pow(10, 8);
    // double newV1 = (int64)round(v1 * N) / double(N);
    // double newV2 = (int64)round(v2 * N) / double(N);
    // printf("%.8f\n", newV1);
    // printf("%.8f\n", newV2);

    cpp_dec_float_50 amount_d = 100.00000;
    cpp_dec_float_50 price = 1248.97;

    cout << amount_d.convert_to<std::string>() << ", " << price.convert_to<std::string>() << endl;

    cpp_dec_float_50 volume = amount_d / price;



    printf("amount_d: %.8f, price: %.8f, volume: %.8f, new_price: %.8f \n", 
            amount_d, price, volume, amount_d/volume);
}

void TestNumbMain() {
    test_double();
}