#pragma once
#include <vector>
#include <set>
#include <iostream>
#include <random>
#include <cassert>
#include <string>
#include <folly/FBVector.h>
#include <folly/small_vector.h>


struct A {
    std::string s;
    A() {
        std::cout << " construct defalut" << endl;
    }
    A(std::string str) : s(std::move(str))  { 
        std::cout << " constructed "<< s <<"\n"; 
        }
    A(const A& o) : s(o.s) { 
        std::cout << " copy constructed "<< o.s <<"\n"; 
        }
    A(A&& o) : s(std::move(o.s)) 
    { 
        std::cout << " move constructed -- " << s <<"\n"; 
    }
    A& operator=(const A& other) {
        s = other.s;
        std::cout << " copy assigned " << other.s << "\n";
        return *this;
    }
    A& operator=(A&& other) {
        s = std::move(other.s);
        std::cout << " move assigned == "<< other.s << "\n";
        return *this;
    }
    std::string get_value() {return s;}

    friend std::ostream & operator<<(std::ostream &cout, A &a) {
        std::cout << a.get_value() << std::endl;
    }

    ~A() {
        std::cout << "Destory: " << s << endl;
        }

};

inline A GetA(bool flag) {
    A a("a");
    A b("b");
    if (flag) {
        return a;
    } else {
        return b;
    }
}