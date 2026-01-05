#include "KlineCompute.h"
#include "comm_define.h"
#include "market_data_manager.h"

// 工具：创建指定维度vector并初始化0
static my_vector<my_vector<float>> create_mat(int rows, int cols, float val = 0.0f) {
    my_vector<my_vector<float>> mat(rows, my_vector<float>(cols, val));
    return mat;
}

// 移动窗口求和（纯循环优化，减少重复计算）
my_vector<my_vector<float>> rolling_sum(const my_vector<my_vector<float>>& mat, int window) {
    if (window < 1 || mat.empty() || mat[0].empty()) return {};
    int rows = mat.size();
    int cols = mat[0].size();
    auto res = create_mat(rows, cols);

    for (int i = 0; i < rows; ++i) {
        float sum = 0.0f;
        for (int j = 0; j < cols; ++j) {
            sum += mat[i][j];
            if (j >= window - 1) {
                res[i][j] = sum;
                sum -= mat[i][j - window + 1];
            }
        }
    }
    return res;
}

// 移动窗口标准差（基于求和结果优化，避免重复遍历）
my_vector<my_vector<float>> rolling_stddev(const my_vector<my_vector<float>>& mat, int window) {
    if (window < 2 || mat.empty() || mat[0].empty()) return {};
    int rows = mat.size();
    int cols = mat[0].size();
    auto res = create_mat(rows, cols);
    auto sum1 = rolling_sum(mat, window);
    auto mat_square = mat;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            mat_square[i][j] = mat[i][j] * mat[i][j];
    auto sum2 = rolling_sum(mat_square, window);

    for (int i = 0; i < rows; ++i) {
        for (int j = window - 1; j < cols; ++j) {
            float mean = sum1[i][j] / window;
            float var = (sum2[i][j] - window * mean * mean) / (window - 1); // 无偏方差
            res[i][j] = sqrt(std::max(var, 1e-8f)); // 防负数开方
        }
    }
    return res;
}

// 移动窗口相关系数
my_vector<my_vector<float>> rolling_corr(const my_vector<my_vector<float>>& x, 
                                                       const my_vector<my_vector<float>>& y, int window) {
    if (window < 2 || x.empty() || y.empty() || x.size() != y.size() || x[0].size() != y[0].size()) return {};
    int rows = x.size();
    int cols = x[0].size();
    auto res = create_mat(rows, cols);

    auto sum_x = rolling_sum(x, window);
    auto sum_y = rolling_sum(y, window);
    auto sum_xy = create_mat(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            sum_xy[i][j] = x[i][j] * y[i][j];
    sum_xy = rolling_sum(sum_xy, window);
    
    auto sum_x2 = rolling_sum(x, window);
    auto sum_y2 = rolling_sum(y, window);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            sum_x2[i][j] = x[i][j] * x[i][j];
            sum_y2[i][j] = y[i][j] * y[i][j];
        }
    sum_x2 = rolling_sum(sum_x2, window);
    sum_y2 = rolling_sum(sum_y2, window);

    for (int i = 0; i < rows; ++i) {
        for (int j = window - 1; j < cols; ++j) {
            float cov = sum_xy[i][j] - sum_x[i][j] * sum_y[i][j] / window;
            float std_x = sqrt(sum_x2[i][j] - sum_x[i][j] * sum_x[i][j] / window);
            float std_y = sqrt(sum_y2[i][j] - sum_y[i][j] * sum_y[i][j] / window);
            res[i][j] = cov / std::max(std_x * std_y, 1e-8f); // 防除0
        }
    }
    return res;
}

// 线性衰减（权重归一化，纯原生实现）
my_vector<my_vector<float>> decay_linear(const my_vector<my_vector<float>>& mat, int window) {
    if (window < 1 || mat.empty() || mat[0].empty()) return mat;
    int rows = mat.size();
    int cols = mat[0].size();
    auto res = create_mat(rows, cols);

    // 预计算权重并归一化
    my_vector<float> weights(window);
    for (int k = 0; k < window; ++k) weights[k] = window - k;
    float weight_sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    for (auto& w : weights) w /= weight_sum;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (j < window - 1) {
                res[i][j] = mat[i][j];
            } else {
                float val = 0.0f;
                for (int k = 0; k < window; ++k) {
                    val += mat[i][j - k] * weights[k];
                }
                res[i][j] = val;
            }
        }
    }
    return res;
}

// 截面rank（单周期排序，Alpha101标准0~1归一）
my_vector<my_vector<float>> cross_section_rank(const my_vector<my_vector<float>>& mat) {
    if (mat.empty() || mat[0].empty()) return {};
    int rows = mat.size();
    int cols = mat[0].size();
    auto res = create_mat(rows, cols);

    for (int j = 0; j < cols; ++j) { // 按周期遍历
        my_vector<std::pair<float, int>> stock_vals; // 因子值+股票索引
        for (int i = 0; i < rows; ++i) {
            stock_vals.emplace_back(mat[i][j], i);
        }
        // 升序排序
        std::sort(stock_vals.begin(), stock_vals.end(), 
                  [](const std::pair<float, int>& a, const std::pair<float, int>& b) {
                      return a.first < b.first;
                  });
        // 赋值rank（0~1）
        for (int k = 0; k < rows; ++k) {
            int stock_idx = stock_vals[k].second;
            res[stock_idx][j] = rows == 1 ? 0.5f : (float)k / (rows - 1);
        }
    }
    return res;
}

// 移动窗口最小值索引（Ts_ArgMin，返回窗口内最小值的位置）
my_vector<my_vector<int>> rolling_argmin(const my_vector<my_vector<float>>& mat, int window) {
    if (window < 1 || mat.empty() || mat[0].empty()) return {};
    int rows = mat.size();
    int cols = mat[0].size();
    my_vector<my_vector<int>> res(rows, my_vector<int>(cols, 0));

    for (int i = 0; i < rows; ++i) {
        for (int j = window - 1; j < cols; ++j) {
            int start = j - window + 1;
            int argmin = start;
            float min_val = mat[i][start];
            for (int k = start + 1; k <= j; ++k) {
                if (mat[i][k] < min_val) {
                    min_val = mat[i][k];
                    argmin = k;
                }
            }
            res[i][j] = argmin;
        }
    }
    return res;
}

// ========== Alpha101因子核心实现 ==========
// Alpha001: (rank(Ts_ArgMin(SignedPower(((returns<0)?stddev(returns,20):close),2),5)) - 0.5)
my_vector<my_vector<float>> calc_alpha001(const KLineDataManager* data) {
    if (data->vecHigh.empty() || data->vecLow.empty() || data->vecClose.empty() || data->vecHigh.size() != data->vecLow.size() || data->vecHigh.size() != data->vecClose.size()) return {};
    int rows = data->vecHigh.size();
    int cols = data->vecHigh[0].size();
    auto stddev_ret = rolling_stddev(data->vecHigh, 20);
    auto signed_power_mat = create_mat(rows, cols);

    // 条件赋值+平方（SignedPower(x,2)=x?）
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float val = data->vecHigh[i][j] < 0 ? stddev_ret[i][j] : data->vecClose[i][j];
            signed_power_mat[i][j] = val * val;
        }
    }

    auto argmin_mat = rolling_argmin(signed_power_mat, 5);
    // 索引转float矩阵用于rank
    auto argmin_float = create_mat(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            argmin_float[i][j] = (float)argmin_mat[i][j];
    
    auto rank_mat = cross_section_rank(argmin_float);
    // 减0.5
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            rank_mat[i][j] -= 0.5f;
    return rank_mat;
}

// Alpha010: rank(((sum(open,5)*sum(high,5)) - (sum(low,5)*sum(close,5))))
my_vector<my_vector<float>> calc_alpha010(const KLineDataManager* data) {
    if (data->vecOpen.empty() || data->vecHigh.empty() || data->vecLow.empty() || data->vecClose.empty() || data->vecOpen.size() != data->vecHigh.size() || data->vecHigh.size() != data->vecLow.size() || data->vecLow.size() != data->vecClose.size()) return {};
    auto sum_open = rolling_sum(data->vecOpen, 5);
    auto sum_high = rolling_sum(data->vecHigh, 5);
    auto sum_low = rolling_sum(data->vecLow, 5);
    auto sum_close = rolling_sum(data->vecClose, 5);
    int rows = sum_open.size();
    int cols = sum_open[0].size();
    auto factor_mat = create_mat(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            factor_mat[i][j] = sum_open[i][j] * sum_high[i][j] - sum_low[i][j] * sum_close[i][j];
        }
    }
    return cross_section_rank(factor_mat);
}

// Alpha036: rank(decay_linear(correlation(high,volume,5),3) - rank(stddev(close,10)))
my_vector<my_vector<float>> calc_alpha036(const KLineDataManager* data) {
    if (data->vecHigh.empty() || data->vecVolume.empty() || data->vecClose.empty() || data->vecHigh.size() != data->vecVolume.size() || data->vecHigh.size() != data->vecClose.size()) return {};
    auto corr = rolling_corr(data->vecHigh, data->vecVolume, 5);
    auto decay_corr = decay_linear(corr, 3);
    auto stddev_close = rolling_stddev(data->vecClose, 10);
    auto rank_stddev = cross_section_rank(stddev_close);
    
    int rows = decay_corr.size();
    int cols = decay_corr[0].size();
    auto factor_mat = create_mat(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            factor_mat[i][j] = decay_corr[i][j] - rank_stddev[i][j];
        }
    }
    return cross_section_rank(factor_mat);
}

bool KlineCompute_1::calculate_kline_indicator() {
    my_vector<float> ret;
    KLineDataManager* kline_data_manager = get_kline_data_manager();

    if (kline_data_manager != nullptr && kline_data_manager->GetDataCount() >= iMinimumDataCount) {
        auto tmpRst = calc_alpha001(kline_data_manager);        
        for (int i = 0; i < tmpRst.size(); i++) {
            ret.push_back(tmpRst[i][tmpRst[i].size()-1]);
        }
    }
    kline_data_manager->UpdateKlineIndicator(ret, KlineIndicatorType::Alpha_001);    

    return true;
}

bool KlineCompute_10::calculate_kline_indicator() {
    my_vector<float> ret;

    KLineDataManager* kline_data_manager = get_kline_data_manager();

    if (kline_data_manager != nullptr && kline_data_manager->GetDataCount() >= iMinimumDataCount) {
        auto tmpRst = calc_alpha010(kline_data_manager);        
        for (int i = 0; i < tmpRst.size(); i++) {
            ret.push_back(tmpRst[i][tmpRst[i].size()-1]);
        }
    }

    kline_data_manager->UpdateKlineIndicator(ret, KlineIndicatorType::Alpha_010);

    return true;
}

bool KlineCompute_36::calculate_kline_indicator() {
    my_vector<float> ret;


    
    KLineDataManager* kline_data_manager = get_kline_data_manager();

    if (kline_data_manager != nullptr && kline_data_manager->GetDataCount() >= iMinimumDataCount) {
        auto tmpRst = calc_alpha036(kline_data_manager);        
        for (int i = 0; i < tmpRst.size(); i++) {
            ret.push_back(tmpRst[i][tmpRst[i].size()-1]);
        }
    }


    kline_data_manager->UpdateKlineIndicator(ret, KlineIndicatorType::Alpha_036);

    return true;
}
