#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdint>
#include <string>
#include <chrono>
#include <functional>

// ====================== 全局枚举定义 (量化回测标准) ======================
// 订单方向
enum class OrderDir : uint8_t {
    BUY = 0,    // 买入
    SELL = 1    // 卖出
};

// 订单类型
enum class OrderType : uint8_t {
    LIMIT = 0,      // 限价单(核心)
    MARKET = 1,     // 市价单
    STOP_LOSS = 2,  // 止损单
    TAKE_PROFIT =3  // 止盈单
};

// 订单状态
enum class OrderStatus : uint8_t {
    NEW = 0,        // 新订单-已挂单
    PART_FILLED =1, // 部分成交
    FULL_FILLED =2, // 完全成交
    CANCELED =3,    // 已撤销/过期
    REJECTED =4     // 已拒绝(无效订单)
};

// 撮合阶段
enum class MatchPhase : uint8_t {
    CONTINUOUS =0,  // 连续竞价(盘中)
    COLLECT =1      // 集合竞价(开盘/收盘)
};

// ====================== 核心数据结构定义 (量化回测专用) ======================
/**
 * 行情数据结构 (兼容Tick/K线，回测核心数据源)
 * 支持Level2盘口深度(买一/卖一)，是撮合的核心依据
 */
struct MarketData {
    std::string symbol;        // 标的代码
    uint64_t ts;               // 时间戳(纳秒)
    double open;               // 开盘价
    double high;               // 最高价
    double low;                // 最低价
    double close;              // 收盘价/最新价
    int64_t vol;               // 成交量
    double bid1;               // 买一价
    int64_t bid1_qty;          // 买一量
    double ask1;               // 卖一价
    int64_t ask1_qty;          // 卖一量
    double up_limit;           // 涨停价
    double down_limit;         // 跌停价
};

/**
 * 订单结构体 (量化回测专用，字段完整，无冗余)
 */
struct Order {
    uint64_t order_id;         // 唯一订单ID
    std::string symbol;        // 标的代码
    OrderDir dir;              // 买卖方向
    OrderType type;            // 订单类型
    double price;              // 委托价格(限价单有效)
    int64_t qty;               // 委托数量
    int64_t filled_qty;        // 已成交数量
    double avg_fill_price;     // 平均成交价
    uint64_t create_ts;        // 委托时间戳
    uint64_t fill_ts;          // 成交时间戳
    OrderStatus status;        // 订单状态
    double trigger_price;      // 触发价(止损/止盈单有效)
    int hang_bar_cnt;          // 挂单已持续的Bar数

    Order(uint64_t oid, std::string sym, OrderDir d, OrderType t, double p, int64_t q, uint64_t ts)
        : order_id(oid), symbol(std::move(sym)), dir(d), type(t), price(p), qty(q)
        , filled_qty(0), avg_fill_price(0), create_ts(ts), fill_ts(0), status(OrderStatus::NEW)
        , trigger_price(0), hang_bar_cnt(0) {}
};

/**
 * 成交记录结构体 (回测核心输出，用于计算收益/滑点/胜率)
 */
struct Trade {
    uint64_t trade_id;
    uint64_t order_id;
    std::string symbol;
    OrderDir dir;
    double fill_price;
    int64_t fill_qty;
    uint64_t fill_ts;
    double slip;               // 本次成交的滑点(成交价格-最优盘口价)
};

/**
 * 撮合配置结构体 (核心！所有规则可参数化配置，无需改代码)
 * 回测调优的核心入口，滑点/成交量比例/挂单过期等都在这里配置
 */
struct MatchConfig {
    double slip_ratio = 0.001;     // 比例滑点 0.1%
    double slip_fixed = 0.01;      // 固定滑点 0.01元
    bool use_slip_ratio = true;    // 默认启用比例滑点
    double vol_ratio = 0.8;        // 成交量成交比例 80%
    int hang_timeout = 5;          // 挂单5根Bar过期
    double pre_close = 0.0;        // 昨日收盘价(集合竞价用)
};

// ====================== 核心工具函数 ======================
// 纳秒时间戳
inline uint64_t get_nano_ts() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

// 价格精度比较 (解决浮点误差)
inline bool price_eq(double a, double b) { return std::fabs(a - b) < 1e-6; }
inline bool price_gt(double a, double b) { return a - b > 1e-6; }
inline bool price_lt(double a, double b) { return b - a > 1e-6; }
inline bool price_ge(double a, double b) { return price_gt(a, b) || price_eq(a, b); }
inline bool price_le(double a, double b) { return price_lt(a, b) || price_eq(a, b); }

// ====================== 工业级回测撮合核心类 (完整实现所有规则) ======================
class BacktestMatcher {
private:
    using PriceLevel = std::pair<double, int64_t>; // 价格-数量 档位

    std::string m_symbol;
    MatchPhase m_phase;
    MatchConfig m_cfg;
    uint64_t m_next_order_id = 1;
    uint64_t m_next_trade_id = 1;

    std::unordered_map<uint64_t, Order> m_orders;          // 所有订单
    std::vector<Trade> m_trades;                           // 成交记录
    std::map<double, int64_t, std::greater<double>> m_buy_book;  // 买单簿: 价格降序
    std::map<double, int64_t> m_sell_book;                     // 卖单簿: 价格升序

public:
    BacktestMatcher(std::string symbol, MatchConfig cfg = MatchConfig{})
        : m_symbol(std::move(symbol)), m_phase(MatchPhase::CONTINUOUS), m_cfg(std::move(cfg)) {}

    // ---------------------- 核心接口1：切换撮合阶段 ----------------------
    void set_match_phase(MatchPhase phase) { m_phase = phase; }

    // ---------------------- 核心接口2：提交订单 (回测挂单入口) ----------------------
    uint64_t submit_order(OrderDir dir, OrderType type, double price, int64_t qty, const MarketData& md) {
        if (qty <= 0 || price_le(price, 0) || md.symbol != m_symbol) return 0;
        uint64_t oid = m_next_order_id++;
        Order order(oid, m_symbol, dir, type, price, qty, md.ts);
        // 订单合法性校验：涨跌停、价格有效性
        if (price_lt(price, md.down_limit) || price_gt(price, md.up_limit)) {
            order.status = OrderStatus::REJECTED;
            m_orders[oid] = order;
            return oid;
        }
        // 止损/止盈单 初始化触发价
        if (type == OrderType::STOP_LOSS || type == OrderType::TAKE_PROFIT) {
            order.trigger_price = price;
            order.price = (dir == OrderDir::BUY) ? md.ask1 : md.bid1;
        }
        m_orders[oid] = order;
        // 触发撮合
        match_order(order, md);
        return oid;
    }

    // ---------------------- 核心接口3：执行撮合 (回测核心入口，每根K线/Tick调用一次) ----------------------
    void on_market_data(const MarketData& md) {
        if (md.symbol != m_symbol) return;
        // 处理挂单过期
        expire_hang_orders();
        // 处理止损/止盈触发单
        trigger_stop_take_orders(md);
        // 对所有未成交订单执行撮合
        for (auto& [oid, order] : m_orders) {
            if (order.status == OrderStatus::NEW || order.status == OrderStatus::PART_FILLED) {
                match_order(order, md);
            }
        }
    }

    // ---------------------- 核心接口4：查询结果 (回测数据导出) ----------------------
    std::vector<Trade> get_trades() const { return m_trades; }
    std::unordered_map<uint64_t, Order> get_orders() const { return m_orders; }
    double get_total_slip() const { // 累计滑点
        double slip = 0;
        for (const auto& t : m_trades) slip += t.slip * t.fill_qty;
        return slip;
    }

    // ---------------------- 核心接口5：集合竞价撮合 (开盘价生成) ----------------------
    std::pair<double, int64_t> run_collect_auction(const std::vector<MarketData>& md_list) {
        m_phase = MatchPhase::COLLECT;
        // 统计集合竞价的买卖总量
        std::map<double, int64_t> buy_total, sell_total;
        for (const auto& md : md_list) {
            if (price_gt(md.bid1, 0)) buy_total[md.bid1] += md.bid1_qty;
            if (price_gt(md.ask1, 0)) sell_total[md.ask1] += md.ask1_qty;
        }
        // 三层集合竞价规则 (你指定的完整规则)
        double final_price = calc_collect_price(buy_total, sell_total);
        int64_t total_buy = calc_total_qty(buy_total, final_price, true);
        int64_t total_sell = calc_total_qty(sell_total, final_price, false);
        int64_t match_qty = std::min(total_buy, total_sell) * m_cfg.vol_ratio;
        m_phase = MatchPhase::CONTINUOUS;
        return {final_price, match_qty};
    }

private:
    // ====================== 私有核心：连续竞价撮合引擎 (价格优先+时间优先，核心核心) ======================
    void match_order(Order& order, const MarketData& md) {
        if (m_phase == MatchPhase::COLLECT) { add_to_book(order); return; }
        if (order.status != OrderStatus::NEW && order.status != OrderStatus::PART_FILLED) return;

        int64_t remain_qty = order.qty - order.filled_qty;
        if (remain_qty <= 0) { order.status = OrderStatus::FULL_FILLED; return; }

        // 按订单类型执行不同撮合逻辑
        if (order.type == OrderType::LIMIT) {
            match_limit_order(order, md, remain_qty);
        } else if (order.type == OrderType::MARKET) {
            match_market_order(order, md, remain_qty);
        }
        // 更新订单状态
        update_order_status(order);
    }

    // 限价单撮合 (核心，严格价格优先)
    void match_limit_order(Order& order, const MarketData& md, int64_t remain_qty) {
        double best_price = 0;
        int64_t best_qty = 0;
        bool can_fill = false;

        if (order.dir == OrderDir::BUY) {
            // 买单：委托价 >= 卖一价 可成交，成交价格=卖一价+滑点
            can_fill = price_ge(order.price, md.ask1) && price_gt(md.ask1, 0);
            best_price = md.ask1;
            best_qty = md.ask1_qty;
        } else {
            // 卖单：委托价 <= 买一价 可成交，成交价格=买一价-滑点
            can_fill = price_le(order.price, md.bid1) && price_gt(md.bid1, 0);
            best_price = md.bid1;
            best_qty = md.bid1_qty;
        }

        if (!can_fill) { add_to_book(order); return; }

        // 计算实际可成交数量：盘口量 × 成交量比例
        int64_t fill_qty = std::min(remain_qty, (int64_t)(best_qty * m_cfg.vol_ratio));
        // 计算滑点后的成交价格
        double fill_price = calc_slip_price(order.dir, best_price);
        // 生成成交记录
        gen_trade(order, fill_price, fill_qty, md.ts);
        // 更新订单成交信息
        order.filled_qty += fill_qty;
        order.avg_fill_price = (order.avg_fill_price * (order.filled_qty - fill_qty) + fill_price * fill_qty) / order.filled_qty;
    }

    // 市价单撮合 (成交优先，价格次之)
    void match_market_order(Order& order, const MarketData& md, int64_t remain_qty) {
        double best_price = 0;
        int64_t best_qty = 0;

        if (order.dir == OrderDir::BUY) {
            best_price = md.ask1 > 0 ? md.ask1 : md.close;
            best_qty = md.ask1_qty > 0 ? md.ask1_qty : md.vol;
        } else {
            best_price = md.bid1 > 0 ? md.bid1 : md.close;
            best_qty = md.bid1_qty > 0 ? md.bid1_qty : md.vol;
        }

        int64_t fill_qty = std::min(remain_qty, (int64_t)(best_qty * m_cfg.vol_ratio));
        double fill_price = calc_slip_price(order.dir, best_price);
        gen_trade(order, fill_price, fill_qty, md.ts);

        order.filled_qty += fill_qty;
        order.avg_fill_price = (order.avg_fill_price * (order.filled_qty - fill_qty) + fill_price * fill_qty) / order.filled_qty;
    }

    // ====================== 私有核心：滑点计算 (防失真核心) ======================
    double calc_slip_price(OrderDir dir, double best_price) {
        if (price_eq(best_price, 0)) return best_price;
        double slip = 0;
        if (m_cfg.use_slip_ratio) {
            slip = best_price * m_cfg.slip_ratio;
        } else {
            slip = m_cfg.slip_fixed;
        }
        // 买单滑点：成交价变高，成本增加
        if (dir == OrderDir::BUY) return best_price + slip;
        // 卖单滑点：成交价变低，收益减少
        else return best_price - slip;
    }

    // ====================== 私有核心：集合竞价三层规则 (完整实现) ======================
    double calc_collect_price(const std::map<double, int64_t>& buy, const std::map<double, int64_t>& sell) {
        std::set<double> prices;
        for (auto [p, q] : buy) prices.insert(p);
        for (auto [p, q] : sell) prices.insert(p);
        if (prices.empty()) return 0;

        struct Candidate { double p; int64_t q; int64_t diff; };
        std::vector<Candidate> cand;
        for (double p : prices) {
            int64_t b = calc_total_qty(buy, p, true);
            int64_t s = calc_total_qty(sell, p, false);
            cand.push_back({p, std::min(b, s), (int64_t)std::abs(b - s)});
        }

        // 规则1：成交量最大
        int64_t max_q = 0;
        for (auto& c : cand) max_q = std::max(max_q, c.q);
        std::vector<Candidate> r1;
        for (auto& c : cand) if (c.q == max_q) r1.push_back(c);
        if (r1.size() == 1) return r1[0].p;

        // 规则2：差值最小
        int64_t min_diff = LLONG_MAX;
        for (auto& c : r1) min_diff = std::min(min_diff, c.diff);
        std::vector<Candidate> r2;
        for (auto& c : r1) if (c.diff == min_diff) r2.push_back(c);
        if (r2.size() == 1) return r2[0].p;

        // 规则3：兜底规则
        std::vector<double> final_p;
        for (auto& c : r2) final_p.push_back(c.p);
        return get_final_collect_price(final_p);
    }

    double get_final_collect_price(const std::vector<double>& ps) {
        std::vector<double> buy_side, sell_side, equal_side;
        for (double p : ps) {
            if (price_eq(p, m_cfg.pre_close)) equal_side.push_back(p);
            else if (price_gt(p, m_cfg.pre_close)) buy_side.push_back(p);
            else sell_side.push_back(p);
        }
        if (!equal_side.empty()) return equal_side[0];
        if (!buy_side.empty() && sell_side.empty()) return *std::max_element(buy_side.begin(), buy_side.end());
        if (!sell_side.empty() && buy_side.empty()) return *std::min_element(sell_side.begin(), sell_side.end());
        double min_dist = DBL_MAX, res = 0;
        for (double p : ps) {
            double d = std::fabs(p - m_cfg.pre_close);
            if (d < min_dist) { min_dist = d; res = p; }
        }
        return res;
    }

    int64_t calc_total_qty(const std::map<double, int64_t>& book, double p, bool is_buy) {
        int64_t total =0;
        if (is_buy) {
            for (auto [price, qty] : book) if (price_ge(price, p)) total += qty;
        } else {
            for (auto [price, qty] : book) if (price_le(price, p)) total += qty;
        }
        return total;
    }

    // ====================== 私有工具函数 ======================
    void add_to_book(const Order& order) {
        if (order.dir == OrderDir::BUY) m_buy_book[order.price] += order.qty - order.filled_qty;
        else m_sell_book[order.price] += order.qty - order.filled_qty;
    }

    void gen_trade(const Order& order, double price, int64_t qty, uint64_t ts) {
        Trade t;
        t.trade_id = m_next_trade_id++;
        t.order_id = order.order_id;
        t.symbol = order.symbol;
        t.dir = order.dir;
        t.fill_price = price;
        t.fill_qty = qty;
        t.fill_ts = ts;
        t.slip = (order.dir == OrderDir::BUY) ? (price - (price_gt(order.price,0) ? order.price : 0)) : (0 - price);
        m_trades.push_back(t);
    }

    void update_order_status(Order& order) {
        if (price_eq(order.filled_qty, order.qty)) {
            order.status = OrderStatus::FULL_FILLED;
            order.fill_ts = get_nano_ts();
        } else if (order.filled_qty > 0) {
            order.status = OrderStatus::PART_FILLED;
        } else {
            order.hang_bar_cnt++;
        }
    }

    void expire_hang_orders() {
        for (auto& [oid, order] : m_orders) {
            if (order.hang_bar_cnt >= m_cfg.hang_timeout && order.status != OrderStatus::FULL_FILLED) {
                order.status = OrderStatus::CANCELED;
            }
        }
    }

    void trigger_stop_take_orders(const MarketData& md) {
        for (auto& [oid, order] : m_orders) {
            if (order.type != OrderType::STOP_LOSS && order.type != OrderType::TAKE_PROFIT) continue;
            bool trigger = false;
            if (order.type == OrderType::STOP_LOSS) {
                trigger = (order.dir == OrderDir::BUY) ? price_lt(md.close, order.trigger_price) : price_gt(md.close, order.trigger_price);
            } else {
                trigger = (order.dir == OrderDir::BUY) ? price_gt(md.close, order.trigger_price) : price_lt(md.close, order.trigger_price);
            }
            if (trigger) {
                order.type = OrderType::MARKET;
                match_market_order(order, md, order.qty - order.filled_qty);
            }
        }
    }
};

// ====================== 测试用例 (完整回测撮合流程，可直接运行) ======================
int main() {
    // 1. 初始化撮合器 + 配置回测参数
    MatchConfig cfg;
    cfg.slip_ratio = 0.0005; // 0.05%滑点
    cfg.vol_ratio = 0.9;     // 90%成交量成交比例
    cfg.hang_timeout = 3;    // 3根Bar挂单过期
    BacktestMatcher matcher("600000.SH", cfg);

    // 2. 模拟行情数据 (Tick/K线)
    MarketData md1 = {
        "600000.SH", get_nano_ts(), 10.0, 10.2, 9.8, 10.0, 10000,
        9.95, 500, 10.05, 600, 11.0, 9.0
    };
    MarketData md2 = {
        "600000.SH", get_nano_ts(), 10.05, 10.3, 9.9, 10.2, 12000,
        10.1, 400, 10.2, 700, 11.0, 9.0
    };

    // 3. 提交订单 (限价买单+市价卖单+止损单)
    uint64_t o1 = matcher.submit_order(OrderDir::BUY, OrderType::LIMIT, 10.05, 200, md1);
    uint64_t o2 = matcher.submit_order(OrderDir::SELL, OrderType::MARKET, 0, 150, md1);
    uint64_t o3 = matcher.submit_order(OrderDir::SELL, OrderType::STOP_LOSS, 9.8, 100, md1);

    // 4. 行情更新，执行撮合
    matcher.on_market_data(md1);
    matcher.on_market_data(md2);

    // 5. 集合竞价测试
    auto collect_res = matcher.run_collect_auction({md1, md2});

    // 6. 输出回测结果
    std::cout << "===== 量化回测撮合结果 =====" << std::endl;
    std::cout << "集合竞价开盘价: " << collect_res.first << " 成交总量: " << collect_res.second << std::endl;
    std::cout << "累计成交笔数: " << matcher.get_trades().size() << std::endl;
    std::cout << "累计滑点成本: " << matcher.get_total_slip() << std::endl;

    auto trades = matcher.get_trades();
    for (const auto& t : trades) {
        std::cout << "成交ID: " << t.trade_id << " 方向: " << (t.dir == OrderDir::BUY ? "买" : "卖")
                  << " 价格: " << t.fill_price << " 数量: " << t.fill_qty << " 滑点: " << t.slip << std::endl;
    }

    return 0;
}