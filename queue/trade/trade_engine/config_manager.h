#pragma once
#include "thread_safe_singleton.h"

class ConfigManager {
public:
    void Init();

    int GetIntValue(const std::string& section, const std::string& key, int default_value);
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
