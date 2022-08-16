#include "test_attribute.h"
#include <stdio.h>

void myprint(const char* format, ...) __attribute__((format(printf, 1, 2)));

void myprint(const char* format, ...)
{

}

__attribute__((constructor)) void Before() {
    printf("***** Before Main *****\n");
}

__attribute__((destructor)) void After() {
     printf("***** After Main *****\n");
}


void TestFormat() {
    myprint("i=%d\n", 6);
    myprint("i=%s\n", 6);
    myprint("i=%s\n", "abc");
    myprint("%s, %d, %d\n", 1,2);
}

void TestAttribute() {
    TestFormat();
    printf("---------TestAttribute----\n");
}