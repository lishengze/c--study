#pragma once

#include "comm_define.h"
#include "share_comm_external_message.h"
#include "config_manager.h"


class IKlineCompute;

/// @brief 用于存储并计算K线数据 -- 支持多股票多周K线数据;
struct KLineDataManager {
    KLineDataManager(BarFrequency iFrequency) : data_count_(0), iFrequency_(iFrequency) {

        data_limit_ = GetDataLimit(iFrequency);

        // my_set<my_string> stock_set  = CONFIG_MANAGER_INSTANCE->GetStockSet();

        my_unorder_map<my_string, int> mapStockIndex = CONFIG_MANAGER_INSTANCE->GetStockIndexDic();


        vecOpen.resize(mapStockIndex.size());
        vecHigh.resize(mapStockIndex.size());
        vecLow.resize(mapStockIndex.size());
        vecClose.resize(mapStockIndex.size());
        vecVolume.resize(mapStockIndex.size());
        vecAmount.resize(mapStockIndex.size());

        for (int i = 0; i < mapStockIndex.size(); ++i) {
            vecOpen[i].reserve(data_limit_);
            vecHigh[i].reserve(data_limit_);
            vecLow[i].reserve(data_limit_);
            vecClose[i].reserve(data_limit_);
            vecVolume[i].reserve(data_limit_);
            vecAmount[i].reserve(data_limit_);
        }

        vecLatestKlineAtom.resize(mapStockIndex.size());

        srand((unsigned int)time(NULL));
    }

    /// @brief  根据 depth 数据聚合K线数据
    /// @param vecKlineAtomSrc 
    my_vector<KlineAtomSharedPtr> AggregateKline(const my_vector<DepthDataAtomSharedPtr>& vecKlineAtomSrc);

    /// @brief  根据 Kline 高频数据聚合K线低频数据
    /// @param vecKlineAtomSrc 
    my_vector<KlineAtomSharedPtr> AggregateKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc);

    /// @brief 根据配置初始化K线指标计算类
    void Init();

    void AddKlineAtom(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc) {
        if (vecKlineAtomSrc.empty()) {
            return;
        }

        {
            // 加锁，确保线程安全;
            std::lock_guard<std::mutex> lock(update_indicator_mutex_);
            for (auto kline_atom: vecKlineAtomSrc) {
                // LOG_INFO("AddKlineAtom: {}", kline_atom->str());

                vecOpen[kline_atom->stock_index].push_back(kline_atom->open_price);
                vecHigh[kline_atom->stock_index].push_back(kline_atom->high_price);
                vecLow[kline_atom->stock_index].push_back(kline_atom->low_price);
                vecClose[kline_atom->stock_index].push_back(kline_atom->close_price);
                vecVolume[kline_atom->stock_index].push_back(kline_atom->volume);
                vecAmount[kline_atom->stock_index].push_back(kline_atom->amount);

                vecLatestKlineAtom[kline_atom->stock_index] = kline_atom;
            }
            data_count_++;            
        }


        LOG_DEBUG("AddKlineAtom:  data_count_: {}", data_count_);

        StartCalculateKlineIndicator();
    }

    int GetDataCount() {
        return data_count_;
    }

    void UpdateKlineIndicator(my_vector<float>& vecCurIndicatorValue, KlineIndicatorType indicator_type);

    void StartCalculateKlineIndicator();

    void SetKlineCallback(KlineVectorCallbackFuncType funcKlineVectorCallback) {
        funcKlineVectorComputeDoneCallback_ = funcKlineVectorCallback;
    }


    unsigned int data_count_;
    unsigned int data_limit_;    
    BarFrequency iFrequency_;   // 当前K线数据的频率;

    my_vector<my_vector<float>> vecOpen;        // 开盘价
    my_vector<my_vector<float>> vecHigh;        // 最高价
    my_vector<my_vector<float>> vecLow;        // 最低价
    my_vector<my_vector<float>> vecClose;        // 收盘价
    my_vector<my_vector<float>> vecVolume;        // 成交量
    my_vector<my_vector<float>> vecAmount;        // 成交额    

    my_vector<KlineAtomSharedPtr> vecLatestKlineAtom; // 存储当前最新的K线数据，更新分为两步骤，1. 根据depth 数据聚合K线数据：2. 根据配置计算相关指标;
    my_vector<my_vector<DepthDataAtomSharedPtr>> vecDepthAtom; // 存储当前累积的depth 数据，用于聚合K线数据;

    my_unorder_map<KlineIndicatorType, IKlineCompute*> mapKlineIndicatorCompute_; // 存储当前配置需要计算的K线指标类型;

    my_mutex update_indicator_mutex_; // 用于更新K线指标的互斥锁;
    my_set<KlineIndicatorType> setKlineIndicatorType_; // 用于记录当前需要计算的K线指标类型;


    KlineVectorCallbackFuncType funcKlineVectorComputeDoneCallback_; // K线数据回调函数 
};

using KLineDataManagerSharePtr = std::shared_ptr<KLineDataManager>;

class MarketDataManager {
    // my_unorder_map<std::string, std::vector<MarketData>> market_data_map;
public:

    bool Init() {
        my_set<int> vecIndicatorTypes = CONFIG_MANAGER_INSTANCE->GetKlineFreqSet();

        for (int indicatorType : vecIndicatorTypes) {
            KLineDataManagerSharePtr kline_data_manager = std::make_shared<KLineDataManager>(BarFrequency(indicatorType));
            kline_data_manager->Init();
            kline_data_map_[indicatorType] = kline_data_manager;
        }

        return true;
    }

    bool Start() {
        LOG_INFO("MarketDataManager Start");
        return true;
    }

    void AggrateKline(my_vector<DepthDataAtomSharedPtr>& vecDepthAtomSrc) {
        for (auto iter: kline_data_map_) {
            my_vector<KlineAtomSharedPtr> vecKlineAtom = iter.second->AggregateKline(vecDepthAtomSrc);  
            iter.second->AddKlineAtom(vecKlineAtom);
        }
    }

    void AggrateKline(my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc) {
        for (auto iter: kline_data_map_) {
            my_vector<KlineAtomSharedPtr> vecKlineAtom = iter.second->AggregateKline(vecKlineAtomSrc);  
            iter.second->AddKlineAtom(vecKlineAtom);
        }
    }    

    void ProcessVecKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc);

    void SetKlineCallback(KlineVectorCallbackFuncType funcKlineVectorCallback) {
        funcKlineVectorCallback_ = funcKlineVectorCallback;
        for (auto iter: kline_data_map_) {
            iter.second->SetKlineCallback(funcKlineVectorCallback_);
        }
    }

private:
    DepthData depth_data_;
    my_unorder_map<int, KLineDataManagerSharePtr> kline_data_map_; // 根据配置确定要处理的Kline 数据类型;

    KlineVectorCallbackFuncType funcKlineVectorCallback_; // K线数据回调函数 -- 这里

};

using MarketDataManagerSharePtr = std::shared_ptr<MarketDataManager>;
