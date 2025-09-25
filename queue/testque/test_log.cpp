#include "test_log.h"

TestLog::TestLog() {
    mapLogLevelInfo_[0] = "[Detail]";
    mapLogLevelInfo_[1] = "[Debug]";
    mapLogLevelInfo_[2] = "[Warn]";
    mapLogLevelInfo_[3] = "[Error]";


    setSuccessCase_.clear();
    setFailedCase_.clear();
    setSuccessSuitCase_.clear();
    setFailedSuitCase_.clear();
}


TestLog::~TestLog() {
    if (fsJsonRst_.is_open()) {
        fsJsonRst_ << "\n]\n";
        fsJsonRst_.close();
    }			

    if (fsLogRst_.is_open()) {
        fsLogRst_.close();
    }

    if (fsDetail_.is_open()) {
        fsDetail_.close();
    }	

    if (fsDebug_.is_open()) {
        fsDebug_.close();
    }	

    if (fsWarn_.is_open()) {
        fsWarn_.close();
    }	

    if (fsError_.is_open()) {
        fsError_.close();
    }												
} 

bool TestLog::Init(bool bTestMain) {

    if (bTestMain) {
        fsJsonRst_.open("test_result.json", std::ios::out);
        if (!fsJsonRst_.is_open()) {
            cout << "Open test_result.json Failed!" << endl;
            return false;
        }
        fsJsonRst_ << "[\n";


    }

    fsLogRst_.open("test_result.log", std::ios::out);	
    if (!fsLogRst_.is_open()) {
        cout << "Open test_result.log Failed!" << endl;
        return false;
    }

    fsDetail_.open("test_detail.log", std::ios::out);		
    if (!fsDetail_.is_open()) {
        cout << "Open test_detail.log Failed!" << endl;
        return false;
    }		

    fsDebug_.open("test_debug.log", std::ios::out);		
    if (!fsDebug_.is_open()) {
        cout << "Open test_debug.log Failed!" << endl;
        return false;
    }	

    fsWarn_.open("test_warn.log", std::ios::out);		
    if (!fsWarn_.is_open()) {
        cout << "Open test_warn.log Failed!" << endl;
        return false;
    }	

    fsError_.open("test_error.log", std::ios::out);		
    if (!fsError_.is_open()) {
        cout << "Open test_error.log Failed!" << endl;
        return false;
    }		

    vecFSHandler_.push_back(&fsDetail_);																
    vecFSHandler_.push_back(&fsDebug_);	
    vecFSHandler_.push_back(&fsWarn_);	
    vecFSHandler_.push_back(&fsError_);	

    bJsonLogRecorded_ = false;

    setSuccessCase_.clear();
    setFailedCase_.clear();	

    return true;		
}

void TestLog::logInfo(string caseName, string caseInfo) {
    try
    {			
        
        jsRst_[caseName] = GetUtf8String(caseInfo);				
        string info = jsRst_.dump(2);
        info = GetGBKString(info);
        cout << "\n" << info << endl;

        if (fsJsonRst_.is_open()) {

            if (bJsonLogRecorded_) {
                fsJsonRst_ << ",\n" << info; 
            } else {
                fsJsonRst_ << "\n" << info;
            }
        }



        bJsonLogRecorded_ = true;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"logInfo string " << caseName << ", " << e.what() << '\n';
    }

}

void TestLog::logInfo(string caseName, njson& caseInfo) {
    try
    {
        jsRst_[caseName] = caseInfo;
        string info = jsRst_.dump(2);
        info = GetGBKString(info);
        // cout << "\n" << info << endl;

        if (fsJsonRst_.is_open()) {
            if (bJsonLogRecorded_) {
                fsJsonRst_ << ",\n" << info;
            } else {
                fsJsonRst_ << "\n" << info;
            }
        }

        bJsonLogRecorded_ = true;

        if (caseInfo.is_array()) {
            for (njson::iterator it = caseInfo.begin(); it != caseInfo.end(); ++it)
            {
                njson jsAtom = *it;

                if (!jsAtom["info"].is_string()) {
                    logInfoBase(3, caseName + " need info!");
                    continue;
                }

                string info = GET_JSON_STR_FIELD(jsAtom, "info");

                if (!jsAtom["result"].is_string()) {
                    logInfoBase(2,  caseName + " need result!");
                    continue;
                } 

                string result =  GET_JSON_STR_FIELD(jsAtom, "result");			

                if (result == "SUCCEED") {
                    setSuccessCase_.insert(caseName + "." + info);		
                    setSuccessSuitCase_.insert(caseName);				
                } else if (result == "FAILED") {
                    setFailedCase_.insert(caseName + "." + info);
                    setFailedSuitCase_.insert(caseName);
                } else {
                    logInfoBase(2, caseName + " result is not clear, should be SUCCEED or FAILED");
                }						

            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr <<"logInfo json " << caseName << ", " << e.what() << '\n';
    }
}

void TestLog::logInfo(string caseName, std::map<int, TestDetailResultShrPtr>& caseMap) {
    try
    {
        njson jsonArray;
        for (auto iter:caseMap) {
            jsonArray.push_back(iter.second->jsData);
        }
        logInfo(caseName, jsonArray);

    }
    catch(const std::exception& e)
    {
        std::cerr <<"logInfo json " << caseName << ", " << e.what() << '\n';
    }
}	

void TestLog::logResult(string sRstInfo) {
    if (fsLogRst_.is_open()) {
        fsLogRst_  << sRstInfo;
        std::cout << sRstInfo << endl;
    }
}

void TestLog::logResut(string sCaseInfo, bool bResult, string sErrMsg) {
    try
    {
        string info ="";

        if (bResult) {
            info += "{\n\"" + sCaseInfo + "\": SUCCESSED" + "},\n"; 
        } else {
            info += "{\n\"" + sCaseInfo + "\": FAILED" + "\n"
                    + "\"Err\": "+ sErrMsg +"},\n"; 
        }

        if (fsJsonRst_.is_open()) {
            if (bJsonLogRecorded_) {
                fsJsonRst_ << ",\n" << info;
            } else {
                fsJsonRst_ << "\n" << info;
            }
        }

        bJsonLogRecorded_ = true;

        if (bResult) {
            setSuccessCase_.insert(sCaseInfo);						
        } else {
            setFailedCase_.insert(sCaseInfo);
        } 
    }
    catch(const std::exception& e)
    {
        std::cerr <<"logResut  " << sCaseInfo << ", " << e.what() << '\n';
    }
}

void TestLog::logInfoBase(int iLogLevel,  string info) {
    size_t pos = info.find_first_of("|");
    string sLeftStr = info.substr(0, pos);
    string sRightStr = info.substr(pos+1);
    string msg = sLeftStr + mapLogLevelInfo_[iLogLevel] + " " + sRightStr;
    for (int i = 0; i <= iLogLevel; ++i) {
        *(vecFSHandler_[i]) << msg << "\n";
    }
    cout << msg << endl;
}


void TestLog::logInfoBase(int iLogLevel,  string info,  std::mutex& logMutex) {
    std::lock_guard<std::mutex> lk(logMutex);

    size_t pos = info.find_first_of("|");
    string sLeftStr = info.substr(0, pos);
    string sRightStr = info.substr(pos+1);
    string msg = sLeftStr + mapLogLevelInfo_[iLogLevel] + " " + sRightStr;
    for (int i = 0; i <= iLogLevel; ++i) {
        *(vecFSHandler_[i]) << msg << "\n";
        *(vecFSHandler_[i]) << std::unitbuf;
    }
    cout << msg << endl;
}
