#pragma once
#include "comm_define.h"

#include "thread_safe_singleton.h"
#include "json_util.hpp"

class ConfigManager {
public:
    ConfigManager() : sConfigFileName_("config.json") {}

    bool Init();

    int GetIntValue(const my_string& section, const my_string& key, int default_value);

    my_string GetStringValue(const my_string& section, const my_string& key, const my_string& default_value);

    double GetDoubleValue(const my_string& section, const my_string& key, double default_value);

    void RefreshConfig();

    my_vector<my_string> GetStringListValue(const my_string& section, const my_string& key);

    my_vector<int> GetIntListValue(const my_string& section, const my_string& key);

private:
    my_string sConfigFileName_;
    njson reqJsonData_;
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
