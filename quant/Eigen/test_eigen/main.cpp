#include "alpha101_factors.h"
#include <iostream>

using namespace alpha101;
using namespace Eigen;

int main() {
    // 模拟数据：5只股票，60个交易日（量化常用测试规模）
    const int n_stocks = 5;
    const int n_periods = 60;
    MarketData data;
    // 随机初始化行情数据（实际替换为真实行情）
    data.open = MatrixXf::Random(n_stocks, n_periods);
    data.high = MatrixXf::Random(n_stocks, n_periods) * 10;
    data.low = MatrixXf::Random(n_stocks, n_periods) * 0.5;
    data.close = MatrixXf::Random(n_stocks, n_periods);
    data.volume = MatrixXf::Random(n_stocks, n_periods) * 1000;
    data.returns = MatrixXf::Random(n_stocks, n_periods) * 0.05f; // 收益率±5%

    // 计算因子
    MatrixXf alpha001 = calc_alpha001(data);
    MatrixXf alpha010 = calc_alpha010(data);
    MatrixXf alpha036 = calc_alpha036(data);

    // 输出结果（最后10个交易日的因子值，验证正确性）
    std::cout << "=== Alpha001 因子值（5只股票×最后10天）===" << std::endl;
    std::cout << alpha001.block(0, n_periods-10, n_stocks, 10) << std::endl;
    std::cout << "\n=== Alpha010 因子值（5只股票×最后10天）===" << std::endl;
    std::cout << alpha010.block(0, n_periods-10, n_stocks, 10) << std::endl;
    std::cout << "\n=== Alpha036 因子值（5只股票×最后10天）===" << std::endl;
    std::cout << alpha036.block(0, n_periods-10, n_stocks, 10) << std::endl;

    return 0;
}