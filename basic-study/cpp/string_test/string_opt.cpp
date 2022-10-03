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

std::string cpy_refrence(string const& src) { 
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

void cpy_param_iter(string const& src, string& rst) { 
    rst.reserve(src.length());

    for (auto iter = src.begin(); iter != src.end(); ++iter) {
        if (*iter > 0x20) {
            rst += *iter;
        }
    }
}

void cpy_param_noniter(string const& src, string& rst) { 
    rst.reserve(src.length());

    for (int i=0; i < src.length(); ++i) {
        if (src[i] > 0x20) {
            rst += src[i];
        }
    }
}


void cpy_array(char* dst, char const* src, size_t size) {
    for (size_t i=0;i < size; ++i) {
        if (src[i] >= 0x20) {
            *dst++= src[i];
        }
    }
    *dst='\0';
}



void cpy_append(string const& src, string& rst) { 
    rst = src;

    for (int i=0,b=i; i < src.length(); i+=b) {
        for (b=i; b < src.length() ;++b)
            if (src[b] < 0x20) break;
        
        rst.append(src, i, b-i);
    }
}

void cpy_erase(string const& src, string& rst) { 
    rst.reserve(src.length());

    for (int i=0; i < rst.length(); ++i) {
        if (rst[i] < 0x20) {
            rst.erase(i,1);
        }
    }
}

void TestCopyAtom() {
    std::string src_str = "This is test! HaHaHaHaHaHaHa";
    int test_count = 100000;
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
    printf("cpy_operator Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_reserve(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_reserve Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);     

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_refrence(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_refrence Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);   

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        cpy_iterator(src_str);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_iterator Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);   

    start_time = utrade::pandora::NanoTime();

    for (int i=0; i < test_count; ++i) {
        std::string rst;
        cpy_param_iter(src_str, rst);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_param_iter Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);  

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        std::string rst2;
        cpy_param_noniter(src_str, rst2);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_param_noniter Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);      

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        char dst[100];
        cpy_array(dst, src_str.c_str(), src_str.length());
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_array Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count); 

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        std::string rst2;
        cpy_append(src_str, rst2);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_append Test %d, Ave Cost: %d ns\n", test_count, (end_time - start_time) / test_count);   

    start_time = utrade::pandora::NanoTime();
    for (int i=0; i < test_count; ++i) {
        std::string rst3;
        cpy_erase(src_str, rst3);
    }
    end_time = utrade::pandora::NanoTime();
    printf("cpy_erase Test %d, Ave Cost: %d ns\n\n", test_count, (end_time - start_time) / test_count);          
}

void TestCopy() {
    int test_count = 10;

    for (int i =0; i < test_count; ++i) {
        TestCopyAtom();
    }
}

void StringOPTMain() {
    printf("----- String OPT Test Start! ----\n");
    TestCopy();
}