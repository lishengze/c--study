#pragma once

#include "comm_define.h"
#include "share_comm_external_message.h"


class IKlineCompute;

/// @brief 用于存储并计算K线数据 -- 支持多股票多周K线数据;
struct KLineDataManager {
    KLineDataManager(BarFrequency iFrequency) : data_count_(0), iFrequency_(iFrequency) {

        data_limit_ = GetDataLimit(iFrequency);
        vecOpen.resize(data_limit_);
        vecHigh.resize(data_limit_);
        vecLow.resize(data_limit_);
        vecClose.resize(data_limit_);
        vecVolume.resize(data_limit_);
        vecAmount.resize(data_limit_);
    }

    /// @brief  根据 depth 数据聚合K线数据
    /// @param vecKlineAtomSrc 
    my_vector<KlineAtomSharedPtr> AggregateKline(const my_vector<DepthDataAtomSharedPtr>& vecKlineAtomSrc);

    /// @brief  根据 Kline 高频数据聚合K线低频数据
    /// @param vecKlineAtomSrc 
    my_vector<KlineAtomSharedPtr> AggregateKline(const my_vector<KlineAtomSharedPtr>& vecKlineAtomSrc);

    /// @brief 根据配置初始化K线指标计算类
    void Init();

    void AddKlineAtom(my_vector<KlineAtomSharedPtr>& vecKlineAtom) {
        if (vecKlineAtom.empty()) {
            return;
        }

        for (auto kline_atom: vecKlineAtom) {
            vecOpen[kline_atom->stock_index][data_count_] = kline_atom->open_price;
            vecHigh[kline_atom->stock_index][data_count_] = kline_atom->high_price;
            vecLow[kline_atom->stock_index][data_count_] = kline_atom->low_price;
            vecClose[kline_atom->stock_index][data_count_] = kline_atom->close_price;
            vecVolume[kline_atom->stock_index][data_count_] = kline_atom->volume;
            vecAmount[kline_atom->stock_index][data_count_] = kline_atom->amount;

            vecKlineAtom[kline_atom->stock_index] = kline_atom;
        }
        data_count_++;

        StartCalculateKlineIndicator();
    }

    void StartCalculateKlineIndicator();

    void UpdateKlineAtom(const my_vector<float>& vecCurIndicatorValue, KlineIndicatorType indicator_type);


    unsigned int data_count_;
    unsigned int data_limit_;    
    BarFrequency iFrequency_;   // 当前K线数据的频率;

    my_vector<my_vector<float>> vecOpen; // 开盘价
    my_vector<my_vector<float>> vecHigh; // 最高价
    my_vector<my_vector<float>> vecLow; // 最低价
    my_vector<my_vector<float>> vecClose; // 收盘价
    my_vector<my_vector<float>> vecVolume; // 成交量
    my_vector<my_vector<float>> vecAmount; // 成交额    

    my_vector<KlineAtomSharedPtr> vecKlineAtom; // 存储当前最新的K线数据，更新分为两步骤，第一步，根据depth 数据聚合K线数据：2. 根据配置计算相关指标;
    my_vector<my_vector<DepthDataAtomSharedPtr>> vecDepthAtom; // 存储当前累积的depth 数据，用于聚合K线数据;

    my_unorder_map<KlineIndicatorType, IKlineCompute*> mapKlineIndicatorCompute_; // 存储当前配置需要计算的K线指标类型;
};

using KLineDataManagerSharePtr = std::shared_ptr<KLineDataManager>;

class MarketDataManager {
    // my_unorder_map<std::string, std::vector<MarketData>> market_data_map;
public:
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


private:
    DepthData depth_data_;
    my_unorder_map<int, KLineDataManagerSharePtr> kline_data_map_; // 根据配置确定要处理的Kline 数据类型;

};
