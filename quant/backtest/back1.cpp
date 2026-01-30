#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>
#include <cstdint>
#include <string>

// ====================== 枚举定义 ======================
// 策略类型
enum class StrategyType {
    TREND_STRATEGY,    // 趋势策略 (K线驱动)
    TRADE_STRATEGY     // 成交策略 (Depth驱动)
};

// 行情数据类型
enum class MarketDataType {
    KLINE_DATA,
    DEPTH_DATA
};

// ====================== 核心数据结构 ======================
/**
 * 1. K线数据结构 (标准化，兼容所有周期)
 */
struct KlineData {
    std::string symbol;    // 标的代码
    uint64_t ts;           // 时间戳(毫秒)
    double open;           // 开盘价
    double high;           // 最高价
    double low;            // 最低价
    double close;          // 收盘价
    int64_t volume;        // 成交量
    int period;            // K线周期(分钟)
};

/**
 * 2. Depth深度数据结构 (标准化，Level2买五卖五)
 */
struct DepthData {
    std::string symbol;    // 标的代码
    uint64_t ts;           // 时间戳(毫秒)
    double last;           // 最新价
    double bid[5];         // 买1-买5价
    int64_t bid_qty[5];    // 买1-买5量
    double ask[5];         // 卖1-卖5价
    int64_t ask_qty[5];    // 卖1-卖5量
    double spread() const { return ask[0] - bid[0]; } // 买卖价差
};

/**
 * 3. 策略信号结构体
 */
struct StrategySignal {
    std::string symbol;
    uint64_t ts;
    bool is_buy;           // 买入/卖出
    double trigger_price;  // 触发价格
    int64_t qty;           // 委托数量
};

/**
 * 4. 成交记录结构体 (真实成交，基于Depth)
 */
struct TradeRecord {
    std::string symbol;
    uint64_t ts;
    bool is_buy;
    double fill_price;     // 成交价格
    int64_t fill_qty;      // 成交数量
    double slip;           // 滑点
};

// ====================== 核心工具函数 ======================
// 计算均线 (K线策略信号示例)
double calc_ma(const std::vector<KlineData>& klines, int n) {
    if (klines.size() < n) return 0;
    double sum = 0;
    for (int i = klines.size()-n; i < klines.size(); ++i) sum += klines[i].close;
    return sum / n;
}

// 价格精度对齐
double align_price(double price, double tick_size) {
    return std::round(price / tick_size) * tick_size;
}

// ====================== 工业级回测数据适配核心类 ======================
class BacktestDataAdapter {
private:
    std::vector<KlineData> m_kline_data;
    std::vector<DepthData> m_depth_data;
    std::unordered_map<std::string, double> m_tick_size; // 标的最小变动单位
    StrategyType m_strategy_type;
    std::vector<StrategySignal> m_signals;
    std::vector<TradeRecord> m_trades;

    // 基于K线生成趋势策略信号 (均线金叉示例)
    void gen_trend_signal() {
        if (m_kline_data.size() < 10) return;
        double ma5 = calc_ma(m_kline_data, 5);
        double ma10 = calc_ma(m_kline_data, 10);
        static double prev_ma5 = 0, prev_ma10 = 0;
        if (prev_ma5 < prev_ma10 && ma5 > ma10) {
            // 金叉：生成买入信号
            m_signals.push_back({m_kline_data.back().symbol, m_kline_data.back().ts, true, m_kline_data.back().close, 1000});
        } else if (prev_ma5 > prev_ma10 && ma5 < ma10) {
            // 死叉：生成卖出信号
            m_signals.push_back({m_kline_data.back().symbol, m_kline_data.back().ts, false, m_kline_data.back().close, 1000});
        }
        prev_ma5 = ma5;
        prev_ma10 = ma10;
    }

    // 基于Depth执行精细化撮合 (核心，真实成交模拟)
    void match_with_depth(const StrategySignal& signal) {
        // 找到信号触发时间附近的Depth数据
        for (const auto& depth : m_depth_data) {
            if (depth.ts < signal.ts || depth.symbol != signal.symbol) continue;
            TradeRecord trade;
            trade.symbol = signal.symbol;
            trade.ts = depth.ts;
            trade.is_buy = signal.is_buy;
            trade.fill_qty = 0;

            if (signal.is_buy) {
                // 买入：以卖一价成交，穿透卖盘
                double fill_price = depth.ask[0];
                int64_t fill_qty = std::min(signal.qty, depth.ask_qty[0]);
                if (fill_qty > 0) {
                    trade.fill_price = align_price(fill_price, m_tick_size[signal.symbol]);
                    trade.fill_qty = fill_qty;
                    trade.slip = trade.fill_price - signal.trigger_price; // 滑点计算
                }
            } else {
                // 卖出：以买一价成交，穿透买盘
                double fill_price = depth.bid[0];
                int64_t fill_qty = std::min(signal.qty, depth.bid_qty[0]);
                if (fill_qty > 0) {
                    trade.fill_price = align_price(fill_price, m_tick_size[signal.symbol]);
                    trade.fill_qty = fill_qty;
                    trade.slip = signal.trigger_price - trade.fill_price;
                }
            }

            if (trade.fill_qty > 0) m_trades.push_back(trade);
            break;
        }
    }

public:
    BacktestDataAdapter(StrategyType type) : m_strategy_type(type) {
        // 初始化标的精度
        m_tick_size["600000.SH"] = 0.01;
        m_tick_size["BTC-USDT"] = 0.0001;
    }

    // 加载K线数据
    void load_kline_data(const std::vector<KlineData>& data) { m_kline_data = data; }

    // 加载Depth数据
    void load_depth_data(const std::vector<DepthData>& data) { m_depth_data = data; }

    // 执行回测 (核心入口，自动适配策略类型)
    void run_backtest() {
        m_signals.clear();
        m_trades.clear();

        if (m_strategy_type == StrategyType::TREND_STRATEGY) {
            // 纯趋势策略：K线生成信号 + K线简单成交
            gen_trend_signal();
            std::cout << "趋势策略回测完成，生成信号数: " << m_signals.size() << std::endl;
        } else {
            // 成交策略：K线生成信号 + Depth精细化撮合 (最优方案)
            gen_trend_signal();
            for (const auto& signal : m_signals) match_with_depth(signal);
            std::cout << "成交策略回测完成，生成信号数: " << m_signals.size() << ", 实际成交数: " << m_trades.size() << std::endl;
        }
    }

    // 获取成交记录
    std::vector<TradeRecord> get_trades() const { return m_trades; }
};

// ====================== 测试用例 ======================
int main() {
    // 1. 初始化回测适配器：成交策略 (最优方案，Kline+Depth)
    BacktestDataAdapter adapter(StrategyType::TRADE_STRATEGY);

    // 2. 模拟加载K线数据和Depth数据
    std::vector<KlineData> klines = {
        {"600000.SH", 1710000000000, 10.00, 10.05, 9.95, 10.02, 10000, 1},
        {"600000.SH", 1710000060000, 10.02, 10.08, 10.00, 10.06, 12000, 1},
        {"600000.SH", 1710000120000, 10.06, 10.10, 10.03, 10.09, 15000, 1}
    };

    std::vector<DepthData> depths = {
        {"600000.SH", 1710000060000, 10.06, {10.05,10.04,10.03,10.02,10.01}, {2000,1500,1000,800,500},
                                             {10.06,10.07,10.08,10.09,10.10}, {1800,1200,900,700,400}}
    };

    adapter.load_kline_data(klines);
    adapter.load_depth_data(depths);

    // 3. 执行回测
    adapter.run_backtest();

    // 4. 输出成交结果
    auto trades = adapter.get_trades();
    std::cout << "\n真实成交记录：" << std::endl;
    for (const auto& t : trades) {
        std::cout << "标的: " << t.symbol << ", 方向: " << (t.is_buy ? "买入" : "卖出")
                  << ", 成交价格: " << t.fill_price << ", 成交数量: " << t.fill_qty
                  << ", 滑点: " << t.slip << std::endl;
    }

    return 0;
}