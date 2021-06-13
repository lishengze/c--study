#include "test_algo.h"
#include "basic_algo.h"

void TestAlgo()
{
    string ori_str = "ABCABCDABCABBHIJK";
    string substr = "ABCABB";

    KMP kmp;
    int pos = kmp.get_pos(ori_str, substr);

    cout << "ori_str: " << ori_str << "\n"
         << "substr: " << substr << "\n"
         << "subpos " << pos
         << endl;
}
