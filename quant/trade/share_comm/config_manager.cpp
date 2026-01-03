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

    return true;
} 

void ConfigManager::RefreshConfig() {
    if (!Init()) {
        LOG_ERROR("ConfigManager::RefreshConfig, Init failed");
        return;
    }
}

int ConfigManager::GetIntValue(const std::string& section, const std::string& key, int default_value) {

    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_number()) {
        return reqJsonData_[section][key].get<int>();
    }

    return default_value;
}
double ConfigManager::GetDoubleValue(const std::string& section, const std::string& key, double default_value) {
    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_number()) {
        return reqJsonData_[section][key].get<double>();
    }
    return default_value;
}
std::string ConfigManager::GetStringValue(const std::string& section, const std::string& key, const std::string& default_value) {
    RefreshConfig();

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_string()) {
        return reqJsonData_[section][key].get<std::string>();
    }
    return default_value;
}

std::vector<std::string> ConfigManager::GetStringListValue(const std::string& section, const std::string& key) {
    RefreshConfig();

    std::vector<std::string> ret;

    if (reqJsonData_.contains(section) && reqJsonData_[section].contains(key) && reqJsonData_[section][key].is_array()) {
        return reqJsonData_[section][key].get<std::vector<std::string>>();
    }
    return ret;
}
