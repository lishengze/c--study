#include "func_math.h"
#include <cstdlib>
#include <stdio.h>
using std::rand;

bool randBool()
{
    bool result = true;
    int maxValue = 100;
    int randValue = rand() % maxValue;
    if (randValue > maxValue / 2)
    {
        result = false;
    }
    return result;
}

class TestMath
{
public:
    TestMath();

    void testRandBool();
};

TestMath::TestMath()
{
    testRandBool();
}

void TestMath::testRandBool()
{
    int AllNumb = 100;

    int positiveNumb = 0;
    int negativeNumb = 0;

    for (int i = 0; i < AllNumb; ++i)
    {
        if (randBool())
        {
            ++positiveNumb;
        }
        else
        {
            ++negativeNumb;
        }
    }

    printf("AllNumb: %d, positiveNumb: %d, negativeNumb: %d \n", 
            AllNumb, positiveNumb, negativeNumb);
}

void testMathFunc()
{
    TestMath testObj;
}