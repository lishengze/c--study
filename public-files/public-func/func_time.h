#ifndef _FUNC_TIME_H_
#define _FUNC_TIME_H_
#include <string>
using std::string;


int getCurrTimeSecs();

string getCurrDateTimeStr();

long long getCurrDatetime();

int getCurrDate();

class TestFuncTimeMain{

    public:
        TestFuncTimeMain();

        void test_getCurrDateTimeStr();

        void test_getCurrTimeSecs();
};

void testFuncTimeMain();

#endif // !_FUNC_TIME_H_
