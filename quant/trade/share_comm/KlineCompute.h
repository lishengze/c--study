#pragma once

#include "thread_pool.h"

class KLineDataManager;

class IKlineCompute {
public:
    virtual ~IKlineCompute() = default;

    bool set_kline_data_manager(KLineDataManager* kline_data_manager) {
        if (kline_data_manager_ == nullptr) {
            return false;
        }
        kline_data_manager_ = kline_data_manager;   
        return true;
    }

    virtual bool StartCompute() {
        return true;
    }

private:
    KLineDataManager* kline_data_manager_;

};

class KlineCompute_1 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }
};


class KlineCompute_2 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }
};


class KlineCompute_3 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }
};


class KlineCompute_4 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }
};


class KlineCompute_5 : public IKlineCompute {
public:
    bool StartCompute() override {
        return true;
    }
};

