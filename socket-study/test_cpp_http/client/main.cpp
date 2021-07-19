#include "test.h"
#include "test_curl.h"
#include "../include/global_declare.h"

int main()
{
    cout << "Test Cpp-Http " << endl;
    TestMain();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    TestCurl();

    return 1;
}