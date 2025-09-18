#pragma once

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <chrono>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::cout;
using std::endl;
using std::fstream;
using std::map;
using std::set;
using std::vector;

#include "json.hpp"

using njson = nlohmann::json;

#define ILL_FILE 0
#define REG_FILE 1
#define DIR_FILE 2

#define CMP_UPLOAD  1 
#define CMP_DEFAULT 0 

#define MILLI_PER_SECOND 1000
#define MICRO_PER_MILLI 1000
#define NANO_PER_MICRO 1000

#define MICRO_PER_SECOND (MILLI_PER_SECOND * MICRO_PER_MILLI)
#define NANO_PER_SECOND (MICRO_PER_SECOND * NANO_PER_MICRO)
#define NANO_PER_MILLI (NANO_PER_MICRO * MICRO_PER_MILLI)

class Error {
public:
    Error():m_iErrorCode{0}, m_sErrorMsg{""} {

    }

    Error(int iErrorCode, string sErrMsg):m_iErrorCode(iErrorCode), m_sErrorMsg{sErrMsg} {

    }

    Error Set(int iErrorCode, string sErrorMsg) {
        m_iErrorCode = iErrorCode;
        m_sErrorMsg = sErrorMsg;

        return *this;
    }

    string Str() {
        return string("err_code: ") + std::to_string(m_iErrorCode) + ", err_msg: " + m_sErrorMsg;
    }

    int     m_iErrorCode;
    string  m_sErrorMsg;

    bool IsFailed() {
        return m_iErrorCode > 0;
    }
};

inline string GetShortFileName(const string& sOriFileName, string sSubDirName) { 
    string::size_type pos =  sOriFileName.find(sSubDirName);
    string sRst = sOriFileName;
    if (pos != std::string::npos) {
        sRst = sOriFileName.substr(pos+sSubDirName.length());
    }
    return sRst;
}

inline void GetBaseWorkDirInfo(string& baseDir, string& flag, int rootDepth = 1) {
    string curFilePath = __FILE__;
    flag = "/";

#if defined(_WIN32) || defined(_WIN64)
	flag = "\\";
#endif

    string::size_type i = curFilePath.find_last_of(flag);
    string workDir = curFilePath.substr(0, i);

    cout << "workDir: " << workDir << endl;

    if (workDir.find_last_of(".") != std::string::npos ) {
        string::size_type pos =  workDir.find_first_of(".");
        workDir = curFilePath.substr(0, pos-1);
        rootDepth--;
    }
    baseDir = workDir;
    // cout << "****** curFilePath: " << curFilePath <<"\nworkDir: " << workDir <<  endl;

	for (int j = 0; j < rootDepth; ++j) {
		string::size_type i = workDir.find_last_of(flag);
		baseDir = workDir.substr(0, i);
        // cout << "baseDir: " << baseDir << endl;
		workDir = baseDir;
	}


}

inline void GetIndexedWorkDirInfo(string& baseDir, string& flag, string indexedValue, int rootDepth = 1) {
    string curFilePath = __FILE__;
    flag = "/";

#if defined(_WIN32) || defined(_WIN64)
	flag = "\\";
#endif

    string::size_type i = curFilePath.find(indexedValue);
    string workDir = curFilePath.substr(0, i) + indexedValue;

    cout << "workDir: " << workDir << endl;

    baseDir = workDir;
}

inline  string GetWorkDir(int rootDepth = 0) {
    string rst;
    string flag;

    GetBaseWorkDirInfo(rst, flag,0);

    // cout << "GetWorkDir: " << rst << endl;

    return rst + flag;
}

inline  string GetSimpleWorkDir() {
    string curFilePath = __FILE__;
    string flag = "/";

#if defined(_WIN32) || defined(_WIN64)
	flag = "\\";
#endif

    string::size_type i = curFilePath.find_last_of(flag);
    string workPath = curFilePath.substr(0, i) + flag;
    // cout << "workPath: " << workPath << endl;

    return workPath;
}

inline bool isGBK(const char* str, int length)
{
    unsigned int nBytes = 0;//GBK??1-2?????,???? ,????
    unsigned char chr = *str;
    bool bAllAscii = true; //??????ASCII,
    for (unsigned int i = 0; i < length && str[i] != '\0'; ++i){
        chr = *(str + i);
        if ((chr & 0x80) != 0 && nBytes == 0){// ????ASCII??,????,??????GBK
        bAllAscii = false;
        }
        if (nBytes == 0) {
            if (chr >= 0x80) {
                if (chr >= 0x81 && chr <= 0xFE){
                    nBytes = +2;
                }
                else{
                    return false;
                }
                nBytes--;
            }
        }
        else{
            if (chr < 0x40 || chr>0xFE){
            return false;
            }
            nBytes--;
        }//else end
    }
    if (nBytes != 0) {   //????
    return false;
    }
    if (bAllAscii){ //??????ASCII, ??GBK
    return true;
    }
    return true;
}

inline bool isGBK(string& sSrc) {
    return isGBK(sSrc.c_str(), sSrc.length());
}

inline bool isUtf8(const char* str, int length)
{
    unsigned int nBytes = 0;//UFT8??1-6?????,ASCII?????
    unsigned char chr = *str;
    bool bAllAscii = true;
    for (unsigned int i = 0; i < length && str[i] != '\0'; ++i){
        chr = *(str + i);
        //????ASCII??,????,??????UTF8,ASCII?7???,??????0,0xxxxxxx
        if (nBytes == 0 && (chr & 0x80) != 0){
            bAllAscii = false;
        }
        if (nBytes == 0) {
            //????ASCII?,???????,?????
            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD){
                    nBytes = 6;
                }
                else if (chr >= 0xF8){
                    nBytes = 5;
                }
                else if (chr >= 0xF0){
                    nBytes = 4;
                }
                else if (chr >= 0xE0){
                    nBytes = 3;
                }
                else if (chr >= 0xC0){
                    nBytes = 2;
                }
                else{
                    return false;
                }
                    nBytes--;
                }
            }
            else{
                //?????????,?? 10xxxxxx
                if ((chr & 0xC0) != 0x80){
                    return false;
                }
                //??????
                nBytes--;
            }
    }
    //??UTF8????
    if (nBytes != 0) {
        return false;
    }
    if (bAllAscii){ //??????ASCII, ??UTF8
        return true;
    }
    return true;
}

inline bool isUtf8(string& sSrc) {
    return isUtf8(sSrc.c_str(), sSrc.length());
}

#if defined(_WIN32) || defined(_WIN64)

inline string GbkToUtf8(const char *src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
} 

inline string Utf8ToGbk(const char *src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}
#else
#include <iconv.h>

inline int GbkToUtf8( const char *src_str, size_t src_len, char *dst_str, size_t dst_len)
{
    iconv_t cd = iconv_open("utf8", "gbk");
    // char** pin = &src_str;
    // char** pout = &dst_str;

    if (cd == 0) {
        return 1;
    }
        
    memset(dst_str, 0, dst_len);
    if (iconv(cd, (char**)&src_str, &src_len, (char**)&dst_str, &dst_len) == -1) {
        return 2;
    }

    iconv_close(cd);

    return 0;
}


inline int Utf8ToGbk(const char *src_str, size_t src_len, char *dst_str, size_t dst_len)
{
    iconv_t cd = iconv_open("gbk", "utf8");
    if (cd == 0)
        return 1;
    memset(dst_str, 0, dst_len);
    if (iconv(cd, (char**)&src_str, &src_len, (char**)&dst_str, &dst_len) == -1)
        return 2;
    iconv_close(cd);

    return 0;
}
#endif

inline bool IsJsonAcceptUtf8(const string& sOriStr) {
    try
    {
        nlohmann::json tmpJson;
        tmpJson["test"] = sOriStr;
        tmpJson.dump(2);

        return true;
    }
    catch(const std::exception& e)
    {
        // cout <<"[FAILED] Json Dump " << sOriStr << "\n[Exception]: "<< e.what() << "\n" << endl;
        return false;
    }
    
    return false;
}

inline string GetUtf8String(const char* sSrc, int iSrcLen) {
    string result(sSrc);

    if (isGBK(sSrc, iSrcLen)) {
        int iDstLen = iSrcLen * 4;
        char* sDst = new char[iDstLen];
        memset(sDst, 0, iDstLen);

        int flag = GbkToUtf8(sSrc, iSrcLen, sDst, iDstLen);
        if (0 == flag) {
            result =  sDst;
        } 
        delete []sDst;

    } else {
        // cout << "Char[] sSrc: " << sSrc << " is UTF8" << endl;
    }

    if (!IsJsonAcceptUtf8(result)) {
        result = "";
    }

    return result;
}

inline double GetUtf8String(double& sSrc, int iSrcLen) {
    return sSrc;
}

inline string GetUtf8String(string sSrc) {
    return  GetUtf8String(sSrc.c_str(), sSrc.length());;
}

inline string GetGBKString(const char* sSrc, int iSrcLen) {
    string result(sSrc);

    if (iSrcLen == 0) return result;

    if (isUtf8(sSrc, iSrcLen)) {

        int iDstLen = iSrcLen*4;
        char* sDst = new char[iDstLen];
        memset(sDst, 0, iDstLen);

        int flag = Utf8ToGbk(sSrc, iSrcLen, sDst, iDstLen);
        if (0 == flag) {
            result = sDst;
        } else {
            // cout << "[FAILED] sSrcCpy: " << sSrcCpy << ", Len: " << len  << ", flag: " << flag << endl;
            result = "";     
        }
        delete []sDst;

    } else {
        // cout << "Char[] sSrc: " << sSrc << " is UTF8" << endl;
    }

    return result;
}

inline string GetGBKString(string sSrc) {
    if (sSrc.length() == 0) return sSrc;
    return  GetGBKString(sSrc.c_str(), sSrc.length());
}

inline int DecodeFileToJson(std::fstream& fileStream, njson& dstJson) {
    try
    {
        string line;
        string sumResult="";

        bool bJsonStart = false;

        while(getline(fileStream, line)) {
            // cout << "line: " << line << endl;
            if (line.find_first_of("]") != std::string::npos) {
                // cout <<  "文件结束" << endl;
                return -1;
            }

            if (line.find_first_of("{") != std::string::npos) {
                sumResult += line + "\n";
                bJsonStart =  true;
                break;
            }            
        }

        if (!bJsonStart) {
            // cout <<  "文件没有{" << endl;
            return 0;            
        }

        while(getline(fileStream, line)) 
        {
            if (bJsonStart) {
                if (line.find_first_of("}") != std::string::npos) {
                    sumResult += "}\n";
                    break;
                } else {
                    sumResult += line + "\n";
                }                
            }
        }

        // cout << "sumResult: " << sumResult << "\n" << endl;

        dstJson =  njson::parse(GetUtf8String(sumResult));

        return 1;

    }
    catch(const std::exception& e)
    {
        cout << string("[FAILED] DecodeFileToJson: ") +  e.what() << endl;
    }
    
    return 0;
}

inline int getIndexOfSigns(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    
    return -1;
}

inline int Trans0XNumber(string srcStr, string & sErrMsg) {
    try
    {
        string transStr;

        if (srcStr.find("0x") != std::string::npos) {
            string::size_type pos = srcStr.find("0x") + 2;
            transStr = srcStr.substr(pos, srcStr.length());
        } else if (srcStr.find("0X") != std::string::npos) {
            string::size_type pos = srcStr.find("0X") + 2;
            transStr = srcStr.substr(pos, srcStr.length());
        } else {
            return -1;
        }

        // cout << "transStr: " << transStr << endl;

        int tmp = 1;
        int rst = -1;
        for (int i=transStr.length()-1; i>-1; --i) {

            if (rst < 0) rst = 0;

            int tmpValue = getIndexOfSigns(transStr[i]);

            // cout << "transStr[" << i << "]: " << transStr[i] << ", tmpValue: " << tmpValue << endl;

            if (tmpValue < 0) return -1;

            rst += tmpValue * tmp;
            tmp *= 16;            
        }

        return rst;
    }
    catch(const std::exception& e)
    {
        sErrMsg = string("[Failed] Trans 16 Number: ") + srcStr + "\n[Exception] " + e.what();
        std::cerr << e.what() << '\n';
    }
    
    return -1;
}

inline int GetDate(int deltaDays=0) {
    time_t now;
    struct tm *timeinfo;
    char str[20];

    time(&now);
    now += 24 * 60 * 60 * deltaDays;
    timeinfo = localtime(&now);

    strftime(str, sizeof(str), "%Y%m%d", timeinfo);

    int rst = atoi(str);

    return rst;
}


inline std::chrono::steady_clock::time_point GetTimeNow() {
    timespec tp;

    clock_gettime(CLOCK_MONOTONIC, &tp);

    return std::chrono::steady_clock::time_point(
        std::chrono::steady_clock::duration(
            std::chrono::seconds(tp.tv_sec) + std::chrono::nanoseconds(tp.tv_nsec)));
}

inline long NanoTime() {
    std::chrono::high_resolution_clock::time_point curtime = std::chrono::high_resolution_clock().now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(curtime.time_since_epoch()).count();
}


inline struct tm ToSecondStruct(long nano) {
    time_t sec_num = nano / NANO_PER_SECOND;
    return *gmtime(&sec_num);
}

inline string ToSecondStr(long nano, string time_format="%Y-%m-%d %H:%M:%S") {
    if (nano <=0) {
        return "NULL";
    }

    nano /= NANO_PER_SECOND;
    struct  tm* dt ={0};
    char buffer[30] = {0};
    dt = gmtime(&nano);
    strftime(buffer, sizeof(buffer), time_format.c_str(), dt);

    return std::string(buffer);
}

inline std::string SecTimeStr(std::string time_format="%Y-%m-%d %H:%M:%S") {
    long nano_time = NanoTime();
    return ToSecondStr(nano_time, time_format);
}

inline std::string NanoTimeStr(std::string time_format="%Y-%m-%d %H:%M:%S"){
    long nano_time = NanoTime();
    string time_now = ToSecondStr(nano_time, time_format);

    time_now += "." + std::to_string(nano_time % NANO_PER_SECOND);

    return time_now;
}

inline std::string MilliTimeStr(std::string time_format="%Y-%m-%d %H:%M:%S"){
    long nano_time = NanoTime();
    string time_now = ToSecondStr(nano_time, time_format);

    long nano_secs = nano_time % NANO_PER_SECOND;
    long milli_secs = nano_secs / NANO_PER_MILLI;

    time_now += "." + std::to_string(milli_secs);
    return time_now;
}

inline int IsValidFileOrDir(string sSrc) {
    struct stat buf;
    int result = stat(sSrc.c_str(), &buf);

    if (S_ISDIR(buf.st_mode)) {
        return DIR_FILE;
    } else if (S_ISREG(buf.st_mode )) {
        return REG_FILE;
    } else {
        return ILL_FILE;
    }
}

inline string FilterFileAffix(string sSrc, string sAffix) {
    string::size_type i = sSrc.find(sAffix);
    string result = sSrc;
    if (i != std::string::npos) {
        result = sSrc.substr(0, i);
        // printf("\t\tAfter Filter %s ->  %s\n", sAffix.c_str(), sSrc.c_str());
    }
    return result;
    
}

inline bool GetAllDirFile(string sDirName, map<string, string>& dstmap, string sAffix) {
    DIR* dir = opendir(sDirName.c_str());

    if (NULL == dir) {
        return false;
    } 

    struct dirent* dirp;
    while(true) {
        dirp = readdir(dir);
        if (NULL == dirp) break;

        if (DT_DIR == dirp->d_type) {
            // printf("\t%s 是一个目录;\n", dirp->d_name);
        } else if (DT_REG == dirp->d_type) {
            // printf("\t%s 是一个普通文件 \n", dirp->d_name);
            string sFileName = dirp->d_name;
            string sTidName = FilterFileAffix(sFileName, sAffix);

            dstmap[sTidName] = sDirName + "/" + sFileName;
        } else {
            break;
        }
    }
    
    closedir(dir);
    return true;
}

inline bool GetTargetAllFile(string sDirName, std::set<string>& fileSet, string sTargetFileName) {
    DIR* dir = opendir(sDirName.c_str());

    if (NULL == dir) {
        return false;
    } 

    struct dirent* dirp;
    while(true) {
        dirp = readdir(dir);
        if (NULL == dirp) break;

        if (DT_DIR == dirp->d_type) {
            if (dirp->d_name[0] == '.') continue;

            string sNextDirName = sDirName + "/" + string(dirp->d_name);
            GetTargetAllFile(sNextDirName, fileSet, sTargetFileName);
            // cout << "sNextDirName: " << sNextDirName << ", d_name: " << dirp->d_name << endl;
            // break;
        } else if (DT_REG == dirp->d_type) {
            // printf("\t%s 是一个普通文件 \n", dirp->d_name);
            string sFileName = dirp->d_name;
            if (sFileName == sTargetFileName) {
                sFileName = sDirName + "/" + sFileName;
                fileSet.insert(sFileName);
                // cout << "sFileName: " << sFileName << endl;
            }
        } else {
            break;
        }
    }
    
    closedir(dir);
    return true;
}

inline void VecPrint(vector<string>& srcVec) {
    cout << "size: " << srcVec.size() << endl;
    for (auto tmp:srcVec) {
        cout << tmp << "|";
    }    
    cout << endl;
}

inline void VecExtend( vector<string>& srcVec,  vector<string>& dstVec) {
    for (auto tmp:dstVec) {
        if (tmp.length() > 0 && tmp != " " && tmp != "") {
            srcVec.push_back(tmp);
        }
    }
}

inline void FilterStringSet(vector<string>& srcVec, set<string>& filterSet) {
    vector<string> vecDest;
    for (auto tmpString:srcVec) {
        if (filterSet.find(tmpString) == filterSet.end()) {
            vecDest.push_back(tmpString);
        }
    }
    srcVec = vecDest;
}

inline void FilterChar(string& srcStr, char filterChar) {
    string strDst;
    for (int i = 0; i < srcStr.length(); ++i) {
        if (srcStr[i] != filterChar) {
            strDst.push_back(srcStr[i]);
        }
    }
    srcStr = strDst;
}

inline void FilterVecStringChar(vector<string>& srcVec, vector<char>& filterVec) {
    for (vector<string>::iterator iter = srcVec.begin(); iter != srcVec.end(); ++iter) {
        for (auto tmpChar:filterVec) {
            FilterChar(*iter, tmpChar);
        }        
    }
}

inline vector<string> StringSplit(const string &s, const string seperator) {
    vector<string> result;

    string tmpStr = s;
    size_t pos = tmpStr.find_first_of(seperator);
    bool bLastStrValid = true;
    while(pos!= std::string::npos) {

        // 说明分隔符不在最左边,将分隔符左边的字串压入数组;
        if (pos != 0) { 
            string sLeftStr = tmpStr.substr(0, pos);
            result.push_back(sLeftStr);
        }

        // 说明分隔符不在最右边,将右边的字串作为新的字符串继续循环;
        if (pos + seperator.length() != tmpStr.length()) { 
            string sRightStr = tmpStr.substr(pos + seperator.length());
            tmpStr = sRightStr;
            pos =  tmpStr.find_first_of(seperator);
            bLastStrValid = true;
        } else {
            bLastStrValid = false; // 分割符是最右边的子串, 左子串已经压入数组,无需再考虑;
            break;
        }
    }

    if (bLastStrValid) {
        result.push_back(tmpStr);
    }

    return result;
}

inline vector<string> StringSplit(const string &s, vector<string>& vecSeperator){
    vector<string> srcResult = {s};
    for (auto seperator:vecSeperator) {
        vector<string> curResult;
        for (auto tmpStr:srcResult) {
            vector<string> tmpResult = StringSplit(tmpStr, seperator);
            VecExtend(curResult, tmpResult);
            // cout << "OriStr: " << tmpStr << " After Sepate:" << seperator << endl;
            // VecPrint(tmpResult);
        }
        srcResult = curResult;
        // cout << "After Parse: " << seperator << endl;
        // VecPrint(srcResult);

    }
    return srcResult;
}

