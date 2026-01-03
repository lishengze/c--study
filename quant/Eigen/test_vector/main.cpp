#include "alpha_vector.h"
#include <iostream>
#include <random>

using namespace alpha101;

// 生成随机行情数据（股票数n_stocks，周期数n_periods）
MarketData gen_random_market_data(int n_stocks, int n_periods) {
    MarketData data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> price_dist(10.0f, 100.0f);
    std::uniform_real_distribution<float> vol_dist(1000.0f, 100000.0f);
    std::uniform_real_distribution<float> ret_dist(-0.05f, 0.05f);

    // 初始化vector容量（避免动态扩容开销）
    data.open.resize(n_stocks);
    data.high.resize(n_stocks);
    data.low.resize(n_stocks);
    data.close.resize(n_stocks);
    data.volume.resize(n_stocks);
    data.returns.resize(n_stocks);
    for (int i = 0; i < n_stocks; ++i) {
        data.open[i].resize(n_periods);
        data.high[i].resize(n_periods);
        data.low[i].resize(n_periods);
        data.close[i].resize(n_periods);
        data.volume[i].resize(n_periods);
        data.returns[i].resize(n_periods);
    }

    // 填充数据
    for (int i = 0; i < n_stocks; ++i) {
        float prev_close = price_dist(gen);
        for (int j = 0; j < n_periods; ++j) {
            data.close[i][j] = price_dist(gen);
            data.open[i][j] = price_dist(gen);
            data.high[i][j] = std::max(data.open[i][j], data.close[i][j]) + price_dist(gen) * 0.05f;
            data.low[i][j] = std::min(data.open[i][j], data.close[i][j]) - price_dist(gen) * 0.05f;
            data.volume[i][j] = vol_dist(gen);
            data.returns[i][j] = (data.close[i][j] / prev_close) - 1;
            prev_close = data.close[i][j];
        }
    }
    return data;
}

int main() {
    // 模拟量化常用规模：5只股票，60个交易日
    const int n_stocks = 5;
    const int n_periods = 60;
    MarketData data = gen_random_market_data(n_stocks, n_periods);

    // 计算因子
    auto alpha001 = calc_alpha001(data);
    auto alpha010 = calc_alpha010(data);
    auto alpha036 = calc_alpha036(data);

    // 输出最后10个周期因子值（验证正确性）
    std::cout << "=== Alpha001 因子值（5只股票×最后10天）===" << std::endl;
    for (int i = 0; i < n_stocks; ++i) {
        for (int j = n_periods - 10; j < n_periods; ++j) {
            printf("%.4f ", alpha001[i][j]);
        }
        std::cout << std::endl;
    }

    std::cout << "\n=== Alpha010 因子值（5只股票×最后10天）===" << std::endl;
    for (int i = 0; i < n_stocks; ++i) {
        for (int j = n_periods - 10; j < n_periods; ++j) {
            printf("%.4f ", alpha010[i][j]);
        }
        std::cout << std::endl;
    }

    std::cout << "\n=== Alpha036 因子值（5只股票×最后10天）===" << std::endl;
    for (int i = 0; i < n_stocks; ++i) {
        for (int j = n_periods - 10; j < n_periods; ++j) {
            printf("%.4f ", alpha036[i][j]);
        }
        std::cout << std::endl;
    }

    return 0;
}