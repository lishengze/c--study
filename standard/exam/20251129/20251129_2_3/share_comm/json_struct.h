#pragma once

#include "share_comm_external_message.h"
#include "json_util.hpp"
#include "thread_safe_singleton.h"

#include <iostream>
#include <sstream> 
#include <iomanip>
#include <iostream>
#include <vector>
using std::ios;
using namespace std;


#include "logger.h"

using namespace share_common;

class JsonMeta {
public:
    string Init(std::string strSrcJsonFileName) {
        Error error;
        njson reqJsonData;
        if (!GetJsonFromFile(reqJsonData, strSrcJsonFileName)) {
            LOG_ERROR("JsonStructHelper::Init, ParseJsonFile:{} failed", strSrcJsonFileName);
            return "";
        }
        
        strWriteName_ = "test_ute";
        uiReadKey_ = 1;
        uiWaitSec_ = 10;
        uiHeartbeatSec_ = 3;

        string sErrMsg;

        GetJsonUnsignedIntField(reqJsonData["read"], "key", uiReadKey_, sErrMsg);  
        GetJsonUnsignedIntField(reqJsonData["read"], "wait_sec", uiWaitSec_, sErrMsg);     
        GetJsonUnsignedIntField(reqJsonData["read"], "heartbeat_sec", uiHeartbeatSec_, sErrMsg);   

        GetJsonStringField(reqJsonData["write"], "write_process_name", strWriteName_, sErrMsg);   


        std::stringstream ssObj; 
        ssObj.setf(ios::left); 
        ssObj << std::fixed; 
        ssObj << std::setw(30) << "strWriteName_:" << strWriteName_ <<"\n";
        ssObj << std::setw(30) << "uiReadKey_:" << uiReadKey_ <<"\n";
        ssObj << std::setw(30) << "uiWaitSec_:" << uiWaitSec_ <<"\n";
        ssObj << std::setw(30) << "uiHeartbeatSec_:" << uiHeartbeatSec_ <<"\n";

        return ssObj.str();

    }  

    string strWriteName_;
    unsigned int uiReadKey_;
    unsigned int uiWaitSec_;
    unsigned int uiHeartbeatSec_;
};
