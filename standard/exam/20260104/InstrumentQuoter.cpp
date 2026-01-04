#include <string>
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <system_error>

#define MINIUM_VALUE 0.0000001
#define IS_LESS_EQUAL_ZERO(a)		((a)<MINIUM_VALUE)					/*小于等于0*/
#define IS_GREATER_EQUAL_ZERO(a) 	((a)>-MINIUM_VALUE)					/*大于等于0*/
#define IS_LESS_ZERO(a) 			((a)<-MINIUM_VALUE)					/*小于0*/
#define IS_GREATER_ZERO(a) 			((a)>MINIUM_VALUE)					/*大于0*/
#define IS_EQUAL_ZERO(a)			((a)<MINIUM_VALUE&&(a)>-MINIUM_VALUE)	/*等于0*/
#define IS_NOTEQUAL_ZERO(a) 		((a)>=MINIUM_VALUE||(a)<=-MINIUM_VALUE)	/*不等于0*/

const unsigned int G_BUY_ORDER_ID_COUNTER_INIT = 1;
const unsigned int G_SELL_ORDER_ID_COUNTER_INIT = 1000000000;

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
    }

    void requestOrderRemove(
        uint32_t id)
    {
    }
};

enum class OrderSide {
    Buy = 'B',
    Sell = 'S'
};

class InstrumentQuoter
{
public:
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
          m_currentBidOrderId(0),
          m_currentAskOrderId(0),
          m_theoreticalPrice(0.0),
          m_lastBestBid(0.0),
          m_lastBestOffer(0.0)
    {
        if (IS_LESS_EQUAL_ZERO(tickWidth)) {
            throw std::invalid_argument("tickWidth must be greater than 0");
        }
        if (IS_LESS_EQUAL_ZERO(quoteOffset)) {
            throw std::invalid_argument("quoteOffset cannot be negative");
        }
    }

    void OnTheoreticalPrice(double theoreticalPrice)
    {
        m_theoreticalPrice = theoreticalPrice;
        UpdateQuotes();
    }

    void OnBestBidOffer(double bidPrice, double offerPrice)
    {
        m_lastBestBid = bidPrice;
        m_lastBestOffer = offerPrice;
        UpdateQuotes();
    }

    void OnOrderAddConfirm(uint32_t id)
    {
        if (id == m_currentBidOrderId) {
        } else if (id == m_currentAskOrderId) {
        }
    }

    void OnOrderRemoveConfirm(uint32_t id)
    {
        if (id == m_currentBidOrderId) {
            m_currentBidOrderId = 0;
            SubmitBuyOrder();
        } else if (id == m_currentAskOrderId) {
            m_currentAskOrderId = 0;
            SubmitSellOrder();
        }
    }

private:
    void UpdateQuotes()
    {
        if (m_currentBidOrderId != 0) {
            m_execution.requestOrderRemove(m_currentBidOrderId);
        } else {
            SubmitBuyOrder();
        }

        if (m_currentAskOrderId != 0) {
            m_execution.requestOrderRemove(m_currentAskOrderId);
        } else {
            SubmitSellOrder();
        }
    }

    double CalculateBuyOrderPrice()
    {
        double initialBuyPrice = m_theoreticalPrice - m_quoteOffset;
        double alignedBuyPrice = AlignToTick(initialBuyPrice);

        if (IS_GREATER_ZERO(m_lastBestOffer)) {
            double maxValidBuyPrice = m_lastBestOffer - m_tickWidth;
            if (alignedBuyPrice > maxValidBuyPrice) {
                alignedBuyPrice = maxValidBuyPrice;
                alignedBuyPrice = AlignToTick(alignedBuyPrice);
            }
        }

        return alignedBuyPrice;
    }

    double CalculateSellOrderPrice()
    {
        double initialSellPrice = m_theoreticalPrice + m_quoteOffset;
        double alignedSellPrice = AlignToTick(initialSellPrice);

        if (IS_GREATER_ZERO(m_lastBestBid)) {
            double minValidSellPrice = m_lastBestBid + m_tickWidth;
            if (alignedSellPrice < minValidSellPrice) {
                alignedSellPrice = minValidSellPrice;
                alignedSellPrice = AlignToTick(alignedSellPrice);
            }
        }

        return alignedSellPrice;
    }

    double AlignToTick(double price)
    {
        double roundedRatio = std::round(price / m_tickWidth);
        return roundedRatio * m_tickWidth;
    }

    void SubmitBuyOrder()
    {
        if (IS_LESS_EQUAL_ZERO(m_theoreticalPrice)) {
            throw std::invalid_argument("lastTheoreticalPrice must be greater than 0");
            return;
        }

        if (IS_LESS_EQUAL_ZERO(m_lastBestOffer)) {
            throw std::invalid_argument("lastBestOffer must be greater than 0");
            return;
        }


        double buyPrice = CalculateBuyOrderPrice();

        static uint32_t buyOrderIdCounter = G_BUY_ORDER_ID_COUNTER_INIT;
        m_currentBidOrderId = buyOrderIdCounter++;

        m_execution.requestOrderAdd(
            m_currentBidOrderId,
            m_feedcode,
            static_cast<char>(OrderSide::Buy),
            buyPrice,
            m_quoteVolume
        );
    }

    void SubmitSellOrder()
    {
        if (IS_LESS_EQUAL_ZERO(m_theoreticalPrice)) {
            throw std::invalid_argument("lastTheoreticalPrice must be greater than 0");
            return;
        }

        if (IS_LESS_EQUAL_ZERO(m_lastBestBid)) {
            throw std::invalid_argument("lastBestBid must be greater than 0");
            return;
        }

        double sellPrice = CalculateSellOrderPrice();

        static uint32_t sellOrderIdCounter = G_SELL_ORDER_ID_COUNTER_INIT;
        m_currentAskOrderId = sellOrderIdCounter++;

        m_execution.requestOrderAdd(
            m_currentAskOrderId,
            m_feedcode,
            static_cast<char>(OrderSide::Sell),
            sellPrice,
            m_quoteVolume
        );
    }

private:
    std::string m_feedcode;
    double m_quoteOffset;
    uint32_t m_quoteVolume;
    double m_tickWidth;
    Execution &m_execution;

    uint32_t m_currentBidOrderId;
    uint32_t m_currentAskOrderId;
    double m_theoreticalPrice;
    double m_lastBestBid;
    double m_lastBestOffer;
};

