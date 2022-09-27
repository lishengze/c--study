// #include "test_vector.h"
// #include <vector>
// #include <set>
// #include <iostream>
// #include <random>
// #include <cassert>

// using namespace std;

// void vec_basic_test()
// {
//     std::vector<string> empty_vec;
//     cout << "empty_vec: " << sizeof(empty_vec) << endl;
// }

// void test_multi_vector()
// {
//     int first_dimension = 5;
//     int second_dimension = 10;

//     vector<vector<set<int>>> data;
//     vector<set<int>> empty_vs;
//     set<int> empyt_si;

//     // std::string data;

//     data.reserve(first_dimension);
//     empty_vs.reserve(second_dimension);

//     // data.resize(first_dimension);
//     // empty_vs.resize(second_dimension);    

//     cout << "data.size: " << data.size() << ", empty_vs.size: " << empty_vs.size() <<endl;

//     for (int i = 0; i < second_dimension; ++i)
//     {
//         empty_vs.emplace_back(empyt_si);
//     }

//     for (int i = 0; i < first_dimension; ++i)
//     {
//         data.emplace_back(empty_vs);
//     }    


//     int test_first_dimension = 1;
//     int test_seconde_dimension = 3;
//     int test_count = 10;

//     uniform_int_distribution<unsigned> u(0,test_count);
//     default_random_engine e;    // 生成无符号随机整数

//     for (int i = 0; i < test_count; ++i)
//     {
//         data[test_first_dimension][test_seconde_dimension].emplace(u(e));
//     }


//     for (auto value : data[test_first_dimension][test_seconde_dimension])
//     {
//         cout << value << endl;
//     }

// }

// struct President
// {
//     std::string name;
//     std::string country;
//     int year;
 
//     President(std::string p_name, std::string p_country, int p_year)
//         : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
//     {
//         std::cout << "I am being constructed.\n";
//     }
//     President(President&& other)
//         : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
//     {
//         std::cout << "I am being moved.\n";
//     }
//     President& operator=(const President& other) = default;
// };
 
// void test_emplace_back()
// {
//     std::vector<President> elections;
//     std::cout << "emplace_back:\n";
//     auto& ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);

//     assert(ref.year == 1994 && "uses a reference to the created object (C++17)");
 
//     std::vector<President> reElections;
//     std::cout << "\npush_back:\n";
//     reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));
 
//     std::cout << "\nContents:\n";
//     for (President const& president: elections) {
//         std::cout << president.name << " was elected president of "
//                   << president.country << " in " << president.year << ".\n";
//     }
//     for (President const& president: reElections) {
//         std::cout << president.name << " was re-elected president of "
//                   << president.country << " in " << president.year << ".\n";
//     }
// }

// struct A {
//     std::string s;
//     A(std::string str) : s(std::move(str))  { std::cout << " constructed\n"; }
//     A(const A& o) : s(o.s) { std::cout << " copy constructed\n"; }
//     A(A&& o) : s(std::move(o.s)) { std::cout << " move constructed\n"; }
//     A& operator=(const A& other) {
//         s = other.s;
//         std::cout << " copy assigned\n";
//         return *this;
//     }
//     A& operator=(A&& other) {
//         s = std::move(other.s);
//         std::cout << " move assigned\n";
//         return *this;
//     }
// };
 
// void TestEmplace()
// {
//     std::vector<A> container;
//     // reserve enough place so vector does not have to resize
//     container.reserve(10);
//     std::cout << "construct 2 times A:\n";
//     A two { "two" };
//     A three { "three" };
 
//     std::cout << "emplace:\n";
//     container.emplace(container.end(), "one");
 
//     std::cout << "emplace with A&:\n";
//     container.emplace(container.end(), two);
 
//     std::cout << "emplace with A&&:\n";
//     container.emplace(container.end(), std::move(three));

//     std::cout << "push_back with A& \n";
//     container.push_back(two);

//     std::cout << "push_back with A&& \n";
//     container.push_back(std::move(three));
          
 
//     // std::cout << "content:\n";
//     // for (const auto& obj : container)
//     //     std::cout << ' ' << obj.s;
//     // std::cout << '\n';
// }

// void TestVectorMain()
// {
//     // test_string();

//     // vec_basic_test();

//     // test_multi_vector();

//     // test_emplace_back();

//     TestEmplace();
// }