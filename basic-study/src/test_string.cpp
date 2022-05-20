#include "test_string.h"
#include "global_def.h"
#include <folly/FBString.h>
// #define _GLIBCXX_FULLY_DYNAMIC_STRING 1

void basic_test() {
    std::string empty;

    std::cout << "Empty String Size: " << sizeof (empty) << std::endl;
}

void test_refer() {
    std::string a = "TestData";
    std::string b = a;

    a.c_str();

    cout << reinterpret_cast<void*>(const_cast<char *>(a.data())) << ", " 
         << reinterpret_cast<void*>(const_cast<char *>(b.data()))  << endl;
}

void test_std_string_allocate() {
    cout << "\n-------------- test_std_string_allocate " << endl;
    int cout = 100;
    std::string a = "abc";
    std::string b = "abc";
    int last_cap = 0;

    for (int i =0; i < cout; ++i) {
        a += b;

        
        std::cout << reinterpret_cast<void*>(const_cast<char *>(a.data())) 
                  << ", size: " << a.size() << " cap: " << a.capacity();

        if (a.capacity() != last_cap) {
            if (last_cap == 0) {
                std::cout << ", init size: " << a.capacity();
            } else {
                std::cout << ", rate: " << float(a.capacity()) / last_cap;
            }
            last_cap = a.capacity();
        }    
        std::cout << std::endl;
                //   << ", " << a << std::endl;
    }
}

void test_folly_string_allocate() {
    cout << "\n-------------- test_folly_string_allocate " << endl;
    int cout = 100;
    folly::fbstring a = "abc";
    folly::fbstring b = "abc";
    int last_cap = 0;

    for (int i =0; i < cout; ++i) {
        a += b;
        std::cout << reinterpret_cast<void*>(const_cast<char *>(a.data())) 
                  << ", size: " << a.size() << " cap: " << a.capacity();
                //   << ", " << a << std::endl;

        if (a.capacity() != last_cap) {
            if (last_cap == 0) {
                std::cout << ", init size: " << a.capacity();
            } else {
                std::cout << ", rate: " << float(a.capacity()) / last_cap;
            }
            last_cap = a.capacity();
        }    
        std::cout << std::endl;                
    }
}

void TestStringMain()
{
    // basic_test();

    test_std_string_allocate();

    test_folly_string_allocate();

    // test_refer();
}