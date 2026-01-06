#include "market_data_manager.h"

#include "KlineCompute.h"

#include "config_manager.h"

#define INITTRIAL_KLINE_COMPUTE(TRIAL_UNIT) do{ \
    KlineCompute_##TRIAL_UNIT *tmp = new KlineCompute_##TRIAL_UNIT(); \
    tmp->set_kline_data_manager(this, KlineIndicatorType(TRIAL_UNIT));\
    mapKlineIndicatorCompute_[KlineIndicatorType(TRIAL_UNIT)] = tmp; \ 
}while(0)\

void KLineDataManager::Init() {
    my_set<int> vecIndicatorTypes = CONFIG_MANAGER_INSTANCE->GetIndicatorSet();

    for (int indicatorType : vecIndicatorTypes) {
        switch (indicatorType)
        {
        case 1:
            INITTRIAL_KLINE_COMPUTE(1);
            break;
        case 10:
            INITTRIAL_KLINE_COMPUTE(10);
            break;
        case 36:
            INITTRIAL_KLINE_COMPUTE(36);
            break;
        default:
            break;
        }
    }

    LOG_INFO("KLineDataManager Init, mapKlineIndicatorCompute_.size: {}, iFrequency_: {}", mapKlineIndicatorCompute_.size(), (int)iFrequency_);
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

void KLineDataManager::UpdateKlineIndicator(my_vector<float>& vecCurIndicatorValue, KlineIndicatorType indicator_type) {

    // 这一段代码线程安全;不同的线程更新的指标值不同;
    if (vecCurIndicatorValue.size() == vecLatestKlineAtom.size()) {
        for (int i = 0; i < vecCurIndicatorValue.size(); i++) {
            vecLatestKlineAtom[i]->UpdateKlineIndicatorValue(indicator_type, vecCurIndicatorValue[i]);
        }
    }

    {
        std::lock_guard<std::mutex> lock(update_indicator_mutex_);
        setKlineIndicatorType_.insert(indicator_type);

        // 所有指标计算完成;
        if (setKlineIndicatorType_.size() == mapKlineIndicatorCompute_.size()) {
            if (funcKlineVectorComputeDoneCallback_ != nullptr) {
                funcKlineVectorComputeDoneCallback_(vecLatestKlineAtom);
            } else {
                LOG_ERROR("MarketDataManager ProcessVecKline, funcKlineVectorComputeDoneCallback_ is nullptr");
            }
            setKlineIndicatorType_.clear();
        }

        
    }

}

void MarketDataManager::ProcessVecKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc) {
    if (vecKlineAtomSrc.empty()) {
        LOG_WARN("MarketDataManager ProcessVecKline, vecKlineAtomSrc empty");
        return; 
    }

    int iBarIndex = vecKlineAtomSrc[0]->bar_index;

    if (kline_data_map_.find(iBarIndex) != kline_data_map_.end()) {
        kline_data_map_[iBarIndex]->AddKlineAtom(vecKlineAtomSrc);
    } else {
        LOG_WARN("MarketDataManager ProcessVecKline, bar_index %d not found", iBarIndex);
    }
}