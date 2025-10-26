#pragma once
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

struct Block1
{
    /* data */
    char data[120];
    unsigned long long ulTime;

    Block1(const Block1& other) {
        // cout << "Block1 copy constructor:  " << other.ulTime << endl;
        memcpy(data, other.data, sizeof(data));
        ulTime = other.ulTime;
    }

    Block1() {
        memset(data, 0, sizeof(data));
        ulTime = 0;
    }

    Block1(const Block1&& other) {
        cout << "Block1 move constructor" << endl;
        memcpy(data, other.data, sizeof(data));
        ulTime = other.ulTime;
    }

    Block1& operator=(const Block1& other) {
        // cout << "Block1 copy assignment operator :" << other.ulTime << endl;
        memcpy(data, other.data, sizeof(data));
        ulTime = other.ulTime;
        return *this;
    }
};

