#include <stdio.h>

int func(int* p) {
    int y = *p;
    return y;
}

int main() {
    
    int* p = NULL;

    int a = func(p);

    return a;
}