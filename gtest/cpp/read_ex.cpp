#include "global_declare.h"

int FuncCall1(int n) {
    // Before Work;
    ++n;
    // After Work;
    return n;
}

int FuncCall2(int n) {
    // Before Work;
    ++n;
    ++n;
    ++n;
    ++n;
    // After Work;
    return n;
}

void FuncCall3(int n) {
    // Before Work;
    ++n;
    --n;
    ++n;
    --n;
    n ^= 1;
    --n;
    ++n;
    ++n;
    // After Work;
}

void FuncCall4(int n) {

    // Before Work;
    ++n;
    --n;
    n ^= n++;
    ++n;
    --n;
    n |= n--;
    --n;
    ++n;
    ++n;
    // After Work;
}