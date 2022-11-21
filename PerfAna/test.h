#pragma once
#include "global_declare.h"
#include <benchmark/benchmark.h>

#define MAX_STRING_LEN 1024

class BaseClass {
    
private:
    unsigned long a_;
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
    BaseClass():a_{0} {

    }
    
    void SetA(int a) { a_ = a;}
    void SetB(double b) { b_ = b;}
    void SetD(char* src, int len) { memcpy(d1_, src, len >MAX_STRING_LEN?MAX_STRING_LEN:len);}
    void UpdateA() {a_++;}

    int GetA() {return a_;}
};

class MemberClass{
public:
    BaseClass   base_obj_;
};

class InheritClass: public BaseClass{

};


void TestMember(benchmark::State& state);

BENCHMARK(TestMember);

void TestInherit(benchmark::State& state);

BENCHMARK(TestInherit);


void TestMemberPointer(benchmark::State& state);

BENCHMARK(TestMemberPointer);

void TestInheritPointer(benchmark::State& state);

BENCHMARK(TestInheritPointer);