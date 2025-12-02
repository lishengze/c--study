#include "base_util.h"

// 将整形纳秒按照每三位一个逗号的形式输出;
 string NanoStr2(unsigned long long ulNanoSecs) {
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

 string NanoStr(unsigned long long ulNanoSecs) {
    std::string sRst = std::to_string(ulNanoSecs);

    int len = sRst.length();

    int dotCount = (sRst.length()-1) / 3;
    int iFirstPos = sRst.length() % 3; // 6 ->0, 4->1, 5->2; 
    iFirstPos = iFirstPos == 0 ? 3 : iFirstPos;

    // 9为例， 123456789 -> 123456,789 (9-3 == 6) -> 123,456,789 (10-3-4==3);
    // 7为例， 1234567 -> 1,234567 (1 + 0*4== 1) ->1,234,456->(1+1*4 = 5)
    for (int i = 0 ; i<dotCount; ++i) {
        
        int iCurInsertPos = iFirstPos + i * 4;

        // if (iCurInsertPos )

        sRst.insert(iCurInsertPos, ",");

        len++; // 每插入一次字符串增长一位;
    }


    return sRst;
}
