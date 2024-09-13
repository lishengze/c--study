#include "test_map.h"
#include <map>
#include <unordered_map>
#include <string>
#include <stdio.h>
#include <string.h>
using std::map;
using std::string;

// 测试 以 string 和 int 分别做key, 高速插入，删除的性能比较；
// 10个交易所， 100种币对， 2个买卖方向;

void test_string_key()
{
    map<string, map<string, map<string, int>>>      string_map;
}

void test_int_key()
{
    
}

void test_unorder_map() {
    struct AtomTest {
        double dvalue1_;
        double dvalue2_;
    };

    struct UMap {
        int iValue1_;
        double dValue2_;
        // std::unordered_map<std::string, AtomTest> stValue_;
        std::map<std::string, AtomTest> stValue_;
    };

    UMap stData1;
    UMap stData2;

    memcpy(&stData2,&stData1,sizeof(UMap));
    
}