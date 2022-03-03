#include "test_vector.h"
#include <iostream>
#include <vector>
#include <exception>
using std::vector;
using std::cout;
using std::endl;

void test_vector_size()
{
    try
    {
        struct Base
        {
            Base(int i):a{i}
            {
                cout << "construct " << a << endl;
            }
            Base(const Base& other) 
            {
                a = other.a;
                cout << "copy construct " << a << endl;
            }
            Base(Base&& other) noexcept:a{std::move(other.a)}
            {
                cout << "move construct " << a << endl;
            }

            ~Base() {cout << "deconstruct " << a << endl;}

            int a;
            int b;
            int c;
        };

        vector<Base> vec;

        int test_numb = 10;
        Base* original_first = nullptr;

        for (int i = 0; i < test_numb; ++i)
        {
            // vec.emplace_back(std::move(Base(i)));

            cout << "\n----------------" << endl;

            vec.push_back(Base(test_numb - i));


            cout << i << ": size: " << vec.size() << ", capacity: " << vec.capacity();

            if (i > 0)
            {
                // cout << ", address: " << (&vec[vec.size()-1]) - (&vec[0]);

                cout << ", first address: " << (&vec[0]) << ", last address: " <<  (&vec[vec.size()-1]);
            }
            else
            {
                 cout << ", first address: " << (&vec[0]) << endl;
            }

            if (original_first == nullptr && i > 2) original_first = (&vec[0]);

            // if ((void*)(&vec[0]) != (void *)(original_first) && i > 2)
            if (i > 2)
            {
                cout << ", original_first: " << original_first << ", a: " << original_first->a << ", swap address: " <<  (&vec[0]) << ", a: " << vec[0].a;

                // cout << ", original_first: " << original_first->a << ", swap data: " << vec[0].a << endl;
            }

            cout << endl;
        }

        // cout << "Before Clear" << endl;
        // vec.clear();
        // cout << "After Clear " << endl;

        // vector<Base> empty_vec;
        // cout << "Before Swap" << endl;
        // vec.swap(empty_vec);
        // cout << "After Swap " << endl;        

        vec.erase(vec.begin());
        cout << "After Test Erase Begin" << endl;

        // vec.remove();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void TestVector()
{
    test_vector_size();
}