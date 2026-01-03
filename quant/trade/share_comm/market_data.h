#pragma once

#include <cstring>
#include <array>
#include <functional>
#include <algorithm>
#include <string>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <mutex>
#include "share_comm_external_message.h"


// 模板化通用别名，可复用
template <typename T>
using my_vector = std::vector<T>;

template <typename key, typename value>
using my_unorder_map = std::unordered_map<key, value>;

using my_mutex = std::mutex;


// 行情频率枚举（支持扩展，新增频率只需在此添加）
enum class BarFrequency {
    TICK = 0,        // 最细粒度：Tick数据（逐笔）
    MINUTE_1 = 1,    // 1分钟线
    MINUTE_5 = 5,    // 5分钟线
    HOUR_1 = 60,     // 1小时线
    DAY = 1440,      // 日线（1440分钟）
    WEEK = 10080     // 周线（10080分钟）
};

struct DepthDataAtom {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    double bid_price[5]; // 买盘价格
    double bid_volume[5]; // 买盘成交量
    double ask_price[5]; // 卖盘价格
    double ask_volume[5]; // 卖盘成交量
    unsigned long long timestamp; // 时间戳（纳秒）
    unsigned long long ulID;
};

using DepthDataAtomSharedPtr = std::shared_ptr<DepthDataAtom>;

struct DepthData {
    my_vector<my_vector<DepthDataAtom>> depth_data_atoms;   // 深度数据，第一层代表股票代码，第二层代码同一个股票代码，多个时刻的深度数据
    my_vector<my_mutex> depth_mutex_;

};
using DepthDataSharePtr = std::shared_ptr<DepthData>;


unsigned int GetDataLimit(BarFrequency iFrequency) {
    switch (iFrequency) {
        case BarFrequency::TICK:
            return 1000000;
        case BarFrequency::MINUTE_1:
            return 1000000;
        case BarFrequency::MINUTE_5:
            return 1000000;
        case BarFrequency::HOUR_1:
            return 1000000;
        case BarFrequency::DAY:
            return 1000000;
        case BarFrequency::WEEK:
            return 1000000;
        default:
            return 1000000;
    }
}

struct KlineAtom {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    double open_price; // 开盘价
    double high_price; // 最高价
    double low_price; // 最低价
    double close_price; // 收盘价
    double volume; // 成交量
    double amount; // 成交额
    unsigned long long timestamp; // 时间戳（纳秒）
    int bar_index; // 时间戳对应的K线索引, 1,5,60,1440, 10080;
    unsigned short stock_index; // 股票索引
};

struct KLineData {
    KLineData(BarFrequency iFrequency) : data_count_(0), iFrequency_(iFrequency) {

        data_limit_ = GetDataLimit(iFrequency);
        vecOpen.resize(data_limit_);
        vecHigh.resize(data_limit_);
        vecLow.resize(data_limit_);
        vecClose.resize(data_limit_);
        vecVolume.resize(data_limit_);
        vecAmount.resize(data_limit_);
    }


    void UpdateKlineAtom(const KlineAtom& kline_atom) {
        vecOpen[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.open_price;
        vecHigh[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.high_price;
        vecLow[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.low_price;
        vecClose[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.close_price;
        vecVolume[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.volume;
        vecAmount[kline_atom.stock_index][kline_atom.bar_index] = kline_atom.amount;
        data_count_++;
    }


    unsigned int data_count_;
    unsigned int data_limit_;    
    BarFrequency iFrequency_;

    my_vector<my_vector<float>> vecOpen; // 开盘价
    my_vector<my_vector<float>> vecHigh; // 最高价
    my_vector<my_vector<float>> vecLow; // 最低价
    my_vector<my_vector<float>> vecClose; // 收盘价
    my_vector<my_vector<float>> vecVolume; // 成交量
    my_vector<my_vector<float>> vecAmount; // 成交额    
};

using KLineDataSharePtr = std::shared_ptr<KLineData>;

class MarketDataManager {
    // my_unorder_map<std::string, std::vector<MarketData>> market_data_map;
public:
    void AggrateKline(my_vector<DepthDataAtomSharedPtr>& vecDepthAtomSrc) {

    }


private:
    DepthData depth_data_;
    my_unorder_map<int, KLineDataSharePtr> kline_data_map_; // 根据配置确定要处理的Kline 数据类型;

};
