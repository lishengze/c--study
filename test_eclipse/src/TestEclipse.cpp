//============================================================================
// Name        : TestEclipse.cpp
// Author      : LSZ
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "add.h"
#include "sub.h"
#include "sum.h"
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char* args[])
{
    int a = add(1,2);
    int b = sub(1,2);
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << a + b << std::endl;

    printf("Hello, World! \n");
    for (int i = 0; i < argc; ++i)
    {
        printf("%d, %s \n", i, args[i]);
    }
    getchar();

	cout << "TestEclipse" << endl; // prints TestEclipse
	return 0;
}