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

    my_unorder_map<my_string, int>& GetStockIndexDic() { return mapStockIndex_; }

    my_set<my_string>& GetStrategySet() { return setStrategySet_; }

    my_unorder_map<my_string, int>& GetIndicatorIndexDic() { return mapIndicatorIndex_; }

    my_set<int>& GetKlineFreqSet() { return setKlineFreqSet_; }

    my_set<int>& GetIndicatorSet() { return setIndicatorSet_; }

    my_set<my_string>& GetStockSet() { return setStockSet_; }

private:
    my_string sConfigFileName_;
    njson reqJsonData_;

    my_unorder_map<my_string, int> mapStockIndex_ ;
    my_unorder_map<my_string, int> mapIndicatorIndex_ ;

    my_set<my_string> setStrategySet_;
    my_set<int> setKlineFreqSet_;
    my_set<int> setIndicatorSet_;
    my_set<my_string> setStockSet_;
};


#define CONFIG_MANAGER_INSTANCE ThreadSafeSingleton<ConfigManager>::Instance()
