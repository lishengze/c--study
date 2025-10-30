#include "test_external_msg.h"
#include "share_comm_external_message.h"

using namespace share_common;

void test_external_msg() {
    // TODO: Implement test_external_msg()

    TradeOrderReq objSrc;

    // strcpy(objSrc.trade_order_user.cust_id, "TestOrder");


    TradeOrderReq objDst1((char*)(&objSrc));

    // LOG_DEBUG("objDST1.cust_id: {}", objDst1.trade_order_user.cust_id);


    TradeOrderReq objDst2 = std::move(objSrc);

    // LOG_DEBUG("objDST2.cust_id: {}", objDst2.trade_order_user.cust_id);
}