#pragma once
#include "thread_safe_singleton.h"
#include <string>
#include "json_util.hpp"

class ConfigManager {
public:
    ConfigManager() : sConfigFileName_("config.json") {}

    bool Init();

    int GetIntValue(const std::string& section, const std::string& key, int default_value);

    std::string GetStringValue(const std::string& section, const std::string& key, const std::string& default_value);

    double GetDoubleValue(const std::string& section, const std::string& key, double default_value);

    void RefreshConfig();

    std::vector<std::string> GetStringListValue(const std::string& section, const std::string& key);


private:
    std::string sConfigFileName_;
    njson reqJsonData_;
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
