#include "test_vector.h"
#include <vector>
#include <set>
#include <iostream>
#include <random>

using namespace std;


void test_multi_vector()
{
    int first_dimension = 5;
    int second_dimension = 10;

    vector<vector<set<int>>> data;
    vector<set<int>> empty_vs;
    set<int> empyt_si;

    data.reserve(first_dimension);
    empty_vs.reserve(second_dimension);

    // data.resize(first_dimension);
    // empty_vs.resize(second_dimension);    

    cout << "data.size: " << data.size() << ", empty_vs.size: " << empty_vs.size() <<endl;

    for (int i = 0; i < second_dimension; ++i)
    {
        empty_vs.emplace_back(empyt_si);
    }

    for (int i = 0; i < first_dimension; ++i)
    {
        data.emplace_back(empty_vs);
    }    


    int test_first_dimension = 1;
    int test_seconde_dimension = 3;
    int test_count = 10;

    uniform_int_distribution<unsigned> u(0,test_count);
    default_random_engine e;    // 生成无符号随机整数

    for (int i = 0; i < test_count; ++i)
    {
        data[test_first_dimension][test_seconde_dimension].emplace(u(e));
    }


    for (auto value : data[test_first_dimension][test_seconde_dimension])
    {
        cout << value << endl;
    }

}

void test_vector()
{
    test_multi_vector();
}