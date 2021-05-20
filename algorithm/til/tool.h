#pragma once

#include "../global_declare.h"

template<class T>
void PrintData(T* data, int left, int right, string intro="")
{
    cout << intro << endl;
    for (int i = left; i <= right; ++i)
    {
        cout << data[i] << " ";
    }
    cout << endl;
}