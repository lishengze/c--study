#pragma once
#include "global_declare.h"
#include <benchmark/benchmark.h>

#define MAX_STRING_LEN 1024

class BaseClass {
    
private:
    int a_;
    double b_;
    char d1_[MAX_STRING_LEN];
    char d2_[MAX_STRING_LEN];
    char d3_[MAX_STRING_LEN];
    char d4_[MAX_STRING_LEN];
    char d5_[MAX_STRING_LEN];
    char d6_[MAX_STRING_LEN];
    char d7_[MAX_STRING_LEN];
    char d8_[MAX_STRING_LEN];

public:
    void SetA(int a) { a_ = a;}
    void SetB(double b) { b_ = b;}
    void SetD(char* src, int len) { memcpy(d1_, src, len >MAX_STRING_LEN?MAX_STRING_LEN:len);}

    int GetA() {return a_;}
};

class MemberClass{
public:
    BaseClass   base_obj_;
};

class InheritClass: public BaseClass{

};


void TestMemberObj(benchmark::State& state);

BENCHMARK(TestMemberObj)->Arg(1000);

void TestInheritClass(benchmark::State& state);

BENCHMARK(TestInheritClass)->Arg(1000);
