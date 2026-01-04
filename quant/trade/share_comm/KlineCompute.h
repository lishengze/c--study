#pragma once

#include "comm_define.h"
#include "thread_pool.h"

#include "share_comm_external_message.h"

class KLineDataManager;

class IKlineCompute {
public:
    virtual ~IKlineCompute() = default;

    bool set_kline_data_manager(KLineDataManager* kline_data_manager, KlineIndicatorType indicator_type) {
        if (kline_data_manager_ == nullptr) {
            return false;
        }
        kline_data_manager_ = kline_data_manager;   
        indicator_type_ = indicator_type;
        return true;
    }

    virtual bool StartCompute() {
        return true;
    }

private:
    KLineDataManager* kline_data_manager_;
    KlineIndicatorType indicator_type_;

};

class KlineCompute_1 : public IKlineCompute {
public:
    bool StartCompute() override {

        // THREAD_POOL_SIMPLE->submit(&KlineCompute_1::calculate_kline_indicator, this);

        return true;
    }

    bool calculate_kline_indicator();
};


class KlineCompute_2 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }

    bool calculate_kline_indicator();
};


class KlineCompute_3 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }

    bool calculate_kline_indicator();
};


class KlineCompute_4 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }

    bool calculate_kline_indicator();
};


class KlineCompute_5 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }

    bool calculate_kline_indicator();
};