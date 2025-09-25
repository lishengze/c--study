#include "base_util.h"

// 将整形纳秒按照每三位一个逗号的形式输出;
 string NanoStr(unsigned long long ulNanoSecs) {
    std::string sRst = std::to_string(ulNanoSecs);
    int len = sRst.length();
    int insertPos = len % 3;

    for (int i = 0; i < (len - 1)/3; ++i) {
        int pos = (insertPos == 0 ? : insertPos )  + i * 4;
        // cout << "pos: " << pos << endl;
        if (pos < sRst.length()) {
            sRst.insert(pos, ",");
        }
    }
    return sRst;
}
