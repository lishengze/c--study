#include "market_data.h"

#include "KlineCompute.h"

#include "config_manager.h"

#define INITTRIAL_KLINE_COMPUTE(TRIAL_UNIT) do{ \
    KlineCompute_##TRIAL_UNIT *tmp = new KlineCompute_##TRIAL_UNIT(); \
    tmp->set_kline_data_manager(this);\
    mapKlineIndicatorCompute_[KlineIndicatorType(TRIAL_UNIT)] = tmp;\      
}while(0)

void KLineDataManager::Init() {
    my_vector<int> vecIndicatorTypes = CONFIG_MANAGER_INSTANCE->GetIntListValue("kline", "indicator_types");

    for (int indicatorType : vecIndicatorTypes) {
        switch (indicatorType)
        {
        case 1:
            INITTRIAL_KLINE_COMPUTE(1);
            break;
        case 2:
            INITTRIAL_KLINE_COMPUTE(2);
            break;
        case 3:
            INITTRIAL_KLINE_COMPUTE(3);
            break;
        case 4:
            INITTRIAL_KLINE_COMPUTE(4);
            break;
        case 5:
            INITTRIAL_KLINE_COMPUTE(5);
            break;
        default:
            break;
        }
    }
}

void KLineDataManager::StartCalculateKlineIndicator() {
    for (auto iter: mapKlineIndicatorCompute_) {
        iter.second->StartCompute();
    }
}

void KLineDataManager::UpdateKlineAtom() {
    
}