#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include <algorithm>

// 量化因子计算命名空间，避免冲突
namespace alpha101 {
    // 行情数据结构体（单股票多周期行情，对齐量化数据格式）
    struct MarketData {
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> open;   // 股票数×时间序列长度
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> high;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> low;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> close;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> volume;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> returns; // 收益率 = (close/prev_close)-1
    };

    // ========== 工具函数（Alpha101核心依赖，高性能实现） ==========
    // 移动窗口求和（股票数×时间序列，窗口长度window，输出同维度矩阵）
    Eigen::MatrixXf rolling_sum(const Eigen::MatrixXf& mat, int window);
    // 移动窗口标准差（无偏估计，ddof=1）
    Eigen::MatrixXf rolling_stddev(const Eigen::MatrixXf& mat, int window);
    // 移动窗口相关系数（x和y：股票数×时间序列，窗口window）
    Eigen::MatrixXf rolling_corr(const Eigen::MatrixXf& x, const Eigen::MatrixXf& y, int window);
    // 线性衰减（decay_linear，半衰期window）
    Eigen::MatrixXf decay_linear(const Eigen::MatrixXf& mat, int window);
    // 行内rank归一化（0~1，适配Alpha101要求）
    Eigen::MatrixXf cross_section_rank(const Eigen::MatrixXf& mat);
    // 移动窗口内求最小值索引（Ts_ArgMin）
    Eigen::MatrixXi rolling_argmin(const Eigen::MatrixXf& mat, int window);

    // ========== Alpha101标志性因子接口（核心） ==========
    // Alpha001：反转类因子，输入行情数据，输出因子值（股票数×时间序列）
    Eigen::MatrixXf calc_alpha001(const MarketData& data);
    // Alpha010：量价趋势类因子，计算高效，实用性强
    Eigen::MatrixXf calc_alpha010(const MarketData& data);
    // Alpha036：波动率+相关性复合因子
    Eigen::MatrixXf calc_alpha036(const MarketData& data);
}