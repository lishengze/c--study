#include "operator.h"
#include <set>
#include <iostream>


void test_operator()
{
    std::set<Atom> test_set;
    test_set.emplace(1, 2);
    test_set.emplace(1, 1);
    test_set.emplace(2, 4);
    test_set.emplace(2, 1);

    for (auto obj:test_set)
    {
        std::cout << "Price: " << obj.m_price << ", Time: " << obj.m_time << std::endl;
    }
}

void test_string_int()
{
    
}