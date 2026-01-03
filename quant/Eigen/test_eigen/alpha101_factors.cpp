#include "alpha101_factors.h"
#include <cmath>

using namespace Eigen;
using namespace alpha101;

// 移动窗口求和（前向填充，不足窗口长度补0）
MatrixXf alpha101::rolling_sum(const MatrixXf& mat, int window) {
    if (window < 1 || mat.cols() < window) return MatrixXf::Zero(mat.rows(), mat.cols());
    MatrixXf res(mat.rows(), mat.cols());
    for (int i = 0; i < mat.rows(); ++i) {
        float sum = 0.0f;
        for (int j = 0; j < mat.cols(); ++j) {
            sum += mat(i, j);
            if (j >= window - 1) {
                res(i, j) = sum;
                sum -= mat(i, j - window + 1);
            } else {
                res(i, j) = 0.0f; // 不足窗口长度，量化常用补0
            }
        }
    }
    return res;
}

// 移动窗口标准差（基于窗口求和优化，减少重复计算）
MatrixXf alpha101::rolling_stddev(const MatrixXf& mat, int window) {
    if (window < 2 || mat.cols() < window) return MatrixXf::Zero(mat.rows(), mat.cols());
    MatrixXf res(mat.rows(), mat.cols());
    MatrixXf sum1 = rolling_sum(mat, window);
    MatrixXf sum2 = rolling_sum(mat.array().square(), window);
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = window - 1; j < mat.cols(); ++j) {
            float mean = sum1(i, j) / window;
            float var = (sum2(i, j) - window * mean * mean) / (window - 1); // 无偏方差
            res(i, j) = sqrt(std::max(var, 1e-8f)); // 防止负数开方
        }
    }
    return res;
}

// 移动窗口相关系数（Pearson）
MatrixXf alpha101::rolling_corr(const MatrixXf& x, const MatrixXf& y, int window) {
    if (window < 2 || x.rows() != y.rows() || x.cols() != y.cols()) return MatrixXf::Zero(x.rows(), x.cols());
    MatrixXf res(x.rows(), x.cols());
    MatrixXf sum_x = rolling_sum(x, window);
    MatrixXf sum_y = rolling_sum(y, window);
    MatrixXf sum_xy = rolling_sum(x.array() * y.array(), window);
    MatrixXf sum_x2 = rolling_sum(x.array().square(), window);
    MatrixXf sum_y2 = rolling_sum(y.array().square(), window);

    for (int i = 0; i < x.rows(); ++i) {
        for (int j = window - 1; j < x.cols(); ++j) {
            float cov = sum_xy(i,j) - sum_x(i,j)*sum_y(i,j)/window;
            float std_x = sqrt(sum_x2(i,j) - sum_x(i,j)*sum_x(i,j)/window);
            float std_y = sqrt(sum_y2(i,j) - sum_y(i,j)*sum_y(i,j)/window);
            res(i,j) = cov / std::max(std_x*std_y, 1e-8f); // 防止除0
        }
    }
    return res;
}

// 线性衰减（decay_linear，权重随时间线性递减）
MatrixXf alpha101::decay_linear(const MatrixXf& mat, int window) {
    if (window < 1) return mat;
    MatrixXf res(mat.rows(), mat.cols());
    VectorXf weights = VectorXf::LinSpaced(window, window, 1); // 权重 [window, window-1,...1]
    weights /= weights.sum(); // 归一化

    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            if (j < window - 1) {
                res(i,j) = mat(i,j);
            } else {
                float val = 0.0f;
                for (int k = 0; k < window; ++k) {
                    val += mat(i, j - k) * weights(k);
                }
                res(i,j) = val;
            }
        }
    }
    return res;
}

// 截面rank（单时间点，所有股票排序归一化到0~1，Alpha101标准）
MatrixXf alpha101::cross_section_rank(const MatrixXf& mat) {
    MatrixXf res(mat.rows(), mat.cols());
    for (int j = 0; j < mat.cols(); ++j) {
        VectorXf col = mat.col(j);
        VectorXi idx = VectorXi::LinSpaced(col.size(), 0, col.size()-1);
        // 排序（升序）
        std::sort(idx.data(), idx.data()+idx.size(), [&](int a, int b){return col(a) < col(b);});
        // 赋值rank（0~1）
        for (int i = 0; i < idx.size(); ++i) {
            res(idx(i), j) = (float)i / (idx.size() - 1);
        }
    }
    return res;
}

// 移动窗口内最小值索引（Ts_ArgMin）
MatrixXi alpha101::rolling_argmin(const MatrixXf& mat, int window) {
    if (window < 1 || mat.cols() < window) return MatrixXi::Zero(mat.rows(), mat.cols());
    MatrixXi res(mat.rows(), mat.cols());
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = window - 1; j < mat.cols(); ++j) {
            int argmin = j - window + 1;
            float min_val = mat(i, argmin);
            for (int k = j - window + 2; k <= j; ++k) {
                if (mat(i,k) < min_val) {
                    min_val = mat(i,k);
                    argmin = k;
                }
            }
            res(i,j) = argmin;
        }
    }
    return res;
}

// ========== Alpha101因子核心实现 ==========
// Alpha001: (rank(Ts_ArgMin(SignedPower(((returns<0)?stddev(returns,20):close),2),5)) - 0.5)
MatrixXf alpha101::calc_alpha001(const MarketData& data) {
    int n_stocks = data.returns.rows();
    int n_periods = data.returns.cols();
    MatrixXf stddev_ret = rolling_stddev(data.returns, 20);
    MatrixXf signed_power_mat(n_stocks, n_periods);

    // 条件赋值：returns<0用stddev，否则用close，再平方（SignedPower(x,2)=x?）
    for (int i = 0; i < n_stocks; ++i) {
        for (int j = 0; j < n_periods; ++j) {
            float val = (data.returns(i,j) < 0) ? stddev_ret(i,j) : data.close(i,j);
            signed_power_mat(i,j) = val * val;
        }
    }

    MatrixXi argmin_mat = rolling_argmin(signed_power_mat, 5);
    MatrixXf argmin_float = argmin_mat.cast<float>();
    return cross_section_rank(argmin_float).array() - 0.5f;
}

// Alpha010: rank(((sum(open,5)*sum(high,5)) - (sum(low,5)*sum(close,5))))
MatrixXf alpha101::calc_alpha010(const MarketData& data) {
    MatrixXf sum_open = rolling_sum(data.open, 5);
    MatrixXf sum_high = rolling_sum(data.high, 5);
    MatrixXf sum_low = rolling_sum(data.low, 5);
    MatrixXf sum_close = rolling_sum(data.close, 5);
    MatrixXf factor_mat = (sum_open.array() * sum_high.array()) - (sum_low.array() * sum_close.array());
    return cross_section_rank(factor_mat);
}

// Alpha036: rank(decay_linear(correlation(high,volume,5),3) - rank(stddev(close,10)))
MatrixXf alpha101::calc_alpha036(const MarketData& data) {
    MatrixXf corr = rolling_corr(data.high, data.volume, 5);
    MatrixXf decay_corr = decay_linear(corr, 3);
    MatrixXf stddev_close = rolling_stddev(data.close, 10);
    MatrixXf rank_stddev = cross_section_rank(stddev_close);
    MatrixXf factor_mat = decay_corr.array() - rank_stddev.array();
    return cross_section_rank(factor_mat);
}