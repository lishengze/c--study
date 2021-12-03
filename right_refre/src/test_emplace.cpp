#include "test_emplace.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
using std::string;
using std::map;
using std::pair;
using std::vector;
using std::make_pair;

using std::cout;
using std::endl;

class Year
{
    public:
        Year(){
            cout << "Year default" << endl;
        }

        Year(int year):year_{year}{
            cout << "Year Cons" << endl;
        }

        Year(Year& other): year_{other.year_}
        {
            cout << "Year Copy" << endl;
        }

        Year(const Year&& other): year_{std::move(other.year_)}
        {
            cout << "Year Move" << endl;
        }

        int year_;
};

class Complated
{
    public:
    Year            m_year;
    double          m_version;
    string          m_name;

    Complated()
    {
        cout << "Empty Constructor" << endl;
    }

    Complated(int year, double version, string name):
        m_year(year), m_version(version), m_name(name)
    {        
        cout << "Complated Cons" << endl;
    }

    Complated(Complated& other):
        m_year(other.m_year), m_version(other.m_version), m_name(other.m_name)
    {
        cout << "Complated Copy" << endl;
    }

    Complated& operator = (const Complated& other)
    {
        cout << "operator = " << endl;
        return *this;
    }


    // Complated(const Complated& other):
    //     m_year(other.m_year), m_version(other.m_version), m_name(other.m_name)
    // {
    //     cout << "Copy Contructor" << endl;
    // }

    Complated(const Complated&& other):
        m_year{std::move(other.m_year)}, m_version{std::move(other.m_version)}, m_name(other.m_name)
    {
        // cout << "\nori year address: " << &(other.m_year) << ", moved year address: " << &m_year << endl;
        // cout << "ori version address: " << &(other.m_version) << ", moved version address: " << &m_version << endl;
        // cout << "ori name address: " << &(other.m_name) << ", moved name address: " << &m_name << endl;
        
        cout << "Complated Move " << endl;
    }    

};

void TestEmplace()
{
    map<int, Complated> mapValue;
    int year = 10;
    double version = 1.0;
    string name = "Wps";

    cout << "-- Map Insert --" << endl;
    mapValue.insert(make_pair(1, Complated(year, version, name)));
    cout << endl;

    cout << "-- Map[1] -- " << endl;
    Complated tmp(year, version, name);
    mapValue[1] = tmp;

    cout << "-- Map Emplace --" << endl;
    mapValue.emplace(2, Complated(year, version, name));
    cout << endl;

    cout << "-- Vec Emplace_Back --" << endl;
    vector<Complated>  vec_value;
    // vec_value.emplace_back(year, version, name);
    vec_value.emplace_back(Complated(year, version, name));
    cout << endl;

    cout << "-- Vec Push Back --" << endl;
    vec_value.push_back(Complated(year, version, name));
}

void test_stack_move()
{
    int year = 2021;
    map<int, Complated> test_map;
    {
        Complated tmp(year, 1.1, "TOM");
        test_map.emplace(year, std::move(tmp));

        cout << "Tmp Address Info: " << endl;
        cout << "year: " << &(tmp.m_year.year_) << endl;
        cout << "version: " << &(tmp.m_version) << endl;
        cout << "name: " << &(tmp.m_name) << endl;

    }

    cout << "Moved Address Info: " << endl;
    cout << "year: " << &(test_map[year].m_year.year_) << endl;
    cout << "version: " << &(test_map[year].m_version) << endl;
    cout << "name: " << &(test_map[year].m_name) << endl;
}

void test_ori_move()
{
    int a = 10;
    int b{std::move(a)};
    cout << "a: " << a << ", b: " << b << endl;

    a = 11;
    cout << "a: " << a << ", b: " << b << endl;
}

void test_string_move()
{
    string a = "this is a";
    cout << "a.address: " << (long)(a.data()) << endl;

    string b{std::move(a)};
    cout << "b.address: " << (long)(b.data()) << endl;

    cout << "a: " << a << ", b: " << b << endl;
    
}

void test_vector_move()
{
    cout << "Init " << endl;
    vector<Complated>  vec_value;
    for (int i =0; i < 3; ++i)
    {
        vec_value.emplace_back(Complated(i, i*1.1, std::to_string(i)));
    }

    cout << "\nTest Vec Move" << endl;
    vector<Complated>  moved_vec{std::move(vec_value)};

    cout << "vec_value.size: " << vec_value.size() <<", moved_vec.size: " << moved_vec.size() << endl;
}

void TestMoveMain()
{
    // TestEmplace();

    // test_stack_move();

    // test_ori_move();

    // test_string_move();

    test_vector_move();
}