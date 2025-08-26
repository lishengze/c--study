#pragma once


template<typename T>
class Number {
    long n;
    public:
        Number(long n) : n(n) {}
        Number operator+(const Number& other) const {
            return Number(n + other.n);
        }
};

void test_2025_8_23();