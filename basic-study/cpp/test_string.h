#pragma once
#include "global_def.h"
#include <folly/FBString.h>
#include <string_view>

#include "pandora/util/time_util.h"

using std::cout;

using std::endl;


template<class String>
void TestSubStr(std::string info) {
    cout << info << endl;
    String main_str = "abcdefghijklmopqrstuvwxyz";

    int test_count_main[7] = {10, 30, 100, 1000, 10000, 100000, 1000000};    

    for (int i = 0; i < sizeof(test_count_main) / sizeof(int); ++i) {

        auto start_time = utrade::pandora::NanoTime();

        for (int j = 0; j < test_count_main[i]; ++j) {
            String sub_str = main_str.substr(0, 10);
        }

        auto end_time = utrade::pandora::NanoTime();

        cout <<"TestCount: " << test_count_main[i] <<", DeltaNanos: " << end_time - start_time  
             << ", AveDeltaNanos: " << float(end_time - start_time) / float(test_count_main[i]) << endl;
    }
    cout << endl;
}

template<class String>
void test_string_allocate(std::string info) {
    cout << info << endl;
    int cout = 100;
    String a = "abc";
    String b = "abc";
    int last_cap = 0;

    for (int i =0; i < cout; ++i) {
        a += b;
        if (a.capacity() != last_cap) {
            std::cout << reinterpret_cast<void*>(const_cast<char *>(a.data())) 
                    << ", size: " << a.size() << " cap: " << a.capacity();            
            if (last_cap == 0) {
                std::cout << ", init size: " << a.capacity();
            } else {
                std::cout << ", rate: " << float(a.capacity()) / last_cap;
            }
            last_cap = a.capacity();
            std::cout << std::endl;
        }    
        
                //   << ", " << a << std::endl;
    }
}

void test_stringview_expand(string info) {
    cout << info << endl;
    int cout = 100;
    std::string_view a = "abcdefghijklmopqrstuvwxyz";
    // std::string_view b = "abc";
    // int last_cap = 0;

    std::string_view b = "12345678";

    // a = "12344";

    std::cout 
        << "a.size: " << a.size() 
        << ", a.length: " << a.length() 
        << ", a.max_size: " << a.max_size() 
        
        << std::endl;

    std::string_view a_sub = a.substr(2,4);
    std::cout << "a: " << a << ", a.data: " << a.data() <<  ", address: " << reinterpret_cast<void*>(const_cast<char *>(a.data()))  << "\n"
         << "a_sub: " << a_sub << ", a_sub.data: " << a_sub.data() << ", address: " << reinterpret_cast<void*>(const_cast<char *>(a_sub.data())) 
         << std::endl;

    // for (int i =0; i < cout; ++i) {
    //     // a += b;
    //     // a = a+b;
        
    //     std::cout << reinterpret_cast<void*>(const_cast<char *>(a.data())) 
    //               << ", size: " << a.size();

    //     // if (a.capacity() != last_cap) {
    //     //     if (last_cap == 0) {
    //     //         std::cout << ", init size: " << a.capacity();
    //     //     } else {
    //     //         std::cout << ", rate: " << float(a.capacity()) / last_cap;
    //     //     }
    //     //     last_cap = a.capacity();
    //     // }    
    //     std::cout << std::endl;
    //             //   << ", " << a << std::endl;
    // }
}



void TestStringSub() {
	// cout << "Test Std::String " << endl;
    TestSubStr<std::string>("Test Std::String " );
    TestSubStr<folly::fbstring>("Test Fb::String " );
    TestSubStr<std::string_view>("Test Std::string_view " );
}

void TestStringAllocate() {
    test_string_allocate<std::string>("Test Std::String " );
    test_string_allocate<folly::fbstring>("Test Fb::String " );
    // test_stringview_expand("Test Std::string_view " );    
}


void TestStringMain() {
    // TestStringAllocate();

    TestStringSub();
}
