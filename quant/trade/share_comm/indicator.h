#pragma once

enum class KlineIndicatorType {
    Alpha_001 = 1,        // 最细粒度：Tick数据（逐笔）
    Alpha_010 = 10,    // 1分钟线
    Alpha_036 = 36
};


/// @brief 代码根据配置自动生成;
struct IndicatorAtom {
    
    double alpha_1;
    double alpha_10;
    double alpha_36;

    IndicatorAtom() {}

    void UpdateIndicatorValue(KlineIndicatorType indicator_type, double value) {
        switch (indicator_type) {
            case KlineIndicatorType::Alpha_001:
                alpha_1 = value;
                break;
            case KlineIndicatorType::Alpha_010:
                alpha_10 = value;
                break;
            case KlineIndicatorType::Alpha_036:
                alpha_36 = value;
                break;
            default:
                break;
        }
    }
};
