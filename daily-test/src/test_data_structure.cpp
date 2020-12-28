#include "test_data_structure.h"
#include <set>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <string>

using std::set;
using std::cout;
using std::endl;
using std::string;

class UserSet
{
    public:
        UserSet(double main, double sub): main_{main}, sub_{sub} {}

        // bool operator < (const UserSet& other) const
        // {
        //     return main_ < other.main_;
        // }

        // bool operator == (const UserSet& other) const
        // {
        //     return main_ == other.main_;
        // }

        double  main_;
        double  sub_;
};

using UserSetBPtr = boost::shared_ptr<UserSet>;

class LessUserSetBPtr
{
public:
    bool operator() (const UserSetBPtr& a, const UserSetBPtr& b)
    {
        return a->main_ < b->main_;
    }
};

class LessUserSet
{
public:
    bool operator() (const UserSet& a, const UserSet& b)
    {
        return a.main_ < b.main_;
    }

};


void test_simple_set()
{
    cout << "test_simple_set " << endl;

    set<UserSet, LessUserSet> user_set;
    user_set.emplace(UserSet{5, 10});
    user_set.emplace(UserSet{3, 7});
    user_set.emplace(UserSet{2, 8});
    user_set.emplace(UserSet{7, 12});

    for (auto& iter:user_set)
    {
        cout << iter.main_ << ", " << iter.sub_ << endl;
    }

    set<UserSet>::iterator iter = user_set.find({5,10});

    cout << "\nErase {5, 10}" << endl;
    if (iter != user_set.end())
    {
        cout << iter->main_ << ", " << iter->sub_ << endl;
        user_set.erase(iter);
    }

    cout << "\nLeft Data: " << endl;
    for (auto& iter:user_set)
    {
        cout << iter.main_ << ", " << iter.sub_ << endl;
    }

}

void test_boost_ptr_set()
{
    cout << "test_boost_ptr_set " << endl;

    std::set<UserSetBPtr, LessUserSetBPtr> user_set;
    user_set.insert(UserSetBPtr(new UserSet{5, 10}));
    user_set.insert(UserSetBPtr(new UserSet{3, 7}));
    user_set.insert(UserSetBPtr(new UserSet{2, 8}));
    user_set.insert(UserSetBPtr(new UserSet{7, 12}));



    cout << "sizeof(set): " << sizeof (user_set) << endl;
    

    for (auto& iter:user_set)
    {
        cout << iter->main_ << ", " << iter->sub_ << endl;
    }

    std::set<UserSetBPtr, LessUserSetBPtr>::iterator iter = user_set.find(UserSetBPtr(new UserSet{5, 10}));

    cout << "\nErase {5, 10}" << endl;
    if (iter != user_set.end())
    {
        cout << (*iter)->main_ << ", " << (*iter)->sub_ << endl;
        user_set.erase(iter);
    }

    cout << "\nLeft Data: " << endl;
    for (auto& iter:user_set)
    {
        cout << iter->main_ << ", " << iter->sub_ << endl;
    }
}

void test_size()
{
    std::set<UserSetBPtr, LessUserSetBPtr> user_set;
    user_set.insert(UserSetBPtr(new UserSet{5, 10}));
    user_set.insert(UserSetBPtr(new UserSet{3, 7}));
    user_set.insert(UserSetBPtr(new UserSet{2, 8}));
    user_set.insert(UserSetBPtr(new UserSet{7, 12}));

    cout << "sizeof(set): " << sizeof (user_set) << endl;

    string life = "life";

    cout << "sizeof(string): " << sizeof (life) << endl;

    std::set<std::string>   string_set;

    cout << "sizeof(string_set): " << sizeof (string_set) << endl;

    char* char_data = "life";

    cout << "sizeof(char_data): " << sizeof (char_data) << endl;

}

void test_data_structure()
{
    // test_simple_set();

    // test_boost_ptr_set();

    test_size();
}