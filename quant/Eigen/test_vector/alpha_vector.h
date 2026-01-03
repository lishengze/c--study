#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

// 量化因子命名空间
namespace alpha101 {
    // 行情数据结构体（多股票时间序列，vector嵌套：股票数×周期数）
    struct MarketData {
        std::vector<std::vector<float>> open;    // 开盘价
        std::vector<std::vector<float>> high;    // 最高价
        std::vector<std::vector<float>> low;     // 最低价
        std::vector<std::vector<float>> close;   // 收盘价
        std::vector<std::vector<float>> volume;  // 成交量
        std::vector<std::vector<float>> returns; // 收益率=(close/prev_close)-1
    };

    // ========== 核心工具函数（Alpha101依赖，纯vector实现） ==========
    // 移动窗口求和（窗口window，不足窗口补0，输出同维度vector）
    std::vector<std::vector<float>> rolling_sum(const std::vector<std::vector<float>>& mat, int window);
    // 移动窗口标准差（无偏估计，ddof=1）
    std::vector<std::vector<float>> rolling_stddev(const std::vector<std::vector<float>>& mat, int window);
    // 移动窗口皮尔逊相关系数（x/y：同维度）
    std::vector<std::vector<float>> rolling_corr(const std::vector<std::vector<float>>& x, 
                                                 const std::vector<std::vector<float>>& y, int window);
    // 线性衰减（decay_linear，权重线性递减）
    std::vector<std::vector<float>> decay_linear(const std::vector<std::vector<float>>& mat, int window);
    // 截面rank归一化（单周期所有股票排序，归一到0~1，Alpha101标准）
    std::vector<std::vector<float>> cross_section_rank(const std::vector<std::vector<float>>& mat);
    // 移动窗口最小值索引（Ts_ArgMin，返回索引矩阵）
    std::vector<std::vector<int>> rolling_argmin(const std::vector<std::vector<float>>& mat, int window);

    // ========== Alpha101标志性因子接口（无任何第三方依赖） ==========
    std::vector<std::vector<float>> calc_alpha001(const MarketData& data); // 反转因子
    std::vector<std::vector<float>> calc_alpha010(const MarketData& data); // 量价趋势因子
    std::vector<std::vector<float>> calc_alpha036(const MarketData& data); // 波动率复合因子
};