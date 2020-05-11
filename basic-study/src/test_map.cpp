#include "test_map.h"
#include <map>
#include <string>
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