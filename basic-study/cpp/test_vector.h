#pragma

#include <vector>
#include <set>
#include <iostream>
#include <random>
#include <cassert>
#include <string>
#include <folly/FBVector.h>
#include <folly/small_vector.h>
#include "data_struct.h"

// using namespace std;

void vec_basic_test()
{
    std::vector<std::string> empty_vec;
    cout << "empty_vec: " << sizeof(empty_vec) << endl;
}

void test_multi_vector()
{
    int first_dimension = 5;
    int second_dimension = 10;

    std::vector<std::vector<std::set<int>>> data;
    std::vector<std::set<int>> empty_vs;
    std::set<int> empyt_si;

    // std::string data;

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

    std::uniform_int_distribution<unsigned> u(0,test_count);
    std::default_random_engine e;    // 生成无符号随机整数

    for (int i = 0; i < test_count; ++i)
    {
        data[test_first_dimension][test_seconde_dimension].emplace(u(e));
    }


    for (auto value : data[test_first_dimension][test_seconde_dimension])
    {
        cout << value << endl;
    }

}

struct President
{
    std::string name;
    std::string country;
    int year;
 
    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed.\n";
    }
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved.\n";
    }
    President& operator=(const President& other) = default;
};
 
void test_emplace_back()
{
    std::vector<President> elections;
    std::cout << "emplace_back:\n";
    auto& ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);

    assert(ref.year == 1994 && "uses a reference to the created object (C++17)");
 
    std::vector<President> reElections;
    std::cout << "\npush_back:\n";
    reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));
 
    std::cout << "\nContents:\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
    for (President const& president: reElections) {
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
}

void TestEmplace()
{
    std::vector<A> container;
    // reserve enough place so vector does not have to resize
    container.reserve(10);
    std::cout << "construct 2 times A:\n";
    A two { "two" };
    A three { "three" };
 
    std::cout << "emplace:\n";
    container.emplace(container.end(), "one");
 
    std::cout << "emplace with A&:\n";
    container.emplace(container.end(), two);
 
    std::cout << "emplace with A&&:\n";
    container.emplace(container.end(), std::move(three));

    std::cout << "emplace_back with para Four!\n";
    container.emplace_back("Four");

    std::cout << "emplace_back with two!\n";
    container.emplace_back(two);    

    // std::cout << "emplace_back with para Four!\n";
    // container.push_back("Four");

    std::cout << "push_back with A& \n";
    container.push_back(two);

    std::cout << "push_back with A&& \n";
    container.push_back(std::move(three));

    std::cout << "Test erase 0 start!" << std::endl;

    container.erase(container.begin());

    std::cout << "sleep start!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));


    std::cout << "\nTest Clear ------" << std::endl;
    container.clear();
    std::cout << "sleep start!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
          
 
    // std::cout << "content:\n";
    // for (const auto& obj : container)
    //     std::cout << ' ' << obj.s;
    // std::cout << '\n';
}

template<class Vector>
void TestEmplaceAll(std::string info)
{
    cout << info << endl;
    Vector container;
    // reserve enough place so vector does not have to resize
    // container.reserve(10);
    // std::cout << "construct 2 times A:\n";
    A two { "two" };
    A three { "three" };
 
    std::cout << "emplace:\n";
    container.emplace(container.end(), "one");
 
    std::cout << "emplace with A&:\n";
    container.emplace(container.end(), two);
 
    std::cout << "emplace with A&&:\n";
    container.emplace(container.end(), std::move(three));

    std::cout << "emplace_back with para Four!\n";
    container.emplace_back("Four");

    std::cout << "emplace_back with two!\n";
    container.emplace_back(two);    

    // std::cout << "emplace_back with para Four!\n";
    // container.push_back("Four");

    std::cout << "push_back with A& \n";
    container.push_back(two);

    std::cout << "push_back with A&& \n";
    container.push_back(std::move(three));

    std::cout << "Test erase 0 start!" << std::endl;

    container.erase(container.begin());

    std::cout << "sleep start!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));


    std::cout << "\nTest Clear ------" << std::endl;
    container.clear();
    std::cout << "sleep start!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
          
 
    // std::cout << "content:\n";
    // for (const auto& obj : container)
    //     std::cout << ' ' << obj.s;
    // std::cout << '\n';
}

template<class Vector>
void append_vector(Vector& dst, Vector& src) {
    for (auto& i:src) {
        // dst.push_back(i);
        // std::cout << "emplace back " << i.get_value() << ": ";
        dst.emplace_back(i);
    }
}


// class TestClass
// {
//     public:
        
//         int id;
// };

template<class vector_type>
void test_vector_allocate(std::string info) {
    cout << info << endl;
    int cout = 20;

    std::cout << "-------- Init Start!-----" << std::endl;
    vector_type a = {A{"a"}, A{"b"}};
    vector_type b = {A{"c"}, A{"d"}, A{"e"}, A{"f"}};

    std::cout << "-------- Init Over!-----\n" << std::endl;

    // std::vector<char> a = {'a', 'b', 'c'};
    // std::vector<char> b = {'a', 'b', 'c'};    

    int last_cap = 0;

    // std::cout << reinterpret_cast<void*>(const_cast<A *>(&(a[0]))) 
    //             << ", size: " << a.size() << " cap: " << a.capacity() << endl;

    for (int i =0; i < cout; ++i) {
        if (a.capacity() != last_cap) {
            std::cout << reinterpret_cast<void*>(const_cast<A *>(&(a[0]))) 
                      << ", size: " << a.size() << " cap: " << a.capacity();

            if (last_cap == 0) {
                std::cout << ", init size: " << a.capacity();
            } else {
                std::cout << ", rate: " << float(a.capacity()) / last_cap;
            }
            last_cap = a.capacity();
            std::cout << std::endl;
        }    
        

        // std::cout << "\n*******  Append Start ******" << std::endl;
        append_vector<vector_type>(a, b);
        //  std::cout << "******* Append Over ****** \n" << std::endl;
                //   << ", " << a << std::endl;
    }
    std::cout << std::endl;
}

void TestVectorAllocate() {
	// cout << "Test Std::String " << endl;
    test_vector_allocate<std::vector<A>>("\n------------ Test Std::Vector");
    // test_vector_allocate<folly::fbvector<A>>("Test Fb::fbvector");
    test_vector_allocate<folly::small_vector<A>>("\n------------ Test Fb::small_vector");
}

void test_emplace_main() {
    TestEmplaceAll<std::vector<A>>("\n----- Emplace Std::vector -------");
    TestEmplaceAll<folly::small_vector<A>>("\n----- Emplace Folly::small_vector -------");
}

void TestVectorMain()
{
    // test_string();

    // vec_basic_test();

    // test_multi_vector();

    // test_emplace_back();

    test_emplace_main();

    // TestVectorAllocate();
}

