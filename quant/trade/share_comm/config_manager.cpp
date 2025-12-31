#pragma once
#include "config_manager.h"
#include <iostream>
#include <fstream>


void ConfigManager::Init() {

} 

int ConfigManager::GetIntValue(const std::string& section, const std::string& key, int default_value) {


    return default_value;
}
double ConfigManager::GetDoubleValue(const std::string& section, const std::string& key, double default_value) {

    return default_value;
}
std::string ConfigManager::GetStringValue(const std::string& section, const std::string& key, const std::string& default_value) {

    return default_value;
}

