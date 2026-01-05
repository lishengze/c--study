#include "market_data_manager.h"

#include "KlineCompute.h"

#include "config_manager.h"

#define INITTRIAL_KLINE_COMPUTE(TRIAL_UNIT) do{ \
    KlineCompute_##TRIAL_UNIT *tmp = new KlineCompute_##TRIAL_UNIT(); \
    tmp->set_kline_data_manager(this, KlineIndicatorType(TRIAL_UNIT));\
    mapKlineIndicatorCompute_[KlineIndicatorType(TRIAL_UNIT)] = tmp;\      
}while(0)\

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

/// @brief 根据 depth 数据聚合K线数据
/// @param vecKlineAtomSrc 
/// @return 
my_vector<KlineAtomSharedPtr> KLineDataManager::AggregateKline(const my_vector<DepthDataAtomSharedPtr>& vecKlineAtomSrc) {
    my_vector<KlineAtomSharedPtr> ret;


    return ret;
}

/// @brief 根据 Kline 高频数据聚合K线低频数据
/// @param vecKlineAtomSrc 
/// @return 
my_vector<KlineAtomSharedPtr> KLineDataManager::AggregateKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc) {
    my_vector<KlineAtomSharedPtr> ret;


    return ret;
}


void KLineDataManager::StartCalculateKlineIndicator() {
    for (auto iter: mapKlineIndicatorCompute_) {
        iter.second->StartCompute();
    }
}

void KLineDataManager::UpdateKlineAtom(const my_vector<float>& vecCurIndicatorValue, KlineIndicatorType indicator_type) {
    
    for (int i = 0; i < vecCurIndicatorValue.size(); i++) {
        vecKlineAtom[i]->mapKlineIndicatorValue_[indicator_type] = vecCurIndicatorValue[i];
    }
}