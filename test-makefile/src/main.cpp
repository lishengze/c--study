#include "add.h"
#include "sub.h"
#include "thread_func.h"
#include <iostream>
#include <stdio.h>

int main(int argc, char* args[]) 
{
    int a = add(1,2);
    int b = sub(1,2);
    std::cout << a << std::endl;
    std::cout << b << std::endl;

    printf("Hello, World! \n");
    for (int i = 0; i < argc; ++i)
    {
        printf("%d, %s \n", i, args[i]);
    }
    getchar();
    return 0;
}
