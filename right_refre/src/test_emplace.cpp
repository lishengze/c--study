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

struct Complated
{
    int             m_year;
    double          m_version;
    string          m_name;

    Complated(int year, double version, string name):
        m_year(year), m_version(version), m_name(name)
    {
        cout << "Default Constructor" << endl;
    }

    Complated(const Complated& other):
        m_year(other.m_year), m_version(other.m_version), m_name(other.m_name)
    {
        cout << "Moved Contructor" << endl;
    }

};

void TestEmplace()
{
    map<int, Complated> mapValue;
    int year = 10;
    double version = 1.0;
    string name = "Wps";

    cout << "-- Insert --" << endl;
    mapValue.insert(make_pair(1, Complated(year, version, name)));

    cout << "-- Emplace --" << endl;
    mapValue.emplace(2, Complated(year, version, name));

    cout << "-- Emplace_Back --" << endl;
    vector<Complated>  vecValue;
    // vecValue.emplace_back(year, version, name);
    vecValue.emplace_back(Complated(year, version, name));

    cout << "-- Push Back --" << endl;
    vecValue.push_back(Complated(year, version, name));
}