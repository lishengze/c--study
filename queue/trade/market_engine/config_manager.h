#pragma once
#include "thread_safe_singleton.h"

class ConfigManager {
public:
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
