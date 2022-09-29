#include "string_opt.h"
#include <string>
#include <stdio.h>
#include "pandora/util/time_util.h"

std::string cpy(string src) { 
    std::string rst;

    for (int i=0; i < src.length(); ++i) {
        if (src[i] > 0x20) {
            rst = rst + src[i];
        }
    }

    return rst;
}

std::string cpy_change_operator(string src) { 
    std::string rst;

    for (int i=0; i < src.length(); ++i) {
        if (src[i] > 0x20) {
            rst += src[i];
        }
    }

    return rst;
}


std::string cpy_reserve(string src) { 
    std::string rst;

    rst.reserve(src.length());

    for (int i=0; i < src.length(); ++i) {
        if (src[i] > 0x20) {
            rst += src[i];
        }
    }

    return rst;
}


std::string cpy_iterator(string const& src) { 
    std::string rst;

    rst.reserve(src.length());

    for (auto iter = src.begin(); iter != src.end(); ++iter) {
        if (*iter > 0x20) {
            rst += *iter;
        }
    }
    return rst;
}

void cpy_change_param(string const& src, string& rst) { 
    rst.reserve(src.length());

    for (auto iter = src.begin(); iter != src.end(); ++iter) {
        if (*iter > 0x20) {
            rst += *iter;
        }
    }
}




void TestCopy() {
    std::string src_str = "This is test!";
    int test_count = 1000;
    unsigned long start_time, end_time;
    
    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("CPY Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_change_operator(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_change_operator Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_reserve(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_reserve Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);     

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_iterator(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_iterator Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);   

    start_time = utrade::pandora::NanoTime();

    for (int i=0; i < test_count; ++i) {
        std::string rst;
        cpy_change_param(src_str, rst);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_change_param Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);                      
}

void StringOPTMain() {
    printf("----- String OPT Test Start! ----\n");
    TestCopy();
}