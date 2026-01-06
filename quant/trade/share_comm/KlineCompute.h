#pragma once

#include "comm_define.h"
#include "thread_pool.h"
#include "logger.h"
#include "share_comm_external_message.h"

class KLineDataManager;

class IKlineCompute {
public:
    virtual ~IKlineCompute() = default;

    bool set_kline_data_manager(KLineDataManager* kline_data_manager, KlineIndicatorType indicator_type) {
        if (kline_data_manager == nullptr) {
            return false;
        }
        kline_data_manager_ = kline_data_manager;   
        indicator_type_ = indicator_type;
        return true;
    }

    virtual bool StartCompute() {
        return true;
    }

    KLineDataManager* get_kline_data_manager() {
        return kline_data_manager_;
    }

    int iMinimumDataCount;

private:
    KLineDataManager* kline_data_manager_;
    KlineIndicatorType indicator_type_;

};

class KlineCompute_1 : public IKlineCompute {
public:

    KlineCompute_1() {
        iMinimumDataCount = 20;
    }

    bool StartCompute() override {
        // LOG_INFO("KlineCompute_1::StartCompute");

        CallerObj caller_obj = std::bind(&KlineCompute_1::calculate_kline_indicator, this);
        THREAD_POOL_SIMPLE->post_call(caller_obj);

        // THREAD_POOL_SIMPLE->submit_noreturn(&KlineCompute_1::calculate_kline_indicator, this);

        return true;
    }

    bool calculate_kline_indicator();
};


class KlineCompute_10 : public IKlineCompute {
public:
    KlineCompute_10() {
        iMinimumDataCount = 5;
    }

    bool StartCompute() override {

        // LOG_INFO("KlineCompute_10::StartCompute");

        CallerObj caller_obj = std::bind(&KlineCompute_10::calculate_kline_indicator, this);
        THREAD_POOL_SIMPLE->post_call(caller_obj);

        // THREAD_POOL_SIMPLE->submit_noreturn(&KlineCompute_10::calculate_kline_indicator, this);

        return true;
    }

    bool calculate_kline_indicator();
};

class KlineCompute_36 : public IKlineCompute {
public:
    KlineCompute_36() {
        iMinimumDataCount = 10;
    }
    bool StartCompute() override {
        // LOG_INFO("KlineCompute_36::StartCompute");
        // THREAD_POOL_SIMPLE->submit_noreturn(&KlineCompute_36::calculate_kline_indicator, this);

        CallerObj caller_obj = std::bind(&KlineCompute_36::calculate_kline_indicator, this);
        THREAD_POOL_SIMPLE->post_call(caller_obj);

        return true;
    }

    bool calculate_kline_indicator();
};