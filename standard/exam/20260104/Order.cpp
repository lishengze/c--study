#include <string>
#include <cstdint>
#include <cmath>
#include <stdexcept>

// 前置声明：Execution类（题目假设已定义，此处仅为编译通过做声明）
class Execution;

// 订单方向枚举（辅助管理，增强可读性）
enum class OrderSide {
    Buy = 'B',
    Sell = 'S'
};

// 题目假设Execution类已定义，此处为编译通过实现极简版本（无需关注内部逻辑）
class Execution
{
public:
    void requestOrderAdd(
        uint32_t id,
        std::string const &feedcode,
        char orderSide,
        double orderPrice,
        uint32_t orderVolume)
    {
        // 模拟挂单请求，实际由交易所实现
    }

    void requestOrderRemove(
        uint32_t id)
    {
        // 模拟撤单请求，实际由交易所实现
    }
};

// 核心实现类：InstrumentQuoter
class InstrumentQuoter
{
public:
    // 构造函数：初始化报价器参数
    InstrumentQuoter(
        std::string const &feedcode,
        double quoteOffset,
        uint32_t quoteVolume,
        double tickWidth,
        Execution &execution)
        : m_feedcode(feedcode),
          m_quoteOffset(quoteOffset),
          m_quoteVolume(quoteVolume),
          m_tickWidth(tickWidth),
          m_execution(execution),
          m_currentBidOrderId(0),    // 0表示无有效买单
          m_currentAskOrderId(0),    // 0表示无有效卖单
          m_lastTheoreticalPrice(0.0),
          m_lastBestBid(0.0),
          m_lastBestOffer(0.0)
    {
        // 校验参数合法性（非必须，增强鲁棒性）
        if (tickWidth <= 0.0) {
            throw std::invalid_argument("tickWidth must be greater than 0");
        }
        if (quoteOffset < 0.0) {
            throw std::invalid_argument("quoteOffset cannot be negative");
        }
    }

    // 回调：更新理论价格
    void OnTheoreticalPrice(double theoreticalPrice)
    {
        m_lastTheoreticalPrice = theoreticalPrice;
        // 理论价格变化，重新计算并更新买卖单
        UpdateQuotes();
    }

    // 回调：更新最优买卖盘
    void OnBestBidOffer(double bidPrice, double offerPrice)
    {
        m_lastBestBid = bidPrice;
        m_lastBestOffer = offerPrice;
        // 盘口变化，重新计算并更新买卖单
        UpdateQuotes();
    }

    // 回调：挂单确认
    void OnOrderAddConfirm(uint32_t id)
    {
        // 更新当前挂单ID（确认挂单成功）
        if (id == m_currentBidOrderId) {
            // 买单挂单成功，无需额外操作（状态已标记）
        } else if (id == m_currentAskOrderId) {
            // 卖单挂单成功，无需额外操作（状态已标记）
        }
        // 若ID不匹配，忽略（非当前报价器的订单）
    }

    // 回调：撤单确认
    void OnOrderRemoveConfirm(uint32_t id)
    {
        // 旧单撤单成功后，清空对应方向的当前订单ID，允许挂新单
        if (id == m_currentBidOrderId) {
            m_currentBidOrderId = 0;
            // 买单旧单已撤，重新尝试挂买单
            SubmitBuyOrder();
        } else if (id == m_currentAskOrderId) {
            m_currentAskOrderId = 0;
            // 卖单旧单已撤，重新尝试挂卖单
            SubmitSellOrder();
        }
        // 若ID不匹配，忽略（非当前报价器的订单）
    }

private:
    // 核心方法：更新买卖报价（先检查旧单，再触发撤单/挂单）
    void UpdateQuotes()
    {
        // 处理买单：先检查是否有未撤销的旧买单
        if (m_currentBidOrderId != 0) {
            // 存在旧买单，先发送撤单请求
            m_execution.requestOrderRemove(m_currentBidOrderId);
        } else {
            // 无旧买单，直接尝试挂新买单
            SubmitBuyOrder();
        }

        // 处理卖单：先检查是否有未撤销的旧卖单
        if (m_currentAskOrderId != 0) {
            // 存在旧卖单，先发送撤单请求
            m_execution.requestOrderRemove(m_currentAskOrderId);
        } else {
            // 无旧卖单，直接尝试挂新卖单
            SubmitSellOrder();
        }
    }

    // 计算符合要求的买单价格
    double CalculateBuyOrderPrice()
    {
        // 步骤1：满足报价偏移量要求（理论价格 - 买单价格 ≥ 报价偏移量 → 买单价格 ≤ 理论价格 - 报价偏移量）
        double initialBuyPrice = m_lastTheoreticalPrice - m_quoteOffset;

        // 步骤2：对齐交易所Tick宽度
        double alignedBuyPrice = AlignToTick(initialBuyPrice);

        // 步骤3：校验并调整，避免穿透最优卖盘（买单价格 必须 ≤ 最优卖价 - Tick宽度，防止主动成交）
        // 若最优卖盘有效（大于0，可根据题目假设调整），进行穿透校验
        if (m_lastBestOffer > 0.0) {
            // 买单价格不能高于（最优卖价 - Tick宽度），否则会穿透盘口
            double maxValidBuyPrice = m_lastBestOffer - m_tickWidth;
            if (alignedBuyPrice > maxValidBuyPrice) {
                alignedBuyPrice = maxValidBuyPrice;
                // 再次对齐Tick（防止maxValidBuyPrice未对齐，可选增强）
                alignedBuyPrice = AlignToTick(alignedBuyPrice);
            }
        }

        return alignedBuyPrice;
    }

    // 计算符合要求的卖单价格
    double CalculateSellOrderPrice()
    {
        // 步骤1：满足报价偏移量要求（卖单价格 - 理论价格 ≥ 报价偏移量 → 卖单价格 ≥ 理论价格 + 报价偏移量）
        double initialSellPrice = m_lastTheoreticalPrice + m_quoteOffset;

        // 步骤2：对齐交易所Tick宽度
        double alignedSellPrice = AlignToTick(initialSellPrice);

        // 步骤3：校验并调整，避免穿透最优买盘（卖单价格 必须 ≥ 最优买价 + Tick宽度，防止主动成交）
        // 若最优买盘有效（大于0，可根据题目假设调整），进行穿透校验
        if (m_lastBestBid > 0.0) {
            // 卖单价格不能低于（最优买价 + Tick宽度），否则会穿透盘口
            double minValidSellPrice = m_lastBestBid + m_tickWidth;
            if (alignedSellPrice < minValidSellPrice) {
                alignedSellPrice = minValidSellPrice;
                // 再次对齐Tick（防止minValidSellPrice未对齐，可选增强）
                alignedSellPrice = AlignToTick(alignedSellPrice);
            }
        }

        return alignedSellPrice;
    }

    // 价格对齐到Tick宽度（核心工具方法）
    double AlignToTick(double price)
    {
        // 计算方式：price / tickWidth 取整 → 再乘以 tickWidth，实现对齐
        // 使用round函数四舍五入，确保对齐到最近的Tick价位
        double roundedRatio = std::round(price / m_tickWidth);
        return roundedRatio * m_tickWidth;
    }

    // 提交买单（内部方法）
    void SubmitBuyOrder()
    {
        // 若理论价格未初始化，不提交
        if (m_lastTheoreticalPrice <= 0.0) {
            return;
        }

        // 计算有效买单价格
        double buyPrice = CalculateBuyOrderPrice();

        // 生成唯一订单ID（简单自增，题目允许调用方指定，此处简化实现）
        static uint32_t buyOrderIdCounter = 1000; // 买单ID从1000开始
        m_currentBidOrderId = buyOrderIdCounter++;

        // 发送挂单请求
        m_execution.requestOrderAdd(
            m_currentBidOrderId,
            m_feedcode,
            static_cast<char>(OrderSide::Buy),
            buyPrice,
            m_quoteVolume
        );
    }

    // 提交卖单（内部方法）
    void SubmitSellOrder()
    {
        // 若理论价格未初始化，不提交
        if (m_lastTheoreticalPrice <= 0.0) {
            return;
        }

        // 计算有效卖单价格
        double sellPrice = CalculateSellOrderPrice();

        // 生成唯一订单ID（简单自增，与买单ID区分）
        static uint32_t sellOrderIdCounter = 2000; // 卖单ID从2000开始
        m_currentAskOrderId = sellOrderIdCounter++;

        // 发送挂单请求
        m_execution.requestOrderAdd(
            m_currentAskOrderId,
            m_feedcode,
            static_cast<char>(OrderSide::Sell),
            sellPrice,
            m_quoteVolume
        );
    }

private:
    // 配置参数（构造函数传入，不可修改）
    std::string m_feedcode;       // 标的交易所标识
    double m_quoteOffset;         // 报价偏移量
    uint32_t m_quoteVolume;       // 报价成交量
    double m_tickWidth;           // Tick宽度
    Execution &m_execution;       // 执行服务引用

    // 状态变量（追踪当前挂单状态）
    uint32_t m_currentBidOrderId; // 当前有效买单ID（0表示无）
    uint32_t m_currentAskOrderId; // 当前有效卖单ID（0表示无）
    double m_lastTheoreticalPrice;// 最新理论价格
    double m_lastBestBid;         // 最新最优买价
    double m_lastBestOffer;       // 最新最优卖价
};

