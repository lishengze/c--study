#include "test_print.h"
#include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>


void test_cout_err() {
    fprintf(stdout, "Hello ");
    fflush(stdout);
    
    fprintf(stderr, "World!\n");

    FILE* fd = freopen("a.log", "a+", stdout);

    if (!fd) {
        fprintf(stderr, "Fd is err!\n");
    } else {
        fprintf(stdout, "Put Info Into stdout \n");
        fprintf(fd, "Input To FIlE directly\n");
        fprintf(stderr, "Input Info By Err\n");
    }
}

void TestPrint() {
    test_cout_err();
}