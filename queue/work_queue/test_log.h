#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <set>
#include <sstream> 
#include <map>
#include <memory>
 using std::ios;

#include "thread_safe_singleton.hpp"
#include "json.hpp"
#include "json_util.hpp"
#include "base_util.h"

using std::string;
using std::cout;
using std::endl;
using std::map;
using std::shared_ptr;

inline string getSimpleFileName(string path) {
    string flag = "/";
#if defined(_WIN32) || defined(_WIN64)
	flag = "\\";
#endif
    string::size_type i = path.find_last_of(flag);

	// return path;

	if (i != std::string::npos && i < path.size()) {
		return path.substr(i, path.size() -i);
	} else {
		return path;
	}
    
}

#define LOG_HEADER SecTimeStr() + getSimpleFileName(std::string(__FILE__)) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + ":"

enum LOG_LEVEL {
	eDetail = 0,
	eDebug = 1,
	eWarn = 2,
	eError = 3
};

struct TestDetailResult {
	TestDetailResult() {
		bHasException = false;
		vecCostMicroSecs.clear();
	}
    njson jsData;
    std::vector<double> vecCostMicroSecs;
	bool bHasException;
};

using TestDetailResultShrPtr = std::shared_ptr<TestDetailResult>;

class TestLog {
    public:

		TestLog();

		bool Init(bool bTestMain=false);

		virtual ~TestLog();

		void logInfo(string caseName, string caseInfo);

		void logInfo(string caseName, njson& caseInfo);

		void logInfo(string caseName, std::map<int, TestDetailResultShrPtr>& caseMap);

		void logResut(string sCaseInfo, bool bResult, string sErrMsg="");

		void logResult(string sRstInfo);

		void logInfoBase(int iLogLevel,  string info);

		void logInfoBase(int iLogLevel,  string info, std::mutex& logMutex);

public:
	string	     sRstFileName;

	std::fstream fsJsonRst_;
	njson		 jsRst_; // 记录所有的测试案例，最终以json的形式输出到文件中;

	std::fstream fsLogRst_;

	std::fstream fsDetail_;
	std::fstream fsDebug_;
	std::fstream fsWarn_;
	std::fstream fsError_;

	std::map<int, string> mapLogLevelInfo_;

	std::vector<std::fstream*> vecFSHandler_;

	bool 		 bJsonLogRecorded_;

	std::set<std::string> setSuccessCase_;
	std::set<std::string> setFailedCase_;

	std::set<std::string> setSuccessSuitCase_;
	std::set<std::string> setFailedSuitCase_;	
};



#define TEST_LOG ThreadSafeSingleton<TestLog>::DoubleCheckInstance()

#define TEST_LOG_INFO(sCaseName, jsonCaseRst) TEST_LOG->logInfo(sCaseName, jsonCaseRst);
#define LOG_RST_PASS(sCaseInfo) TEST_LOG->logResut(sCaseInfo, true);
#define LOG_RST_FAILD(sCaseInfo, sErrMsg) TEST_LOG->logResut(sCaseInfo, false, sErrMsg);
#define LOG_RST(info) TEST_LOG->logResult(info);
// #define TEST_LOG_DETAIL(info) TEST_LOG->logInfoBase(0, info);

#define TEST_LOG_DETAIL(info) TEST_LOG->logInfoBase(0, LOG_HEADER + string("|") + info);
#define TEST_LOG_WARN(info) TEST_LOG->logInfoBase(1, LOG_HEADER + string("|") + info);
#define TEST_LOG_ERROR(info) TEST_LOG->logInfoBase(2, LOG_HEADER + string("|") + info);
#define TEST_LOG_FAIL(info) TEST_LOG->logInfoBase(3, LOG_HEADER + string("|") + info);

#define TEST_LOG_DETAIL_THREADS(info, mutex) TEST_LOG->logInfoBase(0, LOG_HEADER + string("|") + info, mutex);
#define TEST_LOG_DEBUG_THREADS(info, mutex) TEST_LOG->logInfoBase(1, LOG_HEADER + string("|") + info, mutex);
#define TEST_LOG_ERROR_THREADS(info,mutex) TEST_LOG->logInfoBase(2, LOG_HEADER + string("|") + info, mutex);
#define TEST_LOG_FAIL_THREADS(info, mutex) TEST_LOG->logInfoBase(3, LOG_HEADER + string("|") + info, mutex);