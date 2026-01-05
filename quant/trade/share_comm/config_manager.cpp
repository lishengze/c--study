#include "config_manager.h"
#include <iostream>
#include <fstream>

#include "logger.h"

using namespace std;

bool ConfigManager::Init() {
    Error error;

    if (!GetJsonFromFile(reqJsonData_, sConfigFileName_)) {
        LOG_ERROR("JsonStructHelper::Init, ParseJsonFile:{} failed", sConfigFileName_);
        return false;
    }

    if (reqJsonData_.contains("StrategyList") && reqJsonData_["StrategyList"].is_array()) {
        for (auto& strategy : reqJsonData_["StrategyList"]) {
            if (strategy.is_string()) {
                setStrategySet_.insert(strategy.get<my_string>());
            }
        }
    } else {
        LOG_ERROR("ConfigManager::Init, StrategyList is empty");
        return false;
    }




    if (reqJsonData_.contains("Instruction") && reqJsonData_["Instruction"].contains("StockDic")) {
        njson& stock_dic = reqJsonData_["Instruction"]["StockDic"];
        for (auto& it : stock_dic.items()) {
            mapStockIndex_[it.key()] = it.value().get<int>();
        }
    } else {
        LOG_ERROR("ConfigManager::Init, StockDic is empty");
        return false;
    }

    if (reqJsonData_.contains("Instruction") && reqJsonData_["Instruction"].contains("IndicatorDic")) {
        njson& indicator_dic = reqJsonData_["Instruction"]["IndicatorDic"];
        for (auto& it : indicator_dic.items()) {
            mapIndicatorIndex_[it.key()] = it.value().get<int>();
        }
    } else {
        LOG_ERROR("ConfigManager::Init, IndicatorDic is empty");
        return false;
    }

    
    if (reqJsonData_.contains("Kline") && reqJsonData_["Kline"].contains("KlineFreqList")) {
        for (auto& freq : reqJsonData_["Kline"]["KlineFreqList"]) {
            if (freq.is_number()) {
                setKlineFreqSet_.insert(freq.get<int>());
            }
        }
    } else {
        LOG_ERROR("ConfigManager::Init, KlineFreqSet is empty");
        return false;
    }


    if (reqJsonData_.contains("Kline") && reqJsonData_["Kline"].contains("IndicatorList")) {
        for (auto& freq : reqJsonData_["Kline"]["IndicatorList"]) {
            if (freq.is_number()) {
                setKlineFreqSet_.insert(freq.get<int>());
            }
        }
    } else {
        LOG_ERROR("ConfigManager::Init, KlineFreqSet is empty");
        return false;
    }


    return true;
} 

void ConfigManager::RefreshConfig() {
    if (!Init()) {
        LOG_ERROR("ConfigManager::RefreshConfig, Init failed");
        return;
    }
}

int ConfigManager::GetIntValue(const my_string& section, const my_string& key, int default_value) {

    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_number()) {
        return reqJsonData_[section][key].get<int>();
    }

    return default_value;
}
double ConfigManager::GetDoubleValue(const my_string& section, const my_string& key, double default_value) {
    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_number()) {
        return reqJsonData_[section][key].get<double>();
    }
    return default_value;
}
my_string ConfigManager::GetStringValue(const my_string& section, const my_string& key, const my_string& default_value) {
    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_string()) {
        return reqJsonData_[section][key].get<my_string>();
    }
    return default_value;
}

my_vector<my_string> ConfigManager::GetStringListValue(const my_string& section, const my_string& key) {
    RefreshConfig();

    std::vector<my_string> ret;

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_array()) {
        // return reqJsonData_[section][key].get<std::vector<my_string>>();
    }
    return ret;
}

my_vector<int> ConfigManager::GetIntListValue(const my_string& section, const my_string& key) {
    RefreshConfig();

    my_vector<int> ret;

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_array()) {
        // return reqJsonData_[section][key].get<std::vector<int>>();
    }
    return ret;
}
