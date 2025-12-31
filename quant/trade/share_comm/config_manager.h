#pragma once
#include "thread_safe_singleton.h"
#include <string>

class ConfigManager {
public:
    void Init();

    int GetIntValue(const std::string& section, const std::string& key, int default_value);

    std::string GetStringValue(const std::string& section, const std::string& key, const std::string& default_value);

    double GetDoubleValue(const std::string& section, const std::string& key, double default_value);
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
